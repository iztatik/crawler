/*
ServoSquared, created by John Adlington
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
#include <Servo.h>
#include "ServoSquared.h"



  void ServoSquared::attach(int pin){
    ThisServo.attach(pin);//or .attach(pin,1000,2000)
    ThisServo.write(90);
  }
  void ServoSquared::attach(int pin, int MinimumPulse, int MaximumPulse){
    ThisServo.attach(pin, MinimumPulse, MaximumPulse);
    ThisServo.write(90);
  }

  //If this returns 0, all should be well
  int ServoSquared::setup180Time(double t){
  if(t<100 or t>100000)return 1;
  DegreeTime180_ms = t;
  return 0;
  }

//This is a new method which uses DegreeTime180_ms.
//If this returns 0, all should be well
  int ServoSquared::setupEase(int NewPositionEnd){
    if(NewPositionEnd<0 or NewPositionEnd>180)return 1;
    PositionStart=ThisServo.read();
    if(PositionStart==NewPositionEnd)return 0;//not necessarily an error
    PositionEnd=NewPositionEnd;
    long ProportionalTime = (DegreeTime180_ms * abs(ThisServo.read() - NewPositionEnd)) / 180L;
    TimePeriod_ms=int(ProportionalTime);
    HasStarted=false;
    if(PositionStart<PositionEnd)DirectionGradient=1;
    if(PositionStart>PositionEnd)DirectionGradient=-1;
    return 0;
  }

  //If this returns 0, all should be well
  int ServoSquared::setupEase(int NewPositionEnd,double NewTimePeriod_ms){
    if(NewPositionEnd<0 or NewPositionEnd>180)return 1;
    if(NewTimePeriod_ms<=0.)return 2;
    PositionStart=ThisServo.read();
    if(PositionStart==NewPositionEnd)return 0;//not necessarily an error
    PositionEnd=NewPositionEnd;
    TimePeriod_ms=NewTimePeriod_ms;
    HasStarted=false;
    if(PositionStart<PositionEnd)DirectionGradient=1;
    if(PositionStart>PositionEnd)DirectionGradient=-1;
    return 0;
  }

  boolean ServoSquared::hasStopped(){
    return DirectionGradient==0;
  }

  boolean ServoSquared::isMoving(){
    return DirectionGradient!=0;
  }

  int ServoSquared::read(){
    return ThisServo.read();
  }
  void ServoSquared::updatePosition(){
    if(DirectionGradient==0 or PositionStart==PositionEnd){
      return;//No need to move
    }
    if(HasStarted==false){
      HasStarted=true;
      StartTime=millis();
    }
    double CurrentTime=millis();
    if((CurrentTime-StartTime)>TimePeriod_ms){
      PositionStart=PositionEnd;
      HasStarted=false;
      DirectionGradient=0;
      ThisServo.write(PositionEnd);
      return;
    }
  double n, t = (CurrentTime - StartTime) / TimePeriod_ms;
  if (DirectionGradient == 1) {
    if (t < 0.5) {
      n = t * t; // t²
      n += n; // 2t²
    }
    else {
      n = 1 - t;
      n *= n; // (1-t)²
      n += n; // 2(1-t)²
      n = 1. - n; // 1 - 2(1 - t)²
    }
  }
  else { //A negative gradient
    if (t < 0.5) {
      n = t * t; // t²
      n += n;// 2t²
      n = 1. - n; // 1-2t²
    }
    else {
      n = 1. - t;
      n *= n; // (1-t)²
      n += n; // 2(1-t)²
    }
  }
  n = abs(PositionStart - PositionEnd ) * n + min(PositionStart, PositionEnd);
    ThisServo.write(int(n));
  }