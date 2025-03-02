# Copper

<p align="center"><a href="https://copper.iantorres.cl" target="_blank"><img src="./logo.png" width="200" alt="Copper"></a></p>

<p align="center">
<a href="https://github.com/Zen0x7/Copper/actions/workflows/ci.yml"><img src="https://github.com/Zen0x7/Copper/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
<a href="https://codecov.io/gh/Zen0x7/Copper"><img src="https://codecov.io/gh/Zen0x7/Copper/branch/master/graph/badge.svg?token=XENML1H9VV" alt="Coverage"></a>
<a href="https://sonarcloud.io/summary/new_code?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=alert_status" alt="Quality Cage"></a>
</p>

## About

Copper is a featured components collection also HTTP/WebSockets base project.

## Features

### API and CLI applications

You can create **controllers** using the following syntax:

```c++
#include <copper/components/controller.hpp>

#include <copper/components/chronos.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers::api {
    using namespace copper::components;

    /**
     * Custom Controller
     */
    class custom_controller final : public components::controller {
    
        /**
         * Invoke 
         */
        components::containers::async_of<components::response> invoke(
              const components::controller_parameters &parameters
        ) override {
            auto _now = components::chronos::now();
            
            const components::json::object _data = {{"timestamp", _now}};
            
            co_return make_response(
                parameters,
                components::status_code::ok,
                serialize(_data),
                "application/json"
            );
        }
    }
}
```

After that, you can push the controller to the **router**:

```c++
router::instance()->push(
    method::get, "/api/custom",
    boost::make_shared<custom_controller>(),
    {
        .use_auth_ = true,
        .use_throttler_ = true,
        .rpm_ = 60,
    });
```

And finally, you can use any HTTP client or the binary.

```bash
./copper --as=command \
         --command=invoke \
         --method=GET \
         --signature=/api/custom
```

### TODO

Showcase more features ...

## License

This software is released PARTIALLY based on his derivative source licenses and ALL OTHERS uses the repository license. 