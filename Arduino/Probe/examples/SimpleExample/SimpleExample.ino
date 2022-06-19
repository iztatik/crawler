#include "ServoSquared.h"

//The equation used is based on t²

ServoSquared HeadServo;
int SweepDirection=1;

void setup() {
  HeadServo.attach(10);//or something like .attach(8,1000,2000)
}

void loop() {
  if(HeadServo.hasStopped()){// there is also .isMoving()
    if(SweepDirection==1){
      SweepDirection=-1;
      HeadServo.setupEase(60,1500);
    }
    else
    {
      SweepDirection=1;
      HeadServo.setupEase(0,1500);
    }
  }
HeadServo.updatePosition();
}
