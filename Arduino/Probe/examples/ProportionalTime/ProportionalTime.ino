#include "ServoSquared.h"

//The equation used is based on t²

ServoSquared HeadServo;
int SweepDirection=1;

void setup() {
  HeadServo.attach(10);//Using pin 10
  //This is the time for the servo to move 180° so to move 90°, it will take half this time (default 2000)
  HeadServo.setup180Time(2000);
}

void loop() {
  //As I set up the time period to 2000 ms to move 180°, the servo should take one second
  //to move 90°
  if(HeadServo.hasStopped()){// there is also .isMoving()
    if(SweepDirection==1){
      SweepDirection=-1;
      //When you use setupEase() without a time period, it calculates the time required
      //from the time supplied with setup180Time()
      HeadServo.setupEase(60);
    }
    else
    {
      SweepDirection=1;
      HeadServo.setupEase(0);
    }
  }
HeadServo.updatePosition();
}
