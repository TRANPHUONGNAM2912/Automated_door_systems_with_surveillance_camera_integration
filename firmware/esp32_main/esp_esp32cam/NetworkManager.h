/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý mạng và Firebase
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <WiFi.h>
#include <FirebaseESP32.h>
#include "config.h"

// Định nghĩa kiểu callback để tránh lỗi biên dịch
typedef void (*StreamDataCallback)(StreamData);
typedef void (*StreamTimeoutCallback)(bool);

class NetworkManager {
private:
    FirebaseData firebaseData;
    FirebaseData firebaseDataSender;
    FirebaseConfig config;
    FirebaseAuth auth;
    bool wifiConnected;
    bool firebaseInitialized;
    bool doorStreamStarted;
    bool lastDoorStatus;
    
    void setupDoorStatusStream();
    static void doorStatusCallback(StreamData data);
    static void doorStreamTimeoutCallback(bool timeout);
    
public:
    NetworkManager();
    bool init();
    bool isWiFiConnected() const;
    bool isFirebaseInitialized() const;
    void updateDoorStatus(bool isOpen);
    void updateBellStatus(bool isActive);
    void updateWarningStatus(const char* warningType, bool isActive);
    void initializeWarningStates();
    void reconnectIfNeeded();
    
    // Setter để thiết lập callback function
    void setDoorStatusCallback(StreamDataCallback callback);
    void setDoorStreamTimeoutCallback(StreamTimeoutCallback callback);
};

#endif // NETWORK_MANAGER_H 