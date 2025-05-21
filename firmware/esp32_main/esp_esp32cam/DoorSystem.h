/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý hệ thống cửa
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef DOOR_SYSTEM_H
#define DOOR_SYSTEM_H

#include "Display.h"
#include "DoorControl.h"
#include "RFIDManager.h"
#include "FingerprintManager.h"
#include "KeypadManager.h"
#include "NetworkManager.h"
#include "BuzzerManager.h"
#include "ButtonManager.h"

class DoorSystem {
private:
    Display* display;
    DoorControl* doorControl;
    RFIDManager* rfidManager;
    FingerprintManager* fingerprintManager;
    KeypadManager* keypadManager;
    NetworkManager* networkManager;
    BuzzerManager* buzzerManager;
    ButtonManager* buttonManager;
    
    // Các biến trạng thái hệ thống
    bool doorIsOpen;
    bool adminMode;
    
    // Các phương thức xử lý sự kiện
    void handleKeypadInput();
    void handleFingerprintScan();
    void handleRFIDScan();
    void handleButtonPress();
    void handleAdminMode(char key);
    
public:
    DoorSystem();
    ~DoorSystem();
    
    void init();
    void update();
};

#endif // DOOR_SYSTEM_H 