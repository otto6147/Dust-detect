# Dust-detect

make by me

# IoT Air Quality Monitoring & Control System (ESP8266 + Firebase)

โปรเจกต์นี้คือระบบตรวจวัดคุณภาพอากาศ (PM1.0, PM2.5, PM10) พร้อมอุณหภูมิและความชื้น โดยใช้บอร์ด **ESP8266** เชื่อมต่อกับ **Firebase Realtime Database** เพื่อรับ-ส่งข้อมูลแบบ Real-time และควบคุมอุปกรณ์ไฟฟ้าผ่าน **Relay** ตามค่าเกณฑ์ (Threshold) ที่ตั้งไว้จาก Node-RED

## คุณสมบัติการทำงาน

* **Sensor Monitoring:** อ่านค่าฝุ่น PM1.0, PM2.5, PM10 จากเซนเซอร์ PMS3003
* **Climate Data:** อ่านค่าอุณหภูมิและความชื้นจากเซนเซอร์ DHT22
* **Cloud Connectivity:** ส่งข้อมูลทั้งหมดขึ้น Firebase ในรูปแบบ JSON
* **Dynamic Control:** รับค่าเกณฑ์การตัดสินใจ (Slider Values) จาก Firebase เพื่อมาควบคุมการเปิด-ปิด Relay แบบอัตโนมัติ
* **Fail-safe:** มีระบบ Reconnect WiFi และระบบตรวจสอบสถานะการทำงานของเซนเซอร์

## อุปกรณ์ที่ใช้ (Hardware)

* **Microcontroller:** ESP8266 (NodeMCU / WeMos D1 mini)
* **Dust Sensor:** PMS3003 (เชื่อมต่อผ่าน SoftwareSerial)
* **Temp/Humid Sensor:** DHT22
* **Actuators:** Relay 2 Channel (Active Low)

## การต่อขาใช้งาน (Pin Mapping)

| อุปกรณ์ | ขา ESP8266 | คำอธิบาย |
| --- | --- | --- |
| **DHT22** | D5 | Data Pin |
| **PMS3003 (TX)** | D3 | ขาส่งข้อมูลจากเซนเซอร์ |
| **PMS3003 (RX)** | D4 | ขารับข้อมูลจากเซนเซอร์ |
| **Relay 1** | D6 | ควบคุมอุปกรณ์ตัวที่ 1 |
| **Relay 2** | D7 | ควบคุมอุปกรณ์ตัวที่ 2 |

## ไลบรารีที่จำเป็น (Libraries)

ต้องทำการติดตั้ง Library เหล่านี้ใน Arduino IDE ก่อนทำการ Compile:

* `DHT sensor library` โดย Adafruit
* `Firebase ESP8266 Client` โดย Firebase Extended
* `SoftwareSerial` (Standard library)

## การตั้งค่าก่อนใช้งาน (Configuration)

แก้ไขข้อมูลส่วนตัวในโค้ดก่อนทำการ Upload:

```cpp
#define FIREBASE_HOST "your-project-id.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret"
#define WIFI_SSID "your-wifi-name"
#define WIFI_PASSWORD "your-wifi-password"

```

## โครงสร้างข้อมูลบน Firebase

ระบบจะส่งข้อมูลแยกเป็น 2 Path หลัก:

1. **PMS:** เก็บค่าฝุ่น `pm10`, `pm2p5`, `pm1p0`
2. **DHT:** เก็บค่า `temp` และ `humid`
3. **Thresholds:** รับค่าจาก `sliderValue/value` ถึง `sliderValue5/value` เพื่อใช้เป็นเกณฑ์ควบคุม Relay

## ตรรกะการควบคุม (Control Logic)

* ระบบจะอ่านค่าจากเซนเซอร์และเปรียบเทียบกับค่าเกณฑ์ที่ดึงมาจาก Firebase
* **เงื่อนไขการเปิด (ON):** หากค่าใดค่าหนึ่ง (PM, Temp, Humid) สูงกว่าเกณฑ์ที่ตั้งไว้ Relay จะทำงาน (ส่งสัญญาณ `LOW`)
* **เงื่อนไขการปิด (OFF):** หากค่าทุกอย่างต่ำกว่าเกณฑ์ทั้งหมด Relay จะหยุดทำงาน (ส่งสัญญาณ `HIGH`)

---
