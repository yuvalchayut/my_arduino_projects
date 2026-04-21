#include <LiquidCrystal.h>
#include <EEPROM.h>


void print_number(int num);
void print_number_full(char Seconds, char Minutes, char Hours);
bool write_password_to_eeprom(unsigned long pass);
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed);
char get_user_input();
int hendel_user();
void rest_clock();


#define EEPROM_DEFAULT_START 4
#define BATTON_PIN_DEFAULT_START 34
#define BATTON_LIST_LEN 11
#define PRINT_4_SEGMENT_DELAY 1000
#define CHANGE_TIME_BATTON 10
#define CHANGE_PASS_BATTON 11
#define FIRST_PIN 22
#define SECOND_PIN 45


const uint32_t SALT = 0x9E377199;
//password related
char last_char = 0;
unsigned long password = 0;


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

char seconds = 0;
char minutes = 0;
char hours = 0;
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
    Serial.println(i + BATTON_PIN_DEFAULT_START);
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
  if (currentMillis - last_clock_cycle >= 100)
  {
    last_clock_cycle += 100;
    seconds++;
    if (seconds % 100 > 59)
    {
      minutes++;
      if(minutes % 100 > 59)
      {
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
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF, HIGH);
    digitalWrite(pinG, LOW);
    break;
  case 1:
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF, LOW);
    digitalWrite(pinG, LOW);
    break;
  case 2:
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF,  LOW);
    digitalWrite(pinG, HIGH);
    break;
  case 3:
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  LOW);
    digitalWrite(pinG, HIGH);
    break;
  case 4:
    digitalWrite(pinA,  LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 5:
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 6:
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 7:
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  LOW);
    digitalWrite(pinG, LOW);
    break;
  case 8:
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
    break;
  case 9:
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
    break;
  }
}

//gives print_number the write numbers and is in charg of liting the corect screens
void print_number_full(char Seconds, char Minutes, char Hours)
{
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
   digitalWrite(D7, HIGH);
  digitalWrite(D8, HIGH);
  int digit = Seconds % 10;
  
  Seconds = Seconds / 10;
  digitalWrite(D6, LOW);
  print_number(digit, SECOND_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D6, HIGH);
  
  digit = Seconds % 10;
  digitalWrite(D5, LOW);
  print_number(digit, SECOND_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D5, HIGH);

  digit = Minutes % 10;
  Minutes = Minutes / 10;
  digitalWrite(D4, LOW);
  print_number(digit, FIRST_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D4, HIGH);

  digit = Minutes % 10;
  digitalWrite(D3, LOW);
  print_number(digit, FIRST_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D3, HIGH);

  digit = Hours % 10;
  Hours = Hours / 10;
  digitalWrite(D2, LOW);
  print_number(digit, FIRST_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D2, HIGH);

  digit = Hours % 10;
  digitalWrite(D1, LOW);
  print_number(digit, FIRST_PIN);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D1, HIGH);
}

//puts a value in the eeprom and checks its ok if not it moves the eeprom location to a fress place and returns false else retur true
bool write_password_to_eeprom(unsigned long pass)
{
  unsigned long test_num = 0;
  unsigned long pass_incript = murmur3_32((uint8_t*)pass, sizeof(pass), SALT);
  //EEPROM.put(eepromAddress, pass_incript); 
  Serial.print("pass_incript: ");
  Serial.println(pass_incript);
  EEPROM.get(eepromAddress, test_num); 
  if(test_num != pass_incript)
  {
    //eepromAddress += 4;
    //EEPROM.put(eeprom_base_address, eepromAddress);
    Serial.println("eeprom err");
    return false;
  }
  return true;
}

//encripts the password so it will be abit harder to crack if you get a hold of the arduino
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed) 
{
    uint32_t h = seed;
    if (len > 3) {
        const uint32_t* key_x4 = (const uint32_t*)key;
        size_t i = len >> 2;
        do {
            uint32_t k = *key_x4++;
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h = h * 5 + 0xe6546b64;
        } while (--i);
        key = (const uint8_t*)key_x4;
    }
    if (len & 3) {
        size_t i = len & 3;
        uint32_t k = 0;
        key = &key[i - 1];
        do {
            k <<= 8;
            k |= *key--;
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

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
  unsigned long pass = 0;
  //static int len = 10;
  int ok = 1;
  //static char* input_char = (char*)calloc(len, sizeof(char));
  char input = get_user_input();
  if(input == CHANGE_TIME_BATTON)
  {
    while(ok)
    {
      count = count % multiply +((analogRead(potentiometer) / 100 % 10) * multiply);
      print_number_full(seconds, minutes, hours);
      if(get_user_input() == CHANGE_TIME_BATTON)
      {
        multiply = multiply * 10;
        if(multiply >= 1001)
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
    pass = 0;
    while(ok)
    {
      input = get_user_input();
      if(input != -1 && input != CHANGE_PASS_BATTON)
      {
        //input pass
        pass += input * multiply;
        multiply = multiply * 10;
       
      }
       if(multiply >= 100000001 || (input == CHANGE_PASS_BATTON && multiply >= 1001))
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
  while (currentMillis - last_clock_cycle >= 60000)
  {
    currentMillis = millis();
    last_clock_cycle += 60000;
    count++;
    if (count % 100 > 59)
    {
      count += -(count % 100) + 100 ;
    }
    count = count % 2400;
  }
  last_print = millis();
}
