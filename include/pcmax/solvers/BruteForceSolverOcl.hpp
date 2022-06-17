#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <pcmax/Instance.hpp>
#include <pcmax/solvers/Solver.hpp>
#include <pcmax/opencl.hpp>

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


        unsigned processors = instance.processors_number;
        unsigned total_tasks = instance.tasks.size();
        // unsigned unlocked_tasks = 6; // 8;
        // unsigned locked_tasks = total_tasks - unlocked_tasks;
        unsigned locked_tasks = 8;
        unsigned unlocked_tasks = total_tasks - locked_tasks;

        cl_int response;
        auto context = createOpenClContext(platform_id);
        auto devices = getClDevices(context);
        auto source = loadClKernelSource(kernel_path);
        auto program = buildClProgram(context, source, devices);
        auto kernel = createClKernelFromProgram(program, "find_best");

        cl::CommandQueue queue(context, devices[device_id], 0, &response);
        check(response, "Could not initialze command queue");
       
        cl::Buffer durations_buffer(context, CL_MEM_READ_ONLY, sizeof(unsigned) * instance.tasks.size());
        queue.enqueueWriteBuffer(durations_buffer, CL_TRUE, 0, sizeof(unsigned) * instance.tasks.size(), instance.tasks.data());

        std::vector<int> answers(std::pow(processors, locked_tasks), 0);
        cl::Buffer answers_buffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * answers.size());
        queue.enqueueWriteBuffer(answers_buffer, CL_TRUE, 0, sizeof(int) * answers.size(), answers.data());

        kernel.setArg(0, durations_buffer);
        kernel.setArg(1, answers_buffer);
        kernel.setArg(2, sizeof(unsigned), &total_tasks);
        kernel.setArg(3, sizeof(unsigned), &unlocked_tasks);
        kernel.setArg(4, sizeof(unsigned), &processors);

        cl::Event event;
        check(queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(answers.size()), cl::NDRange(1), NULL, &event), "Could not enqueue ND range kernel");
        event.wait();
        check(queue.enqueueReadBuffer(answers_buffer, CL_TRUE, 0, sizeof(int) * answers.size(), answers.data()), "Could not enqueue read buffer");

        for (std::size_t i = 0; i < answers.size(); i++) {
            std::cout << "[" << i << "] " << answers[i] << "\n";
        }

        // return *std::min_element(answers.begin(), answers.end());
        return fast_min(answers);
    }

    template <typename T>
    T fast_min(const std::vector<T>& vec) {
        T global_min = std::numeric_limits<T>::max();

        #pragma omp parallel
        {
            T local_min = std::numeric_limits<T>::max();

            #pragma omp for
            for (std::size_t i = 0; i < vec.size(); i++) {
                auto value = vec[i];
                if (value < local_min && value > 0) {
                    local_min = value;
                }
            }

            #pragma omp critical
            {
                if (local_min < global_min) {
                    global_min = local_min;
                }
            }
        }

        return global_min;
    }

private:
    const Instance& instance;
};

