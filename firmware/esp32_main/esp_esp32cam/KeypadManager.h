/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý keypad và mật khẩu
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef KEYPAD_MANAGER_H
#define KEYPAD_MANAGER_H

#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Wire.h>
#include <EEPROM.h>
#include "config.h"

class KeypadManager {
private:
    Keypad_I2C* keypad;
    char password[5];
    int passwordLength;
    int wrongPasswordCount;
    int completePasswordAttempts;
    
    void loadPasswordFromEEPROM();
    void savePasswordToEEPROM();
    
public:
    KeypadManager();
    ~KeypadManager();
    
    void init();
    char getKey();
    bool checkPassword(const char* enteredPassword);
    bool changePassword(const char* newPassword);
    bool verifyCurrentPassword();
    int getWrongPasswordCount() const;
    void resetWrongPasswordCount();
    int getCompletePasswordAttempts() const;
    void incrementCompletePasswordAttempts();
    void resetCompletePasswordAttempts();
};

#endif // KEYPAD_MANAGER_H 