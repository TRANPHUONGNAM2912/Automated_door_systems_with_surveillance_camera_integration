/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý còi báo động - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "BuzzerManager.h"
#include <Arduino.h>

BuzzerManager::BuzzerManager() : 
    buzzerActive(false),
    sw2BuzzerActive(false),
    buzzerStartTime(0),
    sw2BuzzerStartTime(0) {
}

void BuzzerManager::init() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    
    pinMode(BUZZER_SW2_PIN, OUTPUT);
    digitalWrite(BUZZER_SW2_PIN, LOW);
    
    Serial.println(F("Đã khởi tạo module Buzzer"));
}

void BuzzerManager::activateBuzzer() {
    buzzerActive = true;
    buzzerStartTime = millis();
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println(F("Còi báo động đã kích hoạt"));
}

void BuzzerManager::activateSW2Buzzer() {
    sw2BuzzerActive = true;
    sw2BuzzerStartTime = millis();
    digitalWrite(BUZZER_SW2_PIN, HIGH);
    Serial.println(F("Còi SW2 đã kích hoạt"));
}

void BuzzerManager::update() {
    unsigned long currentMillis = millis();
    
    // Xử lý timer cho buzzer chính
    if (buzzerActive && currentMillis - buzzerStartTime >= BUZZER_DURATION) {
        buzzerActive = false;
        digitalWrite(BUZZER_PIN, LOW);
        Serial.println(F("Còi báo động đã tắt"));
    }
    
    // Xử lý timer cho buzzer SW2
    if (sw2BuzzerActive && currentMillis - sw2BuzzerStartTime >= SW2_BUZZER_DURATION) {
        sw2BuzzerActive = false;
        digitalWrite(BUZZER_SW2_PIN, LOW);
        Serial.println(F("Còi SW2 đã tắt"));
    }
}

bool BuzzerManager::isBuzzerActive() const {
    return buzzerActive;
}

bool BuzzerManager::isSW2BuzzerActive() const {
    return sw2BuzzerActive;
} 