/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module điều khiển cửa
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef DOOR_CONTROL_H
#define DOOR_CONTROL_H

#include <ESP32Servo.h>
#include "config.h"

class DoorControl {
private:
    Servo servo;
    bool doorOpen;
    unsigned long doorOpenTime;
    
public:
    DoorControl();
    void init();
    void openDoor();
    void closeDoor();
    void setDoorState(bool isOpen);
    bool isDoorOpen() const;
    void update(); // Gọi trong loop() để tự động đóng cửa sau khoảng thời gian
};

#endif // DOOR_CONTROL_H 