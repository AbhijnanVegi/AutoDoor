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
    int j= 0;
    for (int i = 0; i < 25; i++)
    {

      readings[j]+= readin[i];
      if((i+1)%5==0){
         readings[j] /=5;
         Serial.println(readings[j]);
         j++;
       }




    }
    Serial.println("-----------");
   for (int i =0;i <4;i++){
      if (readings[i+1] - readings[i] > 3 || readings[i+1] - readings[i] < -3){
        nomotion = 0;
        break;
      }
    }
    if(!nomotion)
    {
        for (int i =0;i <4;i++){
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
    sense_motion = digitalRead(PIR);
    int readings[5] = {0};
    com_closer = read_ultrasonic(readings);
    if (sense_motion)
    {
      Serial.println("Motion detected");
      if(com_closer)
      {

            myservo.write(90);
            delay(5000);
            myservo.write(0);
      }
    }
    else
    {
      Serial.println("No motion detected");
        myservo.write(0);
    }
    delay(200);
}
