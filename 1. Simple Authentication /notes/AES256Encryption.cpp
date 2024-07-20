#include <iostream>
#include <fstream>
#include <cstring>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>

// Function to print errors from OpenSSL's error queue
void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

// Function to encrypt the plaintext
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();

    int len;
    int ciphertext_len;

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

// Function to decrypt the ciphertext
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();

    int len;
    int plaintext_len;

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) handleErrors();
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}


std::string base64_encode(const std::string &input) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // Do not use newlines to flush buffer
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);

    std::string encodedData(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);
    return encodedData;
}

// Function to save the encryption key to a file
void saveKeyToFile(unsigned char *key, const char *filename) {
    std::ofstream file{filename, std::ios::out | std::ios::binary};
    if (file.is_open()) {
        file.write(reinterpret_cast<char *>(key), EVP_CIPHER_key_length(EVP_aes_256_cbc()));
        file.close();
        std::cout << "Key saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file " << filename << " for writing." << std::endl;
    }
}

// Function to load the encryption key from a file
bool loadKeyFromFile(unsigned char *key, const char *filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char *>(key), EVP_CIPHER_key_length(EVP_aes_256_cbc()));
        file.close();
        std::cout << "Key loaded from " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Unable to open file " << filename << " for reading." << std::endl;
        return false;
    }
}

int main() {
    unsigned char *plaintext = (unsigned char *)"Some sensitive information here ! ";
    unsigned char key[32]; // 256-bit key
    unsigned char iv[16];  // 128-bit IV
    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];


    // Generate a random key and IV
    // if (!RAND_bytes(key, sizeof(key))) handleErrors();
    // if (!RAND_bytes(iv, sizeof(iv))) handleErrors();
    // Save the encryption key to a file
    // saveKeyToFile(key, "key.txt");

    // Load the encryption key from the file for decryption
    if (!loadKeyFromFile(key, "key.txt")) {
        std::cerr << "Failed to load key from file." << std::endl;
        return 1;
    }

    // Encrypt the plaintext
    int ciphertext_len = encrypt(plaintext, strlen((char *)plaintext), key, iv, ciphertext);

    // Print the ciphertext in hexadecimal format
    std::cout << "Ciphertext is:\n";
    for (int i = 0; i < ciphertext_len; i++) {
        printf("%02x", ciphertext[i]);
    }
    std::cout << std::endl;

    // Decrypt the ciphertext
    int decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);

    // Add a NULL terminator to the decrypted text
    decryptedtext[decryptedtext_len] = '\0';

    // Print the decrypted text
    std::cout << "Decrypted text is:\n";
    std::cout << decryptedtext << std::endl;


    return 0;
}
