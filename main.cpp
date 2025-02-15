#include <iostream>
#include <copper/app.hpp>

int main(int argc, const char * argv[]) {
    std::cout << "Running version " << copper::get_version() << std::endl;
    return copper::run(argc, argv);
}
