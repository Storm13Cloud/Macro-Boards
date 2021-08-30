//#include <Keyboard.h>
#include <HID-Project.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 1


struct rotariesdef {
  byte pin1;
  byte pin2;
  int ccwchar;
  int cwchar;
  volatile unsigned char state;
};

rotariesdef rotaries[NUMROTARIES] {
  {1,0,MEDIA_VOLUME_UP,MEDIA_VOLUME_DOWN,0},
};


// 
//Encoders code from Ben Buxton
//More info: http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html
// 

#define DIR_CCW 0x10
#define DIR_CW 0x20

#define R_START 0x0
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};




void setup() 
{

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  
  Consumer.begin();
  Keyboard.begin();
  Serial.begin (115200);
  Serial.println ("Starting.");
  rotary_init();
}

void loop() 
{
  CheckAllEncoders();
  
  if (digitalRead(2) == HIGH) {
    Consumer.write(MEDIA_NEXT);
    delay(100);
  }
  if (digitalRead(3) == HIGH) {
    Consumer.write(MEDIA_PLAY_PAUSE);
    delay(100);
  }
  if (digitalRead(4) == HIGH) {
    Consumer.write(MEDIA_PREVIOUS);
    delay(100);
  }
  if (digitalRead(5) == HIGH) {
    Keyboard.write(KEY_F24);
    delay(100);
  }
  if (digitalRead(6) == HIGH) {
    Keyboard.write(KEY_F23);
    delay(100);
  }
  if (digitalRead(7) == HIGH) {
    Keyboard.write(KEY_F22);
    delay(100);
  }
  if (digitalRead(8) == HIGH) {
    Keyboard.write(KEY_F21);
    delay(100);
  }
}

/* Call this once in setup(). */
void rotary_init() {
  for (int i=0;i<NUMROTARIES;i++) {
    pinMode(rotaries[i].pin1, INPUT);
    pinMode(rotaries[i].pin2, INPUT);
    #ifdef ENABLE_PULLUPS
      digitalWrite(rotaries[i].pin1, HIGH);
      digitalWrite(rotaries[i].pin2, HIGH);
    #endif
  }
}

unsigned char rotary_process(int _i) {
   unsigned char pinstate = (digitalRead(rotaries[_i].pin2) << 1) | digitalRead(rotaries[_i].pin1);
  rotaries[_i].state = ttable[rotaries[_i].state & 0xf][pinstate];
  return (rotaries[_i].state & 0x30);
}

void CheckAllEncoders(void) {
  for (int i=0;i<NUMROTARIES;i++) {
    unsigned char result = rotary_process(i);
    if (result) {
      Serial.print(rotaries[i].cwchar);
      if (i == 0){
        Consumer.write(result == DIR_CCW ? rotaries[i].ccwchar : rotaries[i].cwchar ); 
      }
//      else if (i == 2){
//        if (result == DIR_CCW){
//          Serial.print("works");
//          Keyboard.write(rotaries[i].ccwchar);
//        }
//        else {
//          Keyboard.write(rotaries[i].cwchar);
//         }
//      }
//      else Keyboard.write(result == DIR_CCW ? rotaries[i].ccwchar : rotaries[i].cwchar );
//      
//      //
//
    }
  }
}
