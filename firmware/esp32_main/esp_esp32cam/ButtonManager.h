/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý nút nhấn
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include "config.h"

class ButtonManager {
private:
    bool prevSw2State;
    bool prevSw3State;
    
public:
    ButtonManager();
    void init();
    bool checkSW2Pressed();
    bool checkSW3Pressed();
    void update();
};

#endif // BUTTON_MANAGER_H