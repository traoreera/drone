#include <Ultrasonic.h>
#include <LoRa.h> // module de reception de donne
#include <Wire.h> // protocole de communication 
#include <PID_v1.h>//correcteur PID
#include <Servo.h>// gestion des moteur 
#include <Adafruit_MPU6050.h>//gestion des axe x,y,z

/*const int triggerPin1  = ;
const int echoPin1 = ;

const int triggerPin2 = ;
const int echoPin2 = ;

const int triggerPin3 = ;
const int echoPin3 = ; 

*/


Adafruit_MPU6050 mpu;
Ultrasonic ultrason_g(); // broche echo et trigger ==> (triggerPin, echoPin)
Ultrasonic ultrason_d(); // broche echo et trigger

//initialisation des vriable du roll pitch yaw a 0.0
const double rollSetpoint = 0.0, pitchSetpoint = 0.0, yawSetpoint = 0.0;

//variable des entre de controle 
double rollInput, pitchInput, yawInput;

//variable de sortie du PID
double rollOutput, pitchOutput, yawOutput;

//configuration des constante PID 
double rollKp = 2.0, rollKi = 0.0, rollKd = 1.0;
double pitchKp = 2.0, pitchKi = 0.0, pitchKd = 1.0;
double yawKp = 2.0, yawKi = 0.0, yawKd = 1.0;

//variable de controle 
int throttle, roll, pitch,yaw;

// mise en place de l'instance de correction du drone
PID rollPID(&rollInput, &rollOutput, &rollSetpoint, rollKp, rollKi, rollKd, DIRECT);
PID pitchPID(&pitchInput, &pitchOutput, &pitchSetpoint, pitchKp, pitchKi, pitchKd, DIRECT);
PID yawPID(&yawInput, &yawOutput, &yawSetpoint, yawKp, yawKi, yawKd, DIRECT);

// instance de gestion des moteurs 
Servo fl_motor1, fr_motor2, br_motor3, bl_motor4;

//initialisation des donnes du giro de l'accele et de lla temp
sensors_event_t a,g, temp;

void setup() {
  Serial.begin(9600);// moniteur serie on
  while(!Serial);
  Wire.begin();
  // activation du controleur pid 
  rollPID.SetMode(AUTOMATIC);
  pitchPID.SetMode(AUTOMATIC);
  yawPID.SetMode(AUTOMATIC);
  
  // configution des configurationde sortie des moteurs
  fl_motor1.attach(9 , 1000, 2000);
  fr_motor2.attach(10, 1000, 2000);
  br_motor3.attach(11, 1000, 2000);
  bl_motor4.attach(12, 1000, 2000);

  // Signal de configuration des moteur 
  fl_motor1.writeMicroseconds(1000);
  fr_motor2.writeMicroseconds(1000);
  br_motor3.writeMicroseconds(1000);
  bl_motor4.writeMicroseconds(1000);

  delay(100);
  // lencement de la rotation des moteurs
  fl_motor1.writeMicroseconds(1100);
  fr_motor2.writeMicroseconds(1100);
  br_motor3.writeMicroseconds(1100);
  bl_motor4.writeMicroseconds(1100);
}

void loop() {
  
  if (LoRa.parsePacket()){
    String receivedData ="";
    while (LoRa.available()) { // reception des valeus de controle du drone 
     receivedData += (char)LoRa.read();
    }
    sscanf(receivedData.c_str(),"%d,%d,%d,%d",&throttle, &roll, &pitch, &yaw); //
    mpu.getEvent(&a, &g,&temp);

    // information et calcule des angle d'inclinaison du drone 
    rollInput = atan2(a.acceleration.y, a.acceleration.z) * RAD_TO_DEG;
    pitchInput = atan2(-a.acceleration.x, a.acceleration.z) * RAD_TO_DEG;
    yawInput = a.acceleration.z;
    
    // Calcul des corrections PID
    rollSetpoint = map(roll, 0, 1023, -30, 30);
    pitchSetpoint = map(pitch, 0, 1023, -30, 30);
    yawSetpoint =  map(yaw, 0, 1023, -30, 30);
    
    rollPID.Compute();
    pitchPID.Compute();
    yawPID.Compute();
    
    // Calcul des commandes des moteurs en utilisant les sorties PID
    int motor1Speed = throttle + rollOutput - pitchOutput + yawOutput;
    int motor2Speed = throttle - rollOutput - pitchOutput - yawOutput;
    int motor3Speed = throttle - rollOutput + pitchOutput + yawOutput;
    int motor4Speed = throttle + rollOutput + pitchOutput - yawOutput;
    
    // Limitation des valeurs pour éviter les dépassements
    motor1Speed = constrain(motor1Speed, 1000, 2000);
    motor2Speed = constrain(motor2Speed, 1000, 2000);
    motor3Speed = constrain(motor3Speed, 1000, 2000);
    motor4Speed = constrain(motor4Speed, 1000, 2000);

    // Contrôle des moteurs avec les valeurs calculées
    fl_motor1.writeMicroseconds(motor1Speed);
    fr_motor2.writeMicroseconds(motor2Speed);
    br_motor3.writeMicroseconds(motor3Speed);
    bl_motor4.writeMicroseconds(motor4Speed); 
    // Attendre avant la prochaine itération
    delay(10);
  }

  else if (!LoRa.available()){
    //si ont na pas de donne disaponible venan du module Lora 
  }
}