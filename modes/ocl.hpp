#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <stdexcept>
#include "../common.hpp"
#include "../opencl.hpp"

struct BruteForceSolverOcl : public Solver {
    BruteForceSolverOcl(const Instance& instance) :
        instance(instance)
    {}

    unsigned solve() override {
        try {
            return app();
        } catch (const AppException& e) {
            std::cerr << "[App Error] " << e.what() << "\n";
        } catch(const OpenClException& e) {
            std::cerr << "[OpenCL Error] " << e.what() << "\n";
        }

        return 0;
    }

    unsigned app() {
        unsigned platform_id = 1;
        unsigned device_id = 0;
        std::string kernel_path = "kernel.cl";

        cl_int response;
        auto context = createOpenClContext(platform_id);
        auto devices = getClDevices(context);
        auto source = loadClKernelSource(kernel_path);
        auto program = buildClProgram(context, source, devices);
        auto kernel = createClKernelFromProgram(program, "multiply");

        /*std::size_t size = 10000;
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
        kernel.setArg(3, size);

        cl::Event event;
        check(queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(size), cl::NDRange(1), NULL, &event), "Could not enqueue ND range kernel");
        event.wait();
        check(queue.enqueueReadBuffer(outC, CL_TRUE, 0, sizeof(int) * size, c.data()), "Could not enqueue read buffer");
        */

        return 33;
    }

private:
    const Instance& instance;
};

