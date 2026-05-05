#include <EEPROM.h>
#include <SHA256.h>
#include <Tone.h> // Include the new library





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
#define ADD_ALARM_BATTON 2
#define REMOVE_ALARM_BATTON 3
#define ACTIVATE_ALARM_BATTON 4
#define CHANGE_RINGTON_BATTON 5
#define CHANGE_BATTON 10
#define FIRST_PIN 22
#define SECOND_PIN 45
#define PASS_HASH 32
#define PASS_LEN 10
#define ALARM_COUNT 3
#define RINGTON_COUNT 3
#define MAX_ALARM_LEN 300
#define ALARM_START_ADDR 200
#define RINGTON_ADDR 300


//music
#define REST      0

// --- OCTAVE 2 (Heavy Sub-Bass) ---
#define NOTE_C2   65
#define NOTE_CS2  69
#define NOTE_D2   73
#define NOTE_DS2  78
#define NOTE_E2   82
#define NOTE_F2   87
#define NOTE_FS2  93
#define NOTE_G2   98
#define NOTE_GS2  104
#define NOTE_A2   110
#define NOTE_AS2  117
#define NOTE_B2   123

// --- OCTAVE 3 (Standard Bassline) ---
#define NOTE_C3   131
#define NOTE_CS3  139
#define NOTE_D3   147
#define NOTE_DS3  156
#define NOTE_E3   165
#define NOTE_F3   175
#define NOTE_FS3  185
#define NOTE_G3   196
#define NOTE_GS3  208
#define NOTE_A3   220
#define NOTE_AS3  233
#define NOTE_B3   247

// --- OCTAVE 4 (Middle / Rhythm Section) ---
#define NOTE_C4   262
#define NOTE_CS4  277
#define NOTE_D4   294
#define NOTE_DS4  311
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_FS4  370
#define NOTE_G4   392
#define NOTE_GS4  415
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_B4   494

// --- OCTAVE 5 (Main Melody Lead) ---
#define NOTE_C5   523
#define NOTE_CS5  554
#define NOTE_D5   587
#define NOTE_DS5  622
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_FS5  740
#define NOTE_G5   784
#define NOTE_GS5  831
#define NOTE_A5   880
#define NOTE_AS5  932
#define NOTE_B5   988

// --- OCTAVE 6 (High Harmony) ---
#define NOTE_C6   1047
#define NOTE_CS6  1109
#define NOTE_D6   1175
#define NOTE_DS6  1245
#define NOTE_E6   1319
#define NOTE_F6   1397
#define NOTE_FS6  1480
#define NOTE_G6   1568
#define NOTE_GS6  1661
#define NOTE_A6   1760
#define NOTE_AS6  1865
#define NOTE_B6   1976

// --- OCTAVE 7 (Piercing / Arpeggios) ---
#define NOTE_C7   2093
#define NOTE_CS7  2217
#define NOTE_D7   2349
#define NOTE_DS7  2489
#define NOTE_E7   2637
#define NOTE_F7   2794
#define NOTE_FS7  2960
#define NOTE_G7   3136
#define NOTE_GS7  3322
#define NOTE_A7   3520
#define NOTE_AS7  3729
#define NOTE_B7   3951


struct Time_format {
  uint8_t seconds = 0;
  uint8_t minutes = 0;
  uint8_t hours = 0;
  bool active = false;
};

struct buzzer_format {
  int  play_time;
  int Hz_melody;
  int Hz_bass;
  bool active;
};

bool check_alarms(Time_format this_time, Time_format* alarms_check);
void to_Time_format(unsigned long num, Time_format* time_set);
void to_Time_format(uint8_t* arr, Time_format* time_set, int arr_len);
void to_Time_format(int num, Time_format* time_set);


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

Time_format clock_time;
Time_format alarms[ALARM_COUNT];

unsigned long count = 0;
int potentiometer = A5;

buzzer_format alarm_rington[MAX_ALARM_LEN];
int pinMelody = 10; 
int pinBass = 11;
Tone buzzerMelody;
Tone buzzerBass;
uint8_t alarm_type = 0;

// the setup routine runs once when you press reset:
void setup() 
{
  randomSeed(analogRead(A0)); 
  Serial.begin(9600);
  // check the eeprom_base_address is valid
  EEPROM.get(eeprom_base_address, counterValue);
  if(counterValue == -1)
  {
    Serial.println("seting divice veriabels");
    EEPROM.put(eeprom_base_address, EEPROM_DEFAULT_START);
    eepromAddress = EEPROM_DEFAULT_START;
    uint8_t pass[PASS_LEN] = {11};
    for (int i = 0; i < PASS_LEN; i++)
    {
      pass[i] = 11;
    }
    pass[0] = 0;
    pass[1] = 0;
    pass[2] = 0;
    pass[3] = 0;
    write_password_to_eeprom(pass);
    EEPROM.put(ALARM_START_ADDR, alarms);
    EEPROM.put(RINGTON_ADDR, alarm_type);
  }


  int i = 0;
  for(i = 0; i < BATTON_LIST_LEN; i++)
  {
    button_arr[i] = BATTON_PIN_DEFAULT_START + i;
    pinMode(button_arr[i], INPUT);
    //Serial.println(i + BATTON_PIN_DEFAULT_START);
  }

  for(i = 0; i < MAX_ALARM_LEN; i++)
  {
    alarm_rington[i] = {0, 0, 0, false};
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

  buzzerMelody.begin(pinMelody);
  buzzerBass.begin(pinBass);
  


  //write_password_to_eeprom(12313);
  EEPROM.get(eeprom_base_address, eepromAddress); 
  EEPROM.get(ALARM_START_ADDR, alarms);
  EEPROM.get(RINGTON_ADDR, alarm_type);
  Serial.print("Read from EEPROM: ");
  Serial.println(eepromAddress);
  

  load_alarm(alarm_rington, MAX_ALARM_LEN, alarm_type);


  delay(1000);
}

// the loop routine runs over and over again forever:
void loop() 
{
  
  currentMillis = millis();
  if (currentMillis - last_clock_cycle >= 1000)
  {
    last_clock_cycle += 1000;
    clock_time.seconds++;
    if (clock_time.seconds > 59)
    {
      clock_time.seconds = 0;
      clock_time.minutes++;
      if(clock_time.minutes > 59)
      {
        clock_time.minutes = 0;
        clock_time.hours++;
      }
    }
    clock_time.hours = clock_time.hours % 24;
    if(check_alarms(clock_time, alarms) == true)
    {
      timer_awake_validation();
    }
  }
  if (currentMillis - last_print >= 10)
  {
    last_print += 10;
    print_number_full(clock_time.seconds, clock_time.minutes, clock_time.hours);
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
  }
  return true;
}

//encripts the password so it will be abit harder to crack if you get a hold of the arduino

char get_user_input()
{
  
  int i = 0;
  int ok = 0;
  static char input_buffer = -1;
  static unsigned long last_press_time = 0;//stops exidental reads
  char input_buffer_check = input_buffer;
  if ( millis() - last_press_time < 50)
  {
    return -1;
  }
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
  last_press_time = millis();
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
      to_Time_format(calc_count, &clock_time);
      print_number_full(clock_time.seconds, clock_time.minutes, clock_time.hours);
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
  else if(input == ADD_ALARM_BATTON)
  {
    int j = -1;
    if(check_for_pass() == false)
    {
      Serial.println("wrong pass ");
      return -1;
    }
    Serial.println("ok pass ");
    while(j >= ALARM_COUNT ||  j < 0)
    {
      j = get_user_input();
    }
    while(ok)
    {
      count = count % multiply +((analogRead(potentiometer) / 100 % 10) * multiply);
      unsigned long calc_count = count;
      to_Time_format(calc_count, &alarms[j]);
      print_number_full(alarms[j].seconds, alarms[j].minutes, alarms[j].hours);
      if(get_user_input() == CHANGE_BATTON)
      {
        multiply = multiply * 10;
        if(multiply >= 100001)
        {
          alarms[j].active = true;
          ok = 0;
          EEPROM.put(ALARM_START_ADDR, alarms);
          rest_clock();
        }
      }
    }
  }
  else if(input == REMOVE_ALARM_BATTON)
  {
    int j = -1;
    if(check_for_pass() == false)
    {
      Serial.println("wrong pass ");
      return -1;
    }
    Serial.println("ok pass ");
    while(j >= ALARM_COUNT ||  j < 0)
    {
      j = get_user_input();
    }
    alarms[j].active = false;
    EEPROM.put(ALARM_START_ADDR, alarms);
    rest_clock();
  }
  else if(input == ACTIVATE_ALARM_BATTON)
  {
    int j = -1;
    if(check_for_pass() == false)
    {
      Serial.println("wrong pass ");
      return -1;
    }
    Serial.println("ok pass ");
    while(j >= ALARM_COUNT ||  j < 0)
    {
      j = get_user_input();
    }
    alarms[j].active = true;
    EEPROM.put(ALARM_START_ADDR, alarms);
    rest_clock();
  }
  else if(input == CHANGE_RINGTON_BATTON)
  {
    int j = -1;
    if(check_for_pass() == false)
    {
      Serial.println("wrong pass ");
      return -1;
    }
    Serial.println("ok pass ");
    while(j >= RINGTON_COUNT ||  j < 0)
    {
      j = get_user_input();
    }
    alarm_type = j;
    EEPROM.put(RINGTON_ADDR, alarm_type);
    load_alarm(alarm_rington, MAX_ALARM_LEN, alarm_type);
    rest_clock();
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
      clock_time.seconds++;
      if (clock_time.seconds % 100 > 59)
      {
        clock_time.seconds = 0;
        clock_time.minutes++;
        if(clock_time.minutes % 100 > 59)
        {
          clock_time.minutes = 0;
          clock_time.hours++;
        }
      }
      clock_time.hours = clock_time.hours % 24;
    }
  }
  last_print = millis();
}

bool check_for_pass()
{
  char input = -1;
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


bool check_alarms(Time_format this_time, Time_format* alarms_check)
{
  int j = 0;
  for (j = 0; j < ALARM_COUNT; j++)
  {
    if(alarms_check[j].active == true && this_time.seconds == alarms_check[j].seconds && this_time.minutes == alarms_check[j].minutes && this_time.hours == alarms_check[j].hours)
    {
      return true;
    }
  }
  return false;
}

void timer_awake_validation()
{
  char input = -1;
  Time_format alarm_code;
  uint8_t pass[PASS_LEN] = {11};
  uint8_t rand_arr [PASS_LEN];
  for (int i = 0; i < PASS_LEN; i++)
  {
    pass[i] = 11;
    rand_arr[i] = random(10);
    Serial.print((int)rand_arr[i]);
  }
  int i = 0;
  bool ok = true;
  while(ok)
  {
    play_alarm(alarm_rington);
    to_Time_format(rand_arr + (i * sizeof(uint8_t)), &alarm_code, PASS_LEN - i);
    print_number_full(alarm_code.seconds, alarm_code.minutes, alarm_code.hours);
    input = get_user_input();
    if(input != -1 && input != CHANGE_BATTON)
    {
      //input pass
      pass[i] = input;
      i++;
    }
    if(i >= PASS_LEN)
    {
      if (memcmp(pass, rand_arr, PASS_LEN) == 0)
      {
        ok = 0;
        rest_clock();
      }
      else
      {
        for (i = 0; i < PASS_LEN; i++)
        {
          pass[i] = 11;
          rand_arr[i] = random(10);
        }
        i = 0;
      }
    }
  }

}

void to_Time_format(int num, Time_format* time_set)
{
  time_set->seconds = num % 100;
  num = num / 100;
  time_set->minutes = num % 100;
  num = num / 100;
  time_set->hours = num % 100;
}

void to_Time_format(unsigned long num, Time_format* time_set)
{
  time_set->seconds = num % 100;
  num = num / 100;
  time_set->minutes = num % 100;
  num = num / 100;
  time_set->hours = num % 100;
}

void to_Time_format(uint8_t* arr, Time_format* time_set, int arr_len)
{
  unsigned long arr_to_int = 0;
  unsigned long mul = 1;
  int i = 0;
  for(i = 0; (i < arr_len) && (i < 6); i++)
  {
    arr_to_int += ((unsigned long)arr[i]) * mul;
    mul = mul * 10;
  }
  to_Time_format(arr_to_int, time_set);
}


void play_alarm(buzzer_format song_array[])
{
  static int current_note = -1;
  static unsigned long start_of_current_note = 0;
  if (current_note == -1 || millis() - start_of_current_note >= song_array[current_note].play_time)
  {
    start_of_current_note = millis();
    current_note++;
    if(song_array[current_note].active == false)
    {
      current_note = 0;
    }
    if(song_array[current_note].Hz_melody == REST)
    {
      buzzerMelody.stop();
    }
    else
    {
      buzzerMelody.play(song_array[current_note].Hz_melody, song_array[current_note].play_time);
    }


    if(song_array[current_note].Hz_bass == REST)
    {
      buzzerBass.stop();
    }
    else
    {
      buzzerBass.play(song_array[current_note].Hz_bass, song_array[current_note].play_time);
    }
  }
}

void load_megalovania_melody(buzzer_format song_array[], int max_size)
{
  int i = 0;

  // ==========================================================
  // BPM & TIMING CONFIGURATION (120 BPM Swung)
  // ==========================================================
  // To make piezos sound aggressive, we use short plays and sharp rests
  const int P_16 = 100; // 16th note play
  const int R_16 = 25;  // 16th note rest (creates the gap)
  const int P_8  = 200; // 8th note play
  const int R_8  = 50;  // 8th note rest

  // ==========================================================
  // PART 1: THE MAIN RIFF (With Swung Timing)
  // ==========================================================
  int bass_roots[] = {NOTE_D4, NOTE_C4, NOTE_B3, NOTE_AS3}; // Top octave bass
  int heavy_bass[] = {NOTE_D3, NOTE_C3, NOTE_B2, NOTE_AS2}; // Sub-octave bass
  
  // The exact 8-note sequence following the double root notes
  int melody[] = {NOTE_D5, NOTE_A4, NOTE_GS4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_G4};
  
  // Custom dialed timings for maximum groove
  int play_t[] = {P_16, P_16, P_8, P_8+50, P_8, P_8, P_16, P_16, P_16, P_16}; 
  int rest_t[] = {R_16, R_16, R_8, R_8,    R_8, R_8, R_16, R_16, R_16, R_16};

  for (int repeat = 0; repeat < 2; repeat++) 
  {
    for (int m = 0; m < 4; m++) 
    {
      int b_note = bass_roots[m];
      int sub_b  = heavy_bass[m];

      // 1. The iconic double staccato strike: "Da Da"
      if (i < max_size - 1) song_array[i++] = {play_t[0], b_note, sub_b, true};
      if (i < max_size - 1) song_array[i++] = {rest_t[0], REST,   REST,  true};
      if (i < max_size - 1) song_array[i++] = {play_t[1], b_note, sub_b, true};
      if (i < max_size - 1) song_array[i++] = {rest_t[1], REST,   REST,  true};

      // 2. The melody riff over the sustained bass root
      for(int n = 0; n < 8; n++) 
      {
        if (i < max_size - 1) song_array[i++] = {play_t[n+2], melody[n], sub_b, true};
        if (i < max_size - 1) song_array[i++] = {rest_t[n+2], REST,      REST,  true};
      }
    }
  }

  // ==========================================================
  // PART 2: THE BREAKDOWN (Heavy Power Chords)
  // ==========================================================
  // We use longer sustains here to contrast the punchy intro
  
  // Phrase 1
  int p2_n1[] = {NOTE_F5, NOTE_F5, NOTE_F5, NOTE_D5, NOTE_D5};
  int p2_p1[] = {125, 125, 250, 125, 375};
  int p2_r1[] = { 25,  25,  50,  25, 125};
  
  // Phrase 2
  int p2_n2[] = {NOTE_D5, NOTE_F5, NOTE_D5, NOTE_F5, NOTE_G5};
  int p2_p2[] = {125, 250, 125, 250, 375};
  int p2_r2[] = { 25,  50,  25,  50, 125};

  // Phrase 3
  int p2_n3[] = {NOTE_GS5, NOTE_G5, NOTE_F5, NOTE_D5, NOTE_F5, NOTE_G5};
  int p2_p3[] = {250, 250, 250, 125, 125, 500};
  int p2_r3[] = { 50,  50,  50,  25,  25, 250};

  for (int repeat = 0; repeat < 2; repeat++) 
  {
    for(int n = 0; n < 5; n++) {
      if (i < max_size - 1) song_array[i++] = {p2_p1[n], p2_n1[n], NOTE_AS2, true};
      if (i < max_size - 1) song_array[i++] = {p2_r1[n], REST,     REST,     true};
    }
    for(int n = 0; n < 5; n++) {
      if (i < max_size - 1) song_array[i++] = {p2_p2[n], p2_n2[n], NOTE_C3,  true};
      if (i < max_size - 1) song_array[i++] = {p2_r2[n], REST,     REST,     true};
    }
    for(int n = 0; n < 6; n++) {
      if (i < max_size - 1) song_array[i++] = {p2_p3[n], p2_n3[n], NOTE_D3,  true};
      if (i < max_size - 1) song_array[i++] = {p2_r3[n], REST,     REST,     true};
    }
  }

  // ==========================================================
  // TERMINATOR
  // ==========================================================
  // Safely cap the array so the playback engine knows to loop
  if (i < max_size) 
  {
    song_array[i].active = false; 
  }
}

void load_tetris_melody(buzzer_format song_array[], int max_size)
{
  int i = 0;

  // ==========================================================
  // BPM & TIMING CONFIGURATION (160 BPM - Fast & Rigid)
  // ==========================================================
  const int Q_P = 325; const int Q_R = 50;  // Quarter Note
  const int E_P = 137; const int E_R = 50;  // Eighth Note
  const int H_P = 700; const int H_R = 50;  // Half Note

  // ==========================================================
  // PART A: THE ICONIC MAIN RIFF (A Minor)
  // ==========================================================
  int m_A[] = {
    NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4,
    NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
    NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
    NOTE_C5, NOTE_A4, NOTE_A4
  };
  
  int p_A[] = {
    Q_P, E_P, E_P, Q_P, E_P, E_P,
    Q_P, E_P, E_P, Q_P, E_P, E_P,
    Q_P, E_P, E_P, Q_P, Q_P,
    Q_P, Q_P, H_P
  };
  
  int r_A[] = {
    Q_R, E_R, E_R, Q_R, E_R, E_R,
    Q_R, E_R, E_R, Q_R, E_R, E_R,
    Q_R, E_R, E_R, Q_R, Q_R,
    Q_R, Q_R, H_R
  };
  
  // The bass flawlessly tracks the melody rhythm with heavy root notes
  int b_A[] = {
    NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3, NOTE_E3,
    NOTE_A2, NOTE_A2, NOTE_A2, NOTE_A2, NOTE_A2, NOTE_A2,
    NOTE_GS2, NOTE_GS2, NOTE_GS2, NOTE_GS2, NOTE_GS2, // G#2 gives it that tense Russian sound!
    NOTE_A2, NOTE_A2, NOTE_A2
  };

  // ==========================================================
  // PART B: THE B-SECTION (D Minor Bridge)
  // ==========================================================
  int m_B[] = {
    NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5,
    NOTE_E5, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
    NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
    NOTE_C5, NOTE_A4, NOTE_A4
  };
  
  int p_B[] = {
    Q_P, E_P, E_P, Q_P, Q_P,
    Q_P, E_P, E_P, Q_P, Q_P,
    Q_P, E_P, E_P, Q_P, Q_P,
    Q_P, Q_P, H_P
  };
  
  int r_B[] = {
    Q_R, E_R, E_R, Q_R, Q_R,
    Q_R, E_R, E_R, Q_R, Q_R,
    Q_R, E_R, E_R, Q_R, Q_R,
    Q_R, Q_R, H_R
  };
  
  int b_B[] = {
    NOTE_D3, NOTE_D3, NOTE_D3, NOTE_D3, NOTE_D3,
    NOTE_C3, NOTE_C3, NOTE_C3, NOTE_C3, NOTE_C3,
    NOTE_GS2, NOTE_GS2, NOTE_GS2, NOTE_GS2, NOTE_GS2,
    NOTE_A2, NOTE_A2, NOTE_A2
  };

  // ==========================================================
  // MEMORY INJECTION
  // ==========================================================
  
  // Loop the whole song twice
  for (int repeat = 0; repeat < 2; repeat++) 
  {
    // Inject Part A (20 Notes)
    for(int n = 0; n < 20; n++) {
      if (i < max_size - 1) song_array[i++] = {p_A[n], m_A[n], b_A[n], true};
      if (i < max_size - 1) song_array[i++] = {r_A[n], REST,   REST,   true};
    }
    
    // Inject Part B (18 Notes)
    for(int n = 0; n < 18; n++) {
      if (i < max_size - 1) song_array[i++] = {p_B[n], m_B[n], b_B[n], true};
      if (i < max_size - 1) song_array[i++] = {r_B[n], REST,   REST,   true};
    }
  }

  // Cap the array to loop
  if (i < max_size) 
  {
    song_array[i].active = false; 
  }
}

void load_mario_melody(buzzer_format song_array[], int max_size)
{
  int i = 0;

  // ==========================================================
  // TIMING CONFIGURATION (Fast, Bouncy, Syncopated)
  // ==========================================================
  const int T_PLAY = 100; // Sharp, staccato play time
  const int T_REST = 50;  // Standard gap
  const int T_LONG = 200; // Sustained note
  const int T_WAIT = 150; // Syncopated rest gap

  // ==========================================================
  // THE ICONIC INTRO 
  // ==========================================================
  // Melody: E E   E   C E   G       G(low)
  int intro_m[] = {NOTE_E5, NOTE_E5, NOTE_E5, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_G4};
  // Bass matches the rhythm but provides a heavy foundation
  int intro_b[] = {NOTE_D3, NOTE_D3, NOTE_D3, NOTE_D3, NOTE_D3, NOTE_G3, NOTE_G2};
  
  int i_play[] = {T_PLAY, T_PLAY, T_PLAY, T_PLAY, T_PLAY, T_PLAY, T_PLAY};
  int i_rest[] = {T_REST, T_WAIT, T_WAIT, T_REST, T_WAIT, T_WAIT*3, T_WAIT*3};

  for(int n = 0; n < 7; n++) {
    if (i < max_size - 1) song_array[i++] = {i_play[n], intro_m[n], intro_b[n], true};
    if (i < max_size - 1) song_array[i++] = {i_rest[n], REST,       REST,       true};
  }

  // ==========================================================
  // THE MAIN WALKING THEME (Loops Twice)
  // ==========================================================
  // The classic bouncy melody
  int main_m[] = {
    NOTE_C5, NOTE_G4, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_AS4, NOTE_A4,
    NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5, NOTE_F5, NOTE_G5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_B4
  };
  
  // The iconic walking bassline underneath it
  int main_b[] = {
    NOTE_E3, NOTE_C3, NOTE_G2, NOTE_F3, NOTE_G3, NOTE_FS3, NOTE_F3,
    NOTE_E3, NOTE_C3, NOTE_E3, NOTE_F3, NOTE_D3, NOTE_E3, NOTE_C3, NOTE_A2, NOTE_B2, NOTE_G2
  };

  // The very specific swing timings that make it sound like Mario
  int m_play[] = {
    T_LONG, T_PLAY, T_LONG, T_PLAY, T_PLAY, T_PLAY, T_PLAY,
    T_PLAY, T_PLAY, T_PLAY, T_LONG, T_PLAY, T_PLAY, T_LONG, T_PLAY, T_PLAY, T_PLAY
  };
  
  int m_rest[] = {
    T_WAIT, T_WAIT, T_WAIT, T_REST, T_WAIT, T_REST, T_WAIT,
    T_REST, T_REST, T_REST, T_WAIT, T_REST, T_WAIT, T_WAIT, T_REST, T_REST, T_WAIT*2
  };

  for (int repeat = 0; repeat < 2; repeat++) {
    for(int n = 0; n < 17; n++) {
      if (i < max_size - 1) song_array[i++] = {m_play[n], main_m[n], main_b[n], true};
      if (i < max_size - 1) song_array[i++] = {m_rest[n], REST,      REST,      true};
    }
  }

  // ==========================================================
  // TERMINATOR
  // ==========================================================
  if (i < max_size) 
  {
    song_array[i].active = false; 
  }
}

void load_alarm(buzzer_format song_array[], int max_size, uint8_t type)
{
  if(type == 0)
  {
    load_megalovania_melody(song_array, max_size);
  }
  if(type == 1)
  {
    load_tetris_melody(song_array, max_size);
  }
  if(type == 2)
  {
    load_mario_melody(song_array, max_size);
  }
}
