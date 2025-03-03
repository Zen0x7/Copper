#ifndef COPPER_COMPONENTS_INVOKE_HPP
#define COPPER_COMPONENTS_INVOKE_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/kernel.hpp>

namespace copper::components {
/**
 * Invoke
 *
 * @param method
 * @param signature
 * @param headers
 * @param body
 * @return async_of<kernel_call_result>
 */
containers::async_of<kernel_call_result> invoke(std::string method,
                                                std::string signature,
                                                std::string headers,
                                                std::string body);

/**
 *  Invoke from console
 *
 * @param method
 * @param signature
 * @param headers
 * @param body
 * @return async_of<void>
 */
containers::async_of<void> invoke_from_console(std::string method,
                                               std::string signature,
                                               std::string headers,
                                               std::string body);
}  // namespace copper::components

#endif