#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <stdexcept>
#include "opencl.hpp"

struct AppException : public std::runtime_error {
    AppException(const std::string& what) :
        std::runtime_error(what)
    {}
};

struct OpenClException : public std::runtime_error {
    OpenClException(const std::string& what) :
        std::runtime_error(what)
    {}
};


void check(cl_int response, const std::string& message = "")  {
    if (response != CL_SUCCESS) {
        std::cerr << "[Uwaga] " << response << "\n";
        throw OpenClException(message);
    }
}

cl::Context createOpenClContext(unsigned platform_id) {
    cl::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.size() == 0) {
        throw OpenClException("There is no available OpenCL platform");
    }

    const auto& platform = platforms[platform_id];
    std::string platform_vendor;
    platform.getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platform_vendor);
    std::cerr << "[Platform Vendor] " << platform_vendor << "\n";

    cl_int response;
    cl_context_properties cprops[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform(), 0};
    cl::Context context(CL_DEVICE_TYPE_GPU, cprops, NULL, NULL, &response);
    check(response, "Could not create context");

    return context;
}

cl::vector<cl::Device> getClDevices(cl::Context& context) {
    cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    if (devices.size() == 0) {
        throw OpenClException("This platform does not support any device");
    }

    return devices;
}

cl::Program::Sources loadClKernelSource(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw AppException("Could not read a file");
    }

    std::string code(
        std::istreambuf_iterator<char>(file),
        (std::istreambuf_iterator<char>())
    );

    cl::Program::Sources source(1, code);
    return source;
}

cl::Program buildClProgram(const cl::Context& context, const cl::Program::Sources& source, const cl::vector<cl::Device>& devices) {
    cl::Program program(context, source);
    check(program.build(devices, ""), "OpenCL program build error");
    return program;
}

cl::Kernel createClKernelFromProgram(const cl::Program& program, const std::string& function_name) {
    cl_int response;
    cl::Kernel kernel(program, function_name.c_str(), &response);
    check(response, "Could not create kernel from program");
    return kernel;
}

std::vector<int> randomIntVector(std::size_t n) {
    std::vector<int> vec(n, 0);

    for (std::size_t i = 0; i < n; i++) {
        vec[i] = random() % 10;
    }

    return vec;
}


int app(int argc, char *argv[]) {
    unsigned platform_id = 1;
    unsigned device_id = 0;
    std::string kernel_path = "kernel.cl";

    cl_int response;
    auto context = createOpenClContext(platform_id);
    auto devices = getClDevices(context);
    auto source = loadClKernelSource(kernel_path);
    auto program = buildClProgram(context, source, devices);
    auto kernel = createClKernelFromProgram(program, "multiply");

    std::size_t size = 10;
    std::vector<int> a = randomIntVector(size);
    std::vector<int> b = randomIntVector(size);
    std::vector<int> c(size, 0);

    cl::Buffer inA(context, CL_MEM_READ_ONLY, sizeof(int) * size);
    cl::Buffer inB(context, CL_MEM_READ_ONLY, sizeof(int) * size);
    cl::Buffer outC(context, CL_MEM_WRITE_ONLY, sizeof(int) * size);

    cl::CommandQueue queue(context, devices[device_id], 0, &response);
    check(response, "Could not initialze command queue");
    queue.enqueueWriteBuffer(inA, CL_TRUE, 0, sizeof(int) * size, a.data());
    queue.enqueueWriteBuffer(inB, CL_TRUE, 0, sizeof(int) * size, b.data());
    
    kernel.setArg(0, inA);
    kernel.setArg(1, inB);
    kernel.setArg(2, outC);

    cl::Event event;
    check(queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(size), cl::NDRange(1), NULL, &event), "Could not enqueue ND range kernel");
    event.wait();
    check(queue.enqueueReadBuffer(outC, CL_TRUE, 0, sizeof(int) * size, c.data()), "Could not enqueue read buffer");
    
    for (int i = 0; i < size; i++) {
        std::cout << a[i] << " * " << b[i] << " = " << c[i] << "\n";
    }

    return 0;
}

int main(int argc, char *argv[]) {
    try {
        return app(argc, argv);
    } catch (const AppException& e) {
        std::cerr << "[App Error] " << e.what() << "\n";
    } catch(const OpenClException& e) {
        std::cerr << "[OpenCL Error] " << e.what() << "\n";
        return 1;
    }
}

