# Ứng dụng Di động cho Hệ thống Cửa Tự động Kết hợp Camera Giám sát

Thư mục này chứa mã nguồn ứng dụng Android cho điều khiển và giám sát hệ thống cửa tự động.

## Cấu trúc thư mục

- `src/`: Mã nguồn ứng dụng Android
- `apk/`: File APK đã biên dịch, sẵn sàng để cài đặt

## Tính năng

- Xem video trực tiếp từ camera ESP32-CAM
- Điều khiển cửa từ xa (mở/đóng)
- Nhận thông báo khi có người bấm chuông
- Nhận cảnh báo an ninh (sai mật khẩu, sai vân tay, sai thẻ RFID)
- Cấu hình địa chỉ IP cho kết nối camera
- Hiển thị trạng thái cửa hiện tại

## Cài đặt và phát triển

### Yêu cầu hệ thống
- Android Studio 4.0+
- JDK 8+
- Android SDK 21+

### Biên dịch từ mã nguồn
1. Mở Android Studio
2. Chọn "Open an existing Android Studio project"
3. Chọn thư mục `src/`
4. Cấu hình Firebase:
   - Tạo dự án trên Firebase Console
   - Tải file `google-services.json` và đặt vào thư mục `app/`
   - Xem file `assets/firebase-config.json.example` để biết cấu trúc

### Cài đặt trực tiếp APK
1. Kết nối thiết bị Android với máy tính
2. Sao chép file `apk/app-debug.apk` vào thiết bị
3. Mở file quản lý tệp tin trên thiết bị và cài đặt APK
4. Cho phép cài đặt từ nguồn không xác định (nếu được yêu cầu)

## Sử dụng ứng dụng

1. Khởi động ứng dụng
2. Nhập địa chỉ IP của ESP32-CAM
3. Nhấn "Kết nối"
4. Sử dụng các nút trên giao diện để mở/đóng cửa

## Tác giả

Trần Phương Nam - tranphuongnam292003@gmail.com 