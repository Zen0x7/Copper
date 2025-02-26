#include <copper/components/normalized_path.hpp>

namespace copper::components {

std::string normalized_path(boost::beast::string_view base,
                            boost::beast::string_view path) {
  // LCOV_EXCL_START
  if (base.empty()) return std::string(path);
  std::string _result(base);
#ifdef BOOST_MSVC
  char constexpr _separator = '\\';
  if (_result.back() == _separator) _result.resize(_result.size() - 1);
  _result.append(path.data(), path.size());
  for (auto& _char : _result)
    if (_char == '/') _char = _separator;
#else
  char constexpr _separator = '/';
  if (_result.back() == _separator) _result.resize(_result.size() - 1);
  _result.append(path.data(), path.size());
#endif
  return _result;
}
// LCOV_EXCL_STOP

}  // namespace copper::components
