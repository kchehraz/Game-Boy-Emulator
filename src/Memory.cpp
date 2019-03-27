#include <iostream>
#include <cstring>
#include "Memory.h"
using namespace std;



Memory::Memory() {
	memset(mem, 0, MEM_SIZE); // The real architecture does NOT do this, but whatever
}

BYTE Memory::ReadByte(WORD addr) {
	return mem[addr];
}

WORD Memory::ReadWord(WORD addr) {
	return (mem[addr]) | (mem[addr + 1] << 8);
}

void Memory::WriteByte(WORD addr, BYTE value) {
    mem[addr] = value;
}

void Memory::WriteWord(WORD addr, WORD value) {
	// Little endian ordering
	mem[addr] = value & 0xFF;
	mem[addr + 1] = value >> 8;
}

// Display flags
BYTE Memory::GetFlagIE() {
	return mem[0xFFFF]; // memory location of IE flag
}

BYTE Memory::GetFlagIF() {
	return mem[0xFF0F]; // memory location of IF flag
}

BYTE Memory::GetFlagLCDC() {
    return mem[0xFF40];
}

// Timing flags
BYTE Memory::GetRegDIV() {
    return mem[0xFF04];
}

BYTE Memory::GetRegTIMA() {
    return mem[0xFF05];
}

BYTE Memory::GetRegTMA() {
    return mem[0xFF06];
}

BYTE Memory::GetRegTAC() {
    return mem[0xFF07];
}

void Memory::SetRegDIV(BYTE val) {
    mem[0xFF04] = val;
}

void Memory::SetRegTIMA(BYTE val) {
    mem[0xFF05] = val;
}

void Memory::SetRegTMA(BYTE val) {
    mem[0xFF06] = val;
}

void Memory::SetRegTAC(BYTE val) {
    mem[0xFF07] = val;
}
