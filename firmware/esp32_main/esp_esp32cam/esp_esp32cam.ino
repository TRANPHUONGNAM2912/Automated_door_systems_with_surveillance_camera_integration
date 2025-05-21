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

// Tạo đối tượng hệ thống cửa
DoorSystem* doorSystem = nullptr;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Khởi động hệ thống cửa tự động..."));
  
  // Khởi tạo hệ thống cửa
  doorSystem = new DoorSystem();
  doorSystem->init();
}

void loop() {
  // Cập nhật trạng thái hệ thống
  doorSystem->update();
}