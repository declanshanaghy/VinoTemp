#include <Bounce2.h>
#include <EEPROM.h>

#define ON LOW
#define OFF HIGH
#define UPSIDE_DOWN false

#define P1A 2
#define P1B 3
#define P1C 4
#define P1D 5
#define P1E 6
#define P1F 7
#define P1G 8

#define P2A 9
#define P2B 10
#define P2C 11
#define P2D 12
#define P2E 13
#define P2F A0
#define P2G A1

#define BTN_UP A3
#define BTN_DN A2

#define P_TEMP A4
#define P_COMPRESSOR A5

#define MAX_SET 70
#define MIN_SET 30

//Temperature set window
#define TEMP_WINDOW 4

#define BOUNCE_DELAY 5
#define SETTING_DP_PERIOD 1000
#define SETTING_FLASH 1000
#define SETTING_FLASH_OFF_PERIOD 0.25
#define SETTING_TIMEOUT 5000

#define TEMP_COUNT 50
#define TEMP_INTERVAL 100

//Long term storage for variables
#define EEPROM_TEMP 0

// Instantiate a Bounce object
Bounce btnUp = Bounce(); 
Bounce btnDn = Bounce(); 

int tRead = 0;
int tSet = 0;

bool bSetting = false;
long tSetting = 0;
bool bHolding = false;
long tHolding = 0;

short iTemp = 0;
float rTemp = 0;
long tTemp = 0;

void setup() {
  Serial.begin(9600);
  initPins();
  initState();
  loadData();

  displayCount();

  rTemp = analogRead(P_TEMP);  
  convertVtoTemp(rTemp);
  displayTemp(tRead);    
}

void displayCount() {
  off1();
  off2();
  delay(250);
  
  off1();
  zeroone();
  delay(250);
  
  zeroten();
  one();
  delay(250);
  
  ten();
  two();
  delay(250);
  
  twenty();
  three();
  delay(250);
  
  thirty();
  four();
  delay(250);
  
  forty();
  five();
  delay(250);
  
  fifty();
  six();
  delay(250);
  
  sixty();
  seven();
  delay(250);
  
  seventy();
  eight();
  delay(250);
  
  eighty();
  nine();
  delay(250);

  ninety();
  off2();
  delay(250);
  
  off1();
  off2();
  delay(250);
}

void initPins() {
  pinMode(P1A, OUTPUT);
  pinMode(P1B, OUTPUT);
  pinMode(P1C, OUTPUT);
  pinMode(P1D, OUTPUT);
  pinMode(P1E, OUTPUT);
  pinMode(P1F, OUTPUT);
  pinMode(P1G, OUTPUT);

  pinMode(P2A, OUTPUT);
  pinMode(P2B, OUTPUT);
  pinMode(P2C, OUTPUT);
  pinMode(P2D, OUTPUT);
  pinMode(P2E, OUTPUT);
  pinMode(P2F, OUTPUT);
  pinMode(P2G, OUTPUT);

  pinMode(P_COMPRESSOR, OUTPUT);

  // Setup the button with an internal pull-up :
  pinMode(BTN_UP, INPUT_PULLUP);
  btnUp.attach(BTN_UP);
  btnUp.interval(BOUNCE_DELAY);

  // Setup the button with an internal pull-up :
  pinMode(BTN_DN, INPUT_PULLUP);
  btnDn.attach(BTN_DN);
  btnDn.interval(BOUNCE_DELAY);  
}

void initState() {
  bSetting = false;
  digitalWrite(P_COMPRESSOR, LOW);
  off1();
  off2();  
}

void loadData() {
  //Read the set temp from memory
  tSet = EEPROM.read(EEPROM_TEMP);
  if (tSet == 255) {
    tSet = 50;
    EEPROM.write(EEPROM_TEMP, tSet); 
  }  
}

void loop() {
  long m = millis();
  btnUp.update();
  btnDn.update();  
  
  if ( bSetting && (m - tSetting > SETTING_TIMEOUT)) {    
    Serial.println("RESET");
    bSetting = false;   
    //Write the set temp to memory
    EEPROM.write(EEPROM_TEMP, tSet);
    displayTemp(tRead);
  }

  if ( bSetting ) {
    if ( checkAdjustment() )
      tSetting = m;
      
    if ( (m - tSetting) % SETTING_FLASH < SETTING_FLASH * SETTING_FLASH_OFF_PERIOD ) {
      off1();
      off2();
    }
    else {
      displayTemp(tSet);    
    }
  }
  else {
    //checkSinglePress();
    checkDoublePress();  
    if ( readTemp() ) {
      convertVtoTemp(rTemp);
      displayTemp(tRead);
    }
  }

  if ((tRead >= tSet + TEMP_WINDOW) && digitalRead(P_COMPRESSOR) == LOW)
    digitalWrite(P_COMPRESSOR, HIGH);

  if (tRead <= tSet && digitalRead(P_COMPRESSOR) == HIGH)
    digitalWrite(P_COMPRESSOR, LOW);
}

boolean readTemp() {
  long m = millis();
  if ( millis() - tTemp > TEMP_INTERVAL) {
      iTemp++;
      rTemp += analogRead(P_TEMP);  
      tTemp = m;
//      Serial.print("rTemp1: ");
//      Serial.println(rTemp);
  }

  if ( iTemp >= TEMP_COUNT ) {
    rTemp /= TEMP_COUNT;
    return true;
  }

  return false;
}

void convertVtoTemp(int a) {
//  Serial.print("rTemp: ");
//  Serial.println(rTemp);
    
  //Map analog reading to voltage (5/1023)
  float v = 5 - (a * 0.00488758553275);
//  Serial.print("v2: ");
//  Serial.println(v);
  
  //Map voltage to temp using line formula 
  //calculated from TempMapping.xlsx
  
  //Factory Board (5.1k resistor)
  //x = (-y + 4.5635) / 0.027647
  //tRead =  (-v + 4.5635) / 0.027647;
  
  //My Board (5.5k resistor)  
  //x = (y - b) / m
  float m = -0.02;
  float b = 4.11;  
  tRead = (v - b) / m;
      
//  Serial.print("tRead: ");
//  Serial.println(tRead);
//  Serial.println("");

  rTemp = 0;
  iTemp = 0;
}

void checkSinglePress() {
  if ( btnUp.fell() || btnDn.fell() ) {
    bSetting = true;    
    tSetting = millis();   
  }
}

void checkDoublePress() {    
  bool uLow = btnUp.read() == LOW;
  int dLow = btnDn.read() == LOW;
  
  if ( !bHolding && uLow && dLow ) {
    Serial.println("STARTED HOLDING");
    bHolding = true;
    tHolding = millis();
  }

  if ( bHolding && uLow && dLow && (millis() - tHolding > SETTING_DP_PERIOD)) {
    Serial.println("STARTED SETTING");
    bHolding = false;
    bSetting = true;    
    tSetting = millis();   
  }
}

bool checkAdjustment() {
  if ( btnUp.fell() ) {
    Serial.println("UP");
    if ( tSet >= MAX_SET )
      tSet = MIN_SET;
    else
      tSet++;
  }
  if ( btnDn.fell() ) {
    Serial.println("DOWN");
    if ( tSet <= MIN_SET )
      tSet = MAX_SET;
    else
      tSet--;
  }

  return btnUp.fell() || btnDn.fell();
}

void displayTemp(int temp) {
  int tens = temp / 10;
  displayTens(tens);
  int ones = temp % 10;
  displayOnes(ones);
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

void zeroten() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, OFF);
  }
  else {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, OFF);    
  }
}

void zeroone() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, OFF);    
  }
  else {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, OFF);
  }
}

void ten() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, OFF);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, OFF);    
  }
  else {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, OFF);
    digitalWrite(P1G, OFF);    
  }
}

void one() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, OFF);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, OFF);    
  }
  else {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, OFF);
    digitalWrite(P2G, OFF);
  }
}

void twenty() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, OFF);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, OFF);
    digitalWrite(P2G, ON);
  }
  else {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, OFF);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, OFF);
    digitalWrite(P1G, ON);
  }
}

void two() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, OFF);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, OFF);
    digitalWrite(P1G, ON);    
  }
  else {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, OFF);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, OFF);
    digitalWrite(P2G, ON);
  }
}

void thirty() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, OFF);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);    
  }
  else {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, OFF);
    digitalWrite(P1G, ON);
  }
}

void three() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, OFF);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);    
  }
  else {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, OFF);
    digitalWrite(P2G, ON);
  }
}

void forty() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);    
  }
  else {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);
  }
}

void four() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);    
  }
  else {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);
  }
}

void fifty() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);    
  }
  else {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);   
  }
}

void five() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);    
  }
  else {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);
  }
}

void sixty() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);    
  }
  else {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);
  }
}

void six() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);    
  }
  else {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);
  }
}

void seventy() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, OFF);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, OFF);    
  }
  else {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, OFF);
    digitalWrite(P1G, OFF);
  }
}

void seven() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, OFF);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, OFF);    
  }
  else {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, OFF);
    digitalWrite(P2G, OFF);
  }
}

void eighty() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);    
  }
  else {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);   
  }
}

void eight() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);    
  }
  else {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);
  }
}

void ninety() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);    
  }
  else {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);    
  }
}

void nine() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);    
  }
  else {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);   
  }
}

void e1() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, ON);
    digitalWrite(P2C, ON);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, OFF);
    digitalWrite(P2G, ON);    
  }
  else {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, OFF);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, ON);
    digitalWrite(P1F, ON);
    digitalWrite(P1G, ON);
  }
}

void off1() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, OFF);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, OFF);
    digitalWrite(P2G, OFF);
  }
  else {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, OFF);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, OFF);
    digitalWrite(P1G, OFF);
  }
}

void e2() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, ON);
    digitalWrite(P1B, ON);
    digitalWrite(P1C, ON);
    digitalWrite(P1D, ON);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, OFF);
    digitalWrite(P1G, ON);    
  }
  else {
    digitalWrite(P2A, ON);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, OFF);
    digitalWrite(P2D, ON);
    digitalWrite(P2E, ON);
    digitalWrite(P2F, ON);
    digitalWrite(P2G, ON);
  }
}

void off2() {
  if ( UPSIDE_DOWN ) {
    digitalWrite(P1A, OFF);
    digitalWrite(P1B, OFF);
    digitalWrite(P1C, OFF);
    digitalWrite(P1D, OFF);
    digitalWrite(P1E, OFF);
    digitalWrite(P1F, OFF);
    digitalWrite(P1G, OFF);
  }
  else {
    digitalWrite(P2A, OFF);
    digitalWrite(P2B, OFF);
    digitalWrite(P2C, OFF);
    digitalWrite(P2D, OFF);
    digitalWrite(P2E, OFF);
    digitalWrite(P2F, OFF);
    digitalWrite(P2G, OFF);
  }
}

