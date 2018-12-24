#include <Windows.h>

const int MEM_SIZE = 0x10000; // 65536

// The bits that hold the corresponding flags
const int VBLANK = 0x0;
const int LCD_STAT = 0x1;
const int TIMER = 0x2;
const int SERIAL = 0x3;
const int JOYPAD = 0x4;

class Memory {
public:
	BYTE mem[MEM_SIZE];

	Memory();

	BYTE ReadByte(WORD addr);
	WORD ReadWord(WORD addr);
	void WriteByte(WORD addr, BYTE value);
	void WriteWord(WORD addr, WORD value);
	
	BYTE GetFlagIE(); // Interrupt Enable
	BYTE GetFlagIF(); // Interrupt Flag (request)
};