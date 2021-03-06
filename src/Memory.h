const int MEM_SIZE = 0x10000; // 65536

typedef char BOOL;
typedef unsigned char BYTE;
typedef char SIGNED_BYTE;
typedef unsigned short WORD;

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
	
	// Display flags
    BYTE GetFlagIE(); // Interrupt Enable
	BYTE GetFlagIF(); // Interrupt Flag (request)
    BYTE GetFlagLCDC(); // LCD Control
    
    // Timing flags
    BYTE GetRegDIV(); // Divider
    BYTE GetRegTIMA(); // Counter
    BYTE GetRegTMA(); // Modulo
    BYTE GetRegTAC(); // Control
    void SetRegDIV(BYTE val); // Divider
    void SetRegTIMA(BYTE val); // Counter
    void SetRegTMA(BYTE val); // Modulo
    void SetRegTAC(BYTE val); // Control
};
