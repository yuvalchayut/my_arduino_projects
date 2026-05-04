//#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SHA256.h>


void print_number(int num);
void print_number_full(uint8_t Seconds, uint8_t Minutes, uint8_t Hours);
bool write_password_to_eeprom(uint8_t* pass);
char get_user_input();
int hendel_user();
void rest_clock();
bool validate_password(uint8_t* pass);
bool check_for_pass();


#define EEPROM_DEFAULT_START 4
#define BATTON_PIN_DEFAULT_START 34
#define BATTON_LIST_LEN 11
#define PRINT_4_SEGMENT_DELAY 1000
#define CHANGE_TIME_BATTON 0
#define CHANGE_PASS_BATTON 1
#define CHANGE_BATTON 10
#define FIRST_PIN 22
#define SECOND_PIN 45
#define PASS_HASH 32
#define PASS_LEN 10


//const uint32_t SALT = 0x9E377199;
//password related
char last_char = 0;
unsigned long password = 0;

SHA256 sha256;


//EEPROM vars 
int eeprom_base_address = 2;// i have chosen 2 becose i played around with 0-1
int counterValue = 0;
int eepromAddress = 0;
unsigned long password_hash = 0;

//corect time vars
unsigned long last_clock_cycle = 0;
unsigned long last_print = 0;
unsigned long currentMillis = 0;

//pins conected to arduino
int button_arr[BATTON_LIST_LEN];
int pinA1 = 22;
int pinB1 = 23;
int pinC1 = 24;
int pinD1 = 25;
int pinE1 = 26;
int pinF1 = 27;
int pinG1 = 28;
int D1 = 29;
int D2 = 30;
int D3 = 31;
int D4 = 32;

//
int pinA2 = 45;
int pinB2 = 46;
int pinC2 = 47;
int pinD2 = 48;
int pinE2 = 49;
int pinF2 = 50;
int pinG2 = 51;
int D5 = 52;
int D6 = 53;
int D7 = 20;
int D8 = 21;
//

uint8_t seconds = 0;
uint8_t minutes = 0;
uint8_t hours = 0;
unsigned long count = 0;
int potentiometer = A5;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(9600);
  // check the eeprom_base_address is valid
  EEPROM.get(eeprom_base_address, counterValue);
  if(counterValue == -1)
  {
    Serial.println("seting divice veriabels");
    EEPROM.put(eeprom_base_address, EEPROM_DEFAULT_START); 
  }


  int i = 0;
  for(i = 0; i < BATTON_LIST_LEN; i++)
  {
    button_arr[i] = BATTON_PIN_DEFAULT_START + i;
    pinMode(button_arr[i], INPUT);
    //Serial.println(i + BATTON_PIN_DEFAULT_START);
  }
  
  pinMode(potentiometer, INPUT);
  pinMode(pinA1, OUTPUT);
  pinMode(pinB1, OUTPUT);
  pinMode(pinC1, OUTPUT);
  pinMode(pinD1, OUTPUT);
  pinMode(pinE1, OUTPUT);
  pinMode(pinF1, OUTPUT);
  pinMode(pinG1, OUTPUT);
  pinMode(pinA2, OUTPUT);
  pinMode(pinB2, OUTPUT);
  pinMode(pinC2, OUTPUT);
  pinMode(pinD2, OUTPUT);
  pinMode(pinE2, OUTPUT);
  pinMode(pinF2, OUTPUT);
  pinMode(pinG2, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  


  //write_password_to_eeprom(12313);
  EEPROM.get(eeprom_base_address, eepromAddress); 
  Serial.print("Read from EEPROM: ");
  Serial.println(eepromAddress);
  
  delay(1000);
}

// the loop routine runs over and over again forever:
void loop() {
  currentMillis = millis();
  if (currentMillis - last_clock_cycle >= 1000)
  {
    last_clock_cycle += 1000;
    seconds++;
    if (seconds % 100 > 59)
    {
      seconds = 0;
      minutes++;
      if(minutes % 100 > 59)
      {
        minutes = 0;
        hours++;
      }
    }
    hours = hours % 24;
  }
  if (currentMillis - last_print >= 10)
  {
    last_print += 10;
    print_number_full(seconds, minutes, hours);
  }
  hendel_user();
  //get_user_input();
  
  
}

//gets a number and desplays it on the seven segment
void print_number(int num, int start)
{
  
  int pinA = start;
  int pinB = start + 1;
  int pinC = start + 2;
  int pinD = start + 3;
  int pinE = start + 4;
  int pinF = start + 5;
  int pinG = start + 6;
  switch (num)
  {
  case 0:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, HIGH);
    break;
  case 1:
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 2:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, LOW);
    break;
  case 3:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, LOW);
    break;
  case 4:
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, LOW);
    break;
  case 5:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, LOW);
    break;
  case 6:
    digitalWrite(pinA,  LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  LOW);
    digitalWrite(pinG, LOW);
    break;
  case 7:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 8:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, LOW);
    break;
  case 9:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, LOW);
    break;
  }
}

//gives print_number the write numbers and is in charg of liting the corect screens
void print_number_full(uint8_t Seconds, uint8_t Minutes, uint8_t Hours)
{
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, HIGH);
  digitalWrite(D8, HIGH);
  int digit = 0;

  digit = Hours % 10;
  Hours = Hours / 10;
  digitalWrite(D2, HIGH);
  print_number(digit, FIRST_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D2, LOW);

  digit = Seconds % 10;
  Seconds = Seconds / 10;
  digitalWrite(D6, HIGH);
  print_number(digit, SECOND_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D6, LOW);

  digit = Minutes % 10;
  Minutes = Minutes / 10;
  digitalWrite(D4, HIGH);
  print_number(digit, FIRST_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D4, LOW);

  digit = Hours % 10;
  digitalWrite(D1, HIGH);
  print_number(digit, FIRST_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D1, LOW);
  
  digit = Seconds % 10;
  digitalWrite(D5, HIGH);
  print_number(digit, SECOND_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D5, LOW);
  
  

  digit = Minutes % 10;
  digitalWrite(D3, HIGH);
  print_number(digit, FIRST_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D3, LOW);

 
}

//puts a value in the eeprom and checks its ok if not it moves the eeprom location to a fress place and returns false else retur true
bool write_password_to_eeprom(uint8_t* pass)
{
  uint8_t test_num[PASS_HASH];
  uint8_t result[PASS_HASH];
  sha256.reset();
  sha256.update(pass, 10);
  sha256.finalize(result, PASS_HASH);
  EEPROM.put(eepromAddress, result); 
  Serial.print("result: ");
  EEPROM.get(eepromAddress, test_num); 
  if(memcmp(test_num, result, PASS_HASH) != 0)
  {
    eepromAddress += PASS_HASH;
    EEPROM.put(eeprom_base_address, eepromAddress);
    Serial.println("eeprom err");
    return false;
  }
  return true;
}

//encripts the password so it will be abit harder to crack if you get a hold of the arduino

char get_user_input()
{
  
  int i = 0;
  int ok = 0;
  static char input_buffer;
  char input_buffer_check = input_buffer;
  
  for(i = 0; i < BATTON_LIST_LEN; i++)
  {
    if(digitalRead(button_arr[i]) == 1)
    {
      input_buffer_check = i;
      ok++;
    }
  }
  if (ok != 1)
  {
    input_buffer = -1;
    return -1;
  }
  if(input_buffer_check == input_buffer)
  {
    return -1;
  }
  input_buffer = input_buffer_check;
  Serial.print("Read from bufer: ");
  Serial.println(int(input_buffer_check));
  return input_buffer;
}

int hendel_user()
{
  unsigned long multiply = 1;
  uint8_t pass[PASS_LEN] = {11};
  for (int i = 0; i < PASS_LEN; i++)
  {
    pass[i] = 11;
  }
  //static int len = 10;
  int ok = 1;
  //static char* input_char = (char*)calloc(len, sizeof(char));
  char input = get_user_input();
  if(input == CHANGE_TIME_BATTON)
  {
    if(check_for_pass() == false)
    {
      Serial.println("wrong pass ");
      return -1;
    }
    Serial.println("ok pass ");
    while(ok)
    {
      count = count % multiply +((analogRead(potentiometer) / 100 % 10) * multiply);
      unsigned long calc_count = count;
      seconds = calc_count % 100;
      calc_count = calc_count / 100;
      minutes = calc_count % 100;
      calc_count = calc_count / 100;
      hours = calc_count % 100;
      print_number_full(seconds, minutes, hours);
      if(get_user_input() == CHANGE_BATTON)
      {
        multiply = multiply * 10;
        if(multiply >= 100001)
        {
          ok = 0;
          last_clock_cycle = millis();
          last_print = millis();
        }
      }
    }
  }
  else if(input == CHANGE_PASS_BATTON)
  {
    if(check_for_pass() == false)
    {
      Serial.println("wrong pass ");
      return -1;
    }
    Serial.println("ok pass ");
    int i = 0;
    while(ok)
    {
      input = get_user_input();
      if(input != -1 && input != CHANGE_BATTON)
      {
        //input pass
        pass[i] = input;
        i++;
       
      }
       if(i >= PASS_LEN || (input == CHANGE_BATTON && i >= 4))
        {
          ok = 0;
          write_password_to_eeprom(pass);
          rest_clock();
        }
    }
  }
  return 0;
}


void rest_clock()
{
  currentMillis = millis();
  while (currentMillis - last_clock_cycle >= 1000)
  {
    currentMillis = millis();
    if (currentMillis - last_clock_cycle >= 1000)
    {
      last_clock_cycle += 1000;
      seconds++;
      if (seconds % 100 > 59)
      {
        seconds = 0;
        minutes++;
        if(minutes % 100 > 59)
        {
          minutes = 0;
          hours++;
        }
      }
      hours = hours % 24;
    }
  }
  last_print = millis();
}

bool check_for_pass()
{
  char input = get_user_input();
  uint8_t pass[PASS_LEN] = {11};
  for (int i = 0; i < PASS_LEN; i++)
  {
    pass[i] = 11;
  }
  int i = 0;
  bool ok = true;
  while(ok)
  {
    input = get_user_input();
    if(input != -1 && input != CHANGE_BATTON)
    {
      //input pass
      pass[i] = input;
      i++;
     
    }
     if(i >= PASS_LEN || (input == CHANGE_BATTON && i >= 4))
      {
        ok = 0;
        rest_clock();
      }
  }
  return validate_password(pass);
}

bool validate_password(uint8_t* pass)
{
  uint8_t test_num[PASS_HASH];
  uint8_t result[PASS_HASH];
  sha256.reset();
  sha256.update(pass, 10);
  sha256.finalize(result, PASS_HASH);
  EEPROM.get(eepromAddress, test_num); 
  if(memcmp(test_num, result, PASS_HASH) == 0)
  {
    return true;
  }
  return false;
}
