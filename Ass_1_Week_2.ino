#include <WiFi.h>
#include <WebServer.h>

// ==========================================
// ⚠️ 1. ตั้งค่า WiFi ของคุณตรงนี้
// ==========================================
const char* ssid = "iPhone ของ Chatatast";
const char* password = "pondzanalove";

// สร้าง Web Server ที่ Port 80
WebServer server(80);

// กำหนดขา Pin 2 (ไฟ LED สีฟ้าที่ติดมากับบอร์ด ESP32)
const int ledPin = 2; 

void setup() {
  // ตั้งค่าความเร็วการสื่อสารกับคอมพิวเตอร์
  Serial.begin(115200);
  
  // ตั้งค่าให้ขา Pin 2 เป็น Output และสั่งให้ไฟดับเป็นค่าเริ่มต้น
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); 

  // ==========================================
  // 2. เริ่มขั้นตอนการเชื่อมต่อ WiFi
  // ==========================================
  Serial.println();
  Serial.print("กำลังเชื่อมต่อ WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  // วนลูปเช็คสถานะ หากยังไม่เชื่อมต่อจะแสดงจุด ...
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("เชื่อมต่อ WiFi สำเร็จแล้ว!");
  Serial.print("📌 IP Address ของ ESP32 คือ: ");
  Serial.println(WiFi.localIP()); // เราจะนำเลข IP นี้ไปใส่ใน HTML

  // ==========================================
  // 3. กำหนดเส้นทางรับคำสั่ง (Endpoints)
  // ==========================================
  
  // เมื่อได้รับคำสั่งมาที่ /on (ให้เปิดไฟ)
  server.on("/on", []() {
    digitalWrite(ledPin, HIGH);
    
    // สำคัญ: อนุญาตให้เว็บจาก VS Code (CORS) ส่งคำสั่งมาได้
    server.sendHeader("Access-Control-Allow-Origin", "*"); 
    server.send(200, "text/plain", "เปิดไฟ LED แล้ว (ON)");
    Serial.println("ได้รับคำสั่ง: เปิดไฟ");
  });

  // เมื่อได้รับคำสั่งมาที่ /off (ให้ปิดไฟ)
  server.on("/off", []() {
    digitalWrite(ledPin, LOW);
    
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "ปิดไฟ LED แล้ว (OFF)");
    Serial.println("ได้รับคำสั่ง: ปิดไฟ");
  });

  // เริ่มเปิดระบบ Server
  server.begin();
  Serial.println("Web Server เริ่มทำงานแล้ว!");
}

void loop() {
  // ให้ Server คอยตรวจจับคำสั่งที่ส่งเข้ามาตลอดเวลา
  server.handleClient();
}