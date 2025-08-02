# Cải tiến Hệ thống Cửa Tự động - v2.0

## 📋 Tổng quan các cải tiến

Phiên bản 2.0 đã được cải thiện đáng kể với nhiều tính năng mới và tối ưu hóa:

### ✨ Tính năng mới

1. **Firebase Configuration Module** - Quản lý cấu hình Firebase riêng biệt
2. **Security Manager** - Mã hóa mật khẩu và bảo mật dữ liệu
3. **OTA Manager** - Cập nhật firmware qua WiFi
4. **Watchdog Timer** - Tự động khởi động lại khi hệ thống treo
5. **System Health Monitoring** - Theo dõi memory, WiFi, Firebase
6. **Improved Error Handling** - Xử lý lỗi tốt hơn và retry logic
7. **Admin Mode** - Chế độ quản trị với các tính năng nâng cao

### 🔧 Cải tiến kỹ thuật

- **Memory Management**: Theo dõi và cảnh báo memory leak
- **Connection Stability**: Tự động kết nối lại WiFi và Firebase
- **Performance Monitoring**: Đo hiệu suất loop và uptime
- **Secure Password Storage**: Mã hóa SHA256 với salt
- **Modular Architecture**: Code được tổ chức tốt hơn

## 🚀 Cài đặt và Cấu hình

### 1. Cấu hình Firebase

Chỉnh sửa file `firebase_config.h`:

```cpp
#define FIREBASE_HOST "your-project-id-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret-key"
```

### 2. Cấu hình WiFi

Chỉnh sửa file `config.cpp`:

```cpp
const char* WIFI_SSID = "your-wifi-name";
const char* WIFI_PASSWORD = "your-wifi-password";
```

### 3. Thư viện cần thiết

Cài đặt các thư viện sau trong Arduino IDE:

```
- FirebaseESP32 (v4.4.12 trở lên)
- ESP32 Camera Driver
- MFRC522
- Adafruit Fingerprint Sensor
- ESP32 Servo
- Keypad & Keypad_I2C
- LiquidCrystal_I2C
- ArduinoOTA
```

## 📱 Tính năng Admin Mode

Để vào chế độ Admin:
1. Nhấn phím `*` trên keypad
2. Nhập mật khẩu admin (mặc định: `1234`)
3. Sử dụng các lệnh sau:

- `1`: Bật OTA Update
- `2`: Tắt OTA Update  
- `3`: Hiển thị trạng thái hệ thống
- `#`: Thoát chế độ Admin

## 🔄 OTA Update

### Cách sử dụng OTA:

1. Vào Admin Mode và nhấn `1` để bật OTA
2. Trong Arduino IDE:
   - Tools → Port → Network Ports
   - Chọn "AutoDoorSystem_[ChipID] at [IP]"
   - Upload code như bình thường

### Bảo mật OTA:
- Password mặc định: `admin123`
- Device name: `AutoDoorSystem_[ChipID]`

## 📊 System Monitoring

Hệ thống tự động kiểm tra và báo cáo:

- **Memory Usage**: Cảnh báo khi RAM < 10KB
- **WiFi Status**: Tự động kết nối lại
- **Firebase Connection**: Monitoring realtime
- **Loop Performance**: Đo loops/second
- **Uptime**: Thời gian hoạt động

## 🔐 Bảo mật

### Mã hóa mật khẩu:
- Sử dụng SHA256 với salt
- Không lưu mật khẩu dạng plain text
- Secure memory wipe sau sử dụng

### Kết nối an toàn:
- WPA2/WPA3 WiFi encryption
- Firebase SSL/TLS
- OTA password protection

## 🐛 Debug và Troubleshooting

### Serial Monitor Output:

```
========================================
  Hệ thống Cửa Tự động - v2.0
  Tác giả: Trần Phương Nam
========================================
ESP32 Chip ID: 12345678
Flash Size: 4 MB
Free Heap: 298765 bytes
CPU Frequency: 240 MHz
Configuring Watchdog Timer...
Initializing Door System...
WiFi connection attempt 1/3
Connecting to: YourWiFi
..........
WiFi connected successfully!
IP address: 192.168.1.100
Signal strength: -45 dBm
Firebase initialized successfully
Door System initialized successfully!
System ready!
========================================
```

### Các lỗi thường gặp:

1. **WiFi Connection Failed**:
   - Kiểm tra SSID và password
   - Kiểm tra tín hiệu WiFi
   - Reset router nếu cần

2. **Firebase Connection Failed**:
   - Kiểm tra FIREBASE_HOST và FIREBASE_AUTH
   - Kiểm tra kết nối internet
   - Kiểm tra Firebase rules

3. **Memory Issues**:
   - Monitor heap usage
   - Kiểm tra memory leaks
   - Restart nếu heap < 10KB

4. **OTA Failed**:
   - Kiểm tra WiFi connection
   - Verify OTA password
   - Ensure sufficient flash space

## 📈 Performance Tips

1. **Memory Optimization**:
   - Sử dụng F() macro cho string constants
   - Avoid String concatenation trong loops
   - Free unused objects

2. **Network Stability**:
   - Đặt router gần ESP32
   - Sử dụng static IP nếu cần
   - Monitor signal strength

3. **Power Management**:
   - Sử dụng deep sleep khi không cần
   - Optimize loop delays
   - Disable unused peripherals

## 🔄 Firmware Update Process

### Qua OTA:
1. Enable OTA trong Admin Mode
2. Upload từ Arduino IDE
3. Hệ thống tự động reboot

### Qua USB:
1. Kết nối USB cable
2. Upload bình thường
3. Monitor Serial output

## 📝 Changelog v2.0

- ✅ Thêm Firebase configuration module
- ✅ Cải thiện error handling và retry logic  
- ✅ Thêm watchdog timer protection
- ✅ Tối ưu memory usage và leak detection
- ✅ Thêm OTA update functionality
- ✅ Cải thiện security với password hashing
- ✅ Thêm system health monitoring
- ✅ Thêm admin mode với advanced features
- ✅ Cải thiện network stability
- ✅ Thêm performance monitoring

## 🤝 Đóng góp

Để đóng góp vào dự án:

1. Fork repository
2. Tạo feature branch
3. Commit changes
4. Create pull request

## 📞 Hỗ trợ

- Email: tranphuongnam292003@gmail.com
- GitHub: [Repository Link]

---

**Lưu ý**: Đây là phiên bản cải tiến với nhiều tính năng mới. Hãy đọc kỹ hướng dẫn trước khi triển khai.