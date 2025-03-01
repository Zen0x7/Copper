//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include <copper/components/base64.hpp>
#include <copper/components/cipher.hpp>

using namespace copper::components;

TEST(Components_Cipher, Generate_HMAC_Using_Existing_Key) {
  const std::string _app_key =
      base64_decode("sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=");
  const std::string _input = "hello world";
  const std::string _input_encrypted =
      base64_decode("RoD37/zcdnzO0WvPOp+izO6IfL3X6op9jPEMS4MMF4U=");
  const std::string _encrypted = base64_encode(cipher_hmac(_input, _app_key));
  ASSERT_EQ(base64_encode(_input_encrypted), _encrypted);
}

TEST(Components_Cipher, Generate_HMAC_Using_Random_Key) {
  const auto _app_key = cipher_generate_sha_256();
  const std::string _input = "hello world";
  const std::string _other = "h3ll0 w0rld";
  const std::string _result_one = base64_encode(cipher_hmac(_input, _app_key));
  const std::string _result_two = base64_encode(cipher_hmac(_input, _app_key));
  const std::string _result_three =
      base64_encode(cipher_hmac(_other, _app_key));
  const std::string _result_four = base64_encode(cipher_hmac(_other, _app_key));
  ASSERT_EQ(_result_one, _result_two);
  ASSERT_EQ(_result_three, _result_four);
  ASSERT_NE(_result_one, _result_three);
  ASSERT_NE(_result_two, _result_four);
}

TEST(Components_Cipher, Encrypt_And_Decrypt_Using_Existing_Key) {
  const std::string _secret_key =
      base64_decode("TsBen/ynU6jtihMXGRT3ZAtzTSiQGzyk9dkgY03WAn0=");
  const std::string _secret_iv = base64_decode("nt27vWC02To2ZzqXKaP7yw==");
  const std::string _expected_encrypted = "2h8OT3N+7G8UEEV+RixbsQ==";
  const std::string _input = "hello world";

  const std::string _encrypted =
      base64_encode(cipher_encrypt(_input, _secret_key, _secret_iv));
  ASSERT_EQ(_expected_encrypted, _encrypted);

  const std::string _decrypted =
      cipher_decrypt(base64_decode(_encrypted), _secret_key, _secret_iv);
  ASSERT_EQ(_decrypted, _input);
}

TEST(Components_Cipher, Encrypt_And_Decrypt_Using_Random_Key) {
  auto [_secret_key, _secret_iv] = cipher_generate_aes_key_iv();
  const std::string _input = "hello world";

  const std::string _encrypted =
      base64_encode(cipher_encrypt(_input, _secret_key, _secret_iv));
  const std::string _decrypted =
      cipher_decrypt(base64_decode(_encrypted), _secret_key, _secret_iv);

  ASSERT_EQ(_decrypted, _input);
}

TEST(Components_Cipher, Generate_BCrypt_Hash) {
  const auto _hash = cipher_password_hash("password", 6);

  ASSERT_TRUE(cipher_password_validator("password", _hash));
  ASSERT_FALSE(cipher_password_validator("other_password", _hash));
}
