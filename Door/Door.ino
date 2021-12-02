#include <Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "secrets.h"

#define trigger 33
#define echo 32
#define PIR 34

static const int Spin = 27;

Servo myservo;
WebServer server(80);


int thresh = 0;
long duration;
long distance;
bool sense_motion = 0;
bool com_closer = 0;

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
  myservo.attach(Spin);
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  pinMode(PIR, INPUT);
  Serial.begin(9600);
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
    page += "<button onclick='open()'>Open</button>\r\n";
    page += "<script>\r\n";
    page += "function sendReq(url) {\r\n";
    page += "var xhttp = new XMLHttpRequest();\r\n";
    page += "xhttp.open('GET', url, true);\r\n";
    page += "xhttp.send();\r\n";
    page += "}\r\n";
    page += "function open() {\r\n";
    page += "sendReq('/open');\r\n";
    page += "}\r\n";
    page += "</script>\r\n";
    page += "</body>\r\n";
  });

  server.on("/open", []() {
    myservo.write(90);
    delay(1000);
    myservo.write(0);
  });

  myservo.write(0);
}

bool read_ultrasonic(int readings[])
{
  int readin[25];
  bool com_closer = 1;
  bool nomotion = 1;
  for (int i = 0; i < 25; i++)
  {
    digitalWrite(trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    duration = pulseIn(echo, HIGH);
    readin[i] = getDistance(duration);
    delay(200);
  }
  int j = 0;
  for (int i = 0; i < 25; i++)
  {

    readings[j] += readin[i];
    if ((i + 1) % 5 == 0)
    {
      readings[j] /= 5;
      Serial.println(readings[j]);
      j++;
    }
  }
  Serial.println("-----------");
  for (int i = 0; i < 4; i++)
  {
    if (readings[i + 1] - readings[i] > 3 || readings[i + 1] - readings[i] < -3)
    {
      nomotion = 0;
      break;
    }
  }
  if (!nomotion)
  {
    for (int i = 0; i < 4; i++)
    {
      if (readings[i + 1] - readings[i] > 10)
      {
        return 0;
      }
    }

    return 1;
  }
  return 0;
}

void loop()
{
  sense_motion = digitalRead(PIR);
  int readings[5] = {0};
  com_closer = read_ultrasonic(readings);
  if (sense_motion)
  {
    Serial.println("Motion detected");
    if (com_closer)
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
  delay(200);
}
