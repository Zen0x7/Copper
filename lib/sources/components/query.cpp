#include <copper/components/query.hpp>

namespace copper::components {

std::string query_from_request(const request &request) {
  json::object _object;

  containers::map_of<std::string, containers::vector_of<std::string>> _map;

  const size_t _symbol = request.target().find('?');
  const bool _has_params = _symbol != std::string::npos;
  const std::string _query{_has_params ? request.target().substr(_symbol + 1)
                                      : ""};

  std::istringstream _ss(_query);
  std::string _pair;

  while (std::getline(_ss, _pair, '&')) {
    size_t _pos = _pair.find('=');
    if (_pos == std::string::npos) continue;

    std::string _key = _pair.substr(0, _pos);
    std::string _value = _pair.substr(_pos + 1);

    if (_key.size() > 2 && _key.substr(_key.size() - 2) == "[]") {
      _key = _key.substr(0, _key.size() - 2);
      _map[_key].push_back(_value);
    } else {
      _object[_key] = _value;
    }
  }

  for (const auto &[key, values] : _map) {
    json::array array;
    for (const auto &value : values) {
      array.push_back(json::string(value));
    }
    _object[key] = array;
  }

  return std::string(serialize(_object));
}
}  // namespace copper::components