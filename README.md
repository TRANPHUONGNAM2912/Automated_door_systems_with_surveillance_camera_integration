# Hệ thống Cửa Tự động Kết hợp Camera Giám sát

Dự án này là một giải pháp hoàn chỉnh cho hệ thống cửa tự động thông minh có tích hợp camera giám sát. Hệ thống cho phép người dùng điều khiển cửa từ xa thông qua ứng dụng di động, đồng thời giám sát khu vực thông qua camera ESP32-CAM.

## Tính năng

### Đa dạng phương thức xác thực
- Sử dụng mật khẩu để mở cửa thông qua keypad
- Quẹt thẻ RFID để mở cửa
- Xác thực vân tay để mở cửa

### Quản lý người dùng
- Thêm/xóa thẻ RFID thông qua giao diện phím bấm
- Thêm/xóa vân tay thông qua giao diện phím bấm
- Thay đổi mật khẩu mở khóa

### An ninh & Cảnh báo
- Phát cảnh báo khi nhập sai mật khẩu, quẹt sai thẻ hoặc vân tay quá số lần quy định
- Cảnh báo qua còi báo động tại chỗ
- Thông báo cảnh báo đến ứng dụng điện thoại

### Điều khiển cửa
- Sử dụng động cơ servo mô phỏng quá trình đóng/mở cửa
- Điều khiển đóng/mở cửa từ xa thông qua ứng dụng điện thoại

### Giám sát & Thông báo
- Nút chuông tại cửa, khi có khách bấm sẽ phát âm thanh
- Thông báo về ứng dụng điện thoại khi có người bấm chuông
- Xem video trực tiếp từ camera ESP32-CAM trên ứng dụng di động
- Theo dõi trạng thái cửa (đóng/mở) trên ứng dụng

### Kết nối
- Kết nối qua WiFi
- Giao tiếp qua Firebase
- Giao diện LCD hiển thị trạng thái tại chỗ

## Cấu trúc thư mục

```
/
├── hardware/                    # Thiết kế phần cứng
│   ├── schematic/               # Sơ đồ mạch điện
│   └── pcb/                     # Thiết kế PCB
│
├── firmware/                    # Mã nguồn firmware
│   ├── esp32_main/              # Mã nguồn cho ESP32 chính
│   └── esp32cam/                # Mã nguồn cho ESP32-CAM
│
├── mobile_app/                  # Ứng dụng di động Android
│   ├── src/                     # Mã nguồn
│   └── apk/                     # File cài đặt APK
│
└── docs/                        # Tài liệu
    ├── images/                  # Hình ảnh và sơ đồ
    └── user_manual/             # Hướng dẫn sử dụng
```

## Thành phần phần cứng

- ESP32 (điều khiển chính)
- ESP32-CAM (camera và xử lý hình ảnh)
- Màn hình LCD I2C
- Đầu đọc thẻ RFID MFRC522
- Cảm biến vân tay
- Bàn phím ma trận (keypad)
- Động cơ servo điều khiển cửa
- Còi báo động
- Nút bấm chuông
- Các cảm biến phụ trợ

## Cài đặt

### Yêu cầu phần cứng
- Board ESP32
- Module ESP32-CAM
- Các thành phần điện tử khác theo sơ đồ mạch

### Cài đặt firmware
1. Cài đặt Arduino IDE
2. Thêm thư viện ESP32 và các thư viện phụ thuộc
3. Cấu hình Firebase:
   - Tạo dự án Firebase trên [Firebase Console](https://console.firebase.google.com/)
   - Trong thư mục `firmware/esp32_main`, mở file `firebase_config.h` và cập nhật:
     ```
     #define FIREBASE_HOST "your-firebase-project-id.firebaseio.com"
     #define FIREBASE_AUTH "your-firebase-secret-key"
     ```
   - Tương tự, cập nhật cấu hình trong `firmware/esp32cam/firebase_config.h`
4. Nạp firmware tương ứng cho ESP32 và ESP32-CAM

### Cài đặt ứng dụng di động
1. Thêm cấu hình Firebase cho ứng dụng Android:
   - Tải file `google-services.json` từ dự án Firebase của bạn
   - Đặt file này vào thư mục `mobile_app/src/app`
   - File mẫu `firebase-config.json.example` đã được cung cấp để tham khảo
2. Cài đặt file APK trên thiết bị Android
3. Cấu hình kết nối đến hệ thống

## Phát triển

### Môi trường phát triển
- Arduino IDE (firmware)
- Android Studio (ứng dụng di động)
- Altium Designer (thiết kế phần cứng)

### Thư viện yêu cầu
- Firebase ESP32 Client
- ESP32 Camera Driver
- MFRC522 (RFID)
- Adafruit Fingerprint Sensor
- ESP32 Servo
- Keypad & Keypad I2C
- LiquidCrystal I2C

## Tác giả

- Trần Phương Nam - tranphuongnam292003@gmail.com

## Giấy phép

Dự án này được phân phối dưới giấy phép MIT. Xem file `LICENSE` để biết thêm chi tiết. 