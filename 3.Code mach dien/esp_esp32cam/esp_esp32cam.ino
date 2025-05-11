#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <Key.h>
#include <Keypad.h>
#include <Keypad_I2C.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <EEPROM.h>
#include <FirebaseESP32.h>
#define I2CADDR 0x20
#define PIN_SG90 2 
#define MODEM_RX 16
#define MODEM_TX 17
#define mySerial Serial2 
#define EEPROM_SIZE 512
#define SS_PIN 5
#define RST_PIN 13
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
Servo sg90;
LiquidCrystal_I2C lcd(0x27, 16, 2);  
#define CARD_COUNT_ADDRESS 4      // Lưu số lượng thẻ tại vị trí 4
#define RFID_START_ADDRESS 5      // Bắt đầu lưu dữ liệu thẻ từ vị trí 5
#define MAX_CARDS 10              // Số lượng thẻ tối đa
#define CARD_SIZE 4               // Mỗi thẻ lưu 4 byte (UID của thẻ MIFARE)
unsigned long lastTime = 0;

unsigned long startTime;
const unsigned long timeoutDuration = 5000;
int thresholdValue = 500; 
int gasValue = 0;
int alertState = 0; 

const int sw2 = 26;
const int sw3 = 25;
bool prevSw2State = HIGH;
bool prevSw3State = HIGH;
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR);

char password[5] = "1234";
int a=0;
int check=0;
String inputString = "";
bool stringComplete = false;  
uint8_t id;
uint8_t checkTask = 0; 

unsigned long time1 = 0;
int valuepir=0;
boolean check1 = false;
int flag=0,flag1=0,flag3=0,flag2=0,flag4=0;
int count=0;
unsigned long previousMillis = 0;
const long interval = 5000; // Thời gian đèn sáng (5 giây)
bool pirState = false;
unsigned long previousGasMillis = 0;
const long gasInterval = 5000; // Thời gian chân 14 giữ mức HIGH (5 giây)
bool gasState = false;  // Trạng thái của giá trị gas

unsigned long previousMillis1 = 0;  // Biến lưu thời điểm LED được bật
const long interval1 = 5000;        // Khoảng thời gian LED sáng (3 giây)
bool ledState1 = LOW;               // Trạng thái của LED
// Khai báo đối tượng
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
// Biến lưu trạng thái
byte cardUID[4];
byte storedCards[MAX_CARDS][CARD_SIZE];
byte cardCount = 0;
bool addCardMode = false;
bool deleteCardMode = false;

// Thêm thông tin WiFi
const char* ssid = "wifi";
const char* passwordwf = "11111111";

// Thêm chân buzzer
#define BUZZER_PIN 14

// Thêm biến đếm số lần thất bại
int wrongPasswordCount = 0;
int wrongFingerprintCount = 0;
int wrongRFIDCount = 0;

// Thêm biến cho timer buzzer
unsigned long buzzerStartTime = 0;
bool buzzerActive = false;
const long buzzerDuration = 2000; // Buzzer duration 2 seconds

// Firebase configuration
#define FIREBASE_HOST "esp32-cam-d2349-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "P093Gr5A3Sv850Cz4yShUU7n3lNB2jM4jbVGxXpB"

// Tạo hai đối tượng FirebaseData riêng biệt, một cho streaming và một cho gửi dữ liệu
FirebaseData firebaseData;       // Đối tượng cho streaming door_status
FirebaseData firebaseDataSender; // Đối tượng mới cho việc gửi dữ liệu
FirebaseConfig config;
FirebaseAuth auth;

// Add global variables to track warning states
bool fingerprintWarning = false;
bool passwordWarning = false;
bool rfidWarning = false;

// Add a variable to track complete password attempts
int completePasswordAttempts = 0;

const int  BUZZER_SW2_PIN =27 ; // Chân chuông khi nút SW2 được nhấn

// Biến quản lý chuông khi SW2 được nhấn
bool sw2BuzzerActive = false;
unsigned long sw2BuzzerStartTime = 0;
const long sw2BuzzerDuration = 3000;  // Thời gian chuông kêu 1 giây

// Biến để lưu trạng thái cửa trước đó
bool lastDoorStatus = false;
bool doorStreamStarted = false;

// Biến để theo dõi trạng thái của cửa (true = mở, false = đóng)
bool doorIsOpen = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  keypad.begin();
  
  // Khởi tạo servo
  sg90.setPeriodHertz(50); 
  sg90.attach(PIN_SG90, 500, 2400);
  sg90.write(70);
    pinMode(sw2, OUTPUT);
  digitalWrite(sw2, HIGH);
    pinMode(sw3, OUTPUT);
  digitalWrite(sw3, HIGH);
  // Khởi tạo LCD
  lcd.init(); 
  lcd.backlight();
  
  // Kết nối WiFi với timeout 5 giây
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("DANG KET NOI");
  lcd.setCursor(8,1);
  lcd.print("WIFI");
  
  WiFi.begin(ssid, passwordwf);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < 5000) {
    Serial.print(".");
    delay(500);
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("DA KET NOI WIFI");
    delay(1000);
    
    // Initialize Firebase after WiFi is connected
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    config.timeout.serverResponse = 10 * 1000; // 10 seconds timeout

    // Initialize Firebase
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    
    Serial.println("Firebase initialized");
    
    // Initialize warning states
    initializeWarningStates();
    
    // Thiết lập stream cho door_status
    setupDoorStatusStream();
  } else {
    Serial.println("\nFailed to connect to WiFi");
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("KHONG KET NOI");
    lcd.setCursor(3,1);
    lcd.print("DUOC WIFI");
    delay(1000);
  }
  
  // Khởi tạo EEPROM
  EEPROM.begin(EEPROM_SIZE);
  
  // Khởi tạo SPI và RFID
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
  
  // Khởi tạo cảm biến vân tay
  while (!Serial);
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  
  finger.getTemplateCount();
  Serial.print(F("Sensor contains: ")); Serial.print(finger.templateCount); Serial.println(" templates");
  
  // Hiển thị thông báo ban đầu
  lcd.clear();
  lcd.setCursor(2,0); 
  lcd.print("NHAP PASS,RFID"); 
  lcd.setCursor(2,1); 
  lcd.print("HOAC VAN TAY");
  
  // Khởi tạo buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  // Khởi tạo chuông cho SW2
  pinMode(BUZZER_SW2_PIN, OUTPUT);
  digitalWrite(BUZZER_SW2_PIN, LOW);
}

void loop() {
  Serial.println(wrongRFIDCount);
  unsigned long currentMillis = millis();
  
  // Xử lý timer cho servo
  if (ledState1 == HIGH && currentMillis - previousMillis1 >= interval1) {
    ledState1 = LOW;
    sg90.write(90);
  }
  
  // Xử lý timer cho buzzer
  if (buzzerActive && currentMillis - buzzerStartTime >= buzzerDuration) {
    buzzerActive = false;
    digitalWrite(BUZZER_PIN, LOW);
    // Cập nhật Firebase khi chuông tắt
    updateBellStatus(false);
  }
  
  // Xử lý timer cho chuông SW2
  if (sw2BuzzerActive && currentMillis - sw2BuzzerStartTime >= sw2BuzzerDuration) {
    sw2BuzzerActive = false;
    digitalWrite(BUZZER_SW2_PIN, LOW);
    // Cập nhật Firebase khi chuông tắt
    updateBellStatus(false);
  }
  
  // Check and reset warning states on successful authentication
  checkAndResetWarnings();
  
  phimso();
  vantay();
  rfidCheck();
  led();
  
  // Kiểm tra nút SW2
  bool currentSw2State = digitalRead(sw2);
  if (currentSw2State == LOW && prevSw2State == HIGH) {
    // Nút SW2 vừa được nhấn, kích hoạt chuông trên chân 27
    sw2BuzzerActive = true;
    sw2BuzzerStartTime = millis();
    digitalWrite(BUZZER_SW2_PIN, HIGH);
    // Cập nhật Firebase khi chuông kêu
    updateBellStatus(true);
    Serial.println("Nút SW2 được nhấn, chuông trên chân 27 kêu");
  }
  prevSw2State = currentSw2State;
  bool currentSw3State = digitalRead(sw3);
  if (currentSw3State == LOW && prevSw3State == HIGH) {
    // Nút SW3 vừa được nhấn, chuyển đổi trạng thái của cửa
    doorIsOpen = !doorIsOpen;
    if (doorIsOpen) {
      updateDoorState(true);
      Serial.println("Mở cửa từ nút SW2");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("MO CUA ");
 
    } else {
      updateDoorState(false);
      Serial.println("Đóng cửa từ nút SW2");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("DONG CUA");

    }
  }
  prevSw3State = currentSw3State; 
  
  // Kiểm tra kết nối WiFi và thiết lập lại stream nếu cần
  if (WiFi.status() == WL_CONNECTED && !doorStreamStarted) {
    setupDoorStatusStream();
  }
}

void phimso(){
  static char entered_keys[5] = {'\0', '\0', '\0', '\0', '\0'};  // Lưu các phím đã nhập
  char key = keypad.getKey();
  
  if (key) {
    // Serial.println(key);
    
    // Xử lý các phím quản lý RFID
    if (key == '*') {
      checkpass();
      addCardMode = true;
      deleteCardMode = false;
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("THEM THE MOI");
      lcd.setCursor(2,1);
      lcd.print("QUET THE VAO");
      Serial.println(F("Chế độ thêm thẻ mới. Đưa thẻ lại gần đầu đọc..."));
      return;
    } 
    else if (key == '#') {
      checkpass();
      addCardMode = false;
      deleteCardMode = true;
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("XOA THE");
      lcd.setCursor(0,1);
      lcd.print("QUET THE CAN XOA");
      Serial.println(F("Chế độ xóa thẻ. Đưa thẻ cần xóa lại gần đầu đọc..."));
      return;
    }
    else if (key == 'B') {
      addCardMode = false;
      deleteCardMode = false;
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("THOAT CHE DO");
      lcd.setCursor(2,1);
      lcd.print("THEM/XOA THE");
      delay(1000);
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("NHAP PASS,RFID");
      lcd.setCursor(2,1);
      lcd.print("HOAC VAN TAY");
      Serial.println(F("Đã thoát chế độ thêm/xóa thẻ."));
      a = 0; // Reset counter
      memset(entered_keys, '\0', sizeof(entered_keys)); // Reset entered keys
      return;
    }
    else if (key == 'C') {
      checkpass();
      changePassword();
      a = 0; // Reset counter
      memset(entered_keys, '\0', sizeof(entered_keys)); // Reset entered keys
      return;
    } 
    else if (key == 'A') {
      checkpass();
      Serial.println("Checking fingerprint...");
      Serial.println("Fingerprint matched. Adding new fingerprint...");
      checkTask = 1;
      a = 0; // Reset counter
      memset(entered_keys, '\0', sizeof(entered_keys)); // Reset entered keys
      return;
    } 
    else if (key == 'D') {
      checkpass();
      Serial.println("Deleting all fingerprints...");
      lcd.clear();
      lcd.setCursor(2,0); 
      lcd.print("DELETE ALL"); 
      lcd.setCursor(2,1); 
      lcd.print("FINGERPRINTS");
      delay(1000);
      finger.emptyDatabase();
      Serial.println("Now database is empty");
      a = 0; // Reset counter
      memset(entered_keys, '\0', sizeof(entered_keys)); // Reset entered keys
      return;
    }
      
    // Xử lý nhập mật khẩu
    if (key >= '0' && key <= '9') {
      a++;
      if(a == 1){
        lcd.clear();
        memset(entered_keys, '\0', sizeof(entered_keys)); // Reset entered keys
      }
      
      if (a <= 4) {
        entered_keys[a-1] = key; // Lưu phím vào mảng
        lcd.setCursor(a+4,0);
        lcd.print("*");
      }
      
      // Đã nhập đủ 4 chữ số
      if(a == 4){
        // Kiểm tra mật khẩu
        bool correct = true;
        for (int i = 0; i < 4; i++) {
          if (entered_keys[i] != password[i]) {
            correct = false;
            break;
          }
        }
        
        if (correct) {
          // Mật khẩu đúng
          lcd.clear();
          lcd.setCursor(3,0);
          lcd.print("OPEN DOOR");
          delay(1000);
          updateDoorState(true);  // Mở cửa
          check = 1;
          lastTime = millis();
          
          // Reset counters và cảnh báo
          completePasswordAttempts = 0;
          updateFirebaseWarning("password", false);
          
          // Reset để nhập lại
          a = 0;
          memset(entered_keys, '\0', sizeof(entered_keys));
          
          // Hiển thị lại menu
          lcd.clear();
          lcd.setCursor(2,0); 
          lcd.print("NHAP PASS,RFID"); 
          lcd.setCursor(2,1); 
          lcd.print("HOAC VAN TAY");
        } else {
          // Mật khẩu sai
          completePasswordAttempts++;
          Serial.println("Mật khẩu sai. Số lần thử: " + String(completePasswordAttempts));
          
          lcd.clear();
          lcd.setCursor(2,0); 
          lcd.print("MAT KHAU SAI"); 
          lcd.setCursor(2,1); 
          lcd.print("HAY NHAP LAI");
          delay(1000);
          
          // Hiển thị lại menu sau khi báo sai
          lcd.clear();
          lcd.setCursor(2,0); 
          lcd.print("NHAP PASS,RFID"); 
          lcd.setCursor(2,1); 
          lcd.print("HOAC VAN TAY");
          
          if(completePasswordAttempts >= 3) {
             activateBuzzer();
            // Send warning to Firebase
            updateFirebaseWarning("password", true);
            // Activate buzzer
           
            // Reset counter after warning sent
            completePasswordAttempts = 0;
          }
          
          // Reset để nhập lại
          a = 0;
          memset(entered_keys, '\0', sizeof(entered_keys));
        }
      }
    }
  }
  
  if (check==1 && millis() - lastTime >= 3000) {
      updateDoorState(false);  // Đóng cửa
      check = 0;
      Serial.println("tatled");
  }
}

// Hàm kiểm tra mật khẩu đã nhập (sử dụng trong checkpass)
bool checkEnteredPassword(char* entered_password) {
  for (int i = 0; i < 4; i++) {
    if (entered_password[i] != password[i]) {
      flag++;
      return false;
    }
  }
  return true;
  flag=0;
}

void checkpass(){
    bool passwordAccepted = false;
  while (!passwordAccepted) {
    Serial.println("Enter current password:");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CURRENT PASSWORD:"); 
   
    char entered_password[5] = {'\0', '\0', '\0', '\0', '\0'};

    // Nhập mật khẩu cũ
    for (int i = 0; i < 4; i++) {
      char key = NO_KEY;
      // Đợi cho đến khi có phím nhấn hợp lệ
      while (key == NO_KEY) {
        key = keypad.getKey();
        delay(50); // Thêm delay để tránh đọc quá nhanh và debounce
        yield(); // Cho phép ESP32 xử lý các tác vụ khác
      }
      
      // Thêm debounce để tránh đọc nhiều lần khi chỉ nhấn 1 phím
      delay(150);
      
      // Xóa bỏ bất kỳ phím nào còn trong buffer
      while (keypad.getKey() != NO_KEY) {
        delay(5);
      }
      
      entered_password[i] = key;
      Serial.print('*');
      lcd.setCursor(i+5,1);
      lcd.print("*"); 
    }

    // Kiểm tra mật khẩu cũ
    if (checkEnteredPassword(entered_password)) {
      passwordAccepted = true;
      // Hiển thị phản hồi khi mật khẩu đúng
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("MAT KHAU");
      lcd.setCursor(5,1);
      lcd.print("DUNG");
      delay(1000);
    } else {
      // Hiển thị phản hồi khi mật khẩu sai
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("MAT KHAU");
      lcd.setCursor(5,1);
      lcd.print("SAI");
      delay(1000);
      Serial.println("\nIncorrect password. Please try again.");
    }
  }
}
void changePassword() {

  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("NEW PASSWORD:"); 
  Serial.println("Enter new password:");
  char new_password[5] = {'\0', '\0', '\0', '\0', '\0'};
  for (int i = 0; i < 4; i++) {
    char key = keypad.getKey();
    while (key == NO_KEY) {
      key = keypad.getKey();
      // Serial.println(i);
      if(i>0){
      lcd.setCursor(i+5,1);
      lcd.print("*"); 
      }
    }
    new_password[i] = key;
    Serial.print('*'); 

  }
  if (new_password[0] != '\0') {
    for (int i = 0; i < 4; i++) {
      password[i] = new_password[i];
      EEPROM.write(i, password[i]);
      EEPROM.commit();
    }
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("DA THAY DOI"); 
    lcd.setCursor(4,1);
    lcd.print("MAT KHAU"); 
   
    delay(1000);
    a=0;
    lcd.setCursor(2,0); 
    lcd.print("NHAP PASS,RFID"); 
    lcd.setCursor(2,1); 
    lcd.print("HOAC VAN TAY");
    Serial.println("\nPassword changed successfully!");
    flag2=0;
  } else {
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("LOI");
    delay(1000);
    Serial.println("\nPassword change canceled.");
  }
}
void vantay(){
  getFingerprintID();
  delay(50);            //don't ned to run this at full speed.
  if(checkTask == 1) //Add template fingerprint
  {
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("NHAP ID VAN TAY"); 
    lcd.setCursor(6,1); 
    lcd.print("ID=");
    id = readnumber();
    lcd.setCursor(9,1); 
    lcd.print(id); 
    checkTask = 0;
    
    if (id == 0) {// ID #0 not allowed, try again!
       return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(id);
    while (!  getFingerprintEnroll() )
    {
    }
    checkTask = 0;
  }
  else if(checkTask == 2) //check template
  {
    checkTask = 0;
    uint8_t p = finger.loadModel(id);
    if(p == FINGERPRINT_OK)
    {
      Serial.print("ID ");
      Serial.print(id);
      Serial.println(" is exist");
    }
    else
    {
      Serial.print("ID ");
      Serial.print(id);
      Serial.println(" is NO exist");
    }
  }
  else if(checkTask == 3) //delete template
  {
    checkTask = 0;
    if(id == 0) //delete all template
    {
      finger.emptyDatabase();
      Serial.println("Now database is empty");
    }
    else
      deleteFingerprint(id);
  }
}
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a 'X', set a flag so the main loop can
    // do something about it:
    if (inChar == 'X') {
      if(inputString[0] == 'C' && inputString[4] == 'X') //to check ID
      {
        id = (inputString[1] - 48)*100 + (inputString[2] - 48)*10 + (inputString[3] - 48);
        checkTask = 2;
        Serial.print("Check ID...");
        Serial.println(id);
      }
      else if(inputString == "ADDX") //add new fingerprint
      {
        Serial.println("Add fingerprint...");
        checkTask = 1;
      }
      else if(inputString[0] == 'D' && inputString[4] == 'X') //delete fingerprint in memmory
      {
        id = (inputString[1] - 48)*100 + (inputString[2] - 48)*10 + (inputString[3] - 48);
        Serial.print("Delete template ID...");
        Serial.println(id);
        checkTask = 3;
      }
    }
  }
  inputString = "";
}
 
uint8_t readnumber(void) {
  char key;
  uint8_t num = 0;

  while (num == 0) {
    key = keypad.getKey();

    // Nếu key là một số từ '1' đến '9', chuyển đổi thành số nguyên và gán cho biến num
    if (key >= '1' && key <= '9') {
      num = key - '0';  // Chuyển đổi ký tự sang số nguyên
    }
  }

  return num;
}
 
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
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
  
  p = finger.image2Tz();
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
  
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    flag4=0;
    wrongFingerprintCount = 0;
    // Successful authentication - reset warning to false
    updateFirebaseWarning("fingerprint", false);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    wrongFingerprintCount++;
    flag4++;
    
    // Hiển thị thông báo vân tay không khớp
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("VAN TAY KHONG");
    lcd.setCursor(6,1);
    lcd.print("KHOP");
    delay(1000);
    
    // Hiển thị lại menu sau thông báo
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("NHAP PASS,RFID");
    lcd.setCursor(2,1);
    lcd.print("HOAC VAN TAY");
    
    if (wrongFingerprintCount >= 3) {
        activateBuzzer();
      // Send warning to Firebase
      updateFirebaseWarning("fingerprint", true);
      // Activate buzzer
    
      // Reset counter after warning sent
      wrongFingerprintCount = 0;
    }
    
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
  
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  check1 = true;
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("OPEN DOOR");
  updateDoorState(true);  // Mở cửa
  return finger.fingerID;
}
void led(){
  unsigned long time2 = millis();
  // Serial.println(time2-time1);
  if(check1 == true){
    if(time2-time1>=3000){
      updateDoorState(false);  // Đóng cửa
      time1 = time2;
      check1 = false;
      lcd.clear();
      lcd.setCursor(2,0); 
      lcd.print("NHAP PASS,RFID"); 
      lcd.setCursor(2,1); 
      lcd.print("HOAC VAN TAY");
    }
  }
  else{
    time1=time2;
  }
}
uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
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
  p = finger.image2Tz(1);
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
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("NHA TAY");
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("DAT LAI CUNG");
  lcd.setCursor(2,1);
  lcd.print("MOT NGON TAY");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
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
  p = finger.image2Tz(2);
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

    checkTask = 0;
    lcd.clear();
    lcd.setCursor(2,0); 
    lcd.print("NHAP PASS,RFID"); 
    lcd.setCursor(2,1); 
    lcd.print("HOAC VAN TAY");

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    lcd.clear();
    lcd.setCursor(2,0); 
    lcd.print("DAU VAN SAI"); 
 
    delay(1000);
    lcd.clear();
    lcd.setCursor(2,0); 
    lcd.print("NHAP PASS,RFID"); 
    lcd.setCursor(2,1); 
    lcd.print("HOAC VAN TAY");
    return p;
    Serial.println("Fingerprints did not match");
   
    flag3=0;
    checkTask = 0;
  

    // lcd.clear();

    // lcd.setCursor(2,0); 
    // lcd.print("DAU VAN TAY"); 
    // lcd.setCursor(2,1); 
    // lcd.print("KHONG KHOP");
    
    // delay(1000);
    flag3=0;
    checkTask = 0;
  
    lcd.clear();
    lcd.setCursor(2,0); 
    lcd.print("NHAP PASS,RFID"); 
    lcd.setCursor(2,1); 
    lcd.print("HOAC VAN TAY");
  } else {
    Serial.println("Unknown error");
    return p;
  }
 
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    flag3=0;
    checkTask = 0;

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
  return true;
}
 
 
 
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  p = finger.deleteModel(id);
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
// Thêm thẻ mới vào EEPROM
void addCard() {
  // Kiểm tra xem thẻ đã tồn tại chưa
  if (findCard(cardUID) >= 0) {
    Serial.println(F("Thẻ này đã tồn tại trong bộ nhớ!"));
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("THE DA TON TAI");
    lcd.setCursor(2,1);
    lcd.print("TRONG BO NHO");
    delay(2000);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("THEM THE MOI");
    lcd.setCursor(2,1);
    lcd.print("QUET THE VAO");
    return;
  }
  
  // Kiểm tra xem có còn chỗ không
  if (cardCount >= MAX_CARDS) {
    Serial.println(F("Bộ nhớ đầy, không thể thêm thẻ mới!"));
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("BO NHO DA DAY");
    lcd.setCursor(2,1);
    lcd.print("KHONG THE THEM");
    delay(2000);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("THEM THE MOI");
    lcd.setCursor(2,1);
    lcd.print("QUET THE VAO");
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
  
  // Hiển thị thông báo thành công
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("THEM THE");
  lcd.setCursor(3,1);
  lcd.print("THANH CONG");
  delay(2000);
  
  // Tắt chế độ thêm thẻ
  addCardMode = false;
  
  // Hiển thị lại menu chính
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("NHAP PASS,RFID");
  lcd.setCursor(2,1);
  lcd.print("HOAC VAN TAY");
}

// Xóa thẻ khỏi EEPROM
void deleteCard() {
  int cardIndex = findCard(cardUID);
  
  // Kiểm tra xem thẻ có tồn tại không
  if (cardIndex < 0) {
    Serial.println(F("Thẻ này không tồn tại trong bộ nhớ!"));
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("THE KHONG TON");
    lcd.setCursor(2,1);
    lcd.print("TAI TRONG BO NHO");
    delay(2000);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("XOA THE");
    lcd.setCursor(0,1);
    lcd.print("QUET THE CAN XOA");
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
  
  // Hiển thị thông báo thành công
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("XOA THE");
  lcd.setCursor(3,1);
  lcd.print("THANH CONG");
  delay(2000);
  
  // Tắt chế độ xóa thẻ
  deleteCardMode = false;
  
  // Hiển thị lại menu chính
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("NHAP PASS,RFID");
  lcd.setCursor(2,1);
  lcd.print("HOAC VAN TAY");
}

// Kiểm tra thẻ đã quét với danh sách thẻ đã lưu
void checkCard() {
  int cardIndex = findCard(cardUID);
  
  if (cardIndex >= 0) {
    Serial.println(F("Thẻ hợp lệ!"));
    Serial.print(F("Vị trí thẻ trong bộ nhớ: "));
    Serial.println(cardIndex);
    check1 = true;
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("OPEN DOOR");
    updateDoorState(true);  // Mở cửa
    wrongRFIDCount = 0;
    // Successful authentication - reset warning to false
    updateFirebaseWarning("rfid", false);
  } else {
    Serial.println(F("Thẻ không hợp lệ!"));
    wrongRFIDCount++;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("THE KHONG HOP LE");
    lcd.setCursor(3,1);
    lcd.print("THU LAI!");
    
    if (wrongRFIDCount >= 3) {
       activateBuzzer();
       Serial.println("aaaaa");
      // Send warning to Firebase
      updateFirebaseWarning("rfid", true);
      // Activate buzzer
     
      // Reset counter after warning sent
      wrongRFIDCount = 0;
    }
    
    delay(1000);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("NHAP PASS,RFID");
    lcd.setCursor(2,1);
    lcd.print("HOAC VAN TAY");
  }
}

// Tìm thẻ trong danh sách đã lưu
int findCard(byte card[4]) {
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

// Đọc danh sách thẻ từ EEPROM
void loadCardsFromEEPROM() {
  for (byte i = 0; i < cardCount; i++) {
    for (byte j = 0; j < CARD_SIZE; j++) {
      storedCards[i][j] = EEPROM.read(RFID_START_ADDRESS + (i * CARD_SIZE) + j);
    }
  }
  
  Serial.println(F("Đã đọc danh sách thẻ từ EEPROM"));
}

// In mảng byte dưới dạng giá trị hex
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void rfidCheck() {
  // Kiểm tra thẻ RFID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }
  
  // Lưu UID của thẻ
  for (byte i = 0; i < 4; i++) {
    cardUID[i] = rfid.uid.uidByte[i];
  }
  
  // Hiển thị thông tin thẻ
  Serial.print(F("Thẻ UID: "));
  printHex(cardUID, 4);
  Serial.println();
  
  // Xử lý thẻ theo chế độ
  if (addCardMode) {
    addCard();
  } else if (deleteCardMode) {
    deleteCard();
  } else {
    checkCard();
  }
  
  // Dừng thẻ PICC
  rfid.PICC_HaltA();
  // Dừng mã hóa trên PCD
  rfid.PCD_StopCrypto1();
}

// Simplified warning management
void updateFirebaseWarning(String warningType, bool state) {
  if (WiFi.status() == WL_CONNECTED) {
    String path = "/warning/" + warningType;
    if (Firebase.setBool(firebaseDataSender, path, state)) {
      Serial.println("Cập nhật " + warningType + ": " + (state ? "true" : "false"));
    } else {
      Serial.println("Lỗi cập nhật: " + firebaseDataSender.errorReason());
    }
  }
}

// Initialize warning states in setup
void initializeWarningStates() {
  if (WiFi.status() == WL_CONNECTED) {
    updateFirebaseWarning("fingerprint", false);
    updateFirebaseWarning("password", false);
    updateFirebaseWarning("rfid", false);
    
    // Initialize bell status to false on startup
    updateBellStatus(false);
    Serial.println("Bell status initialized to false");
  }
}

// Buzzer activation function - no warning tracking
void activateBuzzer() {
  buzzerActive = true;
  buzzerStartTime = millis();
  digitalWrite(BUZZER_PIN, HIGH);
  // Cập nhật Firebase khi chuông kêu
  updateBellStatus(true);
}

// Move warning resets to loop function to ensure they happen
void checkAndResetWarnings() {
  // For fingerprint warnings
  if (fingerprintWarning && finger.fingerID != 0) {
    fingerprintWarning = false;
    updateFirebaseWarning("fingerprint", false);
  }
  
  // For password warnings - will be handled in checkPassword
  
  // For RFID warnings - handled in checkCard
}

// Thêm cập nhật trạng thái chuông lên Firebase
void updateBellStatus(bool state) {
  if (WiFi.status() == WL_CONNECTED) {
    if (Firebase.setBool(firebaseDataSender, "/bell", state)) {
      Serial.println("Cập nhật trạng thái chuông: " + String(state ? "true" : "false"));
    } else {
      Serial.println("Lỗi cập nhật trạng thái chuông: " + firebaseDataSender.errorReason());
    }
  }
}

// Hàm callback được gọi khi có thay đổi từ Firebase
void doorStatusCallback(StreamData data)
{
  if (data.dataType() == "boolean") {
    bool doorStatus = data.boolData();
    Serial.println("Nhận trạng thái cửa: " + String(doorStatus ? "true" : "false"));
    
    // Kiểm tra xem trạng thái có thay đổi không
    if (doorStatus != lastDoorStatus) {
      if (doorStatus == true) {
        // Từ false sang true: mở cửa
        updateDoorState(true);
        Serial.println("Mở cửa từ lệnh Firebase");
        // lcd.clear();
        // lcd.setCursor(0,0);
        // lcd.print("MO CUA");

      } else {
        // Từ true sang false: đóng cửa
        updateDoorState(false);
        Serial.println("Đóng cửa từ lệnh Firebase");
        // lcd.clear();
        // lcd.setCursor(0,0);
        // lcd.print("DONG CUA ");

      }
      // Cập nhật trạng thái cửa mới
      lastDoorStatus = doorStatus;
    }
  }
}

// Hàm callback khi stream timeout
void doorStreamTimeoutCallback(bool timeout)
{
  if (timeout) {
    Serial.println("Stream timeout, sẽ thử kết nối lại...");
  }
  
  if (!firebaseData.httpConnected()) {
    Serial.println("Lỗi kết nối: " + firebaseData.errorReason());
  }
}

// Hàm thiết lập stream cho door_status
void setupDoorStatusStream() {
  if (WiFi.status() == WL_CONNECTED && !doorStreamStarted) {
    // Thiết lập stream để lắng nghe thay đổi
    if (!Firebase.beginStream(firebaseData, "/door_status")) {
      Serial.println("Không thể bắt đầu stream door_status: " + firebaseData.errorReason());
    } else {
      // Đăng ký callback function
      Firebase.setStreamCallback(firebaseData, doorStatusCallback, doorStreamTimeoutCallback);
      Serial.println("Đang lắng nghe thay đổi door_status trên Firebase...");
      doorStreamStarted = true;
      
      // Đọc trạng thái ban đầu
      if (Firebase.getBool(firebaseData, "/door_status")) {
        lastDoorStatus = firebaseData.boolData();
        Serial.println("Trạng thái cửa ban đầu: " + String(lastDoorStatus ? "true" : "false"));
      }
    }
  }
}

// Hàm tiện ích để đồng bộ trạng thái cửa
void updateDoorState(bool isOpen) {
  doorIsOpen = isOpen;
  if (isOpen) {
    sg90.write(0);
    // Cập nhật trạng thái lên Firebase
    if (WiFi.status() == WL_CONNECTED) {
      Firebase.setBool(firebaseDataSender, "/door_status", true);
    }
  } else {
    sg90.write(70);
    // Cập nhật trạng thái lên Firebase
    if (WiFi.status() == WL_CONNECTED) {
      Firebase.setBool(firebaseDataSender, "/door_status", false);
    }
  }
}