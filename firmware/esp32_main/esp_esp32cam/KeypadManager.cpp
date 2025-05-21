/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý keypad và mật khẩu - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "KeypadManager.h"
#include <Arduino.h>

// Định nghĩa ma trận phím
const byte ROWS = KEYPAD_ROWS;
const byte COLS = KEYPAD_COLS;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

KeypadManager::KeypadManager() : 
    passwordLength(4), 
    wrongPasswordCount(0),
    completePasswordAttempts(0) {
    
    // Sao chép mật khẩu mặc định
    strncpy(password, DEFAULT_PASSWORD, 5);
    
    // Tạo đối tượng keypad
    keypad = new Keypad_I2C(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);
}

KeypadManager::~KeypadManager() {
    delete keypad;
}

void KeypadManager::init() {
    Wire.begin();
    keypad->begin();
    
    // Đọc mật khẩu từ EEPROM
    loadPasswordFromEEPROM();
    
    Serial.println(F("Đã khởi tạo module Keypad"));
}

char KeypadManager::getKey() {
    return keypad->getKey();
}

bool KeypadManager::checkPassword(const char* enteredPassword) {
    for (int i = 0; i < passwordLength; i++) {
        if (enteredPassword[i] != password[i]) {
            wrongPasswordCount++;
            return false;
        }
    }
    wrongPasswordCount = 0;
    return true;
}

bool KeypadManager::changePassword(const char* newPassword) {
    if (strlen(newPassword) == passwordLength) {
        strncpy(password, newPassword, passwordLength);
        password[passwordLength] = '\0';
        savePasswordToEEPROM();
        return true;
    }
    return false;
}

bool KeypadManager::verifyCurrentPassword() {
    char entered_password[5] = {'\0', '\0', '\0', '\0', '\0'};
    
    // Nhập mật khẩu cũ
    for (int i = 0; i < passwordLength; i++) {
        char key = NO_KEY;
        // Đợi cho đến khi có phím nhấn hợp lệ
        while (key == NO_KEY) {
            key = keypad->getKey();
            delay(50); // Thêm delay để tránh đọc quá nhanh và debounce
            yield(); // Cho phép ESP32 xử lý các tác vụ khác
        }
        
        // Thêm debounce để tránh đọc nhiều lần khi chỉ nhấn 1 phím
        delay(150);
        
        // Xóa bỏ bất kỳ phím nào còn trong buffer
        while (keypad->getKey() != NO_KEY) {
            delay(5);
        }
        
        entered_password[i] = key;
        Serial.print('*');
    }
    
    // Kiểm tra mật khẩu cũ
    return checkPassword(entered_password);
}

int KeypadManager::getWrongPasswordCount() const {
    return wrongPasswordCount;
}

void KeypadManager::resetWrongPasswordCount() {
    wrongPasswordCount = 0;
}

int KeypadManager::getCompletePasswordAttempts() const {
    return completePasswordAttempts;
}

void KeypadManager::incrementCompletePasswordAttempts() {
    completePasswordAttempts++;
}

void KeypadManager::resetCompletePasswordAttempts() {
    completePasswordAttempts = 0;
}

// Private methods
void KeypadManager::loadPasswordFromEEPROM() {
    for (int i = 0; i < passwordLength; i++) {
        password[i] = EEPROM.read(i);
        
        // Nếu đọc được ký tự không hợp lệ, sử dụng mật khẩu mặc định
        if (password[i] < '0' || password[i] > '9') {
            strncpy(password, DEFAULT_PASSWORD, 5);
            break;
        }
    }
    password[passwordLength] = '\0';
}

void KeypadManager::savePasswordToEEPROM() {
    for (int i = 0; i < passwordLength; i++) {
        EEPROM.write(i, password[i]);
    }
    EEPROM.commit();
} 