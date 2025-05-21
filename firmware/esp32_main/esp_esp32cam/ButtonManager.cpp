/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý nút nhấn - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "ButtonManager.h"
#include <Arduino.h>

ButtonManager::ButtonManager() : 
    prevSw2State(HIGH),
    prevSw3State(HIGH) {
}

void ButtonManager::init() {
    pinMode(SW2_PIN, INPUT_PULLUP);
    pinMode(SW3_PIN, INPUT_PULLUP);
    
    Serial.println(F("Đã khởi tạo module Button"));
}

bool ButtonManager::checkSW2Pressed() {
    bool currentSw2State = digitalRead(SW2_PIN);
    bool pressed = (currentSw2State == LOW && prevSw2State == HIGH);
    prevSw2State = currentSw2State;
    return pressed;
}

bool ButtonManager::checkSW3Pressed() {
    bool currentSw3State = digitalRead(SW3_PIN);
    bool pressed = (currentSw3State == LOW && prevSw3State == HIGH);
    prevSw3State = currentSw3State;
    return pressed;
}

void ButtonManager::update() {
    // Không cần thêm logic ở đây vì đã xử lý trong các hàm check
} 