#include <gtest/gtest.h>

#include <copper/components/cipher.hpp>

TEST(Components_Cipher, Generate_HMAC_Using_Existing_Key) {
    using namespace copper::components;
    const std::string app_key = base64_decode("sRgrihyQrBq59ltyxPW/azh9BzVN+vuA/K48BS7nJaw=");
    const std::string input = "hello world";
    const std::string input_encrypted = base64_decode("RoD37/zcdnzO0WvPOp+izO6IfL3X6op9jPEMS4MMF4U=");
    const std::string encrypted = base64_encode(cipher_hmac(input, app_key));
    ASSERT_EQ(base64_encode(input_encrypted), encrypted);
}

TEST(Components_Cipher, Generate_HMAC_Using_Random_Key) {
    using namespace copper::components;
    auto app_key = cipher_generate_sha_256();
    const std::string input = "hello world";
    const std::string other = "h3ll0 w0rld";
    const std::string result_one = base64_encode(cipher_hmac(input, app_key));
    const std::string result_two = base64_encode(cipher_hmac(input, app_key));
    const std::string result_three = base64_encode(cipher_hmac(other, app_key));
    const std::string result_four = base64_encode(cipher_hmac(other, app_key));
    ASSERT_EQ(result_one, result_two);
    ASSERT_EQ(result_three, result_four);
    ASSERT_NE(result_one, result_three);
    ASSERT_NE(result_two, result_four);
}

TEST(Components_Cipher, Encrypt_And_Decrypt_Using_Existing_Key) {
    using namespace copper::components;
    const std::string secret_key = base64_decode("TsBen/ynU6jtihMXGRT3ZAtzTSiQGzyk9dkgY03WAn0=");
    const std::string secret_iv = base64_decode("nt27vWC02To2ZzqXKaP7yw==");
    const std::string expected_encrypted = "2h8OT3N+7G8UEEV+RixbsQ==";
    const std::string input = "hello world";

    const std::string encrypted = base64_encode(cipher_encrypt(input, secret_key, secret_iv));
    ASSERT_EQ(expected_encrypted, encrypted);

    const std::string decrypted = cipher_decrypt(base64_decode(encrypted), secret_key, secret_iv);
    ASSERT_EQ(decrypted, input);
}

TEST(Components_Cipher, Encrypt_And_Decrypt_Using_Random_Key) {
    using namespace copper::components;
    auto [secret_key, secret_iv] = cipher_generate_aes_key_iv();
    const std::string input = "hello world";

    const std::string encrypted = base64_encode(cipher_encrypt(input, secret_key, secret_iv));
    const std::string decrypted = cipher_decrypt(base64_decode(encrypted), secret_key, secret_iv);

    ASSERT_EQ(decrypted, input);
}

