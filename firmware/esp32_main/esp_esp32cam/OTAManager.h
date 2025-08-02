/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý cập nhật firmware OTA
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>

class OTAManager {
private:
    bool otaEnabled;
    String deviceName;
    String otaPassword;
    
    // Callback functions
    static void onStart();
    static void onEnd();
    static void onProgress(unsigned int progress, unsigned int total);
    static void onError(ota_error_t error);
    
public:
    OTAManager(const String& deviceName = "AutoDoorSystem", const String& password = "admin123");
    
    // Khởi tạo OTA
    bool init();
    
    // Xử lý OTA requests
    void handle();
    
    // Enable/Disable OTA
    void enable();
    void disable();
    
    // Kiểm tra trạng thái
    bool isEnabled() const;
    
    // Set password for OTA
    void setPassword(const String& password);
    
    // Set device name
    void setDeviceName(const String& name);
};

#endif // OTA_MANAGER_H