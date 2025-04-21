// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <gtest/gtest.h>

#include <copper/components/base64.hpp>
#include <copper/components/cipher.hpp>

using namespace copper::components;

class CipherTestFixture : public ::testing::Test {
 protected:
  const std::string input = "hello world";

  // Para pruebas con claves fijas
  const std::string base64_key = "sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=";
  const std::string base64_key_encryption =
      "TsBen/ynU6jtihMXGRT3ZAtzTSiQGzyk9dkgY03WAn0=";
  const std::string base64_iv_encryption = "nt27vWC02To2ZzqXKaP7yw==";

  std::string key;
  std::string key_enc;
  std::string iv_enc;

  void SetUp() override {
    key = base64_decode(base64_key);
    key_enc = base64_decode(base64_key_encryption);
    iv_enc = base64_decode(base64_iv_encryption);
  }
};

TEST_F(CipherTestFixture, GenerateHMACUsingExistingKey) {
  const std::string expected = "RoD37/zcdnzO0WvPOp+izO6IfL3X6op9jPEMS4MMF4U=";
  const std::string result = base64_encode(cipher_hmac(input, key));

  ASSERT_EQ(result, expected);
}

TEST_F(CipherTestFixture, GenerateHMACUsingRandomKey) {
  const auto random_key = cipher_generate_sha_256();
  const std::string other = "h3ll0 w0rld";

  const std::string one = base64_encode(cipher_hmac(input, random_key));
  const std::string two = base64_encode(cipher_hmac(input, random_key));
  const std::string three = base64_encode(cipher_hmac(other, random_key));

  ASSERT_EQ(one, two);
  ASSERT_NE(one, three);
}

TEST_F(CipherTestFixture, EncryptAndDecryptUsingExistingKey) {
  const std::string expected = "MezGO7smbArUgjeERkQbAjxep8J1+G4YFwjO";

  const std::string encrypted =
      base64_encode(cipher_encrypt(input, key_enc, iv_enc));
  const std::string decrypted =
      cipher_decrypt(base64_decode(encrypted), key_enc, iv_enc);

  ASSERT_EQ(encrypted, expected);
  ASSERT_EQ(decrypted, input);
}

TEST_F(CipherTestFixture, EncryptAndDecryptUsingRandomKey) {
  auto [key_rand, iv_rand] = cipher_generate_aes_key_iv();

  const std::string encrypted =
      base64_encode(cipher_encrypt(input, key_rand, iv_rand));
  const std::string decrypted =
      cipher_decrypt(base64_decode(encrypted), key_rand, iv_rand);

  ASSERT_EQ(decrypted, input);
}

TEST_F(CipherTestFixture, GenerateBCryptHash) {
  const auto hash = cipher_password_hash("password", 6);

  ASSERT_TRUE(cipher_password_validator("password", hash));
  ASSERT_FALSE(cipher_password_validator("other_password", hash));
}
