#ifndef COPPER_COMPONENTS_SUBSCRIPTIONS_HPP
#define COPPER_COMPONENTS_SUBSCRIPTIONS_HPP

#pragma once

// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index_container.hpp>
#include <copper/components/subscription.hpp>

namespace copper::components::subscriptions {

/**
 * By Connection ID
 */
struct by_connection_id {};

/**
 * By Channel Name
 */
struct by_channel_name {};

/**
 * Container
 */
using container = boost::multi_index::multi_index_container<
    subscription,
    boost::multi_index::indexed_by<
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<by_connection_id>,
            boost::multi_index::member<subscription,
                                       boost::uuids::uuid,
                                       &subscription::connection_id_>>,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<by_channel_name>,
            boost::multi_index::
                member<subscription, std::string, &subscription::channel_>>>>;
}  // namespace copper::components::subscriptions

#endif  // COPPER_COMPONENTS_SUBSCRIPTIONS_HPP
