#include <iostream>
#include <string>

typedef char BOOL;
typedef unsigned char BYTE;
typedef char SIGNED_BYTE;
typedef unsigned short WORD;

class CPU {
public:
	// Registers and flags
	// I declared these as unions so that I could write to A, F, and AF freely
	// Apparently this is undefined behavior so yeah.
	union { 
		struct {
			BYTE F, A;
		};
		WORD AF;
	};
	union {
		struct {
			BYTE C, B;
		};
		WORD BC;
	};
	union {
		struct {
			BYTE E, D;
		};
		WORD DE;
	};
	union {
		struct {
			BYTE L, H;
		};
		WORD HL;
	};

	WORD SP, PC;	
	

	// Amount to shift for each flag bit
	const int F_POS = 7;
	const int N_POS = 6;
	const int H_POS = 5;
	const int C_POS = 4;

	// Constructor
	CPU();

	// Set flag bits
	void SetFlagZ(BOOL bit);
	void SetFlagN(BOOL bit);
	void SetFlagH(BOOL bit);
	void SetFlagC(BOOL bit);
	BOOL GetFlagZ();
	BOOL GetFlagN();
	BOOL GetFlagH();
	BOOL GetFlagC();

	void SetFlags(BOOL z, BOOL n, BOOL h, BOOL c);
};
