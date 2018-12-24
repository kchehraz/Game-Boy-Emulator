#include <iostream>
#include "Cartridge.h"
using namespace std;

Cartridge::Cartridge() {
	// Nothing
}

 Cartridge::Cartridge(string filepath) {
	// Open the ROM file, error check
	 system("cd");
	FILE * rom = fopen(filepath.c_str(), "rb");

	if (rom == NULL) {
		perror("Failed to open ROM file");
		return;
	}

	// Get size of ROM file
	fseek(rom, SEEK_END, 0);
	int rom_size = ftell(rom);
	fseek(rom, SEEK_SET, 0);

	// Dynamically allocate cartridge array using ROM size
	// and load all the bytes into it
	fread(cartridge, sizeof(uint8_t), rom_size, rom);

	fclose(rom);
}

Cartridge::~Cartridge() {
	// Free up cartridge memory
}
