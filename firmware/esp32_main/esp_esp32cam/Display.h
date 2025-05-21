/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý hiển thị LCD
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>

class Display {
private:
    LiquidCrystal_I2C* lcd;
    
public:
    Display(uint8_t address, uint8_t cols, uint8_t rows);
    ~Display();
    
    void init();
    void clear();
    void showWelcomeScreen();
    void showPasswordInput(int passwordLength);
    void showAuthenticationSuccess();
    void showAuthenticationFailed(const char* method);
    void showAddCardMode();
    void showDeleteCardMode();
    void showCardAdded();
    void showCardDeleted();
    void showCardExists();
    void showCardNotFound();
    void showMemoryFull();
    void showPasswordChangePrompt();
    void showNewPasswordPrompt();
    void showPasswordChanged();
    void showFingerprintEnrollPrompt();
    void showFingerprintEnrollID(uint8_t id);
    void showFingerprintPlacePrompt();
    void showFingerprintRemovePrompt();
    void showFingerprintPlaceAgainPrompt();
    void showFingerprintMismatch();
    void showFingerprintStored();
    void showDeleteAllFingerprints();
    void showWiFiConnecting();
    void showWiFiConnected();
    void showWiFiConnectionFailed();
    void showDoorOpened();
    void showDoorClosed();
    void showExitCardMode();
    void showMessage(const char* line1, const char* line2 = "");
};

#endif // DISPLAY_H 