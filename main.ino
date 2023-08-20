#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <LoRa.h>

int rst;
int positionX = 0.0; // analog A0
int positionY = 0.0; // analog A1
int Vitesse = 0.0;  //  analog  A2
LiquidCrystal_I2C lcd(0x27, 16, 2);// définit le type d'écran lcd pour ma part j'utilise un 16 x 2

void setup() {
  //analogReference(INTERNAL);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(3,INPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(A7,INPUT);
  rst = verification();
  if (rst != 0){
    Serial.print("stop code ...");
    Serial.print(rst,2);
  }
}
void loop()
{
  int bt1,bt2,bt3,bt4;
  bt1 = digitalRead(4);
  bt2 = digitalRead(3);
  bt3 = digitalRead(5);
  bt4 = digitalRead(7);

  //Serial.print("\tbuton1:");
  //Serial.println(bt1,2);

  //Serial.print("\tbuton2:");
  //Serial.print(bt2,2);

  positionX  = analogRead(A0);
  //positionY  = map (analogRead(A1),0,1023,0,180);
  //Vitesse = map (analogRead(A2),0,1023,0,100);
  //LCDinformation();
  //
  Serial.println(positionX);
  delay(300);
}


int verification(){
  int rst=0;
  Serial.println("Mamette starting...[ok]");
  Serial.print("lcd status : ");
  Serial.println( lcd.status(),2);
  lcd.clear();
  lcd.print("Mannete start");
  //delay(1000);
  lcd.setCursor(0,0); // positionne le curseur à la colonne 0 et à la ligne 2  
  Serial.print("verification Lora ... ");
  if (!LoRa.begin(915E6)){
    Serial.println("[Error]");
    lcd.setCursor(0,1);
    lcd.print("Lora faild");
    rst +=1;
  }
  else{
    Serial.print("[LoRa enable]");
  }
  return rst;
}

void LCDinformation(){
  /*Serial.print("\nvitesse: ");
  Serial.print("1");
  Serial.print("\t niveau de baterie:");
  Serial.print("100");*/
  lcd.clear();
  lcd.print("vst:");
  lcd.setCursor(4,0);
  lcd.print("100");
  lcd.setCursor(8, 0);
  lcd.print("bat:");
  lcd.setCursor(12, 0);
  lcd.print("100%");
  lcd.display();
}