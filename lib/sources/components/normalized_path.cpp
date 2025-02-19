#include <copper/components/normalized_path.hpp>

namespace copper::components {
std::string normalized_path(boost::beast::string_view base,
                            boost::beast::string_view path) {
  // LCOV_EXCL_START
  if (base.empty()) return std::string(path);
  std::string result(base);
#ifdef BOOST_MSVC
  char constexpr path_separator = '\\';
  if (result.back() == path_separator) result.resize(result.size() - 1);
  result.append(path.data(), path.size());
  for (auto& c : result)
    if (c == '/') c = path_separator;
#else
  char constexpr path_separator = '/';
  if (result.back() == path_separator) result.resize(result.size() - 1);
  result.append(path.data(), path.size());
#endif
  return result;
}
// LCOV_EXCL_STOP

}  // namespace copper::components
