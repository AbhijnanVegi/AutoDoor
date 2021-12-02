#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
char *SSID = "prerak";
char *PASS = "prerak@116";
#define cse_ip "192.168.1.12"
#define cse_port "8080"
#include "BluetoothSerial.h"
#include <Servo.h>
#define trigger 33
#define echo 32
#define PIR 34
static const int Spin = 27;
Servo myservo;
int thresh = 0;
long duration;
long distance;
bool sense_motion = 0;
bool com_closer = 0;
int calibrationTime = 60;
char inputfrombluetooth = 0;
BluetoothSerial SerialBT;


WebServer server(80);



String serveraddr = "http://" cse_ip ":" cse_port "/~/in-cse/in-name/";
String ae = "Spoodergang";
String cnt = "MotionDetected";

void createCI(String val)
{
  HTTPClient http;
  http.begin(serveraddr + ae + "/" + cnt + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  int code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");
  Serial.println(code);
  if (code == -1)
    Serial.println("Connection failed");
  http.end();
}

long getDistance(long time_taken)
{
    return 17 * time_taken / 1000;
}

void setup()
{
  Serial.begin(9600);
    SerialBT.begin("ESP32test"); //Bluetooth device name
  myservo.attach(Spin);
    pinMode(echo, INPUT);
    pinMode(trigger, OUTPUT);
    pinMode(PIR, INPUT);
     digitalWrite(PIR, LOW); 

  Serial.print("calibrating sensor "); 
  for(int i = 0; i < calibrationTime; i++)
  { Serial.print("."); 
    delay(1000); 
  } 
  Serial.println(" done"); 
  Serial.println("SENSOR ACTIVE"); delay(50); 
    myservo.write(0);
  
  // Connect to WiFi
  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  // Setup server
  server.on("/", []() {
    String page = "";
    page += "<!DOCTYPE html>\r\n";
    page += "<html>\r\n";
    page += "<body>\r\n";
    page += "<h1>Welcome to the Door Control!</h1>\r\n";
    page += "<button onclick='opendoor()'>Open</button>\r\n";
    page += "<script type=\"application/javascript\">\r\n";
    page += "function sendReq(url) {\r\n";
    page += "var xhttp = new XMLHttpRequest();\r\n";
    page += "xhttp.onreadystatechange = function(){};\r\n";
    page += "xhttp.open('GET', url, true);\r\n";
    page += "xhttp.send();\r\n";
    page += "}\r\n";
    page += "function opendoor() {\r\n";
    page += "sendReq('/open');\r\n";
    page += "}\r\n";
    page += "</script>\r\n";
    page += "</body>\r\n";
    server.send(200,"text/html",page);
  });

  server.on("/open", []() {
    server.send(200);
    myservo.write(90);
    delay(5000);
    myservo.write(0);
  });
  
  myservo.write(0);
  server.begin();
}

bool read_ultrasonic(int readings[])
{
  int readin[15];
    bool com_closer = 1;
    bool nomotion = 1;
    for (int i = 0; i < 15; i++)
    {
        digitalWrite(trigger, LOW);
        delayMicroseconds(2);
        digitalWrite(trigger, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigger, LOW);
        duration = pulseIn(echo, HIGH);
        readin[i] = getDistance(duration);
        delay(100);
    }
    int j= 0;
    for (int i = 0; i < 15; i++)
    {

      readings[j]+= readin[i];
      if((i+1)%5==0){
         readings[j] /=5;
         Serial.println(readings[j]);
         j++;
       }
      
      

     
    }
    Serial.println("-----------");
   for (int i =0;i <2;i++){
      if (readings[i+1] - readings[i] > 3 || readings[i+1] - readings[i] < -3){
        nomotion = 0;
        break;
      }
    }
    if(!nomotion)
    {
        for (int i =0;i <2;i++){
          if (readings[i+1] - readings[i] > 10){
            return 0;
          }
        }
  
      return 1;
    }
    return 0;

}

void loop()
{
  server.handleClient();
   inputfrombluetooth = SerialBT.read();
    sense_motion = digitalRead(PIR);
    int readings[5] = {0};
  
    if (sense_motion)
    {
      Serial.println("Motion detected");
      if(read_ultrasonic(readings))
      {
          createCI("1");
            myservo.write(90);
            delay(5000);
            myservo.write(0);
      }
    }
    else
    {
      createCI("0");
      Serial.println("No motion detected");
        myservo.write(0);
    }
    if(inputfrombluetooth == '0')
    {
      Serial.println("Button pressed. Opening door...");
      myservo.write(90);
    }
    else if(inputfrombluetooth == '1')
    {
      Serial.println("Button pressed. Closing door...");
      myservo.write(0);
    }
  delay(20);
    
}
