//T0005 - Test

// Pololu Github - https://github.com/pololu/dual-vnh5019-motor-shield/
#include <DualVNH5019MotorShield.h>
// Pololu Github - https://github.com/pololu/pushbutton-arduino
#include <Pushbutton.h>

// --- define PINS
#define LED_PIN 13
#define BUTTON_PIN 11
#define RC_START 3 //Remote Controll(StertModule)
#define RC_KILL 5  //Remote Controll(StertModule)
#define SM_LEFT  16 //SenseModule Left
#define SM_CNT   17 //SenseModule Centre
#define SM_RIGHT 18 //SenseModule Right

//rotation direction
#define RIGHT 0
#define LEFT 1

// --- define Variables
// Speed: 0 is stopped, 400 is full speed, -400 reverce 
int FULL_SPEED = 400;         //MAX motor speed
int m1Speed = FULL_SPEED / 2; //Initially motor speed
int m2Speed = FULL_SPEED / 2; //Initially motor speed
int REVERSE_SPEED = 200;      //Reverse motor speed
int TURN_SPEED = 250;         //Turn motor 
int FORWARD_SPEED = 250;      //Forward motor speed
int STOP = 0;                 //Stop motor
int Border_Distance = 1000;    //MAX distance, if bigger = error = 0


//Variables 
byte PS0, PS1, PS2;      //Robot detection(Proximity sensor) vars
byte RDM;                //Robot detection Matrix
byte First_RDM=B10101010;//Robot detection Matrix, loop before
byte buttonState = 0;    //Start buttun
byte RC_Start_State = 0; //Start Module satart state
byte RC_Stop_State = 0;  //Kill Module satart state 
byte goPressed = 0;      //button pressed once
byte goStart = 0;        //started once
byte rotation = LEFT;    //defult rotation to left

// --- define Objects
Pushbutton button(BUTTON_PIN); // button create and init
DualVNH5019MotorShield motors; // MotorDriver create

void RDMprint()
{
Serial.print("PS0:");
Serial.print(PS0);
Serial.print(", PS1:");
Serial.print(PS1);
Serial.print(", PS2:");
Serial.print(PS2);
Serial.print(", Start:");
Serial.print(RC_Start_State);
//Serial.print(", Stop:");
//Serial.print(RC_Stop_State);
Serial.print(" = ");
Serial.println(RDM, BIN);
};

void SensorsRead()
{
  // --- Read Proximity sensors 
  PS0=digitalRead(SM_LEFT);
  PS1=digitalRead(SM_CNT);
  PS2=digitalRead(SM_RIGHT);

 
  // --- Read Start-Stop astates
  RC_Start_State = digitalRead(RC_START);
  RC_Stop_State = digitalRead(RC_KILL);

  RDM=0xFF;
  RDM=RDM<<1|1;  
  RDM=RDM<<1|1;
  RDM=RDM<<1|1;
  RDM=RDM<<1|1;
  RDM=RDM<<1|!PS2;
  RDM=RDM<<1|!PS1;
  RDM=RDM<<1|!PS0;
  RDM=RDM<<1|!RC_Start_State;
  //RDM=RDM<<1|!RC_Stop_State;
  RDM=~RDM;
};
 
void setup()
{   
  Serial.begin(115200);
  Serial.println("rotation = LEFT");
 
  //Init Pins
  pinMode(SM_LEFT, INPUT);
  pinMode(SM_CNT, INPUT);
  pinMode(SM_RIGHT, INPUT);
  pinMode(RC_START, INPUT);
  pinMode(RC_KILL, INPUT);
    
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(LED_PIN, LOW);

  motors.init();                 // MotorDriver init
  Serial.println("motors.init");

  Serial.println("setup done!");
}

void loop()
{
SensorsRead();

buttonState = button.isPressed();
if ((goPressed != HIGH) and (buttonState == HIGH))
  {
    goPressed = 1; //set to High after first press    
    rotation = RIGHT;
    Serial.println("rotation = RIGHT");
  };
digitalWrite(LED_PIN, rotation);

    
if (First_RDM != RDM)//If no changes into RDM, do nothing
  {
    RDMprint(); //debug output of Sharp reading
    First_RDM = RDM;//Save current RDM
    switch(RDM){//Main case, as FSM serch current state of RDM
    case B00001111://Go forward
      motors.setSpeeds(FULL_SPEED, FULL_SPEED); 
      Serial.println("Forward");
    break;
    case B00000011://Go rotate left
      motors.setSpeeds(0, FULL_SPEED); 
      Serial.println("Fast Left");
      rotation=LEFT;
      break;
    case B00000111://Go rotate left
      motors.setSpeeds(FORWARD_SPEED, FULL_SPEED); 
      Serial.println("Left");
      rotation=LEFT;   
    break;
    case B00001001://Go rotate right
      motors.setSpeeds(FULL_SPEED, 0);
      Serial.println("Fast Right");
      rotation=RIGHT;
    break;
    case B00001101://Go rotate right
      motors.setSpeeds(FULL_SPEED, FORWARD_SPEED); 
      Serial.println("Right");
      rotation=RIGHT;
    break;
    case B00001000:
    case B00000010:
    case B00001110:
    case B00000000:    
      motors.setSpeeds(STOP, STOP); 
      Serial.println("Stop");
      rotation=RIGHT;
    break;    
    default:
      switch(rotation){
        if (RC_Start_State == HIGH)//Start then RC_Start pin HIGH, Kill pin LOW
        {     
        switch(rotation){
            case LEFT: motors.setSpeeds(-TURN_SPEED, TURN_SPEED); break;
            case RIGHT: motors.setSpeeds(TURN_SPEED, -TURN_SPEED);break;  
          };
      };
    };
    Serial.println("Find");
  };
};
}
