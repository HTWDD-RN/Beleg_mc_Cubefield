// Aufbau: https://wokwi.com/projects/434766848991859713

#include <FastLED.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

#define N_H0  31
#define N_C1  33
#define N_CS1 35
#define N_D1  37
#define N_DS1 39
#define N_E1  41
#define N_F1  44
#define N_FS1 46
#define N_G1  49
#define N_GS1 52
#define N_A1  55
#define N_AS1 58
#define N_H1  62
#define N_C2  65
#define N_CS2 69
#define N_D2  73
#define N_DS2 78
#define N_E2  82
#define N_F2  87
#define N_FS2 93
#define N_G2  98
#define N_GS2 104
#define N_A2  110
#define N_AS2 117
#define N_H2  123
#define N_C3  131
#define N_CS3 139
#define N_D3  147
#define N_DS3 156
#define N_E3  165
#define N_F3  175
#define N_FS3 185
#define N_G3  196
#define N_GS3 208
#define N_A3  220
#define N_AS3 233
#define N_H3  247
#define N_C4  262
#define N_CS4 277
#define N_D4  294
#define N_DS4 311
#define N_E4  330
#define N_F4  349
#define N_FS4 370
#define N_G4  392
#define N_GS4 415
#define N_A4  440
#define N_AS4 466
#define N_H4  494
#define N_C5  523
#define N_CS5 554
#define N_D5  587
#define N_DS5 622
#define N_E5  659
#define N_F5  698
#define N_FS5 740
#define N_G5  784
#define N_GS5 831
#define N_A5  880
#define N_AS5 932
#define N_H5  988
#define N_C6  1047
#define N_CS6 1109
#define N_D6  1175
#define N_DS6 1245
#define N_E6  1319
#define N_F6  1397
#define N_FS6 1480
#define N_G6  1568
#define N_GS6 1661
#define N_A6  1760
#define N_AS6 1865
#define N_H6  1976
#define N_C7  2093
#define N_CS7 2217
#define N_D7  2349
#define N_DS7 2489
#define N_E7  2637
#define N_F7  2794
#define N_FS7 2960
#define N_G7  3136
#define N_GS7 3322
#define N_A7  3520
#define N_AS7 3729
#define N_H7  3951
#define N_C8  4186
#define N_CS8 4435
#define N_D8  4699
#define N_DS8 4978

#define _64_ 30
#define _32_ 2*_64_
#define _16_ 4*_64_
#define _8_ 8*_64_
#define _4_ 16*_64_
#define _2_ 32*_64_
#define _1_ 64*_64_

#define _64_3_ _64_*2/3


#define DATA_PIN 13
#define ABILITY_PIN 2
#define RESET_PIN 3

#define BUZ_PIN 10


#define ADDRESS 0

#define NUM_LEDS 256
#define PIXELS_X 16
#define PIXELS_Y 16

#define DELAY 20
#define ABILITY_UP_FRAMES 6
#define SLOWDOWN_MS 4800

#define SPEED_UP_COUNT_DOWN 4000
#define MIN_FRMAE_MS 100


unsigned int score = 0;

unsigned int game_frame_ms = 500;
unsigned int game_frame_tmp = game_frame_ms;

unsigned int tick_count = 0;
unsigned int speed_up_tick_count = 0;

unsigned int ability_up_frames = ABILITY_UP_FRAMES;
unsigned int ability_meter = 0;

unsigned int player_pos = 7;

int slowdown_left;

CRGB leds[NUM_LEDS];

// LiquidCrystal(rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int player_hue = 160;
int player_sat = 255;
int player_val = 16;

int cube_hue = 0;
int cube_sat = 255;
int cube_val = 16;

int ability_hue = 96;
int ability_sat = 255;
int ability_val = 24;

int val_x;



int blocks[][2] = {
  {0,2},{0,9},{0,12},
  {1,4},{1,11},
  {2,3},{2,9},{2,12},
  {3,1},{3,7},{3,11},
  {4,0},{4,7},
  {5,1},{5,8},{5,11},{5,13},
  {6,3},{6,8},{6,11},
  {7,5},{7,11},
  {8,2},{8,6},{8,12},
  {9,4},{9,10},{9,13}
};
int blocks_len = sizeof(blocks)/(2*sizeof(int));

int line_spacing = 5;
int frame_count = 0;
int repeat_frames = 10 * line_spacing;


int game_over = 1;


void setup() {

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(ABILITY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ABILITY_PIN), useSlowDown, FALLING);
  pinMode(RESET_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RESET_PIN), reset, FALLING);

  pinMode(BUZ_PIN, OUTPUT);


  randomSeed(analogRead(0));

  lcd.begin(16, 2);
  lcd.home();
  lcd.clear();

  lcd.print("Score: ");
  lcd.print(score);

  uint16_t high_score = readHighScore();
  printHighScore(high_score);


  changePlayerPos(0);


  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;// initialize counter value to 0
  // set compare match register for 50hz increments
  OCR1A = 4999;// = (16*10^6) / (1*64) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 64 prescaler
  TCCR1B |= (1 << CS11) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);


  sei();//allow interrupts
}


ISR(TIMER1_COMPA_vect){
  cli();

  // Spielroutine
  if(game_over == 0){
    if(slowdown_left > 0){  // Fähigkeit aktiv
      if(slowdown_left > DELAY){
        slowdown_left = slowdown_left - DELAY;
        ability_meter = slowdown_left*PIXELS_Y / SLOWDOWN_MS;
        for(int i = ability_meter; i < PIXELS_Y; i++) setPixelAt(15, i, 0,0,0);
        FastLED.show();
      }
      else endSlowDown();
    }

    if(tick_count == 0){
      if(ability_up_frames == 0 && ability_meter < 16 && slowdown_left <= 0){   // erhöhen der Fähigkeits-progress-bar
        ability_meter++;
        setPixelAt(15, ability_meter-1, ability_hue, ability_sat, ability_val);
        //FastLED.show();
        ability_up_frames = ABILITY_UP_FRAMES;


      }
      ability_up_frames = (ability_up_frames-1) % ABILITY_UP_FRAMES;

      clearPlayingBoard();

      val_x = (512-analogRead(A3)) / 256;

      int coll = showCubes();
      changePlayerPos(val_x);

      if(coll) fail();  // game over

      FastLED.show();

      if(frame_count % (line_spacing*2) -2 == 0){
        score++;
        lcd.setCursor(0,0);
        lcd.print("Score: ");
        lcd.print(score);
        tone(BUZ_PIN, N_A4, 30);
      }
    }


    if(speed_up_tick_count == 0) speedUp(10);
    speed_up_tick_count = (speed_up_tick_count + 1) % (SPEED_UP_COUNT_DOWN / DELAY);



  }
  else if(tick_count == 0){
    player_sat ^= 255;
    changePlayerPos(0);
    FastLED.show();
  }


  // ticks hochzählen
  tick_count = (tick_count + 1) % (game_frame_ms / DELAY);

  sei();
}


void fail(){
  game_over = 1;

  tick_count = 1;
  game_frame_ms = 500;

  uint16_t high_score = readHighScore();
  if(score > high_score){
    high_score = score;
    writeHighScore(high_score);
    printHighScore(high_score);
  }

  player_sat ^= 255;
  changePlayerPos(0);
  for(int i = 0; i < PIXELS_Y; i++) setPixelAt(PIXELS_X-1, i, 0,0,0);

  FastLED.show();

  Note(N_F5, 96);
  Note(N_D5, 96);
  Note(N_A4, 96);
  Note(N_GS4,256);
}
void Note(int note, int dur){
  tone(BUZ_PIN, note, dur);
  delay(dur);
}
void loop() {}

void printHighScore(uint16_t high_score){
  lcd.setCursor(0,1);
  lcd.print("High Score:");
  lcd.print(high_score);
  lcd.print("      ");
}
uint16_t readHighScore(){
  uint16_t high_score = 0;
  high_score |= EEPROM.read(ADDRESS);
  high_score |= (uint16_t)EEPROM.read(ADDRESS+1)<<8;
  return high_score;
}
void writeHighScore(uint16_t high_score){
  EEPROM.write(ADDRESS, high_score);
  EEPROM.write(ADDRESS+1, high_score>>8);
}




void setPixelAt(unsigned int pos_x, unsigned int pos_y, int hue, int sat, int val){
  if(pos_x >= 0 && pos_x < PIXELS_X  &&  pos_y >= 0 && pos_y < PIXELS_Y){
    unsigned int board_pos_x;
    unsigned int board_pos_y;

    if(pos_y & 1) board_pos_x = PIXELS_X-1 - pos_x;
    else board_pos_x = pos_x;

    board_pos_y = pos_y;

    unsigned int led = PIXELS_X*board_pos_y + board_pos_x;
    leds[led] = CHSV(hue, sat, val);
  }
}

void changePlayerPos(unsigned int shift){

  setPixelAt(player_pos-1, 1, 0,0,0);
  setPixelAt(player_pos  , 1, 0,0,0);
  setPixelAt(player_pos+1, 1, 0,0,0);
  setPixelAt(player_pos  , 2, 0,0,0);

  if(player_pos + shift >= 1 && player_pos + shift <= PIXELS_X-3){
    player_pos += shift;
  }

  setPixelAt(player_pos-1, 1, player_hue, player_sat, player_val);
  setPixelAt(player_pos  , 1, player_hue, player_sat, player_val);
  setPixelAt(player_pos+1, 1, player_hue, player_sat, player_val);
  setPixelAt(player_pos  , 2, player_hue, player_sat, player_val);
}

int showCubes(){
  int collision = 0;
  for(int i = 0; i < blocks_len; i++){
    int height_on_board = blocks[i][0]*line_spacing - frame_count;
    if(height_on_board < -1) height_on_board += repeat_frames-1;
    height_on_board = height_on_board % repeat_frames;

    if(height_on_board >= -1 && height_on_board <= 15){
      setPixelAt(blocks[i][1], height_on_board, cube_hue, cube_sat, cube_val);
      setPixelAt(blocks[i][1]+1, height_on_board, cube_hue, cube_sat, cube_val);
      setPixelAt(blocks[i][1], height_on_board+1, cube_hue, cube_sat, cube_val);
      setPixelAt(blocks[i][1]+1, height_on_board+1, cube_hue, cube_sat, cube_val);

      if(detectCollisionWithBlock(blocks[i][1], height_on_board)) collision = 1;
    }
  }
  
  frame_count = (frame_count + 1) % repeat_frames;

  return collision;
}
int detectCollisionWithBlock(int x, int y){
  if(y >= 0 && y <= 2){
    int tmp_player_pos = player_pos + val_x;
    return tmp_player_pos==x || tmp_player_pos-1==x || (tmp_player_pos+1==x && (y==0 || y==1)) || (tmp_player_pos-2==x && (y==0 || y==1));
  }
  else return 0;
}

void clearPlayingBoard(){
  for(int y = 0; y < PIXELS_Y; y++){
    for(int x = 0; x < PIXELS_X-1; x++){
      setPixelAt(x,y,0,0,0);
    }
  }
}




void useSlowDown(){
  if(!game_over){
    if(ability_meter == PIXELS_Y){
      game_frame_tmp = game_frame_ms;
      game_frame_ms = 500;
      slowdown_left = SLOWDOWN_MS;
      ability_meter = PIXELS_Y-1;
    }
  }
  else{
    game_over = 0;
      
    player_sat = 255;


    game_frame_ms = 300;
    game_frame_tmp = game_frame_ms;

    tick_count = 0;
    speed_up_tick_count = 0;

    ability_up_frames = ABILITY_UP_FRAMES;
    ability_meter = 0;

    player_pos = 7;

    frame_count = 0;

    score = 0;
    lcd.setCursor(0,0);
    lcd.print("Score: ");
    lcd.print(score);
    lcd.print("       ");
  }
}
void endSlowDown(){
  slowdown_left = 0;
  game_frame_ms = game_frame_tmp;
}

void reset(){
  writeHighScore(0);
  printHighScore(0);
}



void speedUp(int frame_speed_up){
  if(game_frame_ms - frame_speed_up >= MIN_FRMAE_MS) game_frame_ms -= frame_speed_up;
  else game_frame_ms = MIN_FRMAE_MS;
}
