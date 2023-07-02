#include <iostream>
#include <vector>
#include "demoApi.h"
#include "designServices.h"
#include "state_less_demo.h"

int main(int argc, char **argv) {


    APIDemo::main();
    design_services_demo::main();
    state_less_demo::main();
    thread_pool_demo::main();
    

    std::vector<int> someVector(10, 0);

    std::cout << "Hello, world! fro, kdevelop" << std::endl;
    return 0;
}
