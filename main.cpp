#include <iostream>
#include <copper/app.hpp>

int main() {
    std::cout << "Running version " << copper::get_version() << std::endl;
    copper::run();
    return 0;
}
