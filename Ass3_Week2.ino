#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// กำหนดขา Pin 2 (ไฟ LED สีฟ้าบนบอร์ด)
const int ledPin = 2;

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;

// ตั้งค่า UUID มาตรฐานสำหรับคุยแบบ UART
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// คลาสสำหรับตรวจสอบสถานะการเชื่อมต่อ
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("เชื่อมต่อกับ iPhone แล้ว!");
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("iPhone ยกเลิกการเชื่อมต่อ");
      pServer->getAdvertising()->start(); // ปล่อยสัญญาณรอการเชื่อมต่อใหม่
    }
};

// คลาสสำหรับรอรับข้อความจาก iPhone
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue().c_str();

      if (rxValue.length() > 0) {
        char incomingChar = rxValue[0]; // อ่านตัวอักษรแรกที่ส่งมา
        
        Serial.print("ได้รับคำสั่งจากมือถือ: ");
        Serial.println(incomingChar);

        // ==========================================
        // ตรวจสอบเงื่อนไขคำสั่ง
        // ==========================================
        if (incomingChar == '1') {
          digitalWrite(ledPin, HIGH);          // เปิดไฟ
          pTxCharacteristic->setValue("ESP32: LED ON\n"); // เตรียมข้อความ
          pTxCharacteristic->notify();         // ส่งข้อความกลับไปหา iPhone
          Serial.println("สถานะ: เปิดไฟ");
        } 
        else if (incomingChar == '0') {
          digitalWrite(ledPin, LOW);           // ปิดไฟ
          pTxCharacteristic->setValue("ESP32: LED OFF\n"); // เตรียมข้อความ
          pTxCharacteristic->notify();         // ส่งข้อความกลับไปหา iPhone
          Serial.println("สถานะ: ปิดไฟ");
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // เริ่มต้นให้ไฟดับ

  // ==========================================
  // ตั้งค่าและเปิดใช้งาน BLE
  // ==========================================
  Serial.println("กำลังสร้างระบบ BLE...");
  BLEDevice::init("ESP32_MyHome_BLE"); // ชื่อบลูทูธที่จะแสดงใน iPhone
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  // สร้างช่องสำหรับส่งข้อมูลกลับ (TX)
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
  pTxCharacteristic->addDescriptor(new BLE2902());

  // สร้างช่องสำหรับรับข้อมูลเข้า (RX)
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_RX,
                        BLECharacteristic::PROPERTY_WRITE
                      );
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // เริ่มเปิดสัญญาณ BLE
  pService->start();
  pServer->getAdvertising()->start();
  
  Serial.println("ระบบ BLE เริ่มทำงานแล้ว!");
  Serial.println("📌 กรุณาใช้แอปใน iPhone ค้นหาชื่อ 'ESP32_MyHome_BLE'");
}

void loop() {
  // ในโหมด BLE การรับส่งจะทำงานผ่าน Callbacks ด้านบนอัตโนมัติ
  // ทำให้ใน loop() ไม่ต้องเขียนอะไรเพิ่มครับ
  delay(1000);
}