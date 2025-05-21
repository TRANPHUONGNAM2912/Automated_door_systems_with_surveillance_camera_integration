/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý mạng và Firebase - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "NetworkManager.h"
#include "firebase_config.h"
#include <Arduino.h>

// Biến tĩnh để lưu con trỏ đến đối tượng NetworkManager hiện tại
static NetworkManager* currentNetworkManager = nullptr;

NetworkManager::NetworkManager() : 
    wifiConnected(false),
    firebaseInitialized(false),
    doorStreamStarted(false),
    lastDoorStatus(false) {
    
    // Lưu con trỏ đến đối tượng hiện tại
    currentNetworkManager = this;
}

bool NetworkManager::init() {
    // Kết nối WiFi với timeout
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    unsigned long startTime = millis();
    
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < WIFI_CONNECT_TIMEOUT) {
        Serial.print(".");
        delay(500);
    }
    
    wifiConnected = (WiFi.status() == WL_CONNECTED);
    
    if (wifiConnected) {
        Serial.println("\nConnected to WiFi");
        
        // Khởi tạo Firebase
        config.host = FIREBASE_HOST;
        config.signer.tokens.legacy_token = FIREBASE_AUTH;
        config.timeout.serverResponse = 10 * 1000; // 10 seconds timeout
        
        Firebase.begin(&config, &auth);
        Firebase.reconnectWiFi(true);
        
        firebaseInitialized = true;
        Serial.println("Firebase initialized");
        
        // Khởi tạo trạng thái cảnh báo
        initializeWarningStates();
        
        // Thiết lập stream cho door_status
        setupDoorStatusStream();
        
        return true;
    } else {
        Serial.println("\nFailed to connect to WiFi");
        return false;
    }
}

bool NetworkManager::isWiFiConnected() const {
    return wifiConnected;
}

bool NetworkManager::isFirebaseInitialized() const {
    return firebaseInitialized;
}

void NetworkManager::updateDoorStatus(bool isOpen) {
    if (wifiConnected && firebaseInitialized) {
        if (Firebase.setBool(firebaseDataSender, "/door_status", isOpen)) {
            Serial.println("Cập nhật trạng thái cửa: " + String(isOpen ? "true" : "false"));
        } else {
            Serial.println("Lỗi cập nhật trạng thái cửa: " + firebaseDataSender.errorReason());
        }
    }
}

void NetworkManager::updateBellStatus(bool isActive) {
    if (wifiConnected && firebaseInitialized) {
        if (Firebase.setBool(firebaseDataSender, "/bell", isActive)) {
            Serial.println("Cập nhật trạng thái chuông: " + String(isActive ? "true" : "false"));
        } else {
            Serial.println("Lỗi cập nhật trạng thái chuông: " + firebaseDataSender.errorReason());
        }
    }
}

void NetworkManager::updateWarningStatus(const char* warningType, bool isActive) {
    if (wifiConnected && firebaseInitialized) {
        String path = "/warning/";
        path += warningType;
        
        if (Firebase.setBool(firebaseDataSender, path, isActive)) {
            Serial.println("Cập nhật " + String(warningType) + ": " + (isActive ? "true" : "false"));
        } else {
            Serial.println("Lỗi cập nhật: " + firebaseDataSender.errorReason());
        }
    }
}

void NetworkManager::initializeWarningStates() {
    if (wifiConnected && firebaseInitialized) {
        updateWarningStatus("fingerprint", false);
        updateWarningStatus("password", false);
        updateWarningStatus("rfid", false);
        
        // Khởi tạo trạng thái chuông là false khi khởi động
        updateBellStatus(false);
        Serial.println("Bell status initialized to false");
    }
}

void NetworkManager::reconnectIfNeeded() {
    // Kiểm tra kết nối WiFi và thiết lập lại stream nếu cần
    if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        
        if (!doorStreamStarted) {
            setupDoorStatusStream();
        }
    } else {
        wifiConnected = false;
        doorStreamStarted = false;
    }
}

void NetworkManager::setupDoorStatusStream() {
    if (wifiConnected && !doorStreamStarted) {
        // Thiết lập stream để lắng nghe thay đổi
        if (!Firebase.beginStream(firebaseData, "/door_status")) {
            Serial.println("Không thể bắt đầu stream door_status: " + firebaseData.errorReason());
        } else {
            // Đăng ký callback function
            Firebase.setStreamCallback(firebaseData, doorStatusCallback, doorStreamTimeoutCallback);
            Serial.println("Đang lắng nghe thay đổi door_status trên Firebase...");
            doorStreamStarted = true;
            
            // Đọc trạng thái ban đầu
            if (Firebase.getBool(firebaseData, "/door_status")) {
                lastDoorStatus = firebaseData.boolData();
                Serial.println("Trạng thái cửa ban đầu: " + String(lastDoorStatus ? "true" : "false"));
            }
        }
    }
}

// Static callback functions
void NetworkManager::doorStatusCallback(StreamData data) {
    if (currentNetworkManager && data.dataType() == "boolean") {
        bool doorStatus = data.boolData();
        Serial.println("Nhận trạng thái cửa: " + String(doorStatus ? "true" : "false"));
        
        // Kiểm tra xem trạng thái có thay đổi không
        if (doorStatus != currentNetworkManager->lastDoorStatus) {
            // Cập nhật trạng thái cửa mới
            currentNetworkManager->lastDoorStatus = doorStatus;
            
            // TODO: Xử lý sự kiện thay đổi trạng thái cửa
            // Đây là một callback tĩnh, nên không thể gọi trực tiếp các hàm không tĩnh
            // Cần triển khai cơ chế để thông báo cho các đối tượng khác
        }
    }
}

void NetworkManager::doorStreamTimeoutCallback(bool timeout) {
    if (timeout) {
        Serial.println("Stream timeout, sẽ thử kết nối lại...");
    }
    
    if (currentNetworkManager && !currentNetworkManager->firebaseData.httpConnected()) {
        Serial.println("Lỗi kết nối: " + currentNetworkManager->firebaseData.errorReason());
    }
}

// Setter để thiết lập callback function
void NetworkManager::setDoorStatusCallback(StreamDataCallback callback) {
    if (wifiConnected && firebaseInitialized) {
        Firebase.setStreamCallback(firebaseData, callback, doorStreamTimeoutCallback);
    }
}

void NetworkManager::setDoorStreamTimeoutCallback(StreamTimeoutCallback callback) {
    if (wifiConnected && firebaseInitialized) {
        // Lưu ý: Phương thức này không có sẵn trong Firebase-ESP32, chỉ mô phỏng
        // Firebase.setStreamTimeoutCallback(firebaseData, callback);
    }
} 