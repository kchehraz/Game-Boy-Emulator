#include <iostream>
#include <SDL.h>
#include "Emulator.h"
using namespace std;

const string filepath = "cpu_instrs/individual/09-op r,r.gb";

int main(int argc, char *argv[]) {
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



	cout << "Enter the name of the file you want to load: ";
	string in;
	//cin >> in;
	
	
	
	
	
	
	
	
	
	
	
	
	// THIS PART IS FOR DEBUGGING
	FILE * rom = fopen(filepath.c_str(), "rb");

	if (rom == NULL) {
		perror("Failed to open ROM file");
		system("pause");
		return 0;
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

	uint32_t pixels[144][160];
	int slope = (0xFF / 160);
	for (int i = 0; i < 144; i++) {
		for (int j = 0; j < 160; j++) {
			pixels[i][j] = (slope * j) | ((slope << 8) * j) | ((slope << 16) * j) | 0xFF000000;
		}
	}

	BOOL quit = false;
	while (!quit) {
		//printf("PC: %04x\n", emu.cpu.PC);
		//if (emu.memory.mem[emu.cpu.PC]) 
		//	printf("%02x\n", emu.memory.mem[emu.cpu.PC]);
		emu.Update();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			/* handle your event here */

		   //User requests quit
			if (event.type == SDL_QUIT)
				quit = true;
		}
		
		SDL_UpdateTexture(screenTexture, NULL, pixels, 160*sizeof(uint32_t)); // update screen texture
		SDL_RenderCopy(renderer, screenTexture, NULL, NULL); // fill screen with texture
		SDL_RenderPresent(renderer); // render to screen
	}

	return EXIT_SUCCESS;
}