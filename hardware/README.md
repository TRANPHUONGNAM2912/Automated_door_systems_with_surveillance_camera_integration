# Thiết kế Phần cứng cho Hệ thống Cửa Tự động Kết hợp Camera Giám sát

Thư mục này chứa các file thiết kế phần cứng của dự án, bao gồm sơ đồ mạch và thiết kế PCB.

## Cấu trúc thư mục

- `schematic/`: Sơ đồ mạch điện Altium
- `pcb/`: Thiết kế PCB

## Thành phần phần cứng

### Thành phần chính
- ESP32 (điều khiển chính)
- ESP32-CAM (camera và xử lý hình ảnh)

### Ngoại vi
- Màn hình LCD I2C 16x2
- Đầu đọc thẻ RFID MFRC522
- Cảm biến vân tay
- Bàn phím ma trận 4x4
- Động cơ servo
- Còi báo động
- Nút bấm chuông
- Các LED chỉ thị

## Mở và chỉnh sửa các file thiết kế

Các file thiết kế được tạo bằng Altium Designer. Để mở và chỉnh sửa:

1. Cài đặt Altium Designer
2. Mở file `schematic/AutoDoor_App.SchDoc` để xem và chỉnh sửa sơ đồ mạch điện
3. Mở file `pcb/AutoDoor_App.PcbDoc` để xem và chỉnh sửa thiết kế PCB

## Tạo file sản xuất

Để tạo các file sản xuất từ thiết kế:

1. Mở Altium Designer và tải dự án
2. Chọn File > Fabrication Outputs để tạo các file Gerber
3. Chọn File > Assembly Outputs để tạo các file BOM và Pick & Place

## Tác giả

Trần Phương Nam - tranphuongnam292003@gmail.com 