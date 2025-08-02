/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý cập nhật firmware OTA - Implementation
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "OTAManager.h"
#include <esp_task_wdt.h>

OTAManager::OTAManager(const String& deviceName, const String& password) :
    otaEnabled(false),
    deviceName(deviceName),
    otaPassword(password) {
}

bool OTAManager::init() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println(F("OTA: WiFi not connected"));
        return false;
    }
    
    // Cấu hình OTA
    ArduinoOTA.setHostname(deviceName.c_str());
    ArduinoOTA.setPassword(otaPassword.c_str());
    
    // Thiết lập callbacks
    ArduinoOTA.onStart(onStart);
    ArduinoOTA.onEnd(onEnd);
    ArduinoOTA.onProgress(onProgress);
    ArduinoOTA.onError(onError);
    
    // Bắt đầu OTA service
    ArduinoOTA.begin();
    
    otaEnabled = true;
    
    Serial.println(F("OTA: Initialized successfully"));
    Serial.printf("OTA: Device name: %s\n", deviceName.c_str());
    Serial.printf("OTA: IP address: %s\n", WiFi.localIP().toString().c_str());
    
    return true;
}

void OTAManager::handle() {
    if (otaEnabled) {
        ArduinoOTA.handle();
    }
}

void OTAManager::enable() {
    if (WiFi.status() == WL_CONNECTED && !otaEnabled) {
        init();
    }
}

void OTAManager::disable() {
    if (otaEnabled) {
        ArduinoOTA.end();
        otaEnabled = false;
        Serial.println(F("OTA: Disabled"));
    }
}

bool OTAManager::isEnabled() const {
    return otaEnabled;
}

void OTAManager::setPassword(const String& password) {
    otaPassword = password;
    if (otaEnabled) {
        ArduinoOTA.setPassword(password.c_str());
    }
}

void OTAManager::setDeviceName(const String& name) {
    deviceName = name;
    if (otaEnabled) {
        ArduinoOTA.setHostname(name.c_str());
    }
}

// Static callback implementations
void OTAManager::onStart() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
    } else { // U_SPIFFS
        type = "filesystem";
    }
    
    Serial.println("OTA: Start updating " + type);
    
    // Disable watchdog during OTA
    esp_task_wdt_delete(NULL);
}

void OTAManager::onEnd() {
    Serial.println(F("\nOTA: Update completed successfully"));
    Serial.println(F("OTA: Rebooting..."));
}

void OTAManager::onProgress(unsigned int progress, unsigned int total) {
    static unsigned long lastUpdate = 0;
    unsigned long now = millis();
    
    // Chỉ update progress mỗi 1 giây để tránh spam
    if (now - lastUpdate > 1000) {
        lastUpdate = now;
        Serial.printf("OTA Progress: %u%% (%u/%u bytes)\n", 
                     (progress * 100) / total, progress, total);
    }
}

void OTAManager::onError(ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    
    switch (error) {
        case OTA_AUTH_ERROR:
            Serial.println(F("Auth Failed"));
            break;
        case OTA_BEGIN_ERROR:
            Serial.println(F("Begin Failed"));
            break;
        case OTA_CONNECT_ERROR:
            Serial.println(F("Connect Failed"));
            break;
        case OTA_RECEIVE_ERROR:
            Serial.println(F("Receive Failed"));
            break;
        case OTA_END_ERROR:
            Serial.println(F("End Failed"));
            break;
        default:
            Serial.println(F("Unknown Error"));
            break;
    }
    
    // Re-enable watchdog after failed OTA
    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);
}