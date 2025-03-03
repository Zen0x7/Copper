# Copper

<p align="center"><a href="https://copper.iantorres.cl" target="_blank"><img src="./logo.png" width="200" alt="Copper"></a></p>

<p align="center">
<a href="https://github.com/Zen0x7/Copper/actions/workflows/ci.yml"><img src="https://github.com/Zen0x7/Copper/actions/workflows/ci.yml/badge.svg" alt="CI"></a>
<a href="https://codecov.io/gh/Zen0x7/Copper"><img src="https://codecov.io/gh/Zen0x7/Copper/branch/master/graph/badge.svg?token=XENML1H9VV" alt="Coverage"></a>
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=alert_status" alt="Quality Gate"></a>
</p>

<p align="center">
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=bugs" alt="Bugs"></a>
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=vulnerabilities" alt="Vulnerabilities"></a>
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=code_smells" alt="Code Smells"></a>
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=duplicated_lines_density" alt="Duplicated Lines"></a>
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=sqale_index" alt="Technical Debt"></a>
</p>

<p align="center">
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=reliability_rating" alt="Reliability"></a>
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=security_rating" alt="Security"></a>
<a href="https://sonarcloud.io/project/overview?id=Zen0x7_Copper"><img src="https://sonarcloud.io/api/project_badges/measure?project=Zen0x7_Copper&metric=sqale_rating" alt="Maintainability"></a>
</p>


## About

Copper is a featured components collection also HTTP/WebSockets base project.

## Features

### API and CLI applications

You can create **controllers** using the following syntax:

```c++
#include <copper/components/controller.hpp>
#include <copper/components/controller_parameters.hpp>

#include <copper/components/chronos.hpp>
#include <copper/components/json.hpp>
#include <copper/components/status_code.hpp>

namespace copper::controllers::api {
  /**
   * Custom Controller
   */
  class custom_controller final : public controller {
    /**
     * Invoke 
     */
    containers::async_of<response> invoke(
      const controller_parameters parameters
    ) override {
      const json::object _data = {{"timestamp", chronos::now() }};
      co_return make_response(parameters, status_code::ok, serialize(_data), "application/json");
    }
  };
} // namespace copper::controllers::api
```

After that, you can push the controller to the **router**:

```c++
auto _router = router::instance();

_router->push(
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

> This software is released **PARTIALLY** based on his derivative source licenses and **ALL OTHERS** uses the repository license. 