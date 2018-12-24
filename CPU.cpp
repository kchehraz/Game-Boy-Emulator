#include "CPU.h"


CPU::CPU() {
	AF = BC = DE = HL = 0;
	
	SP = 0xFFFE;						// stack pointer initialized to address 65534
	PC = 0x100;							// program starts at 0x100 (cartridge header area)
}

void CPU::SetFlagZ(BOOL bit) {
	F ^= (-bit ^ F) & (1 << F_POS); // Change flag bit to input bit
}
void CPU::SetFlagN(BOOL bit) {
	F ^= (-bit ^ F) & (1 << N_POS);
}
void CPU::SetFlagH(BOOL bit) {
	F ^= (-bit ^ F) & (1 << H_POS);
}
void CPU::SetFlagC(BOOL bit) {
	F ^= (-bit ^ F) & (1 << C_POS);
}
BOOL CPU::GetFlagZ() {
	return F & (1 << F_POS);
}
BOOL CPU::GetFlagN() {
	return F & (1 << N_POS);
}
BOOL CPU::GetFlagH() {
	return F & (1 << H_POS);
}
BOOL CPU::GetFlagC() {
	return F & (1 << C_POS);
}

void CPU::SetFlags(BOOL z, BOOL n, BOOL h, BOOL c) {
	SetFlagZ(z);
	SetFlagN(n);
	SetFlagH(h);
	SetFlagC(c);
}

