/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý bảo mật và mã hóa - Implementation
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "SecurityManager.h"
#include <esp_random.h>

// Salt cố định cho hệ thống (trong thực tế nên lưu trong secure storage)
const char* SecurityManager::SALT = "AutoDoorSystem2025";

SecurityManager::SecurityManager() {
    // Constructor
}

String SecurityManager::hashPassword(const String& password) {
    mbedtls_sha256_context ctx;
    unsigned char hash[32];
    
    // Khởi tạo context
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0); // 0 = SHA256, 1 = SHA224
    
    // Thêm salt trước
    mbedtls_sha256_update(&ctx, (const unsigned char*)SALT, strlen(SALT));
    
    // Thêm password
    mbedtls_sha256_update(&ctx, (const unsigned char*)password.c_str(), password.length());
    
    // Hoàn thành hash
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);
    
    // Chuyển đổi sang hex string
    String hashedPassword = "";
    for (int i = 0; i < 32; i++) {
        if (hash[i] < 16) hashedPassword += "0";
        hashedPassword += String(hash[i], HEX);
    }
    
    return hashedPassword;
}

bool SecurityManager::verifyPassword(const String& inputPassword, const String& hashedPassword) {
    String inputHash = hashPassword(inputPassword);
    return inputHash.equals(hashedPassword);
}

String SecurityManager::generateSalt() {
    String salt = "";
    for (int i = 0; i < 16; i++) {
        salt += String(esp_random() % 256, HEX);
    }
    return salt;
}

String SecurityManager::encryptData(const String& data, const String& key) {
    // Simple XOR encryption (cho demo, trong thực tế nên dùng AES)
    String encrypted = "";
    for (size_t i = 0; i < data.length(); i++) {
        char encryptedChar = data[i] ^ key[i % key.length()];
        encrypted += String(encryptedChar, HEX);
        if (encrypted.length() % 2 == 1) encrypted = "0" + encrypted;
    }
    return encrypted;
}

String SecurityManager::decryptData(const String& encryptedData, const String& key) {
    // Simple XOR decryption
    String decrypted = "";
    for (size_t i = 0; i < encryptedData.length(); i += 2) {
        String hexByte = encryptedData.substring(i, i + 2);
        char encryptedChar = (char)strtol(hexByte.c_str(), NULL, 16);
        char decryptedChar = encryptedChar ^ key[(i/2) % key.length()];
        decrypted += decryptedChar;
    }
    return decrypted;
}

String SecurityManager::generateSessionToken() {
    String token = "";
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    for (int i = 0; i < 32; i++) {
        token += charset[esp_random() % (sizeof(charset) - 1)];
    }
    
    return token;
}

bool SecurityManager::isPasswordStrong(const String& password) {
    if (password.length() < 4) return false;
    
    bool hasDigit = false;
    bool hasLetter = false;
    
    for (size_t i = 0; i < password.length(); i++) {
        if (isdigit(password[i])) hasDigit = true;
        if (isalpha(password[i])) hasLetter = true;
    }
    
    return hasDigit; // Cho keypad chỉ cần có số
}

void SecurityManager::secureMemoryWipe(char* data, size_t length) {
    // Ghi đè memory với random data
    for (size_t i = 0; i < length; i++) {
        data[i] = (char)(esp_random() % 256);
    }
    
    // Sau đó zero out
    memset(data, 0, length);
}