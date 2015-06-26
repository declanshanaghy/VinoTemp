#include <Bounce2.h>

#define CTRL1 A5
#define P1A 2
#define P1B 3
#define P1C 4
#define P1D 5
#define P1E 6
#define P1F 7
#define P1G 8

#define CTRL2 A4
#define P2A 9
#define P2B 10
#define P2C 11
#define P2D 12
#define P2E 13
#define P2F A0
#define P2G A1

#define ON LOW
#define OFF HIGH

#define BTN_UP A2
#define BTN_DN A3

#define MAX_SET 70
#define MIN_SET 30

#define BOUNCE_DELAY 5

// Instantiate a Bounce object
Bounce btnUp = Bounce(); 
Bounce btnDn = Bounce(); 

int temp = 50;

void setup() {
  Serial.begin(9600);
  
  pinMode(CTRL1, OUTPUT);
  pinMode(P1A, OUTPUT);
  pinMode(P1B, OUTPUT);
  pinMode(P1C, OUTPUT);
  pinMode(P1D, OUTPUT);
  pinMode(P1E, OUTPUT);
  pinMode(P1F, OUTPUT);
  pinMode(P1G, OUTPUT);

  pinMode(CTRL2, OUTPUT);
  pinMode(P2A, OUTPUT);
  pinMode(P2B, OUTPUT);
  pinMode(P2C, OUTPUT);
  pinMode(P2D, OUTPUT);
  pinMode(P2E, OUTPUT);
  pinMode(P2F, OUTPUT);
  pinMode(P2G, OUTPUT);

  off1();
  off2();

  // Setup the button with an internal pull-up :
  pinMode(BTN_UP, INPUT_PULLUP);
  btnUp.attach(BTN_UP);
  btnUp.interval(BOUNCE_DELAY);

  // Setup the button with an internal pull-up :
  pinMode(BTN_DN, INPUT_PULLUP);
  btnDn.attach(BTN_DN);
  btnDn.interval(BOUNCE_DELAY);

  displayTemp();
}

void loop() {
  checkButtons();
}

void checkButtons() {
  btnUp.update();
  btnDn.update();

  if ( btnUp.fell() ) {
    if ( temp >= MAX_SET ) {
      temp = MIN_SET;
      //tempLimitExceeded();
    }
    else
      temp++;
  }
  if ( btnDn.fell() ) {
    if ( temp <= MIN_SET ) {
      temp = MAX_SET;
      //tempLimitExceeded();
    }
    else
      temp--;
  }

  if ( btnUp.fell() || btnDn.fell() ) {
    displayTemp();
  }
}

void tempLimitExceeded() {
  e1();
  e2();
  delay(2000);
}

void displayTemp() {
  Serial.print("temp: ");
  Serial.println(temp);
  
  int tens = temp / 10;
  displayTens(tens);
  Serial.print("tens: ");
  Serial.println(tens);
  
  int ones = temp % 10;
  displayOnes(ones);
  Serial.print("ones: ");
  Serial.println(ones);
}

void displayTens(int v) {
  switch (v) {
    case 0:
      zeroten();
      break;
    case 1:
      ten();
      break;
    case 2:
      twenty();
      break;
    case 3:
      thirty();
      break;
    case 4:
      forty();
      break;
    case 5:
      fifty();
      break;
    case 6:
      sixty();
      break;
    case 7:
      seventy();
      break;
    case 8:
      eighty();
      break;
    case 9:
      ninety();
      break;
    default:
      e1();
      break;
  }
}

void displayOnes(int v) {
  switch (v) {
    case 0:
      zeroone();
      break;
    case 1:
      one();
      break;
    case 2:
      two();
      break;
    case 3:
      three();
      break;
    case 4:
      four();
      break;
    case 5:
      five();
      break;
    case 6:
      six();
      break;
    case 7:
      seven();
      break;
    case 8:
      eight();
      break;
    case 9:
      nine();
      break;
    default:
      e2();
      break;
  }
}
void count() {
  int speed = 250;
  e1();
  e2();
  delay(speed);  
  zeroten();
  zeroone();
  delay(speed);  
  ten();
  one();
  delay(speed);  
  twenty();
  two();
  delay(speed);  
  thirty();
  three();
  delay(speed);  
  forty();
  four();
  delay(speed);  
  fifty();
  five();
  delay(speed);  
  sixty();
  six();
  delay(speed);  
  seventy();
  seven();
  delay(speed);  
  eighty();
  eight();
  delay(speed);  
  ninety();
  nine();
  delay(speed);  
}

void zeroten() {
  digitalWrite(P1A, ON);
  digitalWrite(P1B, ON);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, ON);
  digitalWrite(P1E, ON);
  digitalWrite(P1F, ON);
  digitalWrite(P1G, OFF);
}

void zeroone() {
  digitalWrite(P2A, ON);
  digitalWrite(P2B, ON);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, ON);
  digitalWrite(P2E, ON);
  digitalWrite(P2F, ON);
  digitalWrite(P2G, OFF);
}

void ten() {
  digitalWrite(P1A, OFF);
  digitalWrite(P1B, ON);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, OFF);
  digitalWrite(P1E, OFF);
  digitalWrite(P1F, OFF);
  digitalWrite(P1G, OFF);
}

void one() {
  digitalWrite(P2A, OFF);
  digitalWrite(P2B, ON);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, OFF);
  digitalWrite(P2E, OFF);
  digitalWrite(P2F, OFF);
  digitalWrite(P2G, OFF);
}

void twenty() {
  digitalWrite(P1A, ON);
  digitalWrite(P1B, ON);
  digitalWrite(P1C, OFF);
  digitalWrite(P1D, ON);
  digitalWrite(P1E, ON);
  digitalWrite(P1F, OFF);
  digitalWrite(P1G, ON);
}

void two() {
  digitalWrite(P2A, ON);
  digitalWrite(P2B, ON);
  digitalWrite(P2C, OFF);
  digitalWrite(P2D, ON);
  digitalWrite(P2E, ON);
  digitalWrite(P2F, OFF);
  digitalWrite(P2G, ON);
}

void thirty() {
  digitalWrite(P1A, ON);
  digitalWrite(P1B, ON);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, ON);
  digitalWrite(P1E, OFF);
  digitalWrite(P1F, OFF);
  digitalWrite(P1G, ON);
}

void three() {
  digitalWrite(P2A, ON);
  digitalWrite(P2B, ON);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, ON);
  digitalWrite(P2E, OFF);
  digitalWrite(P2F, OFF);
  digitalWrite(P2G, ON);
}

void forty() {
  digitalWrite(P1A, OFF);
  digitalWrite(P1B, ON);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, OFF);
  digitalWrite(P1E, OFF);
  digitalWrite(P1F, ON);
  digitalWrite(P1G, ON);
}

void four() {
  digitalWrite(P2A, OFF);
  digitalWrite(P2B, ON);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, OFF);
  digitalWrite(P2E, OFF);
  digitalWrite(P2F, ON);
  digitalWrite(P2G, ON);
}

void fifty() {
  digitalWrite(P1A, ON);
  digitalWrite(P1B, OFF);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, ON);
  digitalWrite(P1E, OFF);
  digitalWrite(P1F, ON);
  digitalWrite(P1G, ON);
}

void five() {
  digitalWrite(P2A, ON);
  digitalWrite(P2B, OFF);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, ON);
  digitalWrite(P2E, OFF);
  digitalWrite(P2F, ON);
  digitalWrite(P2G, ON);
}

void sixty() {
  digitalWrite(P1A, OFF);
  digitalWrite(P1B, OFF);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, ON);
  digitalWrite(P1E, ON);
  digitalWrite(P1F, ON);
  digitalWrite(P1G, ON);
}

void six() {
  digitalWrite(P2A, OFF);
  digitalWrite(P2B, OFF);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, ON);
  digitalWrite(P2E, ON);
  digitalWrite(P2F, ON);
  digitalWrite(P2G, ON);
}

void seventy() {
  digitalWrite(P1A, ON);
  digitalWrite(P1B, ON);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, OFF);
  digitalWrite(P1E, OFF);
  digitalWrite(P1F, OFF);
  digitalWrite(P1G, OFF);
}

void seven() {
  digitalWrite(P2A, ON);
  digitalWrite(P2B, ON);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, OFF);
  digitalWrite(P2E, OFF);
  digitalWrite(P2F, OFF);
  digitalWrite(P2G, OFF);
}

void eighty() {
  digitalWrite(P1A, ON);
  digitalWrite(P1B, ON);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, ON);
  digitalWrite(P1E, ON);
  digitalWrite(P1F, ON);
  digitalWrite(P1G, ON);
}

void eight() {
  digitalWrite(P2A, ON);
  digitalWrite(P2B, ON);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, ON);
  digitalWrite(P2E, ON);
  digitalWrite(P2F, ON);
  digitalWrite(P2G, ON);
}

void ninety() {
  digitalWrite(P1A, ON);
  digitalWrite(P1B, ON);
  digitalWrite(P1C, ON);
  digitalWrite(P1D, OFF);
  digitalWrite(P1E, OFF);
  digitalWrite(P1F, ON);
  digitalWrite(P1G, ON);
}

void nine() {
  digitalWrite(P2A, ON);
  digitalWrite(P2B, ON);
  digitalWrite(P2C, ON);
  digitalWrite(P2D, OFF);
  digitalWrite(P2E, OFF);
  digitalWrite(P2F, ON);
  digitalWrite(P2G, ON);
}

void e1() {
  digitalWrite(P1A, ON);
  digitalWrite(P1B, OFF);
  digitalWrite(P1C, OFF);
  digitalWrite(P1D, ON);
  digitalWrite(P1E, ON);
  digitalWrite(P1F, ON);
  digitalWrite(P1G, ON);
}

void off1() {
  digitalWrite(P1A, OFF);
  digitalWrite(P1B, OFF);
  digitalWrite(P1C, OFF);
  digitalWrite(P1D, OFF);
  digitalWrite(P1E, OFF);
  digitalWrite(P1F, OFF);
  digitalWrite(P1G, OFF);
}

void e2() {
  digitalWrite(P2A, ON);
  digitalWrite(P2B, OFF);
  digitalWrite(P2C, OFF);
  digitalWrite(P2D, ON);
  digitalWrite(P2E, ON);
  digitalWrite(P2F, ON);
  digitalWrite(P2G, ON);
}

void off2() {
  digitalWrite(P2A, OFF);
  digitalWrite(P2B, OFF);
  digitalWrite(P2C, OFF);
  digitalWrite(P2D, OFF);
  digitalWrite(P2E, OFF);
  digitalWrite(P2F, OFF);
  digitalWrite(P2G, OFF);
}

