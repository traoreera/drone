#include <Servo.h>
Servo esc;

void setup() {
  esc.attach(9,1000,2000);
  esc.write(0);
  delay(1000);
  esc.write(0);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  int i = 0;
  for(i;i<=180;i+=10){
    esc.write(i);
    delay(3000);
  }
  for(i;i>=180;i-=10){
    esc.write(i);
    delay(3000);
    if (i==0){
      break;
    }
  }
}
