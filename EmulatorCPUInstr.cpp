#include "Emulator.h"

// Helper functions
void Emulator::SwapBits(BYTE & value) {
	value = ((value & 0xF) << 8) | ((value & 0xF0) >> 8);
}

BOOL Emulator::TestBit(BYTE value, int bit) {
	return (value >> bit) & 1;
}
void Emulator::SetBit(BYTE & value, int bit) {
	value |= (1 << bit);
}
void Emulator::ResetBit(BYTE & value, int bit) {
	value = value & ~(1 << bit);
}

void Emulator::RotateRight(BYTE & value) { // rotate through carry
	BYTE bit = value & 0x01; // grab LSb
	value = (value >> 1) | (cpu.GetFlagC() << 7);
	BOOL z = (value == 0);
	cpu.SetFlags(z, 0, 0, bit);
}
void Emulator::RotateRightC(BYTE & value) {// rotate, store last bit into carry
	BYTE bit = value & 0x01; // grab LSb
	value = (value >> 1) | (bit << 7);
	BOOL z = (value == 0);
	cpu.SetFlags(z, 0, 0, bit);

}
void Emulator::RotateLeft(BYTE & value) {
	BYTE bit = (0x80 & value) >> 7; // grab MSb
	value = (value << 1) | cpu.GetFlagC();
	BOOL z = (value == 0);
	cpu.SetFlags(z, 0, 0, bit);
}
void Emulator::RotateLeftC(BYTE & value) {
	BYTE bit = (0x80 & value) >> 7; // grab MSb
	value = (value << 1) | bit;
	BOOL z = (value == 0);
	cpu.SetFlags(z, 0, 0, bit);
}

void Emulator::StackPush(WORD value) {
	cpu.SP -= 2;
	memory.WriteWord(cpu.SP, value);
}

WORD Emulator::StackPop() {
	WORD val = memory.ReadByte(cpu.SP);
	cpu.SP += 2;
	return val;
}


// Instructions begin here

// 8-Bit Loads
void Emulator::LD(BYTE & dest, BYTE & src) {
	dest = src;
}
void Emulator::LD_n(BYTE & dest) {
	dest = memory.ReadByte(cpu.PC);
	cpu.PC += 1;
}
void Emulator::LD_nn_A() {
	WORD nn = memory.ReadByte(cpu.PC);
	memory.WriteByte(nn, cpu.A);

	cpu.PC += 2;
}
void Emulator::LDD_A_HL() {
	cpu.A = memory.mem[cpu.HL--];
}
void Emulator::LDD_HL_A() {
	memory.mem[cpu.HL] = cpu.A;
	cpu.HL--;
}
void Emulator::LD_A_nn() {
	WORD nn = memory.ReadWord(cpu.PC);
	cpu.A = memory.ReadByte(nn);

	cpu.PC += 2;
}

// 16-Bit Loads
void Emulator::LD_Rd_Rn(WORD & dest, WORD & src) {
	dest = src;
}
void Emulator::LD_Rd_n(WORD & dest) {
	dest = memory.ReadByte(cpu.PC);
	cpu.PC += 1;
}
void Emulator::LD_Rd_nn(WORD & dest) {
	dest = memory.ReadWord(cpu.PC);
	cpu.PC += 2;
}
void Emulator::LD_nn_Rn(WORD & src) {
	memory.WriteWord(memory.ReadWord(cpu.PC), src);
	cpu.PC += 2;
}

///////////////////////
//  V BUG WARNING V  //
///////////////////////
//  V BUG WARNING V  //
///////////////////////
//  V BUG WARNING V  //
///////////////////////
void Emulator::LD_HL_SP_n() { // **WARNING** Check how Carry flag is actually set if issues occur.
	SIGNED_BYTE n = (SIGNED_BYTE)memory.ReadByte(cpu.PC); // interpret byte as signed byte
	BOOL hc = ((cpu.SP & 0xF) + (n & 0xF)) > 0xF; // check for carry in lower nibble
	BOOL c = ((cpu.SP & 0xFF) + n) > 0xFF;
	cpu.HL = cpu.SP + n;
	cpu.PC += 1;

	cpu.SetFlags(0, 0, hc, c);
}
void Emulator::PUSH(WORD src) {
	StackPush(src);
}
void Emulator::POP(WORD & dest) {
	dest = StackPop();
}

// 8-Bit Arithmetic
void Emulator::ADD_A_Rn(BYTE & src) {
	BYTE A = cpu.A;
	cpu.A += src;

	BOOL z = (cpu.A == 0);
	BOOL n = 0;
	BOOL h = ((src & 0xF) + (A & 0xF)) > 0xF;
	BOOL c = (src + A) > 0xFF;
	cpu.SetFlags(z, n, h, c);
}
void Emulator::ADD_A_n() {
	BYTE n = memory.ReadByte(cpu.PC);
	BYTE A = cpu.A;
	cpu.A += n;
	cpu.PC += 1;

	BOOL z = (cpu.A == 0);
	BOOL n_ = 0;
	BOOL h = ((A & 0xF) + (n & 0xF)) > 0xF;
	BOOL c = (A + n) > 0xFF;
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::ADC_A_Rn(BYTE & src) {
	BYTE n = src + cpu.GetFlagC();
	BYTE A = cpu.A;
	cpu.A += src;

	BOOL z = (cpu.A == 0);
	BOOL n_ = 0;
	BOOL h = ((A & 0xF) + (src & 0xF)) > 0xF;
	BOOL c = (A + src) > 0xFF;
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::ADC_A_n() {
	BYTE n = memory.ReadByte(cpu.PC) + cpu.GetFlagC();
	BYTE A = cpu.A;
	cpu.A += n;
	cpu.PC += 1;

	BOOL z = (cpu.A == 0);
	BOOL n_ = 0;
	BOOL h = ((A & 0xF) + (n & 0xF)) > 0xF;
	BOOL c = (A + n) > 0xFF;
	cpu.SetFlags(z, n_, h, c);
}

// For some reason the documentation on subtracting and setting C/HC flags is just plain wrong
void Emulator::SUB_A(BYTE & src) {
	BYTE A = cpu.A;
	BYTE R8 = src;
	cpu.A -= R8;

	BOOL z = (cpu.A == 0);
	BOOL n_ = 1;
	BOOL h = ((A & 0xF) < (R8 & 0xF)); // set if borrow from bit 4
	BOOL c = (A < R8); // set if borrow
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::SUB_A_n() {
	BYTE A = cpu.A;
	BYTE n = memory.ReadByte(cpu.PC);
	cpu.A -= n;
	cpu.PC += 1;

	BOOL z = (cpu.A == 0);
	BOOL n_ = 1;
	BOOL h = ((A & 0xF) < (n & 0xF)); // set if borrow from bit 4
	BOOL c = (A < n); // set if borrow
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::SBC_A(BYTE & src) {
	BYTE A = cpu.A;
	BYTE R8 = src + cpu.GetFlagC();
	cpu.A -= R8;

	BOOL z = (cpu.A == 0);
	BOOL n_ = 1;
	BOOL h = ((A & 0xF) < (R8 & 0xF));
	BOOL c = (A < R8);
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::SBC_A_n() {
	BYTE A = cpu.A;
	BYTE n = memory.ReadByte(cpu.PC) + cpu.GetFlagC();
	cpu.A -= n;
	cpu.PC += 1;

	BOOL z = (cpu.A == 0);
	BOOL n_ = 1;
	BOOL h = ((A & 0xF) < (n & 0xF)); // set if borrow from bit 4
	BOOL c = (A < n); // set if borrow
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::AND_A(BYTE & src) {
	cpu.A &= src;

	cpu.SetFlags((cpu.A == 0), 0, 1, 0);
}
void Emulator::AND_A_n() {
	BYTE n = memory.ReadByte(cpu.PC);
	cpu.A &= n;
	cpu.PC += 1;

	cpu.SetFlags((cpu.A == 0), 0, 1, 0);
}
void Emulator::OR_A(BYTE & src) {
	cpu.A |= src;

	cpu.SetFlags((cpu.A == 0), 0, 0, 0);
}
void Emulator::OR_A_n() {
	BYTE n = memory.ReadByte(cpu.PC);
	cpu.A |= n;
	cpu.PC += 1;

	cpu.SetFlags((cpu.A == 0), 0, 0, 0);
}
void Emulator::XOR_A(BYTE & src) {
	cpu.A ^= src;

	cpu.SetFlags((cpu.A == 0), 0, 0, 0);
}
void Emulator::XOR_A_n() {
	BYTE n = memory.ReadByte(cpu.PC);
	cpu.A ^= n;
	cpu.PC += 1;

	cpu.SetFlags((cpu.A == 0), 0, 0, 0);
}
void Emulator::CP_A(BYTE & src) {
	BYTE A = cpu.A;
	BYTE R8 = src;

	BOOL z = ((A - R8) == 0);
	BOOL n_ = 1;
	BOOL h = ((A & 0xF) < (R8 & 0xF)); // set if borrow from bit 4
	BOOL c = (A < R8); // set if borrow
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::CP_A_n() {
	BYTE A = cpu.A;
	BYTE n = memory.ReadByte(cpu.PC);
	cpu.PC += 1;

	BOOL z = ((A - n) == 0);
	BOOL n_ = 1;
	BOOL h = ((A & 0xF) < (n & 0xF)); // set if borrow from bit 4
	BOOL c = (A < n); // set if borrow
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::INC(BYTE & src) {
	src++;

	cpu.SetFlags((src == 0), 0, (src & 0xF) == 0, cpu.GetFlagC());
}
void Emulator::DEC(BYTE & src) {
	src--;

	cpu.SetFlags((src == 0), 1, (src & 0xF) == 0xF, cpu.GetFlagC());
}

// 16-Bit Arithmetic
void Emulator::ADD_HL_Rn(WORD src) {
	WORD HL = cpu.HL >> 8; // shift right to make it easier to manipulate
	cpu.HL += src;

	BOOL n = 0;
	BOOL h = ((src & 0xF) + (HL & 0xF)) > 0xF;
	BOOL c = (src + HL) > 0xFF;
	cpu.SetFlags(cpu.GetFlagZ(), n, h, c);
}
void Emulator::ADD_SP_n() {
	SIGNED_BYTE n = (SIGNED_BYTE)memory.ReadByte(cpu.PC);
	WORD SP = cpu.SP;
	cpu.SP += n;
	cpu.PC += 1;

	BOOL z = 0;
	BOOL n_ = 0;
	BOOL h = ((n & 0xF) + (SP & 0xF)) > 0xF;
	BOOL c = (n + SP) > 0xFF;
	cpu.SetFlags(z, n_, h, c);
}
void Emulator::INC_Rd(WORD & reg) {
	reg++;
}
void Emulator::DEC_Rd(WORD & reg) {
	reg--;
}

// Miscellaneous
void Emulator::SWAP() {
	BYTE op = FetchOpcode();
	BOOL z = 0;
	switch (op) {
		case 0x30:
		{
			SwapBits(cpu.B);
			z = (cpu.B == 0);
			break;
		}
		case 0x31:
		{
			SwapBits(cpu.C);
			z = (cpu.C == 0);
			break;
		}
		case 0x32:
		{
			SwapBits(cpu.D);
			z = (cpu.D == 0);
			break;
		}
		case 0x33:
		{
			SwapBits(cpu.E);
			z = (cpu.E == 0);
			break;
		}
		case 0x34:
		{
			SwapBits(cpu.H);
			z = (cpu.H == 0);
			break;
		}
		case 0x35:
		{
			SwapBits(cpu.L);
			z = (cpu.L == 0);
			break;
		}
		case 0x36:
		{
			SwapBits(memory.mem[cpu.HL]);
			z = (memory.mem[cpu.HL] == 0);
			break;
		}
		case 0x37:
		{
			SwapBits(cpu.A);
			z = (cpu.A == 0);
			break;
		}
	}
	cpu.SetFlags(z, 0, 0, 0);
}
// ** WARNING ** Possible bug with this?
void Emulator::DAA() {
	BOOL c = 0;
	if (cpu.A > 0x09) {
		cpu.A += 0x06; // turns decimal value into hexadecimal digits
	}
	if ((cpu.A & 0xF0) > 0x90) {
		if (((cpu.A & 0xF0) + 0x60) > 0xFF)
			c = 1;
		cpu.A += 0x60;
	}
	cpu.SetFlags((cpu.A == 0), cpu.GetFlagN(), 0, c);
}
void Emulator::CPL_A() {
	cpu.A = ~cpu.A;
	cpu.SetFlagN(1);
	cpu.SetFlagH(1);
}
void Emulator::CCF() {
	cpu.SetFlags(cpu.GetFlagZ(), 0, 0, ~cpu.GetFlagC());
}
void Emulator::SCF() {
	cpu.SetFlags(cpu.GetFlagZ(), 0, 0, 1);
}
void Emulator::NOP() {
	
}
void Emulator::HALT() {
	// ** TODO ** //
	isHalted = true;
}
void Emulator::STOP() {
	// ** TODO ** //
	isHalted = true;
}
void Emulator::DI() {
	interruptsOn = false;
}
void Emulator::EI() {
	interruptsOn = true;
}

// Rotates & Shifts
// ** WARNING ** I think Z flag is set correctly (only for non-A functions, first 4), but check if errors occur
void Emulator::RLC_A() {
	RotateLeftC(cpu.A);
	cpu.SetFlagZ(0);
}
void Emulator::RL_A() {
	RotateLeft(cpu.A);
	cpu.SetFlagZ(0);
}
void Emulator::RRC_A() {
	RotateRightC(cpu.A);
	cpu.SetFlagZ(0);
}
void Emulator::RR_A() {
	RotateRight(cpu.A);
	cpu.SetFlagZ(0);
}
void Emulator::RLC_n() {
	BYTE op = FetchOpcode();
	switch (op) {
		case 0x00:
		{
			RotateLeftC(cpu.B);
			break;
		}
		case 0x01:
		{
			RotateLeftC(cpu.C);
			break;
		}
		case 0x02:
		{
			RotateLeftC(cpu.D);
			break;
		}
		case 0x03:
		{
			RotateLeftC(cpu.E);
			break;
		}
		case 0x04:
		{
			RotateLeftC(cpu.H);
			break;
		}
		case 0x05:
		{
			RotateLeftC(cpu.L);
			break;
		}
		case 0x06:
		{
			RotateLeftC(memory.mem[cpu.HL]);
			break;
		}
		case 0x07:
		{
			RotateLeftC(cpu.A);
			break;
		}
	}
}
void Emulator::RL_n() {
	BYTE op = FetchOpcode();
	switch (op) {
		case 0x10:
		{
			RotateLeft(cpu.B);
			break;
		}
		case 0x11:
		{
			RotateLeft(cpu.C);
			break;
		}
		case 0x12:
		{
			RotateLeft(cpu.D);
			break;
		}
		case 0x13:
		{
			RotateLeft(cpu.E);
			break;
		}
		case 0x14:
		{
			RotateLeft(cpu.H);
			break;
		}
		case 0x15:
		{
			RotateLeft(cpu.L);
			break;
		}
		case 0x16:
		{
			RotateLeft(memory.mem[cpu.HL]);
			break;
		}
		case 0x17:
		{
			RotateLeft(cpu.A);
			break;
		}
	}
}
void Emulator::RRC_n() {
	BYTE op = FetchOpcode();
	switch (op) {
		case 0x08:
		{
			RotateRightC(cpu.B);
			break;
		}
		case 0x09:
		{
			RotateRightC(cpu.C);
			break;
		}
		case 0x0A:
		{
			RotateRightC(cpu.D);
			break;
		}
		case 0x0B:
		{
			RotateRightC(cpu.E);
			break;
		}
		case 0x0C:
		{
			RotateRightC(cpu.H);
			break;
		}
		case 0x0D:
		{
			RotateRightC(cpu.L);
			break;
		}
		case 0x0E:
		{
			RotateRightC(memory.mem[cpu.HL]);
			break;
		}
		case 0x0F:
		{
			RotateRightC(cpu.A);
			break;
		}
	}
}
void Emulator::RR_n() {
	BYTE op = FetchOpcode();
	switch (op) {
		case 0x18:
		{
			RotateRightC(cpu.B);
			break;
		}
		case 0x19:
		{
			RotateRightC(cpu.C);
			break;
		}
		case 0x1A:
		{
			RotateRightC(cpu.D);
			break;
		}
		case 0x1B:
		{
			RotateRightC(cpu.E);
			break;
		}
		case 0x1C:
		{
			RotateRightC(cpu.H);
			break;
		}
		case 0x1D:
		{
			RotateRightC(cpu.L);
			break;
		}
		case 0x1E:
		{
			RotateRightC(memory.mem[cpu.HL]);
			break;
		}
		case 0x1F:
		{
			RotateRightC(cpu.A);
			break;
		}
	}
}
void Emulator::SLA_n() {
	BYTE op = FetchOpcode();
	switch (op) {
		case 0x20:
		{
			cpu.SetFlags(0, 0, 0, (cpu.B & 0x80) >> 7);
			cpu.B <<= 1;
			break;
		}
		case 0x21:
		{
			cpu.SetFlags(0, 0, 0, (cpu.C & 0x80) >> 7);
			cpu.C <<= 1;
			break;
		}
		case 0x22:
		{
			cpu.SetFlags(0, 0, 0, (cpu.D & 0x80) >> 7);
			cpu.D <<= 1;
			break;
		}
		case 0x23:
		{
			cpu.SetFlags(0, 0, 0, (cpu.E & 0x80) >> 7);
			cpu.E <<= 1;
			break;
		}
		case 0x24:
		{
			cpu.SetFlags(0, 0, 0, (cpu.H & 0x80) >> 7);
			cpu.H <<= 1;
			break;
		}
		case 0x25:
		{
			cpu.SetFlags(0, 0, 0, (cpu.L & 0x80) >> 7);
			cpu.L <<= 1;
			break;
		}
		case 0x26:
		{
			cpu.SetFlags(0, 0, 0, (memory.mem[cpu.HL] & 0x80) >> 7);
			memory.mem[cpu.HL] <<= 1;
			break;
		}
		case 0x27:
		{
			cpu.SetFlags(0, 0, 0, (cpu.A & 0x80) >> 7);
			cpu.A <<= 1;
			break;
		}
	}
}
void Emulator::SRA_n() {
	BYTE op = FetchOpcode();
	switch (op) {
		case 0x28:
		{
			BOOL msb = cpu.B & 0x80;
			cpu.SetFlags(0, 0, 0, cpu.B & 0x01);
			cpu.B >>= 1;
			cpu.B |= msb; // keep msb
			break;
		}
		case 0x29:
		{
			BOOL msb = cpu.C & 0x80;
			cpu.SetFlags(0, 0, 0, cpu.C & 0x01);
			cpu.C >>= 1;
			cpu.C |= msb;
			break;
		}
		case 0x2A:
		{
			BOOL msb = cpu.D & 0x80;
			cpu.SetFlags(0, 0, 0, cpu.D & 0x01);
			cpu.D >>= 1;
			cpu.D |= msb;
			break;
		}
		case 0x2B:
		{
			BOOL msb = cpu.E & 0x80;
			cpu.SetFlags(0, 0, 0, cpu.E & 0x01);
			cpu.E >>= 1;
			cpu.E |= msb;
			break;
		}
		case 0x2C:
		{
			BOOL msb = cpu.H & 0x80;
			cpu.SetFlags(0, 0, 0, cpu.H & 0x01);
			cpu.H >>= 1;
			cpu.H |= msb;
			break;
		}
		case 0x2D:
		{
			BOOL msb = cpu.L & 0x80;
			cpu.SetFlags(0, 0, 0, cpu.L & 0x01);
			cpu.L >>= 1;
			cpu.L |= msb;
			break;
		}
		case 0x2E:
		{
			BOOL msb = memory.mem[cpu.HL] & 0x80;
			cpu.SetFlags(0, 0, 0, memory.mem[cpu.HL] & 0x01);
			memory.mem[cpu.HL] >>= 1;
			memory.mem[cpu.HL] |= msb;
			break;
		}
		case 0x2F:
		{
			BOOL msb = cpu.A & 0x80;
			cpu.SetFlags(0, 0, 0, cpu.A & 0x01);
			cpu.A >>= 1;
			cpu.A |= msb;
			break;
		}
	}
}
void Emulator::SRL_n() {
	BYTE op = FetchOpcode();
	switch (op) {
		case 0x38:
		{
			cpu.SetFlags(0, 0, 0, cpu.B & 0x01);
			cpu.B >>= 1;
			break;
		}
		case 0x39:
		{
			cpu.SetFlags(0, 0, 0, cpu.C & 0x01);
			cpu.C >>= 1;
			break;
		}
		case 0x3A:
		{
			cpu.SetFlags(0, 0, 0, cpu.D & 0x01);
			cpu.D >>= 1;
			break;
		}
		case 0x3B:
		{
			cpu.SetFlags(0, 0, 0, cpu.E & 0x01);
			cpu.E >>= 1;
			break;
		}
		case 0x3C:
		{
			cpu.SetFlags(0, 0, 0, cpu.H & 0x01);
			cpu.H >>= 1;
			break;
		}
		case 0x3D:
		{
			cpu.SetFlags(0, 0, 0, cpu.L & 0x01);
			cpu.L >>= 1;
			break;
		}
		case 0x3E:
		{
			cpu.SetFlags(0, 0, 0, memory.mem[cpu.HL] & 0x01);
			memory.mem[cpu.HL] >>= 1;
			break;
		}
		case 0x3F:
		{
			cpu.SetFlags(0, 0, 0, cpu.A & 0x01);
			cpu.A >>= 1;
			break;
		}
	}
}

// Bit Operations
void Emulator::BIT_b_r() {
	// Grab the second byte (after initial opcode 0xCB) to see which bit and register to store into
	// There are 64 total operations here: combination of 8 bits and 8 registers
	BYTE op = FetchOpcode();

	int reg = op % 8; // A = 7, B = 0, C = 1, D = 2, etc...
	int bit = (op - 0x40) / 8; // bits 0 - 7

	switch (reg) {
		case 0x0:
		{
			cpu.SetFlags(TestBit(cpu.B, bit), 0, 0, cpu.GetFlagC());
			break;
		}
		case 0x1:
		{
			cpu.SetFlags(TestBit(cpu.C, bit), 0, 0, cpu.GetFlagC());
			break;
		}
		case 0x2:
		{
			cpu.SetFlags(TestBit(cpu.D, bit), 0, 0, cpu.GetFlagC());
			break;
		}
		case 0x3:
		{
			cpu.SetFlags(TestBit(cpu.E, bit), 0, 0, cpu.GetFlagC());
			break;
		}
		case 0x4:
		{
			cpu.SetFlags(TestBit(cpu.H, bit), 0, 0, cpu.GetFlagC());
			break;
		}
		case 0x5:
		{
			cpu.SetFlags(TestBit(cpu.L, bit), 0, 0, cpu.GetFlagC());
			break;
		}
		case 0x6:
		{
			cpu.SetFlags(TestBit(memory.mem[cpu.HL], bit), 0, 0, cpu.GetFlagC());
			break;
		}
		case 0x7:
		{
			cpu.SetFlags(TestBit(cpu.A, bit), 0, 0, cpu.GetFlagC());
			break;
		}
	}
}
void Emulator::SET_b_r() {
	BYTE op = FetchOpcode();

	int reg = op % 8; // A = 7, B = 0, C = 1, D = 2, etc...
	int bit = (op - 0xC0) / 8; // bits 0 - 7

	switch (reg) {
		case 0x0:
		{
			SetBit(cpu.B, bit);
			break;
		}
		case 0x1:
		{
			SetBit(cpu.C, bit);
			break;
		}
		case 0x2:
		{
			SetBit(cpu.D, bit);
			break;
		}
		case 0x3:
		{
			SetBit(cpu.E, bit);
			break;
		}
		case 0x4:
		{
			SetBit(cpu.H, bit);
			break;
		}
		case 0x5:
		{
			SetBit(cpu.L, bit);
			break;
		}
		case 0x6:
		{
			SetBit(memory.mem[cpu.HL], bit);
			break;
		}
		case 0x7:
		{
			SetBit(cpu.A, bit);
			break;
		}
	}
}
void Emulator::RES_b_r() {
	BYTE op = FetchOpcode();

	int reg = op % 8; // A = 7, B = 0, C = 1, D = 2, etc...
	int bit = (op - 0x80) / 8; // bits 0 - 7

	switch (reg) {
		case 0x0:
		{
			ResetBit(cpu.B, bit);
			break;
		}
		case 0x1:
		{
			ResetBit(cpu.C, bit);
			break;
		}
		case 0x2:
		{
			ResetBit(cpu.D, bit);
			break;
		}
		case 0x3:
		{
			ResetBit(cpu.E, bit);
			break;
		}
		case 0x4:
		{
			ResetBit(cpu.H, bit);
			break;
		}
		case 0x5:
		{
			ResetBit(cpu.L, bit);
			break;
		}
		case 0x6:
		{
			ResetBit(memory.mem[cpu.HL], bit);
			break;
		}
		case 0x7:
		{
			ResetBit(cpu.A, bit);
			break;
		}
	}
}

// Jumps
void Emulator::JP_nn() {
	WORD addr = memory.ReadWord(cpu.PC);
	cpu.PC = addr;
}
void Emulator::JP_cc_nn() {
	BYTE op = memory.ReadByte(cpu.PC - 1);
	WORD addr = memory.ReadWord(cpu.PC);

	switch (op) {
		case 0xC2:
		{
			if (!cpu.GetFlagZ()) cpu.PC = addr;
			break;
		}
		case 0xCA:
		{
			if (cpu.GetFlagZ()) cpu.PC = addr;
			break;
		}
		case 0xD2:
		{
			if (!cpu.GetFlagC()) cpu.PC = addr;
			break;
		}
		case 0xDA:
		{
			if (cpu.GetFlagC()) cpu.PC = addr;
			break;
		}
	}

}
void Emulator::JP_HL() {
	cpu.PC = memory.mem[cpu.HL];
}
void Emulator::JR_n() { // SIGNED VALUE
	cpu.PC += (SIGNED_BYTE)FetchOpcode(); // just for simplicity
}
void Emulator::JR_cc_n() {
	BYTE op = memory.ReadByte(cpu.PC - 1);

	switch (op) {
		case 0x20:
		{
			if (!cpu.GetFlagZ()) cpu.PC += (SIGNED_BYTE)FetchOpcode();
			break;
		}
		case 0x28:
		{
			if (cpu.GetFlagZ()) cpu.PC += (SIGNED_BYTE)FetchOpcode();
			break;
		}
		case 0x30:
		{
			if (!cpu.GetFlagC()) cpu.PC += (SIGNED_BYTE)FetchOpcode();
			break;
		}
		case 0x38:
		{
			if (cpu.GetFlagC()) cpu.PC += (SIGNED_BYTE)FetchOpcode();
			break;
		}
	}
}

// Calls
void Emulator::CALL_nn() {
	WORD addr = memory.ReadWord(cpu.PC);
	StackPush(addr);
}
void Emulator::CALL_cc_nn() {
	BYTE op = memory.ReadByte(cpu.PC - 1);
	WORD addr = memory.ReadWord(cpu.PC);

	switch (op) {
		case 0xC4:
		{
			if (!cpu.GetFlagZ()) StackPush(addr);
			break;
		}
		case 0xCC:
		{
			if (cpu.GetFlagZ()) StackPush(addr);
			break;
		}
		case 0xD4:
		{
			if (!cpu.GetFlagC()) StackPush(addr);
			break;
		}
		case 0xDC:
		{
			if (cpu.GetFlagC()) StackPush(addr);
			break;
		}
	}
}

// Restarts
void Emulator::RST_n() {
	cpu.PC = memory.ReadByte(cpu.PC - 1) - 0xC7; // opcode gives addr to jump to
}

// Returns
void Emulator::RET() {
	cpu.PC = StackPop();
}
void Emulator::RET_cc() {
	BYTE op = memory.ReadByte(cpu.PC - 1);

	switch (op) {
		case 0xC4:
		{
			if (!cpu.GetFlagZ()) cpu.PC = StackPop();
			break;
		}
		case 0xCC:
		{
			if (cpu.GetFlagZ()) cpu.PC = StackPop();
			break;
		}
		case 0xD4:
		{
			if (!cpu.GetFlagC()) cpu.PC = StackPop();
			break;
		}
		case 0xDC:
		{
			if (cpu.GetFlagC()) cpu.PC = StackPop();
			break;
		}
	}
}
void Emulator::RETI() {
	cpu.PC = StackPop();
	interruptsOn = true;
}