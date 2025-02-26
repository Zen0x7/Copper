# Copper

<p align="center"><a href="https://copper.iantorres.cl" target="_blank"><img src="./logo.png" width="200" alt="Copper"></a></p>

<p align="center">
<a href="https://codecov.io/gh/Zen0x7/Copper"><img src="https://codecov.io/gh/Zen0x7/Copper/branch/master/graph/badge.svg?token=XENML1H9VV" alt="Coverage"></a>
<a href="https://github.com/Zen0x7/Copper/actions/workflows/ci.yml"><img src="https://github.com/Zen0x7/Copper/actions/workflows/ci.yml/badge.svg" alt="Coverage"></a>
</p>

## About

Copper is a featured components collection also HTTP/WebSockets base project.

## Features

### API and CLI applications

You can create **controllers** using the following syntax:

```c++
/**
 * Custom Controller
 */
class custom_controller final : public components::controller {

    /**
     * Invoke 
     */
    components::containers::async_of<components::response> invoke(
          const components::request &request,
          const components::json::value & body,
          const components::containers::optional_of<
          const components::authentication_result> & auth,
          const components::containers::unordered_map_of_strings & bindings,
          const long start_at
    ) override {
        auto _now = components::chronos::now();
        
        const components::json::object _data = {{"timestamp", _now}};
        
        co_return make_response(
            request,
            components::status_code::ok,
            serialize(_data),
            "application/json",
             start_at
        );
    }
} 
```

After that, you can push the controller to the **router**:

```c++
router->push(
    method::get, "/api/custom",
    boost::make_shared<custom_controller>(state),
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
