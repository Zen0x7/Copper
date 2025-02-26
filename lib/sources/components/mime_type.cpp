#include <copper/components/mime_type.hpp>

namespace copper::components {

boost::beast::string_view mime_type(boost::beast::string_view path) {
  using boost::beast::iequals;
  // LCOV_EXCL_START
  auto const _extension = [&path] {
    auto const _pos = path.rfind(".");
    if (_pos == boost::beast::string_view::npos)
      return boost::beast::string_view{};
    return path.substr(_pos);
  }();
  // LCOV_EXCL_STOP
  if (iequals(_extension, ".htm")) return "text/html";
  if (iequals(_extension, ".html")) return "text/html";
  if (iequals(_extension, ".php")) return "text/html";
  if (iequals(_extension, ".css")) return "text/css";
  if (iequals(_extension, ".txt")) return "text/plain";
  if (iequals(_extension, ".js")) return "application/javascript";
  if (iequals(_extension, ".json")) return "application/json";
  if (iequals(_extension, ".xml")) return "application/xml";
  if (iequals(_extension, ".swf")) return "application/x-shockwave-flash";
  if (iequals(_extension, ".flv")) return "video/x-flv";
  if (iequals(_extension, ".png")) return "image/png";
  if (iequals(_extension, ".jpe")) return "image/jpeg";
  if (iequals(_extension, ".jpeg")) return "image/jpeg";
  if (iequals(_extension, ".jpg")) return "image/jpeg";
  if (iequals(_extension, ".gif")) return "image/gif";
  if (iequals(_extension, ".bmp")) return "image/bmp";
  if (iequals(_extension, ".ico")) return "image/vnd.microsoft.icon";
  if (iequals(_extension, ".tiff")) return "image/tiff";
  if (iequals(_extension, ".tif")) return "image/tiff";
  if (iequals(_extension, ".svg")) return "image/svg+xml";
  if (iequals(_extension, ".svgz")) return "image/svg+xml";
  return "application/text";
}

}  // namespace copper::components