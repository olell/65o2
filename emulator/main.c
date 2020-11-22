#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL_timer.h> 
#include <pthread.h>
#include <time.h>

#include "fake6502.c"

#define RAM_SIZE   16384
#define RAM_START  0

#define ROM_SIZE   32768
#define ROM_START  32768

#define DEBUG_ADDR 0x6003
#define X_ADDR     0x6000
#define Y_ADDR     0x6001
#define C_ADDR     0x6002
#define VRAM_SIZE  65536   // Most significant 8-Bit: X-Address, Least significant 8-Bit: Y-Address

#define WINDOW_WIDTH   1024
#define WINDOW_HEIGHT  768

#define false 0
#define true 1

//#define DEBUG_PRINTS  // Prints debugging information
#define EXECUTE_CYCLES 1000

uint8_t ram[RAM_SIZE];
uint8_t rom[ROM_SIZE];

uint8_t vram[VRAM_SIZE];
uint8_t xaddr;
uint8_t yaddr;

uint8_t display_thread_running = true;

const char* byte_to_binary(uint8_t x)
{
    static char b[sizeof(uint8_t)*8+1] = {0};
    int y;
    long long z;

    for (z = 1LL<<sizeof(uint8_t)*8-1, y = 0; z > 0; z >>= 1, y++) {
        b[y] = (((x & z) == z) ? '1' : '0');
    }
    b[y] = 0;

    return b;
}

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res);

    return res;
}


uint8_t read6502(uint16_t addr){
    uint8_t ret = 0x00;
    if (addr >= ROM_START && addr < ROM_START + ROM_SIZE) {
        ret = rom[addr - ROM_START];
    }
    else if (addr >= RAM_START && addr < RAM_START + RAM_SIZE) {
        ret = ram[addr - RAM_START];
    }
    #ifdef DEBUG_PRINTS
        printf("%04x R %02x\n", addr, ret);
    #endif
    return ret;
}

void write6502(uint16_t addr, uint8_t data){
    #ifdef DEBUG_PRINTS
        printf("%04x W %02x\n", addr, data);
    #endif
    if (addr >= ROM_START && addr < ROM_START + ROM_SIZE) {
        // cpu tries to write to rom
    }
    else if (addr >= RAM_START && addr < RAM_START + RAM_SIZE){
        ram[addr - RAM_START] = data;
    }
    else if (addr == X_ADDR) {
        xaddr = data;
    }
    else if (addr == Y_ADDR) {
        yaddr = data;
    }
    else if (addr == C_ADDR) {
        vram[xaddr << 8 | yaddr] = data;
    }
    else if (addr >= DEBUG_ADDR && addr <= DEBUG_ADDR + 16){  // 16 debug registers
        printf("DEBUG: (%04x) | %02x (%s)\n", addr - DEBUG_ADDR, data, byte_to_binary(data));
    }
}

void *displayThread(void *vargp) 
{ 
    
    // Create display
    
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) { 
        printf("error initializing SDL: %s\n", SDL_GetError()); 
    } 
    SDL_Window* screen = SDL_CreateWindow("RetroComp Emulator", 
                                       SDL_WINDOWPOS_CENTERED, 
                                       SDL_WINDOWPOS_CENTERED, 
                                       WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if(!screen) {
        fprintf(stderr, "Could not create window\n");
        display_thread_running = false;
        return NULL;
    }

    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( screen, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear( renderer );

    SDL_Event event;
    SDL_Rect rect;
    rect.w = 4;
    rect.h = 4;

    int vx;
    int vy;
    int r, g, b;

    while (1) {

        for(vx = 0; vx <= 255; vx++){
            for(vy = 0; vy <= 192; vy++){
                rect.x = vx * 4;
                rect.y = vy * 4;
                r = (vram[vx << 8 | vy] & 0b11) * 85;
                g = ((vram[vx << 8 | vy] >> 2) & 0b11) * 85;
                b = ((vram[vx << 8 | vy] >> 4) & 0b11) * 85;
                SDL_SetRenderDrawColor( renderer, r, g, b, 255 );
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        SDL_RenderPresent(renderer);

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;

        SDL_Delay(33);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    display_thread_running = false;

} 

int main(int argc, char* argv[]) {

    // Loading ROM file
    char* rom_path = argv[1];
    size_t size = 0;
    FILE *fp = fopen(rom_path, "rb");
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    if (size != ROM_SIZE){
        printf("Size of rom file is not valid (%d / %d)\n", (int)size, ROM_SIZE);
        exit(1);
    }
    rewind(fp);
    fread(rom, size, 1, fp);

    // Display thread

    pthread_t thread_id; 
    pthread_create(&thread_id, NULL, displayThread, NULL); 
    //pthread_join(thread_id, NULL); 

    // Initial reset
    reset6502();
    #ifdef DEBUG_PRINTS
    clock_t t1, t2;
    float diff;
    #endif
    while (display_thread_running) {
        
        exec6502(EXECUTE_CYCLES);
        
        // sleep 1mS here:
        msleep(1);
    }

    return 0;
}