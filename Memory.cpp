#include <iostream>
#include "Memory.h"



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

BYTE Memory::GetFlagIE() {
	return mem[0xFFFF]; // memory location of IE flag
}

BYTE Memory::GetFlagIF() {
	return mem[0xFF0F]; // memory location of IF flag
}
