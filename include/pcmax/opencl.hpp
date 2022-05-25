#pragma once

#define CL_HPP_TARGET_OPENCL_VERSION 210
#include <CL/cl2.hpp>

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

