#include "DHT.h"    //include DHT library เพื่อให้สามารถใช้ ฟังชันก์ของ DHT ได้
#include <SoftwareSerial.h>     //include เพื่อให้สามารถกำหนดขาพิน RX (รับข้อมูล) และ TX (ส่งข้อมูล) ได้
#include "FirebaseESP8266.h"    //include เพื่อให้บอร์ดสามารถเข้าถึงการใช้ firebase ได้
#include <ESP8266WiFi.h>        //include เพื่อให้บอร์ดสามารถเข้าถึงการใช้ WiFi ได้


    #define FIREBASE_HOST  "project-iot-pms-default-rtdb.firebaseio.com"   //กำหนด Host เพื่อให้ บอร์ดสามารถเข้าถึง firebase ได้
    #define FIREBASE_AUTH  "1Su0Hzwx0CobQcwm9oMGmyN81Lp7kDXLhRNgwCTc"      //กำหนด Auth เพื่อใช้ในการเข้าถึง firebase
      #define WIFI_SSID "NaMooo"           //กำหนดชื่อ Wifi เพื่อให้บอร์ดหาชื่อ WiFi เจอ
      #define WIFI_PASSWORD "youoat2547"   //ให้รหสกับบอร์ดเพื่อเข้าถึง WiFi

 #define DHTPIN D5    //กำหนดให้ขา D6 เป็นขา DHT22   
 #define DHTTYPE DHT22  //ให้ DHTTYPE ชื่อ DHT22
 #define TX D3    //กำหนดให้ขา D3 = ขา TX
 #define RX D4    //กำหนดให้ขา D4 = ขา RX
 #define relay D6    //กำหนดให้ขา D6 = relay
 #define relay2 D7   //nกำหนดให้ขา D7 = relay


    // สร้างออฟเจค
  FirebaseData firebaseData; //สร้างออบเจ็กต์ชื่อ firebaseData จากคลาส FirebaseData ใช้เพื่อสื่อสารระหว่างอุปกรณ์กับ firebase เช่น set(ส่ง)/get(รับ)
  FirebaseJson json;  //สร้างออบเจ็กต์ชื่อ json จากคลาส FirebaseJson เพื่อเก็บข้อมูลในรูปแบบ JSON ก่อนส่งไปยัง firebase 
  FirebaseJson json2; //สร้างอีกอันเพื่อส่งไปอีก path
 String path = "PMS"; //สร้างชื่อ path ของค่าฝุ่น ไว้เก็บค่าฝุ่นทั้ง 3 แบบ
  String path2 = "DHT";  //สร้างชื่อ path ของค่า DHT ไว้เก็บค่าของอุณหภูมิกับความชื่น
 



 
 
// กำหนดขาเชื่อมต่อสำหรับการสื่อสาร
  SoftwareSerial pmsSerial(TX, RX); // (TX, RX) ส่งข้อมูลแบบอนุกรม ส่งข้อมูลแบบ bit by bit(ทีละบิท) ตามความเร็ว baud
      // หากใช้เพียงสาย TX(ส่ง) หรือ RX(รับ) จะทำให้การสื่อสารเป็น ทางเดียว (Half-Duplex) หากต่อสองสายจะเป็นการสื่อสารแบบ สองทาง (Full-Duplex)


uint8_t buf[32];  // สร้าง buffer สำหรับเก็บข้อมูลที่อ่านได้จาก PMS3003 โดยเป็นตัวแปรที่เก็บค่าได้ 2กำลัง8 หรือ 0-255
                  // ไว้สำหรับเก็บข้อมูลดิบจาก PMS3003





 DHT dht(DHTPIN, DHTTYPE);  //กำหนดให้ dht เป็นประเภท DHT22 และ ขา D4 เพื่อนำไปใช้ในฟังชันก์อ่านค่า

 
 
 

 void setup() {
 Serial.begin(115200); //ให้ความเร็วในการสื่อสาร = 115200 baud 
 
 dht.begin();     //ให้ DHT ทำงาน


  
Serial.println(); //เมื่อเริ่มพิมใน Serial ให้เริ่มบรรทัดใหม่
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // ให้บอร์ดเริมเชื่อมต่อกับ WiFi โดยมีชื่อ และ รหัส ตามที่ใส่ไป
Serial.print("Connecting");  

     //ไว้ตรวจสอบว่า เชื่อมต่อ WiFi ติดรึยัง 
while (WiFi.status() != WL_CONNECTED) {   //ตรวจสอบว่าต่อ WiFi ติดไหม  
  delay(500);
  Serial.println(".");
}
    //เมื่อต่อติดแล้ว
Serial.println();
Serial.print("Connected, IP address: ");
Serial.println(WiFi.localIP());  // โชว์ IP WiFi

 


 
 Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  //ให้ firebase เริ่มทำงานโดย Host กับ Srecret key ที่กำหนดไว้
 Firebase.reconnectWiFi(true);                  // ให้ ต่อ wifi ใหม่เมื่อหลุดอัตโนมัติ
 Serial.println("Firebase connected");
 Firebase.setReadTimeout(firebaseData, 1000 * 60); // เมื่อต่อไม่ได้ติด 60 วินาที ให้ยกเลิกการเชื่อมต่อ

 Firebase.setwriteSizeLimit(firebaseData, "tiny"); //ทำหน้าที่กำหนดขนาดข้อมูลที่จะส่งไปยัง fireBase
                                                   //เลือก tiny เพราะส่งเร็วดี หากเลือกตัวอื่นอย่าง small,medium อาจทำให้ส่งช้อขึ้น แต่ก็ทำให้ข้อมูลที่ส่งมีขนาดใหญ่ขึ้นเช่นกัน 
 
        //กำหมดให้ ขา relay ส่งสัญาณออก
 pinMode(relay , OUTPUT);
 pinMode(relay2 , OUTPUT);


 
 pmsSerial.begin(9600);  //กำหนดให้ pms อ่านข้อมูลด้วยความเร็ว 9600 baud เพราะอ่านได้เร็วแค่นี้ ไม่งั้นข้อมูลจะมีความผิดพลาด
 
 }




 

 void loop() {



 double  h = 0;
 double  t = 0;


 h = dht.readHumidity();             //เก็บค่าความชื่นที่อ่านได้จาก DHT ลงใน h
 t = dht.readTemperature();         //เก็บค่าอุณหภูมิที่อ่านได้จาก DHT ลงใน t


    // โชว์ค่าเท่ๆ
 Serial.print("Humidity: ");
 Serial.print(h);
 Serial.print("% Temperature: ");
 Serial.print(t);
 Serial.println("°C ");

      //กำหนดค่าแยกเพื่อป้องกันไม่ให้ตอนที่ส่งค่าไป firebase ว่าง ทำให้โค้ดเอ๋อ อย่างน้อยก็ให้มันเป็น 0
      double pm100 = 0;
      double pm25 = 0;
      double pm10 = 0;



 
         //pm100 = pm10 , pm25 = pm2.5 , pm10 = pm1.0
       pm10 = buf[10] * 256 + buf[11];
       pm25 = buf[12] * 256 + buf[13];
       pm100 = buf[14] * 256 + buf[15];
        //กำหนดให้อ่านจาก 2 buffer เพื่อให้อ่านทั้งค่าที่เป็น low bit และ high bit และนำ high bit นำไปคูณ 256 เพราะตัว buf เก็บแค่ค่า 0-255 


       // ตรวจสอบว่า PMS ทำงานหรือไม่
  if (pmsSerial.available()) {
      Serial.println("pms ทำงาน");
  }
  else
  {
    Serial.println("pms ไม่ทำงาน");
  }
    // อ่านข้อมูลเข้ามาใน buffer หากไม่ใส่ จะทำให้ไม่มีค่าที่อ่านได้จาก PMS และขึ้นแต่ 0,0,0
      pmsSerial.readBytes(buf, 32);





      // แสดงผลค่าฝุ่นทาง Serial Monitor
      Serial.print("PM1.0: ");
      Serial.print(pm10);
      Serial.print(" µg/m³, ");

      Serial.print("PM2.5: ");
      Serial.print(pm25);
      Serial.print(" µg/m³, ");

      Serial.print("PM10: ");
      Serial.print(pm100);
      Serial.println(" µg/m³");

  

        
     Firebase.getInt(firebaseData, "sliderValue/value"); //รับค่าจาก firebase ที่รับมากจาก node-red อีกที จาก path "sliderValue/value"
      if(pm10>= firebaseData.intData() ) //กำหนดเงื่อนไขเพื่อให้ตัวกรองทำงาน
       {
        digitalWrite(relay , LOW);      //กำหนดให้ relay ปล่อยไฟ
        digitalWrite(relay2 , LOW);
       }
       Serial.print("ค่าแรก");
       Serial.println(firebaseData.intData()); //โชว์ค่าที่อ่านได้จาก firebase ที่รับมากจาก node-red อีกที ครั้งแรก
       int F1;
       F1 = firebaseData.intData(); //เก็บค่าที่รับมาจาก firebase ครั้งแรก ลงใน F1
       Serial.print("ค่า F1 = "); 
       Serial.println(F1);    //โชว์ค่าที่อ่านได้จาก firebase ที่รับมากจาก node-red อีกที ครั้งแรก
       





      //เพิ่มข้อมูลในออฟเจ็ค JSON เพื่อเตรียมส่งใน firebase
     json2.add("temp", t); //เพิ่มข้อมูล t ลงใน temp จะได้ temp : (ค่า)t ในJSON2
     json2.add("humid", h); //เพิ่มข้อมูล h ลงใน humid จะได้ humid : (ค่า)h ในJSON2
     
     //เหตุผลที่ต้องมี JSON 2 ตัว เพราะจะส่งไปคนละ path แต่หากต้องใส่ใน path เดียวก็สามารถทำได้
     json.add("pm10", pm100);  //เพิ่มข้อมูล pm10 ลงใน pm10  จะได้ pm10 : (ค่า)pm10
     json.add("pm2p5",pm25);  //เพิ่มข้อมูล pm25 ลงใน pm2p5  จะได้ pm2p5 : (ค่า)pm25
     json.add("pm1p0",pm10); //เพิ่มข้อมูล pm10 ลงใน pm1p0  จะได้ pm1p0 : (ค่า)pm100
     Firebase.set(firebaseData, path, json);  //ส่งค่าฝุ่นในรูปแบบ JSON ไปยัง fireBase โดย path ชื่อ PMS 
     Firebase.set(firebaseData, path2, json2); //ส่งค่าความชื่นและอุณหภูมิในรูปแบบ JSON ไปยัง fireBase โดย path ชื่อ DHT
     


      //รับค่าจาก firebase ที่ได้มากจาก node-red ครั้งที่ 2 จาก path "sliderValue2/value"
    Firebase.getInt(firebaseData, "sliderValue2/value");
      if(pm25 >= firebaseData.intData())
       {
        digitalWrite(relay , LOW);      //กำหนดให้ relay ปล่อยไฟ
        digitalWrite(relay2 , LOW);
       }
       Serial.print("ค่าสอง");
       Serial.println(firebaseData.intData());  //โชว์ค่าที่อ่านได้จาก firebase ที่ได้มากจาก node-red ครั้งที่2
       int F2;
       F2 = firebaseData.intData();  //เก็บค่าที่รับมาจาก firebase ครั้งที่2 ลงใน F2
       Serial.print("ค่า F2 = ");  
       Serial.println(F2);  //โชว์ค่าที่อ่านได้จาก firebase ที่ได้มากจาก node-red ครั้งที่2






        //รับค่าจาก firebase ที่ได้มากจาก node-red ครั้งที่ 3   จาก path "sliderValue3/value"
       Firebase.getInt(firebaseData, "sliderValue3/value");
      if(pm100>= firebaseData.intData() )
       {
        digitalWrite(relay , LOW);    //กำหนดให้ relay ปล่อยไฟ
        digitalWrite(relay2 , LOW);
       }
       Serial.print("ค่าสาม");
       Serial.println(firebaseData.intData()); //โชว์ค่าที่อ่านได้จาก firebase ที่ได้มากจาก node-red ครั้งที่3
       int F3;
       F3 = firebaseData.intData();   //เก็บค่าที่รับมาจาก firebase ครั้งที่3 ลงใน F3
       Serial.print("ค่า F3 = ");  
       Serial.println(F3);




        //รับค่าจาก firebase ที่ได้มากจาก node-red ครั้งที่ 4   จาก path "sliderValue4/value"
       Firebase.getInt(firebaseData, "sliderValue4/value");
      if( t >= firebaseData.intData() )
       {
        digitalWrite(relay , LOW);    //กำหนดให้ relay ปล่อยไฟ
        digitalWrite(relay2 , LOW);
       }
       Serial.print("ค่าสี่");
       Serial.println(firebaseData.intData());  //โชว์ค่าที่อ่านได้จาก firebase ที่ได้มากจาก node-red ครั้งที่4
       int F4;
       F4 = firebaseData.intData();//เก็บค่าที่รับมาจาก firebase ครั้งที่4 ลงใน F4
       Serial.print("ค่า F4 = ");   
       Serial.println(F4);




        //รับค่าจาก firebase ที่ได้มากจาก node-red ครั้งที่ 5     จาก path "sliderValue5/value"
       Firebase.getInt(firebaseData, "sliderValue5/value");
      if( h >= firebaseData.intData() )
       {
        digitalWrite(relay , LOW);      //กำหนดให้ relay ปล่อยไฟ
        digitalWrite(relay2 , LOW);
       }
       Serial.print("ค่าห้า ");
       Serial.print(firebaseData.intData());    //โชว์ค่าที่อ่านได้จาก firebase ที่ได้มากจาก node-red ครั้งที่5
       int F5;
       F5 = firebaseData.intData();   //เก็บค่าที่รับมาจาก firebase ครั้งที่5 ลงใน F5
       Serial.print("ค่า F5 = ");  
       Serial.println(F5);


      //เหตุผลที่ต้องรับค่าทั้งหมด 5 ครั้งเพราะว่า มีการส่งค่า หา 5 จำนวน โดยต้องเก็บทุกค่าที่ส่งมา เพื่อนำมาตรวจสอบเงื่อนไขสุดท้าย
      
       if (pm100 <= F1 && pm25 <= F2 && pm10 <= F3 && h <= F4 && t <= F5){  //ตรวจสอบทุกค่าต่ำกว่าเกณที่กำหนดไว้
      digitalWrite(relay , HIGH); //กำหนดให้ relay ไม่ปล่อยไฟ
      digitalWrite(relay2 , HIGH);
    }
  delay(500);
 }
