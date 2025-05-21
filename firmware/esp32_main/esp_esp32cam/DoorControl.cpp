/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module điều khiển cửa - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "DoorControl.h"
#include <Arduino.h>

DoorControl::DoorControl() : doorOpen(false), doorOpenTime(0) {
}

void DoorControl::init() {
    servo.setPeriodHertz(50);
    servo.attach(PIN_SG90, 500, 2400);
    closeDoor(); // Đảm bảo cửa đóng khi khởi động
}

void DoorControl::openDoor() {
    servo.write(0);
    doorOpen = true;
    doorOpenTime = millis();
}

void DoorControl::closeDoor() {
    servo.write(70);
    doorOpen = false;
}

void DoorControl::setDoorState(bool isOpen) {
    if (isOpen) {
        openDoor();
    } else {
        closeDoor();
    }
}

bool DoorControl::isDoorOpen() const {
    return doorOpen;
}

void DoorControl::update() {
    // Tự động đóng cửa sau khoảng thời gian
    if (doorOpen && (millis() - doorOpenTime >= DOOR_OPEN_DURATION)) {
        closeDoor();
    }
} 