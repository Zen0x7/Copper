#include <bcrypt/BCrypt.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/report.hpp>

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
  unsigned char bytes[CIPHER_KEY_LENGTH];

  // LCOV_EXCL_START
  if (RAND_bytes(bytes, CIPHER_KEY_LENGTH) != 1) {
    report_for_openssl();
  }
  // LCOV_EXCL_STOP

  std::string result(reinterpret_cast<const char *>(bytes), CIPHER_KEY_LENGTH);

  return base64_encode(result);
}

std::string cipher_hmac(const std::string &input, const std::string &app_key) {
  std::string output;

  // LCOV_EXCL_START
  EVP_PKEY *public_key = EVP_PKEY_new_mac_key(
      EVP_PKEY_HMAC, nullptr,
      reinterpret_cast<const unsigned char *>(app_key.data()),
      CIPHER_KEY_LENGTH);

  if (!public_key) {
    report_for_openssl();
  }

  EVP_MD_CTX *openssl_context = EVP_MD_CTX_new();
  if (!openssl_context) {
    EVP_PKEY_free(public_key);
    report_for_openssl();
  }

  std::size_t length;
  unsigned char digest[CIPHER_DIGEST_LENGTH];

  if (EVP_DigestSignInit(openssl_context, nullptr, EVP_sha256(), nullptr,
                         public_key) != 1) {
    EVP_MD_CTX_free(openssl_context);
    EVP_PKEY_free(public_key);
    report_for_openssl();
  }

  if (EVP_DigestSignUpdate(openssl_context, input.c_str(), input.size()) != 1) {
    EVP_MD_CTX_free(openssl_context);
    EVP_PKEY_free(public_key);
    report_for_openssl();
  }

  if (EVP_DigestSignFinal(openssl_context, nullptr, &length) != 1) {
    EVP_MD_CTX_free(openssl_context);
    EVP_PKEY_free(public_key);
    report_for_openssl();
  }

  if (EVP_DigestSignFinal(openssl_context, digest, &length) != 1) {
    EVP_MD_CTX_free(openssl_context);
    EVP_PKEY_free(public_key);
    report_for_openssl();
  }

  EVP_MD_CTX_free(openssl_context);
  EVP_PKEY_free(public_key);
  // LCOV_EXCL_STOP

  output.assign(reinterpret_cast<char *>(digest), length);

  return output;
// LCOV_EXCL_START
}
// LCOV_EXCL_STOP

std::pair<std::string, std::string> cipher_generate_aes_key_iv() {
  std::pair<std::string, std::string> output;

  // LCOV_EXCL_START
  std::vector<unsigned char> key(CIPHER_KEY_LENGTH);

  if (RAND_bytes(key.data(), (int)key.size()) != 1) {
    report_for_openssl();
  }

  output.first.assign(key.begin(), key.end());

  std::vector<unsigned char> iv(CIPHER_IV_LENGTH);
  if (RAND_bytes(iv.data(), (int)iv.size()) != 1) {
    report_for_openssl();
  }
  // LCOV_EXCL_STOP

  output.second.assign(iv.begin(), iv.end());

  return output;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

std::string cipher_encrypt(const std::string &input, const std::string &key,
                           const std::string &iv) {
  std::string output;

  // LCOV_EXCL_START
  EVP_CIPHER_CTX *openssl_context = EVP_CIPHER_CTX_new();
  if (!openssl_context) {
    report_for_openssl();
  }

  if (EVP_EncryptInit_ex(openssl_context, EVP_aes_256_cbc(), nullptr,
                         reinterpret_cast<const unsigned char *>(key.c_str()),
                         reinterpret_cast<const unsigned char *>(iv.c_str())) !=
      1) {
    EVP_CIPHER_CTX_free(openssl_context);
    report_for_openssl();
  }

  int length;
  int output_length;

  auto *output_buffer =
      new unsigned char[input.size() +
                        EVP_CIPHER_block_size(EVP_aes_256_cbc())];

  if (EVP_EncryptUpdate(openssl_context, output_buffer, &length,
                        reinterpret_cast<const unsigned char *>(input.c_str()),
                        (int)input.size()) != 1) {
    EVP_CIPHER_CTX_free(openssl_context);
    report_for_openssl();
  }

  output_length = length;

  if (EVP_EncryptFinal_ex(openssl_context, output_buffer + length, &length) !=
      1) {
    EVP_CIPHER_CTX_free(openssl_context);
    report_for_openssl();
  }
  // LCOV_EXCL_STOP

  output_length += length;

  output.assign(reinterpret_cast<char *>(output_buffer), output_length);

  delete[] output_buffer;
  EVP_CIPHER_CTX_free(openssl_context);

  return output;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

std::string cipher_decrypt(const std::string &input, const std::string &key,
                           const std::string &iv) {
  std::string output;

  // LCOV_EXCL_START

  EVP_CIPHER_CTX *openssl_context = EVP_CIPHER_CTX_new();
  if (!openssl_context) {
    report_for_openssl();
  }

  if (EVP_DecryptInit_ex(openssl_context, EVP_aes_256_cbc(), nullptr,
                         reinterpret_cast<const unsigned char *>(key.c_str()),
                         reinterpret_cast<const unsigned char *>(iv.c_str())) !=
      1) {
    EVP_CIPHER_CTX_free(openssl_context);
    report_for_openssl();
  }

  int length;
  int input_length;
  auto *input_buffer = new unsigned char[input.size() + EVP_CIPHER_block_size(
                                                            EVP_aes_256_cbc())];

  if (EVP_DecryptUpdate(openssl_context, input_buffer, &length,
                        reinterpret_cast<const unsigned char *>(input.c_str()),
                        (int)input.size()) != 1) {
    EVP_CIPHER_CTX_free(openssl_context);
    report_for_openssl();
  }

  input_length = length;

  if (EVP_DecryptFinal_ex(openssl_context, input_buffer + length, &length) !=
      1) {
    EVP_CIPHER_CTX_free(openssl_context);
    report_for_openssl();
  }

  input_length += length;

  output.assign(reinterpret_cast<char *>(input_buffer), input_length);

  delete[] input_buffer;
  EVP_CIPHER_CTX_free(openssl_context);
  // LCOV_EXCL_STOP

  return output;
}

bool cipher_password_validator(const std::string &input,
                               const std::string &hash) {
  return BCrypt::validatePassword(input, hash);
}

std::string cipher_password_hash(const std::string &input, int workload) {
  return BCrypt::generateHash(input, workload);
}

}  // namespace copper::components