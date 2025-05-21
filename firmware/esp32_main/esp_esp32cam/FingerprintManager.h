/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý vân tay
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef FINGERPRINT_MANAGER_H
#define FINGERPRINT_MANAGER_H

#include <Adafruit_Fingerprint.h>
#include "config.h"

class FingerprintManager {
private:
    Adafruit_Fingerprint* finger;
    uint8_t id;
    uint8_t checkTask;
    int wrongFingerprintCount;
    
public:
    FingerprintManager(Stream* serial);
    ~FingerprintManager();
    
    void init();
    bool checkFingerprint(); // Trả về true nếu nhận dạng thành công
    uint8_t getFingerprintID();
    uint8_t getFingerprintEnroll();
    uint8_t deleteFingerprint(uint8_t id);
    void emptyDatabase();
    void setID(uint8_t newID);
    uint8_t readNumber();
    void setCheckTask(uint8_t task);
    uint8_t getCheckTask() const;
    int getWrongFingerprintCount() const;
    void resetWrongFingerprintCount();
    uint16_t getTemplateCount() const;
};

#endif // FINGERPRINT_MANAGER_H 