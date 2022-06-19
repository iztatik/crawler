/*
ServoSquared, created by John Adlington
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
#include "Arduino.h"
#include <Servo.h>

#ifndef ServoSquared_h
#define ServoSquared_h
class ServoSquared{
  private:
  boolean HasStarted=false;
  int PositionStart=90,PositionEnd=90,DirectionGradient=0;
  double StartTime,TimePeriod_ms;
  //New addition, default time period for moving 180° so if the servo moved 90°, then it's half this time
  double DegreeTime180_ms = 2000;
  Servo ThisServo;

  public:
  void attach(int pin);
  void attach(int pin, int MinimumPulse, int MaximumPulse);

  //If this returns 0, all should be well
  int setupEase(int NewPositionEnd,double NewTimePeriod_ms);
//This uses the time set for 180°
  int setupEase(int NewPositionEnd);
  int setup180Time(double t);

  boolean hasStopped();
  boolean isMoving();
  int read();
  void updatePosition();
};
#endif