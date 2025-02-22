//
// Created by ian on 04-02-25.
//

#ifndef COPPER_APP_HPP
#define COPPER_APP_HPP

#include <copper/components/configuration.hpp>
#include <string>

namespace copper {

/**
 * Get version
 *
 * @return string
 */
std::string get_version();

/**
 * Run
 *
 * @param argc
 * @param argv
 * @return int
 */
int run(int argc, const char* argv[]);

}  // namespace copper

#endif  // COPPER_APP_HPP
