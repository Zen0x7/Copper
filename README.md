# Copper

[![codecov](https://codecov.io/gh/Zen0x7/Copper/branch/master/graph/badge.svg?token=XENML1H9VV)](https://codecov.io/gh/Zen0x7/Copper)

## Requirements

- Boost 1.87.0

## Components

### Base64

Provides optimized Base64 encode and decode algorithms.

#### Location

> copper/components/base64.hpp

#### API

```cpp
std::string base64_encode(const std::string &input, bool padding = true);
std::string base64_decode(const std::string &input);
```

#### Usage

```cpp
#include <copper/components/base64.hpp>

auto encoded = base64_encode("hello");
auto decoded = base64_decode(encoded); 

assert(decoded.data() == "hello");
```

*See more examples in the [test cases](/tests/components/base64_test.cc).*

### Base64url

Provides optimized Base64url encode and decode algorithms.

#### Location

> copper/components/base64url.hpp

#### API

```cpp
std::string base64url_encode(const std::string &input, bool padding = true);
std::string base64url_decode(const std::string &input);
```

#### Usage

```cpp
#include <copper/components/base64url.hpp>

auto encoded = base64url_encode("hello");
auto decoded = base64url_decode(encoded); 

assert(decoded == "hello");
```

*See more examples in the [test cases](/tests/components/base64url_test.cc).*

### Cipher

Provides HMAC and AES-256-CBC encryption and decryption algorithms.

#### Location

> copper/components/cipher.hpp

#### API

```cpp
std::string cipher_generate_sha_256();
std::string ciper_hmac(const std::string &input, const std::string &app_key);
std::pair<std::string, std::string> cipher_generate_aes_key_iv(const std::string &input, const std::string &app_key);
std::string cipher_encrypt(const std::string &input, const std::string &key, const std::string &iv);
std::string cipher_decrypt(const std::string &input, const std::string &key, const std::string &iv);
```

#### Usage

```cpp
#include <copper/components/cipher.hpp>

auto [secret, iv] = cipher_generate_aes_key_iv();

auto input = "hello";

auto encrypted = base64::encode(cipher_encrypt(input, secret, iv));
auto decrypted = cipher_decrypt(base64::decode(encrypted), secret, iv);

assert(encrypted != input);
assert(decrypted == input);
```

*See more examples in the [test cases](/tests/components/cipher_test.cc).*

### Validator

Provides JSON structure validation algorithms.

#### Location

> copper/components/validator.hpp

#### API

```cpp
boost::shared_ptr<instance> make(const std::map<std::string, std::string>& rules, const boost::json::value& value);
```

#### Usage

```cpp
#include <copper/components/validator.hpp>

std::map<std::string, std::string> rules = {
    {"*", "is_object"},
    {"username", "is_string"},
}

std::string json = R"({"username":"zen0x7"})"
auto value = boost::json::parse(json);

auto response = validator_make(rules, value);

assert(response->success);
```

*See more examples in the [test cases](/tests/components/validator_test.cc).*

### Random

Provides random data generation algorithms.

#### Location

> copper/components/random.hpp

#### API

```cpp
std::string random_string(int size);
```

#### Usage

```cpp
#include <copper/components/random.hpp>

auto string = random_string(32);

assert(string.length() == 32);
```

*See more examples in the [test cases](/tests/components/random_test.cc).*

### Expression

Provides regular expression matching and parameter binding extraction algorithms.

#### Location

> copper/components/expression.hpp

#### API

```cpp
boost::shared_ptr<instance> from_string(const std::string &input);
boost::shared_ptr<result> instance::query(const std::string &input);
bool result::matches();
std::string result::get(const std::string &name);
```

#### Usage

```cpp
#include <copper/components/expression.hpp>

auto expression = expression_make("/users/{user}");

auto result = expression->query("/users/7");

assert(result->matches());
assert(result->get("user") == "7");
```

*See more examples in the [test cases](/tests/components/expression_test.cc).*

### Authenticator

Provides JWT issue and decoding algorithms.

#### Location

> copper/components/authenticator.hpp

#### API

```cpp
boost::optional<result> authentication_from_bearer(const std::string &bearer, const std::string &app_key);

std::string authentication_to_bearer(boost::uuids::uuid id, const std::string &app_key, const std::string &type = "App\\Models\\User");
```

#### Usage

```cpp
#include <copper/components/authentication.hpp>

const boost::uuids::uuid id = boost::uuids::random_generator()();

const std::string app_key = "sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=";

const std::string bearer = authentication_to_bearer(id, base64::decode(app_key), "user");

auto result = authentication_from_bearer(bearer, base64::decode(app_key));

assert(result.has_value())
assert(result.value().id == id)
assert(result.value().type == "user");
```

*See more examples in the [test cases](/tests/components/authentication_test.cc).*

### MIME Type

Provides MIME recognition algorithm.

#### Location

> copper/components/mime_type.hpp

#### API

```cpp
boost::beast::string_view mime_type(boost::beast::string_view path);
```

#### Usage

```cpp
#include <copper/components/mime_type.hpp>

auto value = mime_type("app.json");

assert(value == "application/json");
```

*See more examples in the [test cases](/tests/components/mime_type_test.cc).*

### Normalized Path

Provides MIME recognition algorithm.

#### Location

> copper/components/normalized_path.hpp

#### API

```cpp
std::string normalized_path(boost::beast::string_view base, boost::beast::string_view path);
```

#### Usage

```cpp
#include <copper/components/normalized_path.hpp>

auto value = normalized_path("/srv/app", "/manifest.json");

assert(value == "/srv/app/manifest.json");
```

*See more examples in the [test cases](/tests/components/normalized_path_test.cc).*

### Failure Report

Provides error reporting with stacktrace.

#### Location

> copper/components/report.hpp

#### API

```cpp
void report(boost::beast::error_code ec, char const* what)
```

#### Usage

```cpp
#include <copper/components/report.hpp>

boost::system::error_code ec;

report(ec, "All OK");
```

*See more examples in the [test cases](/tests/components/report_test.cc).*

