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
#include "config.h"
#include <Arduino.h>
#include <esp_task_wdt.h>

// Biến tĩnh để lưu con trỏ đến đối tượng NetworkManager hiện tại
static NetworkManager* currentNetworkManager = nullptr;

NetworkManager::NetworkManager() : 
    wifiConnected(false),
    firebaseInitialized(false),
    doorStreamStarted(false),
    lastDoorStatus(false),
    lastReconnectAttempt(0),
    reconnectAttempts(0) {
    
    // Lưu con trỏ đến đối tượng hiện tại
    currentNetworkManager = this;
}

bool NetworkManager::init() {
    Serial.println(F("Initializing NetworkManager..."));
    
    // Reset WiFi để đảm bảo kết nối sạch
    WiFi.disconnect(true);
    delay(1000);
    
    // Cấu hình WiFi
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    
    // Kết nối WiFi với timeout và retry
    return connectToWiFi();
}

bool NetworkManager::connectToWiFi() {
    const int maxRetries = 3;
    
    for (int attempt = 1; attempt <= maxRetries; attempt++) {
        Serial.printf("WiFi connection attempt %d/%d\n", attempt, maxRetries);
        Serial.printf("Connecting to: %s\n", WIFI_SSID);
        
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        unsigned long startTime = millis();
        
        // Chờ kết nối với timeout
        while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < WIFI_CONNECT_TIMEOUT) {
            Serial.print(".");
            delay(500);
            esp_task_wdt_reset(); // Reset watchdog
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            wifiConnected = true;
            Serial.println(F("\nWiFi connected successfully!"));
            Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("Signal strength: %d dBm\n", WiFi.RSSI());
            
            // Khởi tạo Firebase
            if (initializeFirebase()) {
                return true;
            } else {
                Serial.println(F("Firebase initialization failed"));
                return false;
            }
        } else {
            Serial.printf("\nWiFi connection attempt %d failed\n", attempt);
            if (attempt < maxRetries) {
                Serial.println(F("Retrying in 2 seconds..."));
                delay(2000);
            }
        }
    }
    
    Serial.println(F("Failed to connect to WiFi after all attempts"));
    wifiConnected = false;
    return false;
}

bool NetworkManager::initializeFirebase() {
    Serial.println(F("Initializing Firebase..."));
    
    // Sử dụng function từ firebase_config.cpp
    if (connectToFirebase()) {
        firebaseInitialized = true;
        Serial.println(F("Firebase initialized successfully"));
        
        // Khởi tạo trạng thái cảnh báo
        initializeWarningStates();
        
        // Thiết lập stream cho door_status
        setupDoorStatusStream();
        
        return true;
    } else {
        firebaseInitialized = false;
        Serial.println(F("Firebase initialization failed"));
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
    unsigned long currentTime = millis();
    
    // Kiểm tra kết nối WiFi
    if (WiFi.status() == WL_CONNECTED) {
        if (!wifiConnected) {
            Serial.println(F("WiFi reconnected"));
            wifiConnected = true;
            reconnectAttempts = 0;
            
            // Khởi tạo lại Firebase nếu cần
            if (!firebaseInitialized) {
                initializeFirebase();
            }
        }
        
        // Thiết lập lại stream nếu cần
        if (firebaseInitialized && !doorStreamStarted) {
            setupDoorStatusStream();
        }
    } else {
        // WiFi bị ngắt kết nối
        if (wifiConnected) {
            Serial.println(F("WiFi connection lost"));
            wifiConnected = false;
            firebaseInitialized = false;
            doorStreamStarted = false;
        }
        
        // Thử kết nối lại sau một khoảng thời gian
        if (currentTime - lastReconnectAttempt > 30000) { // 30 giây
            lastReconnectAttempt = currentTime;
            reconnectAttempts++;
            
            Serial.printf("Attempting WiFi reconnection #%d\n", reconnectAttempts);
            
            if (reconnectAttempts <= 5) { // Giới hạn số lần thử
                connectToWiFi();
            } else {
                Serial.println(F("Max reconnection attempts reached. Will retry in 5 minutes."));
                if (currentTime - lastReconnectAttempt > 300000) { // 5 phút
                    reconnectAttempts = 0; // Reset counter
                }
            }
        }
    }
    
    // Reset watchdog
    esp_task_wdt_reset();
}

int NetworkManager::getSignalStrength() const {
    if (wifiConnected) {
        return WiFi.RSSI();
    }
    return -999; // Không có kết nối
}

String NetworkManager::getConnectionInfo() const {
    if (wifiConnected) {
        String info = "WiFi: Connected\n";
        info += "SSID: " + WiFi.SSID() + "\n";
        info += "IP: " + WiFi.localIP().toString() + "\n";
        info += "Signal: " + String(WiFi.RSSI()) + " dBm\n";
        info += "Firebase: " + String(firebaseInitialized ? "Connected" : "Disconnected");
        return info;
    } else {
        return "WiFi: Disconnected";
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