#ifndef COPPER_COMPONENTS_GUNZIP_HPP
#define COPPER_COMPONENTS_GUNZIP_HPP

#pragma once

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <copper/components/shared.hpp>
#include <sstream>
#include <string>

namespace copper::components {
/**
 * Compress
 *
 * @param input
 * @return string
 */
std::string gunzip_compress(const std::string& input);

/**
 * Decompress
 *
 * @param input
 * @return string
 */
std::string gunzip_decompress(const std::string& input);
}  // namespace copper::components

#endif