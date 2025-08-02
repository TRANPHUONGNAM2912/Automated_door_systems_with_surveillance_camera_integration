/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * File cấu hình Firebase - Implementation
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "firebase_config.h"
#include <WiFi.h>
#include <Arduino.h>

// Firebase objects
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseDataSender;
FirebaseData firebaseDataReceiver;

void initializeFirebaseConfig() {
    // Cấu hình Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    config.timeout.serverResponse = 10 * 1000; // 10 seconds timeout
    config.timeout.rtdbKeepAlive = 45 * 1000;  // 45 seconds keep alive
    config.timeout.rtdbStreamReconnect = 1 * 1000; // 1 second reconnect
    
    // Cấu hình SSL
    config.cert.data = nullptr;
    config.cert.file = "";
    config.cert.file_storage = mem_storage_type_none;
    
    Serial.println(F("Firebase config initialized"));
}

bool connectToFirebase() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println(F("WiFi not connected, cannot connect to Firebase"));
        return false;
    }
    
    initializeFirebaseConfig();
    
    // Khởi tạo Firebase
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    
    // Test connection
    if (Firebase.ready()) {
        Serial.println(F("Firebase connected successfully"));
        
        // Khởi tạo trạng thái mặc định
        Firebase.setBool(firebaseDataSender, FIREBASE_DOOR_STATUS_PATH, false);
        Firebase.setBool(firebaseDataSender, FIREBASE_BELL_STATUS_PATH, false);
        Firebase.setBool(firebaseDataSender, FIREBASE_WARNING_PATH, false);
        Firebase.setBool(firebaseDataSender, FIREBASE_REMOTE_OPEN_PATH, false);
        Firebase.setString(firebaseDataSender, FIREBASE_SYSTEM_STATUS_PATH, "online");
        
        return true;
    } else {
        Serial.println(F("Failed to connect to Firebase"));
        Serial.println(firebaseDataSender.errorReason());
        return false;
    }
}