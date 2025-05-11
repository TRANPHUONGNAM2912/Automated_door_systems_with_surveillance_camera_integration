# Hướng dẫn sử dụng hệ thống Cửa tự động kết hợp Camera giám sát

## 1. Giới thiệu

Hệ thống cửa tự động kết hợp camera giám sát là một giải pháp an ninh thông minh cho ngôi nhà hoặc văn phòng của bạn. Hệ thống cho phép bạn điều khiển cửa từ xa và giám sát người ra vào thông qua ứng dụng di động.

## 2. Thiết lập ban đầu

### 2.1. Cài đặt phần cứng
- Lắp đặt mạch điều khiển theo sơ đồ đi kèm
- Kết nối ESP32 và ESP32-CAM với nguồn điện
- Kết nối các thiết bị ngoại vi (khóa điện, servo, cảm biến)

### 2.2. Cài đặt phần mềm
1. Cài đặt ứng dụng di động từ file APK đi kèm
2. Mở ứng dụng và thiết lập kết nối WiFi
3. Cấu hình thông tin Firebase (nếu cần)

## 3. Sử dụng hệ thống

### 3.1. Mở khóa cửa
Có nhiều cách để mở khóa cửa:
- Sử dụng ứng dụng di động
- Nhập mật khẩu từ bàn phím mã
- Quét thẻ RFID
- Sử dụng vân tay

### 3.2. Giám sát qua camera
1. Mở ứng dụng di động
2. Chọn chức năng "Camera"
3. Xem hình ảnh trực tiếp từ camera

### 3.3. Cài đặt cấu hình
1. Mở ứng dụng di động
2. Chọn biểu tượng "Cài đặt"
3. Điều chỉnh các thông số theo nhu cầu

## 4. Xử lý sự cố thường gặp

### 4.1. Không kết nối được với hệ thống
- Kiểm tra nguồn điện
- Đảm bảo ESP32 và điện thoại kết nối cùng một mạng WiFi
- Khởi động lại hệ thống

### 4.2. Camera không hiển thị
- Kiểm tra kết nối WiFi của ESP32-CAM
- Đảm bảo camera được cắm đúng cách
- Khởi động lại ESP32-CAM

### 4.3. Cửa không mở
- Kiểm tra kết nối của servo/khóa điện
- Kiểm tra nguồn điện
- Thử mở khóa bằng phương thức khác

## 5. Bảo trì

- Kiểm tra pin của hệ thống định kỳ
- Cập nhật firmware khi có phiên bản mới
- Vệ sinh camera và các cảm biến

## 6. Liên hệ hỗ trợ

Nếu bạn gặp vấn đề cần hỗ trợ, vui lòng liên hệ:
- Email: tranphuongnam292003@gmail.com
- Tác giả: Trần Phương Nam 