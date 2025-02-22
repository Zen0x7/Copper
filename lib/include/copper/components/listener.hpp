#ifndef COPPER_COMPONENTS_LISTENER_HPP
#define COPPER_COMPONENTS_LISTENER_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/detect_session.hpp>
#include <copper/components/task_group.hpp>

namespace copper::components {

/**
 * Forward state
 */
class state;

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
containers::async_of<void> listener(shared<state> state,
                                    shared<task_group> task_group,
                                    boost::asio::ssl::context &ctx,
                                    boost::asio::ip::tcp::endpoint endpoint,
                                    boost::beast::string_view doc_root);

}  // namespace copper::components

#endif