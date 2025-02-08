#include <copper/components/random.hpp>
#include <copper/components/base64url.hpp>

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace copper::components::random {

    std::string string(int size) {
        std::string output;
        output.reserve(size);

        boost::random::random_device device;
        boost::random::uniform_int_distribution<> distribution(0, components::base64url::chars.size() - 1);

        for (int i = 0; i < size; i++) output.push_back(components::base64url::chars[distribution(device)]);

        return output; }
}