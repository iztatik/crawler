void writeServo_1(void){
  char angle;
  while(!BT.available()){;}
  angle = BT.read();
  angle = constrain(angle,S1_home,S1_max);
  //servo_1.write(angle);
  //BT.write(angle);
  return;
}

void writeServo_2(void){
  char angle;
  while(!BT.available()){;}
  angle = BT.read();
  angle = constrain(angle,S2_home,S2_max);
  servo_2.write(angle);
  //BT.write(angle);
  return;
}
