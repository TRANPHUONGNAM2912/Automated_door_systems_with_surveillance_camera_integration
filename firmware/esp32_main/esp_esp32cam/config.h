/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * File cấu hình hệ thống
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef CONFIG_H
#define CONFIG_H

// Pin definitions
#define I2CADDR 0x20
#define PIN_SG90 2 
#define MODEM_RX 16
#define MODEM_TX 17
#define SS_PIN 5
#define RST_PIN 13
#define BUZZER_PIN 14
#define BUZZER_SW2_PIN 27
#define SW2_PIN 26
#define SW3_PIN 25

// EEPROM configuration
#define EEPROM_SIZE 512
#define CARD_COUNT_ADDRESS 4      // Lưu số lượng thẻ tại vị trí 4
#define RFID_START_ADDRESS 5      // Bắt đầu lưu dữ liệu thẻ từ vị trí 5
#define MAX_CARDS 10              // Số lượng thẻ tối đa
#define CARD_SIZE 4               // Mỗi thẻ lưu 4 byte (UID của thẻ MIFARE)

// Keypad configuration
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

// Timing configurations
#define DOOR_OPEN_DURATION 3000   // Thời gian mở cửa (3 giây)
#define BUZZER_DURATION 2000      // Thời gian buzzer kêu (2 giây)
#define SW2_BUZZER_DURATION 3000  // Thời gian chuông kêu khi SW2 được nhấn (3 giây)
#define WIFI_CONNECT_TIMEOUT 5000 // Thời gian timeout kết nối WiFi (5 giây)

// Authentication settings
#define MAX_FAILED_ATTEMPTS 3     // Số lần thử tối đa trước khi cảnh báo

// WiFi configuration
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// Default password
extern char DEFAULT_PASSWORD[5];

#endif // CONFIG_H 