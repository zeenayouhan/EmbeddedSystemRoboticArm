
#include <Servo.h>

Servo myservo[3]; //base turning
Servo myservopick;


#define baseArm A2
#define subArm A3
#define turningPoint A4
#define pick 5
#define pickServo 12

bool button = false; 
bool pickStatus = false; 
bool blockloop = false;
bool runState = false;
bool modeButton =false;
bool modeState = false;

int receivedValue = 60;
int state[3] = {120,120,60}; // define servo state in oreder to A B C

int destination[3] = {170,170,150}; // define servo destination in order to A B C

int progress[3] = {0,0,0};

int progress_state = 0;

int read_position[3]={0,0,0};

void setup() {
  // put your setup code here, to run once:
  myservo[0].attach(9); 
  myservo[1].attach(10);
  myservo[2].attach(11);
  myservopick.attach(pickServo);
  pinMode(pick,OUTPUT);
  pinMode(14,OUTPUT);
  pinMode(15,OUTPUT);
  pinMode(16,OUTPUT);
  //pinMode(17,OUTPUT);
  pinMode(18,INPUT);
  digitalWrite(14,HIGH);
  digitalWrite(15,HIGH);
  digitalWrite(16,HIGH);
  //digitalWrite(17,HIGH);
  Serial.begin(9600);
  start();
  myservopick.write(120);
}
//String y;
void loop() {
  
  stateManager();
  
  if(Serial.available()>1){
      receivedValue = map(Serial.parseInt(),0,550,80,140);
  }

  picker(digitalRead(pick));
}
void start(){
  progress_state=1;
  automated();
}

void manual(){
  int turn = map(analogRead(turningPoint),1,280,60,150);
  int sub = map(analogRead(subArm),1,260,80,170);
  int base = map(analogRead(baseArm),510,730,80,170);
  picker(digitalRead(pick));
  
     if((base>80)&&(base<170)){
     destination[0] = base;
     }
     if((sub>80)&&(sub<170)){
      destination[1] = sub;
     }
     if((turn>60)&&(turn<150)){
      destination[2] = turn;
     }

  stateManager();

  progress_state=1;
  automated();
 
}

void automated(){
  while(progress_state!=0){
    progress_state=0;
    for(int x=0;x<3;x++){
      progress[x] = servoTurn(x);
      myservo[x].write(state[x]);
      progress_state+=progress[x];
      delay(10);
      if(Serial.available()>1){
        receivedValue = map(Serial.parseInt(),0,550,80,140);
      }
      picker(digitalRead(pick));
    }
  }
}

bool servoTurn(int x){
   //myservo.write(pos);
   if(state[x]!=destination[x]){
    if(state[x]>destination[x]){
      state[x]--;
    }else{
      state[x]++;
    }
    Serial.println(state[x]);
    return true;
   }
   else return false;
}

void feedToUser(){
  destination[0]=170;
  destination[1]=100;
  destination[2]=150;
  progress_state=1;
  automated();
  destination[0]=170;
  destination[1]=150;
  destination[2]=150;
  progress_state=1;
  automated();
  destination[0]=170;
  destination[1]=150;
  destination[2]=receivedValue;
  progress_state=1;
  automated();
  destination[0]=120;
  destination[1]=110;
  destination[2]=receivedValue;
  progress_state=1;
  automated();
  destination[0]=120;
  destination[1]=150;
  destination[2]=150;
  progress_state=1;
  automated();
}

void picker(bool x){

  delay(10);
  if(x && !blockloop){
    pickStatus = !pickStatus;
    blockloop = true;
  }else if(!x && blockloop ){
    blockloop = false;
  }

  if(pickStatus){
    myservopick.write(50);
  }else{
    myservopick.write(120);
  }
}

void stateManager(){
  modeButton = digitalRead(18);
  Serial.println(modeButton);
  if(modeButton && !modeState){
    modeState = true;
    runState = !runState;
  }else if(!modeButton && modeState){
    modeState = false;
  }

  if(runState){
    feedToUser();
  }else{
    manual();
  }
}
