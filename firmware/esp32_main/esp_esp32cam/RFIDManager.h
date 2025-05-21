/*
 * Hệ thống Cửa Tự động Kết hợp Camera Giám sát
 * 
 * Module quản lý thẻ RFID
 * 
 * Tác giả: Trần Phương Nam
 * Email: tranphuongnam292003@gmail.com
 */

#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H

#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include "config.h"

class RFIDManager {
private:
    MFRC522 rfid;
    MFRC522::MIFARE_Key key;
    byte cardUID[CARD_SIZE];
    byte storedCards[MAX_CARDS][CARD_SIZE];
    byte cardCount;
    bool addCardMode;
    bool deleteCardMode;
    int wrongRFIDCount;
    
    void loadCardsFromEEPROM();
    int findCard(byte card[CARD_SIZE]);
    void printHex(byte *buffer, byte bufferSize);
    
public:
    RFIDManager();
    void init();
    bool checkCard();
    void addCard();
    void deleteCard();
    void setAddCardMode(bool mode);
    void setDeleteCardMode(bool mode);
    bool isAddCardMode() const;
    bool isDeleteCardMode() const;
    bool isNewCardPresent();
    void readCardUID();
    int getWrongRFIDCount() const;
    void resetWrongRFIDCount();
    byte getCardCount() const;
};

#endif // RFID_MANAGER_H 