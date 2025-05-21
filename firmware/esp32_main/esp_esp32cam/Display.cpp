/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý hiển thị LCD - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "Display.h"
#include <Arduino.h>

Display::Display(uint8_t address, uint8_t cols, uint8_t rows) {
    lcd = new LiquidCrystal_I2C(address, cols, rows);
}

Display::~Display() {
    delete lcd;
}

void Display::init() {
    lcd->init();
    lcd->backlight();
    showWelcomeScreen();
}

void Display::clear() {
    lcd->clear();
}

void Display::showWelcomeScreen() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("NHAP PASS,RFID");
    lcd->setCursor(2, 1);
    lcd->print("HOAC VAN TAY");
}

void Display::showPasswordInput(int passwordLength) {
    clear();
    lcd->setCursor(passwordLength + 4, 0);
    lcd->print("*");
}

void Display::showAuthenticationSuccess() {
    clear();
    lcd->setCursor(3, 0);
    lcd->print("OPEN DOOR");
    delay(1000);
}

void Display::showAuthenticationFailed(const char* method) {
    clear();
    if (strcmp(method, "password") == 0) {
        lcd->setCursor(2, 0);
        lcd->print("MAT KHAU SAI");
    } else if (strcmp(method, "fingerprint") == 0) {
        lcd->setCursor(2, 0);
        lcd->print("VAN TAY KHONG");
        lcd->setCursor(6, 1);
        lcd->print("KHOP");
    } else if (strcmp(method, "rfid") == 0) {
        lcd->setCursor(0, 0);
        lcd->print("THE KHONG HOP LE");
        lcd->setCursor(3, 1);
        lcd->print("THU LAI!");
    }
    delay(1000);
    showWelcomeScreen();
}

void Display::showAddCardMode() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("THEM THE MOI");
    lcd->setCursor(2, 1);
    lcd->print("QUET THE VAO");
}

void Display::showDeleteCardMode() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("XOA THE");
    lcd->setCursor(0, 1);
    lcd->print("QUET THE CAN XOA");
}

void Display::showCardAdded() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("THEM THE");
    lcd->setCursor(3, 1);
    lcd->print("THANH CONG");
    delay(2000);
    showWelcomeScreen();
}

void Display::showCardDeleted() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("XOA THE");
    lcd->setCursor(3, 1);
    lcd->print("THANH CONG");
    delay(2000);
    showWelcomeScreen();
}

void Display::showCardExists() {
    clear();
    lcd->setCursor(1, 0);
    lcd->print("THE DA TON TAI");
    lcd->setCursor(2, 1);
    lcd->print("TRONG BO NHO");
    delay(2000);
    showAddCardMode();
}

void Display::showCardNotFound() {
    clear();
    lcd->setCursor(1, 0);
    lcd->print("THE KHONG TON");
    lcd->setCursor(2, 1);
    lcd->print("TAI TRONG BO NHO");
    delay(2000);
    showDeleteCardMode();
}

void Display::showMemoryFull() {
    clear();
    lcd->setCursor(1, 0);
    lcd->print("BO NHO DA DAY");
    lcd->setCursor(2, 1);
    lcd->print("KHONG THE THEM");
    delay(2000);
    showAddCardMode();
}

void Display::showPasswordChangePrompt() {
    clear();
    lcd->setCursor(0, 0);
    lcd->print("CURRENT PASSWORD:");
}

void Display::showNewPasswordPrompt() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("NEW PASSWORD:");
}

void Display::showPasswordChanged() {
    clear();
    lcd->setCursor(3, 0);
    lcd->print("DA THAY DOI");
    lcd->setCursor(4, 1);
    lcd->print("MAT KHAU");
    delay(1000);
    showWelcomeScreen();
}

void Display::showFingerprintEnrollPrompt() {
    clear();
    lcd->setCursor(0, 0);
    lcd->print("NHAP ID VAN TAY");
    lcd->setCursor(6, 1);
    lcd->print("ID=");
}

void Display::showFingerprintEnrollID(uint8_t id) {
    lcd->setCursor(9, 1);
    lcd->print(id);
}

void Display::showFingerprintPlacePrompt() {
    // Hiển thị khi bắt đầu quá trình lấy mẫu vân tay
    // Không có trong code gốc, thêm nếu cần
}

void Display::showFingerprintRemovePrompt() {
    clear();
    lcd->setCursor(4, 0);
    lcd->print("NHA TAY");
    delay(1000);
}

void Display::showFingerprintPlaceAgainPrompt() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("DAT LAI CUNG");
    lcd->setCursor(2, 1);
    lcd->print("MOT NGON TAY");
}

void Display::showFingerprintMismatch() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("DAU VAN SAI");
    delay(1000);
    showWelcomeScreen();
}

void Display::showFingerprintStored() {
    // Hiển thị khi lưu vân tay thành công
    // Không có trong code gốc, thêm nếu cần
    showWelcomeScreen();
}

void Display::showDeleteAllFingerprints() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("DELETE ALL");
    lcd->setCursor(2, 1);
    lcd->print("FINGERPRINTS");
    delay(1000);
}

void Display::showWiFiConnecting() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("DANG KET NOI");
    lcd->setCursor(8, 1);
    lcd->print("WIFI");
}

void Display::showWiFiConnected() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("DA KET NOI WIFI");
    delay(1000);
}

void Display::showWiFiConnectionFailed() {
    clear();
    lcd->setCursor(1, 0);
    lcd->print("KHONG KET NOI");
    lcd->setCursor(3, 1);
    lcd->print("DUOC WIFI");
    delay(1000);
}

void Display::showDoorOpened() {
    clear();
    lcd->setCursor(0, 0);
    lcd->print("MO CUA");
}

void Display::showDoorClosed() {
    clear();
    lcd->setCursor(0, 0);
    lcd->print("DONG CUA");
}

void Display::showExitCardMode() {
    clear();
    lcd->setCursor(2, 0);
    lcd->print("THOAT CHE DO");
    lcd->setCursor(2, 1);
    lcd->print("THEM/XOA THE");
    delay(1000);
    showWelcomeScreen();
}

void Display::showMessage(const char* line1, const char* line2) {
    clear();
    lcd->setCursor(0, 0);
    lcd->print(line1);
    if (strlen(line2) > 0) {
        lcd->setCursor(0, 1);
        lcd->print(line2);
    }
} 