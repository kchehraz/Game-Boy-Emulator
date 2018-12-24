#include <iostream>

class Cartridge {
public:
	uint8_t cartridge[0x8000];

	Cartridge();
	Cartridge(std::string filepath);
	~Cartridge();
};