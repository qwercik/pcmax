#include <iostream>
#include "opencl.hpp"

int main() {
    cl_int err;
    cl::vector<cl::Platform> platformList;
    cl::Platform::get(&platformList);

    std::cout << "Platform number is " << platformList.size() << '\n';
    for (int i = 0; i < platformList.size(); i++) {
        std::string platformVendor;
        platformList[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
        std::cout << "Platform: " << platformVendor << "\n";

        cl_context_properties cprops[3] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties) (platformList[i])(),
            0
        };

        cl::Context context(CL_DEVICE_TYPE_ALL, cprops, NULL, NULL, &err);
        cl::vector<cl::Device> devices;
        devices = context.getInfo<CL_CONTEXT_DEVICES>();
        for (int j = 0; j < devices.size(); j++) {
            std::string name;
            devices[j].getInfo(CL_DEVICE_NAME, &name);
            std::cout << "Device " << j << " - " << name << "\n";
        }
    }
}

