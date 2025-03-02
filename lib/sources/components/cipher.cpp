//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <bcrypt/BCrypt.hpp>
#include <copper/components/base64.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/report.hpp>
#include <string>
#include <vector>

#ifndef CIPHER_KEY_LENGTH
#define CIPHER_KEY_LENGTH 32
#endif

#ifndef CIPHER_IV_LENGTH
#define CIPHER_IV_LENGTH 16
#endif

#ifndef CIPHER_DIGEST_LENGTH
#define CIPHER_DIGEST_LENGTH 64
#endif

namespace copper::components {

std::string cipher_generate_sha_256() {
  std::string _bytes(CIPHER_KEY_LENGTH, '\0');

  // LCOV_EXCL_START
  if (RAND_bytes(reinterpret_cast<unsigned char *>(&_bytes[0]), CIPHER_KEY_LENGTH) != 1) {
    report_for_openssl();
  }
  // LCOV_EXCL_STOP

  return base64_encode(_bytes);
}

// ReSharper disable once CppDFAConstantFunctionResult
std::string cipher_hmac(const std::string &input, const std::string_view &app_key) {
  std::string _output;

  // LCOV_EXCL_START
  EVP_PKEY *_public_key = EVP_PKEY_new_mac_key(
      EVP_PKEY_HMAC, nullptr,
      reinterpret_cast<const unsigned char *>(app_key.data()),
      CIPHER_KEY_LENGTH);

  if (!_public_key) {
    report_for_openssl();
  }

  EVP_MD_CTX *_openssl_context = EVP_MD_CTX_new();
  if (!_openssl_context) {
    EVP_PKEY_free(_public_key);
    report_for_openssl();
  }

  std::size_t _length;
  std::string _digest(CIPHER_DIGEST_LENGTH, '\0');

  if (EVP_DigestSignInit(_openssl_context, nullptr, EVP_sha256(), nullptr,
                         _public_key) != 1) {
    EVP_MD_CTX_free(_openssl_context);
    EVP_PKEY_free(_public_key);
    report_for_openssl();
  }

  if (EVP_DigestSignUpdate(_openssl_context, input.c_str(), input.size()) !=
      1) {
    EVP_MD_CTX_free(_openssl_context);
    EVP_PKEY_free(_public_key);
    report_for_openssl();
  }

  if (EVP_DigestSignFinal(_openssl_context, nullptr, &_length) != 1) {
    EVP_MD_CTX_free(_openssl_context);
    EVP_PKEY_free(_public_key);
    report_for_openssl();
  }

  if (EVP_DigestSignFinal(_openssl_context, reinterpret_cast<unsigned char *>(&_digest[0]), &_length) != 1) {
    EVP_MD_CTX_free(_openssl_context);
    EVP_PKEY_free(_public_key);
    report_for_openssl();
  }

  EVP_MD_CTX_free(_openssl_context);
  EVP_PKEY_free(_public_key);
  // LCOV_EXCL_STOP

  _digest.resize(_length);

  return _digest;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

std::pair<std::string, std::string> cipher_generate_aes_key_iv() {
  std::pair<std::string, std::string> _output;

  // LCOV_EXCL_START
  containers::vector_of<unsigned char> _key(CIPHER_KEY_LENGTH);

  if (RAND_bytes(_key.data(), static_cast<int>(_key.size())) != 1) {
    report_for_openssl();
  }

  _output.first.assign(_key.begin(), _key.end());

  containers::vector_of<unsigned char> _iv(CIPHER_IV_LENGTH);
  if (RAND_bytes(_iv.data(), static_cast<int>(_iv.size())) != 1) {
    report_for_openssl();
  }
  // LCOV_EXCL_STOP

  _output.second.assign(_iv.begin(), _iv.end());

  return _output;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
std::string cipher_encrypt(const std::string &input, const std::string &key,
                           const std::string &iv) {
  EVP_CIPHER_CTX *_openssl_context = EVP_CIPHER_CTX_new();
  if (!_openssl_context) {
    report_for_openssl();
  }

  if (EVP_EncryptInit_ex(_openssl_context, EVP_aes_256_gcm(), nullptr,
                         reinterpret_cast<const unsigned char *>(key.c_str()),
                         reinterpret_cast<const unsigned char *>(iv.c_str())) !=
      1) {
    EVP_CIPHER_CTX_free(_openssl_context);
    report_for_openssl();
  }

  std::string _output(input.size() + EVP_MAX_BLOCK_LENGTH, '\0');
  int _out_length = 0;

  if (EVP_EncryptUpdate(_openssl_context,
                        reinterpret_cast<unsigned char *>(&_output[0]),
                        &_out_length,
                        reinterpret_cast<const unsigned char *>(input.c_str()),
                        static_cast<int>(input.size())) != 1) {
    EVP_CIPHER_CTX_free(_openssl_context);
    report_for_openssl();
  }

  int _final_length = 0;
  if (EVP_EncryptFinal_ex(
          _openssl_context,
          reinterpret_cast<unsigned char *>(&_output[0]) + _out_length,
          &_final_length) != 1) {
    EVP_CIPHER_CTX_free(_openssl_context);
    report_for_openssl();
  }


  _output.resize(_out_length + _final_length);

  std::string _tag(CIPHER_IV_LENGTH, '\0');

  if (RAND_bytes(reinterpret_cast<unsigned char *>(&_tag[0]), CIPHER_IV_LENGTH) != 1) {
    report_for_openssl();
  }

  if (EVP_CIPHER_CTX_ctrl(_openssl_context, EVP_CTRL_GCM_GET_TAG, CIPHER_IV_LENGTH, &_tag[0]) != 1) {
    EVP_CIPHER_CTX_free(_openssl_context);
    report_for_openssl();
  }

  EVP_CIPHER_CTX_free(_openssl_context);

  return _output + _tag;
}
// LCOV_EXCL_STOP

std::string cipher_decrypt(const std::string &input, const std::string &key,
                           const std::string &iv) {
  std::string _ciphertext = input.substr(0, input.size() - CIPHER_IV_LENGTH);
  std::string _tag = input.substr(input.size() - CIPHER_IV_LENGTH);

  // LCOV_EXCL_START
  EVP_CIPHER_CTX *_openssl_context = EVP_CIPHER_CTX_new();
  if (!_openssl_context) {
    report_for_openssl();
  }

  if (EVP_DecryptInit_ex(_openssl_context, EVP_aes_256_gcm(), nullptr,
                         reinterpret_cast<const unsigned char *>(key.c_str()),
                         reinterpret_cast<const unsigned char *>(iv.c_str())) !=
      1) {
    EVP_CIPHER_CTX_free(_openssl_context);
    report_for_openssl();
  }

  std::string _output(_ciphertext.size(), '\0');
  int _out_length = 0;

  if (EVP_DecryptUpdate(_openssl_context,
                        reinterpret_cast<unsigned char *>(&_output[0]),
                        &_out_length,
                        reinterpret_cast<const unsigned char *>(_ciphertext.c_str()),
                        static_cast<int>(_ciphertext.size())) != 1) {
    EVP_CIPHER_CTX_free(_openssl_context);
    report_for_openssl();
  }

  if (EVP_CIPHER_CTX_ctrl(_openssl_context, EVP_CTRL_GCM_SET_TAG, 16, _tag.data()) != 1) {
    EVP_CIPHER_CTX_free(_openssl_context);
    report_for_openssl();
  }


  int _final_length = 0;
  if (EVP_DecryptFinal_ex(
          _openssl_context,
          reinterpret_cast<unsigned char *>(&_output[0]) + _out_length,
          &_final_length) != 1) {
    EVP_CIPHER_CTX_free(_openssl_context);
    report_for_openssl();
  }

  _output.resize(_out_length + _final_length);

  EVP_CIPHER_CTX_free(_openssl_context);
  // LCOV_EXCL_STOP

  return _output;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

bool cipher_password_validator(const std::string &input,
                               const std::string &hash) {
  return BCrypt::validatePassword(input, hash);
}

std::string cipher_password_hash(const std::string &input, const int workload) {
  return BCrypt::generateHash(input, workload);
}

}  // namespace copper::components