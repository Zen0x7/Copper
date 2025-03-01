#ifndef COPPER_COMPONENTS_LISTENER_HPP
#define COPPER_COMPONENTS_LISTENER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/protocol_handler.hpp>
#include <copper/components/task_group.hpp>

namespace copper::components {

/**
 * Listener
 *
 * @param state
 * @param task_group
 * @param ctx
 * @param endpoint
 * @param doc_root
 * @return async_of<void>
 */
containers::async_of<void> listener(boost::uuids::uuid server_id,
                                    shared<task_group> task_group,
                                    boost::asio::ip::tcp::endpoint endpoint,
                                    boost::beast::string_view doc_root);

}  // namespace copper::components

#endif