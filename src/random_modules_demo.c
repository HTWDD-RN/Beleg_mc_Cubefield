#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#define NUM_LEDS 256
#define PIXELS_X 16
#define PIXELS_Y 16

#define DELAY 20
#define ABILITY_UP_FRAMES 6



uint8_t leds[NUM_LEDS];


unsigned int game_frame_ms = 300;

unsigned int tick_count = 0;
unsigned int speed_up_tick_count = 0;

unsigned int ability_up_frames = ABILITY_UP_FRAMES;
unsigned int ability_meter = 0;


int module_0[][2] = {
    {1,3},{1,10},{1,13},
    {6,4},{6,11},
    {11,3},{11,9},{11,12},
    {16,1},{16,7},{16,11},
    {21,0},{21,7},
    {26,1},{26,8},{26,11},{26,13},
    {31,3},{31,8},{31,11},
    {36,5},{36,11},
    {41,2},{41,6},{41,12},
    {46,4},{46,10},{46,13}
};
int module_1[][2] = {
    { 2, 4},{ 2,12},
    { 5, 2},
    { 7, 5},
    { 8,11},
    {10,13},
    {11, 2},
    {15, 1},
    {16, 8},
    {21, 6},
    {22, 4},{22,13},
    {27, 3},
    {28,11},
    {30, 4},
    {34, 5},
    {35,13},
    {37, 6},
    {41, 0},
    {42, 9},
    {44, 1},
    {45, 9},
    {47, 0},{47, 3}
};
int module_2[][2] = {
    { 1, 1},
    { 2,13},
    { 6, 7},
    { 7,13},
    { 8, 6},
    {13, 1},
    {16, 3},
    {17,11}
};
int module_3[][2] = {
    { 1, 2},{ 1,11},
    { 4, 2},{ 4,11},
    { 7, 2},{ 7,11},
    {10, 2},{10,11},
    {13, 2},{13,11}
};
int module_4[][2] = {
    { 0, 2},{ 0,11},
    { 2, 0},{ 2, 9},
    { 4, 0},{ 4, 7},
    { 6, 0},{ 6, 9},
    { 8, 2},{ 8,11},
    {10, 4},{10,13},
    {12, 6},{12,13},
    {14, 4},{14,13},
    {16, 2},{16,11},
    {18, 0},{18, 9},
    {20, 0},{20, 7},
    {22, 0},{22, 9},
    {24, 2},{24,11},
    {26, 4},{26,13},
    {28, 6},{28,13},
    {30, 4},{30,13}
};
int module_5[][2] = {}; // Startmodul

int *modules[] = {
	&module_0,
    &module_1,
    &module_2,
    &module_3,
    &module_4,
    &module_5
};
int modules_len[6] = {
    50,
    52,
    20,
    16,
    32,
    16
};
int blocks_len[6] = {
    sizeof(module_0)/(2*sizeof(int)),
    sizeof(module_1)/(2*sizeof(int)),
    sizeof(module_2)/(2*sizeof(int)),
    sizeof(module_3)/(2*sizeof(int)),
    sizeof(module_4)/(2*sizeof(int)),
    sizeof(module_5)/(2*sizeof(int))
};
int active_modules[2];

int frame_count = 0;
//int repeat_frames;



void printLeds(){
    int x, y;
    char field[PIXELS_Y * (2*PIXELS_X + 1) + 1];
    int i = 0;
    
    for(y = PIXELS_Y-1; y >= 0; y--){
        for(x = 0; x < PIXELS_X; x++){
            unsigned int led = PIXELS_X * y + x;
            switch(leds[led]){
                case 0:
                    field[i] = '-'; i++; field[i] = ' '; i++;
                    break;
                case 1:
                    field[i] = '['; i++; field[i] = ']'; i++;
                    break;
                case 2:
                    field[i] = '|'; i++; field[i] = '|'; i++;
            }
        }
        field[i] = '\n'; i++;
    }
    field[i++] = 0;
    printf("\n\n%s", field);
}

void setPixelAt(int pos_x, int pos_y, uint8_t val){
    if(pos_x >= 0 && pos_x < PIXELS_X  &&  pos_y >= 0 && pos_y < PIXELS_Y){
        unsigned int led = PIXELS_X*pos_y + pos_x;
        leds[led] = val;
    }
}




void showCubes(){
    frame_count--;
    if(frame_count == 0){
        int new_module = rand() % 5;
        active_modules[0] = active_modules[1];
        active_modules[1] = rand() % 5;
        frame_count = modules_len[active_modules[0]];
    }
    int i;
    int mod = active_modules[0];
    for(i = 0; i < blocks_len[mod]; i++){
        int height_on_board = modules[mod][2*i] - modules_len[mod] + frame_count;

        if(height_on_board >= -1 && height_on_board <= 15){
            setPixelAt(modules[mod][2*i+1], height_on_board, 1);
            setPixelAt(modules[mod][2*i+1]+1, height_on_board, 1);
            setPixelAt(modules[mod][2*i+1], height_on_board+1, 1);
            setPixelAt(modules[mod][2*i+1]+1, height_on_board+1, 1);
        }
    }
    mod = active_modules[1];
    for(i = 0; i < blocks_len[mod]; i++){
        int height_on_board = modules[mod][2*i] + frame_count;

        if(height_on_board >= -1 && height_on_board <= 15){
            setPixelAt(modules[mod][2*i+1], height_on_board, 1);
            setPixelAt(modules[mod][2*i+1]+1, height_on_board, 1);
            setPixelAt(modules[mod][2*i+1], height_on_board+1, 1);
            setPixelAt(modules[mod][2*i+1]+1, height_on_board+1, 1);
        }
    }
}



void clearPlayingBoard(){
    int x, y;
    for(y = 0; y < PIXELS_Y; y++){
        for(x = 0; x < PIXELS_X-1; x++){
            setPixelAt(x,y,0);
        }
    }
}




void play(){
    while(1){
        if(tick_count == 0){
            if(ability_up_frames == 0 && ability_meter < 16){
                ability_meter++;
                setPixelAt(15, ability_meter-1, 2);
                ability_up_frames = ABILITY_UP_FRAMES;
            }
            ability_up_frames = (ability_up_frames-1) % ABILITY_UP_FRAMES;

            clearPlayingBoard();

            showCubes();
            printLeds();
        }
        tick_count = (tick_count + 1) % (game_frame_ms / DELAY);

        usleep(DELAY*1000);
    }
}



int main(){
    srand(time(NULL));
    active_modules[0] = 5;
    active_modules[1] = rand() % 5;
    frame_count = modules_len[active_modules[0]];
    
    sleep(3);

    play();

    return 0;
}
