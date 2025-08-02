/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý bảo mật và mã hóa
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <Arduino.h>
#include <mbedtls/sha256.h>

class SecurityManager {
private:
    static const char* SALT; // Salt cho hash
    
public:
    SecurityManager();
    
    // Mã hóa mật khẩu bằng SHA256 + Salt
    static String hashPassword(const String& password);
    
    // Kiểm tra mật khẩu
    static bool verifyPassword(const String& inputPassword, const String& hashedPassword);
    
    // Tạo salt ngẫu nhiên
    static String generateSalt();
    
    // Mã hóa dữ liệu nhạy cảm
    static String encryptData(const String& data, const String& key);
    static String decryptData(const String& encryptedData, const String& key);
    
    // Tạo session token
    static String generateSessionToken();
    
    // Kiểm tra độ mạnh của mật khẩu
    static bool isPasswordStrong(const String& password);
    
    // Làm sạch dữ liệu nhạy cảm từ memory
    static void secureMemoryWipe(char* data, size_t length);
};

#endif // SECURITY_MANAGER_H