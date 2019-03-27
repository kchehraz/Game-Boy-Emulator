#include "Emulator.h"
using namespace std;

static int count = 0; // op count for debugging purposes

void debugPrintInfo() {
}

Emulator::Emulator() {
    cycles = 0;
    timer = 0;
    timerCounter = 1024; // Initial value is 1024
    
    isHalted = false;
    interruptsOn = false;
}

Emulator::~Emulator() {

}

// Main update function for the emulator
void Emulator::Update() {
    const int MAXCYCLES = 69905;
    int cyclesThisUpdate = 0;

    while (cyclesThisUpdate < MAXCYCLES) {
        BOOL isHandlingInterrupts = false;
        HandleInterrupts();
        if (isHandlingInterrupts) {
            cyclesThisUpdate += 20;
        }

        BYTE opcode = FetchOpcode();
        cyclesThisUpdate += ExecuteOpcode(opcode);
        cycles += cyclesThisUpdate;
        
        UpdateTimers(cyclesThisUpdate);
    }
    UpdateGraphics();
    //RenderScreen();
}

void Emulator::UpdateTimers(int cycles) {
    if (memory.GetRegTAC()) { // Check timer enabled
        timer += cycles;
    }
}

void Emulator::UpdateGraphics() {
    // Draw background tiles if enabled
    if (TestBit(memory.GetFlagLCDC(), 0)) {
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                RenderTile(i, j, TestBit(memory.GetFlagLCDC(), 3)); // bg tile map selection as arg
            }
        }
    }
    
    // Draw window if enabled
    if (TestBit(memory.GetFlagLCDC(), 5)) {
        cout << "window" << endl;
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                RenderTile(i, j, TestBit(memory.GetFlagLCDC(), 6)); // window tile map selection as arg
            }
        }
    }

    // Draw sprites if enabled
    if (TestBit(memory.GetFlagLCDC(), 1)) {
        cout << "sprites" << endl;
        for (int i = 0; i < 40; i++) {
            RenderSpriteTile(i);
        }
    }
}

void Emulator::RenderTile(int row, int column, int mapBank) {
    int tileBaseAddr = 0x8000;
    bool sigd = 0;
    if (TestBit(memory.GetFlagLCDC(), 4) == 0) {
        tileBaseAddr = 0x9000;
        sigd = 1;
    }
    int mapBaseAddr = 0x9800;
    if (mapBank == 1) {
        mapBaseAddr = 0x9C00;
    }
    int mapIndex = (row*32) + column;
    int tileIndex = memory.mem[mapBaseAddr+mapIndex];
    if (sigd) tileIndex = (SIGNED_BYTE)tileIndex;
    int tileAddr = tileBaseAddr + (tileIndex*16);
    for (int i = row*8; i < ((row*8) + 8); i++) {
        for (int j = column*8; j < ((column*8) + 8); j++) {
            BYTE byte1 = memory.mem[tileAddr];
            BYTE byte2 = memory.mem[tileAddr+1];
            pixels[i][j] = TestBit(byte1, 7-(j%8)) | (TestBit(byte2, 7-(j%8)) << 1);
        }
        tileAddr += 2;
    }
}

void Emulator::RenderSpriteTile(int index) {
    int addr = 0xFE00 + (index*4); // 4 bytes each sprite
    int y = memory.ReadByte(addr);
    int x = memory.ReadByte(addr+1);
    int tileIndex = memory.ReadByte(addr+2);
    int flags = memory.ReadByte(addr+3);

    for (int i = y; i < y+8; i++) {
        for (int j = x; j < x+8; j++) {
            BYTE byte1 = memory.mem[0x8000+tileIndex*16];
            BYTE byte2 = memory.mem[0x8000+tileIndex*16+1];
            pixels[i][j] = TestBit(byte1, 7-(j%8)) | (TestBit(byte2, 7-(j%8)) << 1);
        }
    }
}

void Emulator::HandleInterrupts() {
    if (interruptsOn) {
        BYTE interruptsToHandle = memory.GetFlagIE() & memory.GetFlagIF();
        if (interruptsToHandle) {
            isHalted = false; // unhalt when interrupt occurs
            StackPush(cpu.PC);

            for (int i = 0; i < 5; i++) { // Iterate through the bits, check for which are requested and enabled
                if (TestBit(interruptsToHandle, i)) {
                    cpu.PC = (i * 0x8) + 0x40; // 0x40, 0x48, 0x50, 0x58, 0x60
                    interruptsOn = false;
                    ResetBit(memory.mem[0xFF0F], i);
                    break;
                }
            }
        }
    }
}

// Helper functions
// Increment PC when we fetch the opcode
BYTE Emulator::FetchOpcode() {
    //printf("PC: 0x%04X\n", cpu.PC);
    //printf("SP: 0x%04X\n", cpu.SP);
    //printf("Opcode: %02X\n", memory.ReadByte(cpu.PC));
    //printf("AF: 0x%04X, BC: 0x%04X, DE: 0x%04X, HL: 0x%04X\n", cpu.AF, cpu.BC, cpu.DE, cpu.HL);
    //std::cout << "Press enter to continue...";
    //std::cin.get();
    
    return memory.ReadByte(cpu.PC++);
}

// PC is on the first byte after the opcode
int Emulator::ExecuteOpcode(BYTE opcode) {
    int cycles = 0;
    switch (opcode) {
        // Order in terms of functions rather than opcodes for better readability
        // Based on the order of the CPU manual PDF
        case 0x00: { NOP(); cycles += 4; break; }

                   // 8-Bit Loads
        case 0x06: { LD_n(cpu.B); cycles += 4; break; }
        case 0x0E: { LD_n(cpu.C); cycles += 4; break; }
        case 0x16: { LD_n(cpu.D); cycles += 4; break; }
        case 0x1E: { LD_n(cpu.E); cycles += 4; break; }
        case 0x26: { LD_n(cpu.H); cycles += 4; break; }
        case 0x2E: { LD_n(cpu.L); cycles += 4; break; }
        case 0x3E: { LD_n(cpu.A); cycles += 4; break; }

        case 0x7F: { LD(cpu.A, cpu.A); cycles += 4; break; }
        case 0x78: { LD(cpu.A, cpu.B); cycles += 4; break; }
        case 0x79: { LD(cpu.A, cpu.C); cycles += 4; break; }
        case 0x7A: { LD(cpu.A, cpu.D); cycles += 4; break; }
        case 0x7B: { LD(cpu.A, cpu.E); cycles += 4; break; }
        case 0x7C: { LD(cpu.A, cpu.H); cycles += 4; break; }
        case 0x7D: { LD(cpu.A, cpu.L); cycles += 4; break; }
        case 0x7E: { LD(cpu.A, memory.mem[cpu.HL]); cycles += 8; break; }
        case 0x40: { LD(cpu.B, cpu.B); cycles += 4; break; }
        case 0x41: { LD(cpu.B, cpu.C); cycles += 4; break; }
        case 0x42: { LD(cpu.B, cpu.D); cycles += 4; break; }
        case 0x43: { LD(cpu.B, cpu.E); cycles += 4; break; }
        case 0x44: { LD(cpu.B, cpu.H); cycles += 4; break; }
        case 0x45: { LD(cpu.B, cpu.L); cycles += 4; break; }
        case 0x46: { LD(cpu.B, memory.mem[cpu.HL]); cycles += 8;  break; }
        case 0x48: { LD(cpu.C, cpu.B); cycles += 4; break; }
        case 0x49: { LD(cpu.C, cpu.C); cycles += 4; break; }
        case 0x4A: { LD(cpu.C, cpu.D); cycles += 4; break; }
        case 0x4B: { LD(cpu.C, cpu.E); cycles += 4; break; }
        case 0x4C: { LD(cpu.C, cpu.H); cycles += 4; break; }
        case 0x4D: { LD(cpu.C, cpu.L); cycles += 4; break; }
        case 0x4E: { LD(cpu.C, memory.mem[cpu.HL]); cycles += 8;  break; }
        case 0x50: { LD(cpu.D, cpu.B); cycles += 4; break; }
        case 0x51: { LD(cpu.D, cpu.C); cycles += 4; break; }
        case 0x52: { LD(cpu.D, cpu.D); cycles += 4; break; }
        case 0x53: { LD(cpu.D, cpu.E); cycles += 4; break; }
        case 0x54: { LD(cpu.D, cpu.H); cycles += 4; break; }
        case 0x55: { LD(cpu.D, cpu.L); cycles += 4; break; }
        case 0x56: { LD(cpu.D, memory.mem[cpu.HL]); cycles += 8; break; }
        case 0x58: { LD(cpu.E, cpu.B); cycles += 4; break; }
        case 0x59: { LD(cpu.E, cpu.C); cycles += 4; break; }
        case 0x5A: { LD(cpu.E, cpu.D); cycles += 4; break; }
        case 0x5B: { LD(cpu.E, cpu.E); cycles += 4; break; }
        case 0x5C: { LD(cpu.E, cpu.H); cycles += 4; break; }
        case 0x5D: { LD(cpu.E, cpu.L); cycles += 4; break; }
        case 0x5E: { LD(cpu.E, memory.mem[cpu.HL]); cycles += 8; break; }
        case 0x60: { LD(cpu.H, cpu.B); cycles += 4; break; }
        case 0x61: { LD(cpu.H, cpu.C); cycles += 4; break; }
        case 0x62: { LD(cpu.H, cpu.D); cycles += 4; break; }
        case 0x63: { LD(cpu.H, cpu.E); cycles += 4; break; }
        case 0x64: { LD(cpu.H, cpu.H); cycles += 4; break; }
        case 0x65: { LD(cpu.H, cpu.L); cycles += 4; break; }
        case 0x66: { LD(cpu.H, memory.mem[cpu.HL]); cycles += 8; break; }
        case 0x68: { LD(cpu.L, cpu.B); cycles += 4; break; }
        case 0x69: { LD(cpu.L, cpu.C); cycles += 4; break; }
        case 0x6A: { LD(cpu.L, cpu.D); cycles += 4; break; }
        case 0x6B: { LD(cpu.L, cpu.E); cycles += 4; break; }
        case 0x6C: { LD(cpu.L, cpu.H); cycles += 4; break; }
        case 0x6D: { LD(cpu.L, cpu.L); cycles += 4; break; }
        case 0x6E: { LD(cpu.L, memory.mem[cpu.HL]); cycles += 8; break; }
        case 0x70: { LD(memory.mem[cpu.HL], cpu.B); cycles += 8; break; }
        case 0x71: { LD(memory.mem[cpu.HL], cpu.C); cycles += 8; break; }
        case 0x72: { LD(memory.mem[cpu.HL], cpu.D); cycles += 8; break; }
        case 0x73: { LD(memory.mem[cpu.HL], cpu.E); cycles += 8; break; }
        case 0x74: { LD(memory.mem[cpu.HL], cpu.H); cycles += 8; break; }
        case 0x75: { LD(memory.mem[cpu.HL], cpu.L); cycles += 8; break; }
        case 0x36: { LD_n(memory.mem[cpu.HL]); cycles += 12; break; }

        case 0x0A: { LD(cpu.A, memory.mem[cpu.BC]); cycles += 8; break; }
        case 0x1A: { LD(cpu.A, memory.mem[cpu.DE]); cycles += 8; break; }
        case 0xFA: { LD_A_nn(); cycles += 12; break; }
        case 0x47: { LD(cpu.B, cpu.A); cycles += 4; break; }
        case 0x4F: { LD(cpu.C, cpu.A); cycles += 4; break; }
        case 0x57: { LD(cpu.D, cpu.A); cycles += 4; break; }
        case 0x5F: { LD(cpu.E, cpu.A); cycles += 4; break; }
        case 0x67: { LD(cpu.H, cpu.A); cycles += 4; break; }
        case 0x6F: { LD(cpu.L, cpu.A); cycles += 4; break; }
        case 0x02: { LD(memory.mem[cpu.BC], cpu.A); cycles += 8; break; }
        case 0x12: { LD(memory.mem[cpu.DE], cpu.A); cycles += 8; break; }
        case 0x77: { LD(memory.mem[cpu.HL], cpu.A); cycles += 8; break; }
        case 0xEA: { LD_nn_A(); cycles += 16; break; }
        case 0xF2: { LD(cpu.A, memory.mem[0xFF00 + cpu.C]); cycles += 8; break; }
        case 0xE2: { LD(memory.mem[0xFF00 + cpu.C], cpu.A); cycles += 8; break; }
        case 0x3A: { LDD_A_HL(); cycles += 8; break; }
        case 0x32: { LDD_HL_A(); cycles += 8; break; }
        case 0x2A: { LD(cpu.A, memory.mem[cpu.HL++]); cycles += 8; break; }
        case 0x22: { LD(memory.mem[cpu.HL++], cpu.A); cycles += 8; break; }
        case 0xE0: { LD(memory.mem[0xFF00+memory.mem[cpu.PC++]], cpu.A); cycles += 12; break; }
        case 0xF0: { LD(cpu.A, memory.mem[0xFF00 + memory.mem[cpu.PC++]]); cycles += 12; break; }

                   // 16-Bit Loads
        case 0x01: { LD_Rd_nn(cpu.BC); cycles += 12; break; }
        case 0x11: { LD_Rd_nn(cpu.DE); cycles += 12; break; }
        case 0x21: { LD_Rd_nn(cpu.HL); cycles += 12; break; }
        case 0x31: { LD_Rd_nn(cpu.SP); cycles += 12; break; }
        case 0xF9: { LD_Rd_Rn(cpu.SP, cpu.HL); cycles += 8; break; }
        case 0xF8: { LD_HL_SP_n(); cycles += 12; break; }
        case 0x08: { LD_nn_Rn(cpu.SP); cycles += 20; break; }
        case 0xF5: { PUSH(cpu.AF); cycles += 16; break; }
        case 0xC5: { PUSH(cpu.BC); cycles += 16; break; }
        case 0xD5: { PUSH(cpu.DE); cycles += 16; break; }
        case 0xE5: { PUSH(cpu.HL); cycles += 16; break; }
        case 0xF1: { POP(cpu.AF); cpu.F ^= (cpu.F & 0x0F); cycles += 12; break; }
        case 0xC1: { POP(cpu.BC); cycles += 12; break; }
        case 0xD1: { POP(cpu.DE); cycles += 12; break; }
        case 0xE1: { POP(cpu.HL); cycles += 12; break; }

                   // 8-Bit Arithmetic
        case 0x87: { ADD_A_Rn(cpu.A); cycles += 4; break; }
        case 0x80: { ADD_A_Rn(cpu.B); cycles += 4; break; }
        case 0x81: { ADD_A_Rn(cpu.C); cycles += 4; break; }
        case 0x82: { ADD_A_Rn(cpu.D); cycles += 4; break; }
        case 0x83: { ADD_A_Rn(cpu.E); cycles += 4; break; }
        case 0x84: { ADD_A_Rn(cpu.H); cycles += 4; break; }
        case 0x85: { ADD_A_Rn(cpu.L); cycles += 4; break; }
        case 0x86: { ADD_A_Rn(memory.mem[cpu.HL]); cycles += 8; break; }
        case 0xC6: { ADD_A_n(); cycles += 8; break; }
        case 0x8F: { ADC_A_Rn(cpu.A); cycles += 4; break; }
        case 0x88: { ADC_A_Rn(cpu.B); cycles += 4; break; }
        case 0x89: { ADC_A_Rn(cpu.C); cycles += 4; break; }
        case 0x8A: { ADC_A_Rn(cpu.D); cycles += 4; break; }
        case 0x8B: { ADC_A_Rn(cpu.E); cycles += 4; break; }
        case 0x8C: { ADC_A_Rn(cpu.H); cycles += 4; break; }
        case 0x8D: { ADC_A_Rn(cpu.L); cycles += 4; break; }
        case 0x8E: { ADC_A_Rn(memory.mem[cpu.HL]); cycles += 8; break; }
        case 0xCE: { ADC_A_n(); cycles += 8; break; }
        case 0x97: { SUB_A(cpu.A); cycles += 4; break; }
        case 0x90: { SUB_A(cpu.B); cycles += 4; break; }
        case 0x91: { SUB_A(cpu.C); cycles += 4; break; }
        case 0x92: { SUB_A(cpu.D); cycles += 4; break; }
        case 0x93: { SUB_A(cpu.E); cycles += 4; break; }
        case 0x94: { SUB_A(cpu.H); cycles += 4; break; }
        case 0x95: { SUB_A(cpu.L); cycles += 4; break; }
        case 0x96: { SUB_A(memory.mem[cpu.HL]); cycles += 8; break; }
        case 0xD6: { SUB_A_n(); cycles += 8; break; }
        case 0x9F: { SBC_A(cpu.A); cycles += 4; break; }
        case 0x98: { SBC_A(cpu.B); cycles += 4; break; }
        case 0x99: { SBC_A(cpu.C); cycles += 4; break; }
        case 0x9A: { SBC_A(cpu.D); cycles += 4; break; }
        case 0x9B: { SBC_A(cpu.E); cycles += 4; break; }
        case 0x9C: { SBC_A(cpu.H); cycles += 4; break; }
        case 0x9D: { SBC_A(cpu.L); cycles += 4; break; }
        case 0x9E: { SBC_A(memory.mem[cpu.HL]); cycles += 8; break; }
        case 0xDE: { SBC_A_n(); cycles += 8; break; }
        case 0xA7: { AND_A(cpu.A); cycles += 4; break; }
        case 0xA0: { AND_A(cpu.B); cycles += 4; break; }
        case 0xA1: { AND_A(cpu.C); cycles += 4; break; }
        case 0xA2: { AND_A(cpu.D); cycles += 4; break; }
        case 0xA3: { AND_A(cpu.E); cycles += 4; break; }
        case 0xA4: { AND_A(cpu.H); cycles += 4; break; }
        case 0xA5: { AND_A(cpu.L); cycles += 4; break; }
        case 0xA6: { AND_A(memory.mem[cpu.HL]); cycles += 8; break; }
        case 0xE6: { AND_A_n(); cycles += 8; break; }
        case 0xB7: { OR_A(cpu.A); cycles += 4; break; }
        case 0xB0: { OR_A(cpu.B); cycles += 4; break; }
        case 0xB1: { OR_A(cpu.C); cycles += 4; break; }
        case 0xB2: { OR_A(cpu.D); cycles += 4; break; }
        case 0xB3: { OR_A(cpu.E); cycles += 4; break; }
        case 0xB4: { OR_A(cpu.H); cycles += 4; break; }
        case 0xB5: { OR_A(cpu.L); cycles += 4; break; }
        case 0xB6: { OR_A(memory.mem[cpu.HL]); cycles += 8; break; }
        case 0xF6: { OR_A_n(); cycles += 8; break; }
        case 0xAF: { XOR_A(cpu.A); cycles += 4; break; }
        case 0xA8: { XOR_A(cpu.B); cycles += 4; break; }
        case 0xA9: { XOR_A(cpu.C); cycles += 4; break; }
        case 0xAA: { XOR_A(cpu.D); cycles += 4; break; }
        case 0xAB: { XOR_A(cpu.E); cycles += 4; break; }
        case 0xAC: { XOR_A(cpu.H); cycles += 4; break; }
        case 0xAD: { XOR_A(cpu.L); cycles += 4; break; }
        case 0xAE: { XOR_A(memory.mem[cpu.HL]); cycles += 8; break; }
        case 0xEE: { XOR_A_n(); cycles += 8; break; }
        case 0xBF: { CP_A(cpu.A); cycles += 4; break; }
        case 0xB8: { CP_A(cpu.B); cycles += 4; break; }
        case 0xB9: { CP_A(cpu.C); cycles += 4; break; }
        case 0xBA: { CP_A(cpu.D); cycles += 4; break; }
        case 0xBB: { CP_A(cpu.E); cycles += 4; break; }
        case 0xBC: { CP_A(cpu.H); cycles += 4; break; }
        case 0xBD: { CP_A(cpu.L); cycles += 4; break; }
        case 0xBE: { CP_A(memory.mem[cpu.HL]); cycles += 8; break; }
        case 0xFE: { CP_A_n(); cycles += 8; break; }
        case 0x3C: { INC(cpu.A); cycles += 4; break; }
        case 0x04: { INC(cpu.B); cycles += 4; break; }
        case 0x0C: { INC(cpu.C); cycles += 4; break; }
        case 0x14: { INC(cpu.D); cycles += 4; break; }
        case 0x1C: { INC(cpu.E); cycles += 4; break; }
        case 0x24: { INC(cpu.H); cycles += 4; break; }
        case 0x2C: { INC(cpu.L); cycles += 4; break; }
        case 0x34: { INC(memory.mem[cpu.HL]); cycles += 12; break; }
        case 0x3D: { DEC(cpu.A); cycles += 4; break; }
        case 0x05: { DEC(cpu.B); cycles += 4; break; }
        case 0x0D: { DEC(cpu.C); cycles += 4; break; }
        case 0x15: { DEC(cpu.D); cycles += 4; break; }
        case 0x1D: { DEC(cpu.E); cycles += 4; break; }
        case 0x25: { DEC(cpu.H); cycles += 4; break; }
        case 0x2D: { DEC(cpu.L); cycles += 4; break; }
        case 0x35: { DEC(memory.mem[cpu.HL]); cycles += 12; break; }

                   // 16-Bit Arithmetic
        case 0x09: { ADD_HL_Rn(cpu.BC); cycles += 8; break; }
        case 0x19: { ADD_HL_Rn(cpu.DE); cycles += 8; break; }
        case 0x29: { ADD_HL_Rn(cpu.HL); cycles += 8; break; }
        case 0x39: { ADD_HL_Rn(cpu.SP); cycles += 8; break; }
        case 0xE8: { ADD_SP_n(); cycles += 16; break; }
        case 0x03: { INC_Rd(cpu.BC); cycles += 8; break; }
        case 0x13: { INC_Rd(cpu.DE); cycles += 8; break; }
        case 0x23: { INC_Rd(cpu.HL); cycles += 8; break; }
        case 0x33: { INC_Rd(cpu.SP); cycles += 8; break; }
        case 0x0B: { DEC_Rd(cpu.BC); cycles += 8; break; }
        case 0x1B: { DEC_Rd(cpu.DE); cycles += 8; break; }
        case 0x2B: { DEC_Rd(cpu.HL); cycles += 8; break; }
        case 0x3B: { DEC_Rd(cpu.SP); cycles += 8; break; }

                   // Miscellaneous
        case 0xCB: { cycles += ExecuteOpcodeMisc(); cycles += 4; break; } // 256 more ops; containing bit operations

        case 0x27: { DAA(); cycles += 4; break; }
        case 0x2F: { CPL_A(); cycles += 4; break; }
        case 0x3F: { CCF(); cycles += 4; break; }
        case 0x37: { SCF(); cycles += 4; break; }
        case 0x76: { HALT(); cycles += 4; break; }
        case 0x10: { STOP(); cycles += 4; break; }
        case 0xF3: { DI(); cycles += 4; break; }
        case 0xFB: { EI(); cycles += 4; break; }

                   // Rotates & Shifts
        case 0x07: { RLC_A(); cycles += 4; break; }
        case 0x17: { RL_A(); cycles += 4; break; }
        case 0x0F: { RRC_A(); cycles += 4; break; }
        case 0x1F: { RR_A(); cycles += 4; break; }

                   // Jumps
        case 0xC3: { JP_nn(); cycles += 16; break; }
        case 0xC2: { JP_cc_nn(); cycles += 12; if (!cpu.GetFlagZ()) cycles+=4; break; }
        case 0xCA: { JP_cc_nn(); cycles += 12; if (cpu.GetFlagZ()) cycles+=4; break; }
        case 0xD2: { JP_cc_nn(); cycles += 12; if (!cpu.GetFlagC()) cycles+=4; break; }
        case 0xDA: { JP_cc_nn(); cycles += 12; if (cpu.GetFlagC()) cycles+=4; break; }
        case 0xE9: { JP_HL(); cycles += 4; break; }
        case 0x18: { JR_n(); cycles += 12; break; }
        case 0x20: { JR_cc_n(); cycles += 8; if (!cpu.GetFlagZ()) cycles+=4; break; }
        case 0x28: { JR_cc_n(); cycles += 8; if (cpu.GetFlagZ()) cycles+=4; break; }
        case 0x30: { JR_cc_n(); cycles += 8; if (!cpu.GetFlagC()) cycles+=4; break; }
        case 0x38: { JR_cc_n(); cycles += 8; if (cpu.GetFlagC()) cycles+=4; break; }

                   // Calls
        case 0xCD: { CALL_nn(); cycles += 24; break; }
        case 0xC4: { CALL_cc_nn(); cycles += 12; if (!cpu.GetFlagZ()) cycles+=12; break; }
        case 0xCC: { CALL_cc_nn(); cycles += 12; if (cpu.GetFlagZ()) cycles+=12; break; }
        case 0xD4: { CALL_cc_nn(); cycles += 12; if (!cpu.GetFlagC()) cycles+=12; break; }
        case 0xDC: { CALL_cc_nn(); cycles += 12; if (cpu.GetFlagC()) cycles+=12; break; }

                   // Restarts
        case 0xC7: { RST_n(); cycles += 16; break; }
        case 0xCF: { RST_n(); cycles += 16; break; }
        case 0xD7: { RST_n(); cycles += 16; break; }
        case 0xDF: { RST_n(); cycles += 16; break; }
        case 0xE7: { RST_n(); cycles += 16; break; }
        case 0xEF: { RST_n(); cycles += 16; break; }
        case 0xF7: { RST_n(); cycles += 16; break; }
        case 0xFF: { RST_n(); cycles += 16; break; }

                   // Returns
        case 0xC9: { RET(); cycles += 16; break; }
        case 0xC0: { RET_cc(); cycles += 8; if (!cpu.GetFlagZ()) cycles+=12; break; }
        case 0xC8: { RET_cc(); cycles += 8; if (cpu.GetFlagZ()) cycles+=12; break; }
        case 0xD0: { RET_cc(); cycles += 8; if (!cpu.GetFlagC()) cycles+=12; break; }
        case 0xD8: { RET_cc(); cycles += 8; if (cpu.GetFlagC()) cycles+=12; break; }
        case 0xD9: { RETI(); cycles += 16; break; }


                   //
                   // Empty opcodes
                   // If these values are fetched, this is an error
        default: {
                     printf("Error: opcode 0x%02X not recognized.\n", opcode);
                     printf("Exiting...\n");
                     printf("Press any key to continue...");
                     cin.get();
                     return EXIT_FAILURE;
                 }
                   /*case 0xD3: {  }
                     case 0xDB: {  }
                     case 0xDD: {  }
                     case 0xE3: {  }
                     case 0xE4: {  }
                     case 0xEB: {  }
                     case 0xEC: {  }
                     case 0xED: {  }
                     case 0xF4: {  }
                     case 0xFC: {  }
                     case 0xFD: {  }*/


    }
    return cycles;
}

int Emulator::ExecuteOpcodeMisc() {
    BYTE op = memory.mem[cpu.PC]; // don't need to increment here, we do it in the functions

    // Switch statements already set up within the function
    // Certain operations take more cycles
    int cycles = 8; // minimum 8 cycles for all operations
    if (op >= 0x40 && op < 0x80) {
        BIT_b_r();
        if (op%8 == 6) cycles += 4;
    }
    else if (op >= 0x80 && op < 0xC0) {
        RES_b_r();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0xC0 && op <= 0xFF) {
        SET_b_r();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0x00 && op < 0x08) {
        RLC_n();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0x08 && op < 0x10) {
        RRC_n();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0x10 && op < 0x18) {
        RL_n();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0x18 && op < 0x20) {
        RR_n();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0x20 && op < 0x28) {
        SLA_n();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0x28 && op < 0x30) {
        SRA_n();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0x30 && op < 0x38) {
        SWAP();
        if (op%8 == 6) cycles += 8;
    }
    else if (op >= 0x38 && op < 0x40) {
        SRL_n();
        if (op%8 == 6) cycles += 8;
    }
    return cycles;
}

