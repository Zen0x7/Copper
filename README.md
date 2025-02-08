# Copper

[![codecov](https://codecov.io/gh/Zen0x7/Copper/branch/master/graph/badge.svg?token=XENML1H9VV)](https://codecov.io/gh/Zen0x7/Copper)

## Components

### Base64

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

### Base64url

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

### Cipher

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

### Validator

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

### Random

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

### Expression

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


