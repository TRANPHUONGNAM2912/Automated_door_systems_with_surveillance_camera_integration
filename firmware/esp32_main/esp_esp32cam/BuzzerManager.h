/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý còi báo động
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef BUZZER_MANAGER_H
#define BUZZER_MANAGER_H

#include "config.h"

class BuzzerManager {
private:
    bool buzzerActive;
    bool sw2BuzzerActive;
    unsigned long buzzerStartTime;
    unsigned long sw2BuzzerStartTime;
    
public:
    BuzzerManager();
    void init();
    void activateBuzzer();
    void activateSW2Buzzer();
    void update();
    bool isBuzzerActive() const;
    bool isSW2BuzzerActive() const;
};

#endif // BUZZER_MANAGER_H 