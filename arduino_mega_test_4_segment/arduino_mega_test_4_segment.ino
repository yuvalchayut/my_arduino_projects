//#include <LiquidCrystal.h>
#include <EEPROM.h>


void print_number(int num);
void print_number_full(int num);
bool write_password_to_eeprom(unsigned long pass);
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed);
char get_user_input();
int hendel_user();
void rest_clock();


#define EEPROM_DEFAULT_START 4
#define BATTON_PIN_DEFAULT_START 34
#define BATTON_LIST_LEN 8
#define PRINT_4_SEGMENT_DELAY 1000


const uint32_t SALT = 0x9E3779B9;
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
int pinA = 22;
int pinB = 23;
int pinC = 24;
int pinD = 25;
int pinE = 26;
int pinF = 27;
int pinG = 28;
int D1 = 29;
int D2 = 30;
int D3 = 31;
int D4 = 32;
int count = 0;
int potentiometer = A5;

// the setup routine runs once when you press reset:
void setup() {
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
  }
  pinMode(potentiometer, INPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);
  pinMode(pinF, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  Serial.begin(9600);


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
    count++;
    if (count % 100 > 59)
    {
      count += -(count % 100) + 100 ;
    }
    count = count % 2400;
  }
  if (currentMillis - last_print >= 10)
  {
    last_print += 10;
    print_number_full(count);
  }
  hendel_user();
  
  
}

//gets a number and desplays it on the seven segment
void print_number(int num)
{
  if (num == 0)
  {
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, LOW);
  }
  else if (num == 1)
  {
    digitalWrite(pinA,  LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  LOW);
    digitalWrite(pinG, LOW);
  }
  else if (num == 2)
  {
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, LOW);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF,  LOW);
    digitalWrite(pinG, HIGH);
  }
  else if (num == 3)
  {
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  LOW);
    digitalWrite(pinG, HIGH);
  }
  else if (num == 4)
  {
    digitalWrite(pinA,  LOW);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
  }
  else if (num == 5)
  {
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
  }
  else if (num == 6)
  {
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, LOW);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
  }
  else if (num == 7)
  {
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, LOW);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  LOW);
    digitalWrite(pinG, LOW);
  }
  else if (num == 8)
  {
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, HIGH);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
  }
  else if (num == 9)
  {
    digitalWrite(pinA,  HIGH);
    digitalWrite(pinB, HIGH);
    digitalWrite(pinC, HIGH);
    digitalWrite(pinD, HIGH);
    digitalWrite(pinE, LOW);
    digitalWrite(pinF,  HIGH);
    digitalWrite(pinG, HIGH);
  }
}

//gives print_number the write numbers and is in charg of liting the corect screens
void print_number_full(int num)
{
  digitalWrite(D1, HIGH);
  digitalWrite(D2,  HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, HIGH);
  int digit = num % 10;
  num = num / 10;
  digitalWrite(D4, LOW);
  print_number(digit);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D4, HIGH);

  digit = num % 10;
  num = num / 10;
  digitalWrite(D3, LOW);
  print_number(digit);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D3, HIGH);

  digit = num % 10;
  num = num / 10;
  digitalWrite(D2, LOW);
  print_number(digit);
  delayMicroseconds(PRINT_4_SEGMENT_DELAY);
  digitalWrite(D2, HIGH);

  digit = num % 10;
  num = num / 10;
  digitalWrite(D1, LOW);
  print_number(digit);
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
  int multiply = 1;
  unsigned long pass = 0;
  //static int len = 10;
  int ok = 1;
  //static char* input_char = (char*)calloc(len, sizeof(char));
  char input = get_user_input();
  if(input == 6)
  {
    while(ok)
    {
      count = count % multiply +((analogRead(potentiometer) / 100 % 10) * multiply);
      print_number_full(count);
      if(get_user_input() == 6)
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
  else if(input == 7)
  {
    pass = 0;
    while(ok)
    {
      input = get_user_input();
      if(input != -1)
      {
        //input pass
        pass += input * multiply;
        multiply = multiply * 10;
        if(multiply >= 1001)
        {
          ok = 0;
          write_password_to_eeprom(pass);
          rest_clock();
        }
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
    last_clock_cycle += 1000;
    count++;
    if (count % 100 > 59)
    {
      count += -(count % 100) + 100 ;
    }
    count = count % 2400;
  }
  last_print = millis();
}
