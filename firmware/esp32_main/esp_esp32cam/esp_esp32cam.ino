/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Mã nguồn điều khiển ESP32 chính
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 * 
 */

#include "config.h"
#include "DoorSystem.h"
#include <esp_task_wdt.h>
#include <esp_system.h>

// Tạo đối tượng hệ thống cửa
DoorSystem* doorSystem = nullptr;

// Biến theo dõi hiệu suất
unsigned long lastHeapCheck = 0;
unsigned long loopCount = 0;
unsigned long lastLoopTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000); // Đợi Serial khởi động
  
  Serial.println(F("========================================"));
  Serial.println(F("  Hệ thống Cửa Tự động - v2.0"));
  Serial.println(F("  Tác giả: Trần Phương Nam"));
  Serial.println(F("========================================"));
  
  // Hiển thị thông tin hệ thống
  Serial.printf("ESP32 Chip ID: %08X\n", (uint32_t)ESP.getEfuseMac());
  Serial.printf("Flash Size: %d MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
  
  // Cấu hình Watchdog Timer (30 giây)
  Serial.println(F("Configuring Watchdog Timer..."));
  esp_task_wdt_init(30, true); // 30 seconds timeout, panic on timeout
  esp_task_wdt_add(NULL); // Add current task to WDT watch
  
  Serial.println(F("Initializing Door System..."));
  
  // Khởi tạo hệ thống cửa với error handling
  try {
    doorSystem = new DoorSystem();
    if (doorSystem != nullptr) {
      doorSystem->init();
      Serial.println(F("Door System initialized successfully!"));
    } else {
      Serial.println(F("ERROR: Failed to create DoorSystem object"));
      ESP.restart();
    }
  } catch (...) {
    Serial.println(F("ERROR: Exception during DoorSystem initialization"));
    ESP.restart();
  }
  
  Serial.println(F("System ready!"));
  Serial.println(F("========================================"));
  
  lastHeapCheck = millis();
  lastLoopTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  loopCount++;
  
  // Reset watchdog timer
  esp_task_wdt_reset();
  
  // Cập nhật trạng thái hệ thống với error handling
  if (doorSystem != nullptr) {
    try {
      doorSystem->update();
    } catch (...) {
      Serial.println(F("ERROR: Exception in doorSystem->update()"));
      // Không restart ngay, thử tiếp tục
    }
  }
  
  // Kiểm tra memory leak mỗi 30 giây
  if (currentTime - lastHeapCheck > 30000) {
    lastHeapCheck = currentTime;
    
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t minFreeHeap = ESP.getMinFreeHeap();
    
    Serial.printf("Memory Status - Free: %d bytes, Min Free: %d bytes\n", 
                  freeHeap, minFreeHeap);
    
    // Cảnh báo nếu memory thấp
    if (freeHeap < 10000) { // Dưới 10KB
      Serial.println(F("WARNING: Low memory detected!"));
    }
    
    // Hiển thị loop performance
    unsigned long loopsPerSecond = loopCount * 1000 / (currentTime - lastLoopTime + 1);
    Serial.printf("Performance - Loops/sec: %lu, Uptime: %lu ms\n", 
                  loopsPerSecond, currentTime);
    
    loopCount = 0;
    lastLoopTime = currentTime;
  }
  
  // Nhỏ delay để tránh watchdog timeout trong trường hợp system quá bận
  delay(10);
}