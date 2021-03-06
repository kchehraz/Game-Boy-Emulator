#include <iostream>
#include <SDL2/SDL.h>
#include "Emulator.h"
using namespace std;

const string filepath = "cpu_instrs/individual/09-op r,r.gb";

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s [file-name]\n", argv[0]);
		return EXIT_FAILURE;
	}

	Emulator emu;
	//Cartridge c;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "SDL could not initialize. Error: " << SDL_GetError() << endl;
		return EXIT_FAILURE;
	}
	cout << "SDL initialized successfully!" << endl;

	int width = 800, height = 720;
	SDL_Window * window = SDL_CreateWindow("Game Boy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, width, height);
	SDL_Texture* screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 160, 144);



	
	
	
	
	
	
	
	// THIS PART IS FOR DEBUGGING
	FILE * rom = fopen(argv[1], "rb");

	if (rom == NULL) {
		perror("Failed to open ROM file");
		return EXIT_FAILURE;
	}

	// Get size of ROM file
	fseek(rom, SEEK_END, 0);
	int rom_size = ftell(rom);
	fseek(rom, SEEK_SET, 0);

	// Dynamically allocate cartridge array using ROM size
	// and load all the bytes into it
	fread(emu.memory.mem, sizeof(uint8_t), 0x8000, rom);
	fclose(rom);
	// END OF DEBUGGING PART

	//uint32_t pixels[144][160];
	uint32_t pixels[256][256];

	BOOL quit = false;
	while (!quit) {
		emu.Update();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			/* handle your event here */

		   // User requests quit
		    if (event.type == SDL_QUIT)
				quit = true;
		}
		for (int i = 0; i < 256; i++) {
            for (int j = 0; j < 256; j++) {
                pixels[i][j] = 0xFF000000 | (0x00FFFFFF - emu.pixels[i][j] * 0x00AAAAAA);
            }
        }
		SDL_UpdateTexture(screenTexture, NULL, pixels, 256*sizeof(uint32_t)); // update screen texture
		SDL_RenderCopy(renderer, screenTexture, NULL, NULL); // fill screen with texture
		SDL_RenderPresent(renderer); // render to screen
	}

	return EXIT_SUCCESS;
}
