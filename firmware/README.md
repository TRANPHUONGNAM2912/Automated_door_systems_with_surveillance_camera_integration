# Firmware cho Hệ thống Cửa Tự động Kết hợp Camera Giám sát

Thư mục này chứa mã nguồn firmware cho các thành phần ESP32 và ESP32-CAM trong dự án.

## Cấu trúc thư mục

- `esp32_main/`: Mã nguồn cho ESP32 chính, điều khiển hệ thống cửa tự động
- `esp32cam/`: Mã nguồn cho ESP32-CAM, xử lý camera và stream video

## Cài đặt

1. Cài đặt Arduino IDE
2. Thêm hỗ trợ cho ESP32 và ESP32-CAM (qua Boards Manager)
3. Cài đặt các thư viện yêu cầu (xem danh sách bên dưới)
4. Cấu hình Firebase:
   - Tạo file `firebase_config.h` trong mỗi thư mục dựa trên mẫu
   - Cập nhật thông tin Firebase của bạn

## Thư viện yêu cầu

### ESP32 chính
- FirebaseESP32
- MFRC522 (RFID)
- Adafruit Fingerprint Sensor
- ESP32Servo
- Keypad & Keypad I2C
- LiquidCrystal I2C
- SPI
- WiFi
- EEPROM

### ESP32-CAM
- ESP32 Camera Driver
- FirebaseESP32
- WiFi

## Flashing các thiết bị

### ESP32
```
cd esp32_main
arduino-cli compile --fqbn esp32:esp32:esp32 .
arduino-cli upload -p [PORT] --fqbn esp32:esp32:esp32 .
```

### ESP32-CAM
```
cd esp32cam
arduino-cli compile --fqbn esp32:esp32:esp32cam .
arduino-cli upload -p [PORT] --fqbn esp32:esp32:esp32cam .
```

## Tác giả

Trần Phương Nam - tranphuongnam292003@gmail.com 