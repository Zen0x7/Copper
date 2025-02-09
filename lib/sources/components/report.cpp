#include <copper/components/report.hpp>

void copper::components::report(boost::beast::error_code ec, const char *what) {
    std::cerr << boost::stacktrace::stacktrace() << '\n';
    std::cerr << what << ": " << ec.message() << "\n";
}
