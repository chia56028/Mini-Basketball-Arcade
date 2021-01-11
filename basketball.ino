#define pin_servo 9

#define pin_led_din 12
#define pin_led_cs 10
#define pin_led_clk 11

#define pin_ir A0

#define pin_btn_x A2
#define pin_btn_y A3

#include <Servo.h>
Servo myservo; 
int pos = 0;

#include "LedControl.h"
LedControl lc=LedControl(pin_led_din, pin_led_clk, pin_led_cs, 1);

/* we always wait a bit between updates of the display */
//unsigned long delaytime=250;


int flag = 0; // 1 for start game
int game_start = 0;
int re_time = 0;
unsigned long time_previous;
int score = 0;

#include <Wire.h> //插入I2C函式庫<Wire> 


void setup() {
  Serial.begin(9600);
  
  Wire.begin(); //I2C位址與連線設定，主端不用設 

  pinMode(pin_servo, OUTPUT);
  myservo.attach(pin_servo);
  myservo.write(80);

  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
  */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}

void servo_up(){
  for (pos = 80; pos <= 220; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  // reset time & score
  re_time = 20;
  score = 0;
}

void servo_down(){
  
  for (pos = 220; pos >= 80; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void display_time_score(int t, int s){
  lc.clearDisplay(0);
  if((t/1000)%10!=0){
    lc.setDigit(0,4,t%10,false);
    lc.setDigit(0,5,(t/10)%10,false);
    lc.setDigit(0,6,(t/100)%10,false);
    lc.setDigit(0,7,(t/1000)%10,false);
  }else if((t/100)%10!=0){
    lc.setDigit(0,4,t%10,false);
    lc.setDigit(0,5,(t/10)%10,false);
    lc.setDigit(0,6,(t/100)%10,false);
  }else if((t/10)%10!=0){
    lc.setDigit(0,4,t%10,false);
    lc.setDigit(0,5,(t/10)%10,false);
  }else{
    lc.setDigit(0,4,t%10,false);
  }

  if((s/1000)%10!=0){
    lc.setDigit(0,0,s%10,false);
    lc.setDigit(0,1,(s/10)%10,false);
    lc.setDigit(0,2,(s/100)%10,false);
    lc.setDigit(0,3,(s/1000)%10,false);
  }else if((s/100)%10!=0){
    lc.setDigit(0,0,s%10,false);
    lc.setDigit(0,1,(s/10)%10,false);
    lc.setDigit(0,2,(s/100)%10,false);
  }else if((s/10)%10!=0){
    lc.setDigit(0,0,s%10,false);
    lc.setDigit(0,1,(s/10)%10,false);
  }else{
    lc.setDigit(0,0,s%10,false);
  }
}


void loop() {
  // scan button
  int x, y;
  x = analogRead(pin_btn_x);
  y = analogRead(pin_btn_y);

  if(game_start==0 && y>500){
    flag = 1;
  }
  
  if(flag == 1 && game_start == 0){
    Serial.println("game start");
    game_start = 1;
    servo_up(); //打開出球閥
    display_time_score(re_time, score); //時間、分數 歸零
    time_previous = millis(); //紀錄當前時間(for倒數計時)
  }

  if(game_start == 1){
    if(millis() - time_previous > 1000){
      re_time--;
      time_previous += 1000;
      display_time_score(re_time, score);
      
      if(re_time == 0){
        flag = 0;
        game_start = 0;
        servo_down();
      }
    }
    
    int a0 = analogRead(pin_ir);
    Serial.println(a0);
    if(a0<600){ //check whether the ball passed
      delay(100);
      score += 1;
      display_time_score(re_time, score);

      Wire.beginTransmission(3); //開始傳送，指定給從端3號 
      Wire.write("hello \n"); //寫入訊息 hello \n 
      Wire.endTransmission(); //結束通訊 
    }
  }
}
