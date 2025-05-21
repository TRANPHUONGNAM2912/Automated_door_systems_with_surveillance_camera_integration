/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý thẻ RFID - Thực thi
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#include "RFIDManager.h"
#include <Arduino.h>

RFIDManager::RFIDManager() : 
    rfid(SS_PIN, RST_PIN), 
    cardCount(0), 
    addCardMode(false), 
    deleteCardMode(false),
    wrongRFIDCount(0) {
}

void RFIDManager::init() {
    SPI.begin();
    rfid.PCD_Init();
    
    // Thiết lập key cho MFRC522
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    
    // Đọc số lượng thẻ từ EEPROM
    cardCount = EEPROM.read(CARD_COUNT_ADDRESS);
    if (cardCount > MAX_CARDS) {
        cardCount = 0;
        EEPROM.write(CARD_COUNT_ADDRESS, cardCount);
        EEPROM.commit();
    }
    
    // Đọc danh sách thẻ từ EEPROM
    loadCardsFromEEPROM();
    
    Serial.println(F("Đã khởi tạo module RFID"));
}

bool RFIDManager::isNewCardPresent() {
    return rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial();
}

void RFIDManager::readCardUID() {
    for (byte i = 0; i < CARD_SIZE; i++) {
        cardUID[i] = rfid.uid.uidByte[i];
    }
    
    // Hiển thị thông tin thẻ
    Serial.print(F("Thẻ UID: "));
    printHex(cardUID, CARD_SIZE);
    Serial.println();
    
    // Dừng thẻ PICC
    rfid.PICC_HaltA();
    // Dừng mã hóa trên PCD
    rfid.PCD_StopCrypto1();
}

bool RFIDManager::checkCard() {
    int cardIndex = findCard(cardUID);
    
    if (cardIndex >= 0) {
        Serial.println(F("Thẻ hợp lệ!"));
        Serial.print(F("Vị trí thẻ trong bộ nhớ: "));
        Serial.println(cardIndex);
        wrongRFIDCount = 0;
        return true;
    } else {
        Serial.println(F("Thẻ không hợp lệ!"));
        wrongRFIDCount++;
        return false;
    }
}

void RFIDManager::addCard() {
    // Kiểm tra xem thẻ đã tồn tại chưa
    if (findCard(cardUID) >= 0) {
        Serial.println(F("Thẻ này đã tồn tại trong bộ nhớ!"));
        return;
    }
    
    // Kiểm tra xem có còn chỗ không
    if (cardCount >= MAX_CARDS) {
        Serial.println(F("Bộ nhớ đầy, không thể thêm thẻ mới!"));
        return;
    }
    
    // Thêm thẻ vào bộ nhớ
    for (byte i = 0; i < CARD_SIZE; i++) {
        storedCards[cardCount][i] = cardUID[i];
        EEPROM.write(RFID_START_ADDRESS + (cardCount * CARD_SIZE) + i, cardUID[i]);
    }
    
    cardCount++;
    EEPROM.write(CARD_COUNT_ADDRESS, cardCount);
    EEPROM.commit();
    
    Serial.println(F("Thẻ đã được thêm thành công!"));
    Serial.print(F("Số thẻ hiện tại: "));
    Serial.println(cardCount);
    
    // Tắt chế độ thêm thẻ
    addCardMode = false;
}

void RFIDManager::deleteCard() {
    int cardIndex = findCard(cardUID);
    
    // Kiểm tra xem thẻ có tồn tại không
    if (cardIndex < 0) {
        Serial.println(F("Thẻ này không tồn tại trong bộ nhớ!"));
        return;
    }
    
    // Xóa thẻ bằng cách dịch chuyển các thẻ phía sau lên
    for (int i = cardIndex; i < cardCount - 1; i++) {
        for (byte j = 0; j < CARD_SIZE; j++) {
            storedCards[i][j] = storedCards[i + 1][j];
            EEPROM.write(RFID_START_ADDRESS + (i * CARD_SIZE) + j, storedCards[i][j]);
        }
    }
    
    cardCount--;
    EEPROM.write(CARD_COUNT_ADDRESS, cardCount);
    EEPROM.commit();
    
    Serial.println(F("Thẻ đã được xóa thành công!"));
    Serial.print(F("Số thẻ hiện tại: "));
    Serial.println(cardCount);
    
    // Tắt chế độ xóa thẻ
    deleteCardMode = false;
}

void RFIDManager::setAddCardMode(bool mode) {
    addCardMode = mode;
    if (mode) {
        deleteCardMode = false;
    }
}

void RFIDManager::setDeleteCardMode(bool mode) {
    deleteCardMode = mode;
    if (mode) {
        addCardMode = false;
    }
}

bool RFIDManager::isAddCardMode() const {
    return addCardMode;
}

bool RFIDManager::isDeleteCardMode() const {
    return deleteCardMode;
}

int RFIDManager::getWrongRFIDCount() const {
    return wrongRFIDCount;
}

void RFIDManager::resetWrongRFIDCount() {
    wrongRFIDCount = 0;
}

byte RFIDManager::getCardCount() const {
    return cardCount;
}

// Private methods
void RFIDManager::loadCardsFromEEPROM() {
    for (byte i = 0; i < cardCount; i++) {
        for (byte j = 0; j < CARD_SIZE; j++) {
            storedCards[i][j] = EEPROM.read(RFID_START_ADDRESS + (i * CARD_SIZE) + j);
        }
    }
    
    Serial.println(F("Đã đọc danh sách thẻ từ EEPROM"));
}

int RFIDManager::findCard(byte card[CARD_SIZE]) {
    for (byte i = 0; i < cardCount; i++) {
        bool match = true;
        for (byte j = 0; j < CARD_SIZE; j++) {
            if (storedCards[i][j] != card[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return i;
        }
    }
    return -1;  // Không tìm thấy
}

void RFIDManager::printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
} 