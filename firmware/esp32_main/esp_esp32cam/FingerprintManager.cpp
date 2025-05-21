/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý vân tay - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "FingerprintManager.h"
#include <Arduino.h>

FingerprintManager::FingerprintManager(Stream* serial) {
    finger = new Adafruit_Fingerprint(serial);
    id = 0;
    checkTask = 0;
    wrongFingerprintCount = 0;
}

FingerprintManager::~FingerprintManager() {
    delete finger;
}

void FingerprintManager::init() {
    finger->begin(57600);
    if (finger->verifyPassword()) {
        Serial.println("Found fingerprint sensor!");
    } else {
        Serial.println("Did not find fingerprint sensor :(");
        while (1) { delay(1); }
    }
    
    finger->getTemplateCount();
    Serial.print(F("Sensor contains: ")); 
    Serial.print(finger->templateCount); 
    Serial.println(" templates");
}

bool FingerprintManager::checkFingerprint() {
    uint8_t result = getFingerprintID();
    if (result == FINGERPRINT_OK) {
        wrongFingerprintCount = 0;
        return true;
    } else if (result == FINGERPRINT_NOTFOUND) {
        wrongFingerprintCount++;
        return false;
    }
    return false;
}

uint8_t FingerprintManager::getFingerprintID() {
    uint8_t p = finger->getImage();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }
    
    p = finger->image2Tz();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }
    
    p = finger->fingerSearch();
    if (p == FINGERPRINT_OK) {
        Serial.println("Found a print match!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_NOTFOUND) {
        Serial.println("Did not find a match");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }
    
    // Found a match!
    Serial.print("Found ID #"); Serial.print(finger->fingerID);
    Serial.print(" with confidence of "); Serial.println(finger->confidence);
    
    return FINGERPRINT_OK;
}

uint8_t FingerprintManager::getFingerprintEnroll() {
    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
    while (p != FINGERPRINT_OK) {
        p = finger->getImage();
        switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
        }
    }
 
    // OK success!
    p = finger->image2Tz(1);
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }
 
    Serial.println("Remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER) {
        p = finger->getImage();
    }
    Serial.print("ID "); Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    
    while (p != FINGERPRINT_OK) {
        p = finger->getImage();
        switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.print(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
        }
    }
 
    // OK success!
    p = finger->image2Tz(2);
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            Serial.println("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            Serial.println("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            Serial.println("Could not find fingerprint features");
            return p;
        default:
            Serial.println("Unknown error");
            return p;
    }
    
    // OK converted!
    Serial.print("Creating model for #");  Serial.println(id);
    p = finger->createModel();
    if (p == FINGERPRINT_OK) {
        Serial.println("Prints matched!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_ENROLLMISMATCH) {
        Serial.println("Fingerprints did not match");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }
 
    Serial.print("ID "); Serial.println(id);
    p = finger->storeModel(id);
    if (p == FINGERPRINT_OK) {
        Serial.println("Stored!");
        return true;
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
        return p;
    } else if (p == FINGERPRINT_BADLOCATION) {
        Serial.println("Could not store in that location");
        return p;
    } else if (p == FINGERPRINT_FLASHERR) {
        Serial.println("Error writing to flash");
        return p;
    } else {
        Serial.println("Unknown error");
        return p;
    }
}

uint8_t FingerprintManager::deleteFingerprint(uint8_t id) {
    uint8_t p = -1;
    p = finger->deleteModel(id);
    if (p == FINGERPRINT_OK) {
        Serial.println("Deleted!");
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Communication error");
    } else if (p == FINGERPRINT_BADLOCATION) {
        Serial.println("Could not delete in that location");
    } else if (p == FINGERPRINT_FLASHERR) {
        Serial.println("Error writing to flash");
    } else {
        Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    }
    return p;
}

void FingerprintManager::emptyDatabase() {
    finger->emptyDatabase();
    Serial.println("Now database is empty");
}

void FingerprintManager::setID(uint8_t newID) {
    id = newID;
}

uint8_t FingerprintManager::readNumber() {
    uint8_t num = 0;
    boolean validNum = false;
    
    while (!validNum) {
        if (Serial.available()) {
            char c = Serial.read();
            if (isdigit(c)) {
                num *= 10;
                num += c - '0';
                validNum = true;
            }
        }
    }
    return num;
}

void FingerprintManager::setCheckTask(uint8_t task) {
    checkTask = task;
}

uint8_t FingerprintManager::getCheckTask() const {
    return checkTask;
}

int FingerprintManager::getWrongFingerprintCount() const {
    return wrongFingerprintCount;
}

void FingerprintManager::resetWrongFingerprintCount() {
    wrongFingerprintCount = 0;
}

uint16_t FingerprintManager::getTemplateCount() const {
    return finger->templateCount;
} 