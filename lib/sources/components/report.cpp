#include <copper/components/report.hpp>

namespace copper::components {
    void report(
            boost::beast::error_code ec,
            const char *what
    ) {
        std::cerr << boost::stacktrace::stacktrace() << '\n';
        std::cerr << what << ": " << ec.message() << "\n";
    }
}

