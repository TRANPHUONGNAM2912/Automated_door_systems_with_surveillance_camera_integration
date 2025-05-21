/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý hệ thống cửa - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "DoorSystem.h"
#include <Arduino.h>
#include <EEPROM.h>

DoorSystem::DoorSystem() : 
    doorIsOpen(false),
    adminMode(false) {
    
    // Khởi tạo các đối tượng quản lý
    display = new Display(0x27, 16, 2);
    doorControl = new DoorControl();
    rfidManager = new RFIDManager();
    
    // Khởi tạo Serial2 cho cảm biến vân tay
    Serial2.begin(57600, SERIAL_8N1, MODEM_RX, MODEM_TX);
    fingerprintManager = new FingerprintManager(&Serial2);
    
    keypadManager = new KeypadManager();
    networkManager = new NetworkManager();
    buzzerManager = new BuzzerManager();
    buttonManager = new ButtonManager();
}

DoorSystem::~DoorSystem() {
    delete display;
    delete doorControl;
    delete rfidManager;
    delete fingerprintManager;
    delete keypadManager;
    delete networkManager;
    delete buzzerManager;
    delete buttonManager;
}

void DoorSystem::init() {
    // Khởi tạo EEPROM
    EEPROM.begin(EEPROM_SIZE);
    
    // Khởi tạo các module
    display->init();
    doorControl->init();
    rfidManager->init();
    fingerprintManager->init();
    keypadManager->init();
    buzzerManager->init();
    buttonManager->init();
    
    // Khởi tạo kết nối WiFi và Firebase
    bool wifiConnected = networkManager->init();
    if (wifiConnected) {
        display->showWiFiConnected();
    } else {
        display->showWiFiConnectionFailed();
    }
    
    // Hiển thị màn hình chào mừng
    display->showWelcomeScreen();
    
    Serial.println(F("Hệ thống cửa đã được khởi tạo"));
}

void DoorSystem::update() {
    // Cập nhật trạng thái các module
    doorControl->update();
    buzzerManager->update();
    networkManager->reconnectIfNeeded();
    
    // Xử lý các sự kiện đầu vào
    handleKeypadInput();
    handleFingerprintScan();
    handleRFIDScan();
    handleButtonPress();
}

void DoorSystem::handleKeypadInput() {
    static char entered_keys[5] = {'\0', '\0', '\0', '\0', '\0'};
    static int keyIndex = 0;
    
    char key = keypadManager->getKey();
    if (key) {
        Serial.println(key);
        
        // Xử lý chế độ admin
        if (adminMode) {
            handleAdminMode(key);
            return;
        }
        
        // Xử lý các phím quản lý
        if (key == '*') {
            // Yêu cầu xác thực trước khi vào chế độ thêm thẻ
            if (keypadManager->verifyCurrentPassword()) {
                display->showAuthenticationSuccess();
                rfidManager->setAddCardMode(true);
                display->showAddCardMode();
            } else {
                display->showAuthenticationFailed("password");
            }
            return;
        } 
        else if (key == '#') {
            // Yêu cầu xác thực trước khi vào chế độ xóa thẻ
            if (keypadManager->verifyCurrentPassword()) {
                display->showAuthenticationSuccess();
                rfidManager->setDeleteCardMode(true);
                display->showDeleteCardMode();
            } else {
                display->showAuthenticationFailed("password");
            }
            return;
        }
        else if (key == 'B') {
            // Thoát chế độ thêm/xóa thẻ
            rfidManager->setAddCardMode(false);
            rfidManager->setDeleteCardMode(false);
            display->showExitCardMode();
            keyIndex = 0;
            memset(entered_keys, '\0', sizeof(entered_keys));
            return;
        }
        else if (key == 'C') {
            // Yêu cầu xác thực trước khi đổi mật khẩu
            if (keypadManager->verifyCurrentPassword()) {
                display->showAuthenticationSuccess();
                
                // Nhập mật khẩu mới
                display->showNewPasswordPrompt();
                char new_password[5] = {'\0', '\0', '\0', '\0', '\0'};
                
                for (int i = 0; i < 4; i++) {
                    char k = NO_KEY;
                    while (k == NO_KEY) {
                        k = keypadManager->getKey();
                        delay(50);
                    }
                    new_password[i] = k;
                    display->showPasswordInput(i);
                }
                
                if (keypadManager->changePassword(new_password)) {
                    display->showPasswordChanged();
                }
            } else {
                display->showAuthenticationFailed("password");
            }
            keyIndex = 0;
            memset(entered_keys, '\0', sizeof(entered_keys));
            return;
        } 
        else if (key == 'A') {
            // Yêu cầu xác thực trước khi thêm vân tay mới
            if (keypadManager->verifyCurrentPassword()) {
                display->showAuthenticationSuccess();
                
                // Nhập ID vân tay
                display->showFingerprintEnrollPrompt();
                uint8_t id = 0;
                
                // Đợi người dùng nhập ID
                char k = NO_KEY;
                while (k == NO_KEY || k < '1' || k > '9') {
                    k = keypadManager->getKey();
                    delay(50);
                }
                id = k - '0';
                display->showFingerprintEnrollID(id);
                
                // Thiết lập ID và bắt đầu quá trình lưu vân tay
                fingerprintManager->setID(id);
                
                // Lấy mẫu vân tay lần 1
                display->showFingerprintPlacePrompt();
                if (fingerprintManager->getFingerprintEnroll() == true) {
                    display->showFingerprintStored();
                } else {
                    display->showFingerprintMismatch();
                }
            } else {
                display->showAuthenticationFailed("password");
            }
            keyIndex = 0;
            memset(entered_keys, '\0', sizeof(entered_keys));
            return;
        } 
        else if (key == 'D') {
            // Yêu cầu xác thực trước khi xóa tất cả vân tay
            if (keypadManager->verifyCurrentPassword()) {
                display->showAuthenticationSuccess();
                display->showDeleteAllFingerprints();
                fingerprintManager->emptyDatabase();
            } else {
                display->showAuthenticationFailed("password");
            }
            keyIndex = 0;
            memset(entered_keys, '\0', sizeof(entered_keys));
            return;
        }
        
        // Xử lý nhập mật khẩu
        if (key >= '0' && key <= '9') {
            keyIndex++;
            if (keyIndex == 1) {
                display->clear();
                memset(entered_keys, '\0', sizeof(entered_keys));
            }
            
            if (keyIndex <= 4) {
                entered_keys[keyIndex-1] = key;
                display->showPasswordInput(keyIndex);
            }
            
            // Đã nhập đủ 4 chữ số
            if (keyIndex == 4) {
                // Kiểm tra mật khẩu
                if (keypadManager->checkPassword(entered_keys)) {
                    // Mật khẩu đúng
                    display->showAuthenticationSuccess();
                    doorControl->openDoor();
                    
                    // Cập nhật trạng thái cửa lên Firebase
                    networkManager->updateDoorStatus(true);
                    
                    // Reset counters và cảnh báo
                    keypadManager->resetCompletePasswordAttempts();
                    networkManager->updateWarningStatus("password", false);
                } else {
                    // Mật khẩu sai
                    keypadManager->incrementCompletePasswordAttempts();
                    Serial.println("Mật khẩu sai. Số lần thử: " + String(keypadManager->getCompletePasswordAttempts()));
                    
                    display->showAuthenticationFailed("password");
                    
                    if (keypadManager->getCompletePasswordAttempts() >= MAX_FAILED_ATTEMPTS) {
                        buzzerManager->activateBuzzer();
                        networkManager->updateWarningStatus("password", true);
                        networkManager->updateBellStatus(true);
                        
                        // Reset counter after warning sent
                        keypadManager->resetCompletePasswordAttempts();
                    }
                }
                
                // Reset để nhập lại
                keyIndex = 0;
                memset(entered_keys, '\0', sizeof(entered_keys));
            }
        }
    }
}

void DoorSystem::handleFingerprintScan() {
    if (fingerprintManager->checkFingerprint()) {
        display->showAuthenticationSuccess();
        doorControl->openDoor();
        
        // Cập nhật trạng thái cửa lên Firebase
        networkManager->updateDoorStatus(true);
        
        // Reset cảnh báo
        networkManager->updateWarningStatus("fingerprint", false);
    } else if (fingerprintManager->getWrongFingerprintCount() >= MAX_FAILED_ATTEMPTS) {
        buzzerManager->activateBuzzer();
        networkManager->updateWarningStatus("fingerprint", true);
        networkManager->updateBellStatus(true);
        
        // Reset counter after warning sent
        fingerprintManager->resetWrongFingerprintCount();
    }
}

void DoorSystem::handleRFIDScan() {
    if (rfidManager->isNewCardPresent()) {
        rfidManager->readCardUID();
        
        if (rfidManager->isAddCardMode()) {
            rfidManager->addCard();
            display->showCardAdded();
        } else if (rfidManager->isDeleteCardMode()) {
            rfidManager->deleteCard();
            display->showCardDeleted();
        } else {
            if (rfidManager->checkCard()) {
                display->showAuthenticationSuccess();
                doorControl->openDoor();
                
                // Cập nhật trạng thái cửa lên Firebase
                networkManager->updateDoorStatus(true);
                
                // Reset cảnh báo
                networkManager->updateWarningStatus("rfid", false);
            } else {
                display->showAuthenticationFailed("rfid");
                
                if (rfidManager->getWrongRFIDCount() >= MAX_FAILED_ATTEMPTS) {
                    buzzerManager->activateBuzzer();
                    networkManager->updateWarningStatus("rfid", true);
                    networkManager->updateBellStatus(true);
                    
                    // Reset counter after warning sent
                    rfidManager->resetWrongRFIDCount();
                }
            }
        }
    }
}

void DoorSystem::handleButtonPress() {
    // Kiểm tra nút SW2 (chuông cửa)
    if (buttonManager->checkSW2Pressed()) {
        buzzerManager->activateSW2Buzzer();
        networkManager->updateBellStatus(true);
        Serial.println("Nút SW2 được nhấn, chuông trên chân 27 kêu");
    }
    
    // Kiểm tra nút SW3 (mở/đóng cửa thủ công)
    if (buttonManager->checkSW3Pressed()) {
        doorIsOpen = !doorIsOpen;
        if (doorIsOpen) {
            doorControl->openDoor();
            networkManager->updateDoorStatus(true);
            display->showDoorOpened();
            Serial.println("Mở cửa từ nút SW3");
        } else {
            doorControl->closeDoor();
            networkManager->updateDoorStatus(false);
            display->showDoorClosed();
            Serial.println("Đóng cửa từ nút SW3");
        }
    }
}

void DoorSystem::handleAdminMode(char key) {
    // Xử lý các lệnh trong chế độ admin
    // Chưa triển khai
} 