
#include <Keypad_Matrix.h>
#include <HID-Project.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 4

const byte ROWS = 4;
const byte COLS = 4;



// how the keypad has its keys laid out
const char keys[ROWS][COLS] = {
  {'1', '2', '3', 'E'},
  {'7', '8', 'B', 'F'},
  {'n', 'd', '9', 'q'},
  {'*', '0', '#', 'M'},
};

const byte rowPins [ROWS] = {21, 20, 19, 18}; //connect to the row pinouts of the keypad
const byte colPins [COLS] = {15, 14, 16, 10}; //connect to the column pinouts of the keypad

  // Create the Keypad
Keypad_Matrix kpd = Keypad_Matrix( makeKeymap (keys), rowPins, colPins, ROWS, COLS );


struct rotariesdef {
  byte pin1;
  byte pin2;
  int ccwchar;
  int cwchar;
  volatile unsigned char state;
};

rotariesdef rotaries[NUMROTARIES] {
  {0,1,KEY_HOME,KEY_END,0},
  {2,3,KEY_DOWN_ARROW,KEY_UP_ARROW,0},
  {4,5,KEY_PAGE_UP,KEY_PAGE_DOWN,0},
  {6,7,MEDIA_VOLUME_UP,MEDIA_VOLUME_DOWN,0},
};


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




void keyDown (const char which)
  {
  Serial.print (F("Key down: "));
  if (which == 'E'){
    Consumer.write(MEDIA_PLAY_PAUSE);
  }
  else if (which == 'F'){
    Consumer.write(MEDIA_NEXT);
    delay(5);
  }
  else if (which == 'B'){
    Consumer.write(MEDIA_PREVIOUS);
  }
  else if (which == 'M'){
    Keyboard.write(KEY_APPLICATION);
  }
  else if (which == '7'){
    Keyboard.press(KEY_LEFT_CTRL);
  }
  else if (which == '8'){
    Consumer.write(CONSUMER_CONTROL_CONFIGURATION);
  }
  else if (which == 'q'){
    Consumer.write(CONSUMER_EXPLORER);
  }
  else if (which == '*'){
    Keyboard.write(KEY_F24);
  }
  else if (which == '0'){
    Keyboard.write(KEY_F23);
  }
  else if (which == '#'){
    Keyboard.write(KEY_F13);
  }
  else{
      Keyboard.press(which);
      Keyboard.release(which);
      }
 
  }

void keyUp (const char which)
  {
    if (which == '7'){
      Keyboard.release(KEY_LEFT_CTRL);  
    }
  }
  
void setup() 
{
  Consumer.begin();
  Keyboard.begin();
  Serial.begin (115200);
  Serial.println ("Starting.");
  kpd.begin ();
  kpd.setKeyDownHandler (keyDown);
  kpd.setKeyUpHandler (keyUp);
  rotary_init();
}

void loop() 
{
  kpd.scan ();
  CheckAllEncoders();
  // do other stuff here
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
      if (i == 3){
        Consumer.write(result == DIR_CCW ? rotaries[i].ccwchar : rotaries[i].cwchar ); 
      }
      else if (i == 2){
        if (result == DIR_CCW){
          Serial.print("works");
          Keyboard.write(rotaries[i].ccwchar);
        }
        else {
          Keyboard.write(rotaries[i].cwchar);
         }
      }
      else Keyboard.write(result == DIR_CCW ? rotaries[i].ccwchar : rotaries[i].cwchar );
      
      //

    }
  }
}
