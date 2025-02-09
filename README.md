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
std::string encode(const std::string &input, bool padding = true);
std::string decode(const std::string &input);
```

#### Usage

```cpp
#include <copper/components/base64.hpp>

auto encoded = base64::encode("hello");
auto decoded = base64::decode(encoded); 

assert(decoded.data() == "hello");
```

*See more examples in the [test cases](/tests/components/base64_test.cc).*

### Base64url

Provides optimized Base64url encode and decode algorithms.

#### Location

> copper/components/base64url.hpp

#### API

```cpp
std::string encode(const std::string &input, bool padding = true);
std::string decode(const std::string &input);
```

#### Usage

```cpp
#include <copper/components/base64url.hpp>

auto encoded = base64url::encode("hello");
auto decoded = base64url::decode(encoded); 

assert(decoded == "hello");
```

*See more examples in the [test cases](/tests/components/base64url_test.cc).*

### Cipher

Provides HMAC and AES-256-CBC encryption and decryption algorithms.

#### Location

> copper/components/cipher.hpp

#### API

```cpp
std::string generate_sha_256();
std::string hmac(const std::string &input, const std::string &app_key);
std::pair<std::string, std::string> generate_aes_key_iv(const std::string &input, const std::string &app_key);
std::string encrypt_aes_256_cbc(const std::string &input, const std::string &key, const std::string &iv);
std::string decrypt_aes_256_cbc(const std::string &input, const std::string &key, const std::string &iv);
```

#### Usage

```cpp
#include <copper/components/cipher.hpp>

auto [secret, iv] = cipher::generate_aes_key_iv();

auto input = "hello";

auto encrypted = base64::encode(cipher::encrypt_aes_256_cbc(input, secret, iv));
auto decrypted = cipher::decrypt_aes_256_cbc(base64::decode(encrypted), secret, iv);

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

auto response = validator::make(rules, value);

assert(response->success);
```

*See more examples in the [test cases](/tests/components/validator_test.cc).*

### Random

Provides random data generation algorithms.

#### Location

> copper/components/random.hpp

#### API

```cpp
std::string random(int size);
```

#### Usage

```cpp
#include <copper/components/random.hpp>

auto string = random::string(32);

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

auto expression = expression::from_string("/users/{user}");

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
boost::optional<result> from_bearer(const std::string &bearer, const std::string &app_key);

std::string to_bearer(boost::uuids::uuid id, const std::string &app_key, const std::string &type = "App\\Models\\User");
```

#### Usage

```cpp
#include <copper/components/authenticator.hpp>

const boost::uuids::uuid id = boost::uuids::random_generator()();

const std::string app_key = "sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=";

const std::string bearer = authenticator::to_bearer(id, base64::decode(app_key), "user");

auto result = authenticator::from_bearer(bearer, base64::decode(app_key));

assert(result.has_value())
assert(result.value().id == id)
assert(result.value().type == "user");
```

*See more examples in the [test cases](/tests/components/authenticator_test.cc).*

### MIME Type

Provides MIME recognition algorithm.

#### Location

> copper/components/filesystem/mime_type.hpp

#### API

```cpp
boost::beast::string_view get(boost::beast::string_view path);
```

#### Usage

```cpp
#include <copper/components/filesystem/mime_type.hpp>

auto value = filesystem::mime_type::get("app.json");

assert(value == "application/json");
```

*See more examples in the [test cases](/tests/components/filesystem/mime_type_test.cc).*

### Normalized Path

Provides MIME recognition algorithm.

#### Location

> copper/components/filesystem/normalized_path.hpp

#### API

```cpp
std::string get(boost::beast::string_view base, boost::beast::string_view path);
```

#### Usage

```cpp
#include <copper/components/filesystem/normalized_path.hpp>

auto value = filesystem::normalized_path::get("/srv/app", "/manifest.json");

assert(value == "/srv/app/manifest.json");
```

*See more examples in the [test cases](/tests/components/filesystem/normalized_path_test.cc).*

### Failure Report

Provides error reporting with stacktrace.

#### Location

> copper/components/failures/report.hpp

#### API

```cpp
void fail(boost::beast::error_code ec, char const* what)
```

#### Usage

```cpp
#include <copper/components/failures/report.hpp>

boost::system::error_code ec;

failures::report(ec, "All OK");
```

*See more examples in the [test cases](/tests/components/failures/report_test.cc).*

