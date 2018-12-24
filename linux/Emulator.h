#include "Memory.h"
#include "CPU.h"
#include "Cartridge.h"

class Emulator {
public:
	Memory memory;
	CPU cpu;

	BOOL isHalted;
	BOOL interruptsOn; // Interrupt Master Enable (IME)
	int cycles;
	BYTE pixels[144][160]; // 160x144

	Emulator();
	~Emulator();

	BYTE FetchOpcode();
	int ExecuteOpcode(BYTE opcode);
	void ExecuteOpcodeMisc();

	void Update();
	void UpdateGraphics();
    void HandleInterrupts();

	// Helper functions
	void SwapBits(BYTE & value);
	BOOL TestBit(BYTE value, int bit);
	void SetBit(BYTE & value, int bit);
	void ResetBit(BYTE & value, int bit);

	void RotateRight(BYTE & value); // rotate through carry
	void RotateRightC(BYTE & value); // rotate, store LSb in carry
	void RotateLeft(BYTE & value);
	void RotateLeftC(BYTE & value);
	
	void StackPush(WORD value);
	WORD StackPop();

	// Functions to execute instructions
	// 8-Bit Loads
	void LD(BYTE & dest, BYTE & src);
	void LD_n(BYTE & dest);
	void LD_nn_A();
	void LDD_A_HL();
	void LDD_HL_A();
	void LD_A_nn();

	// 16-Bit Loads
	void LD_Rd_Rn(WORD & dest, WORD & src);
	void LD_Rd_n(WORD & dest);
	void LD_Rd_nn(WORD & dest);
	void LD_nn_Rn(WORD & src);
	void LD_HL_SP_n();
	void PUSH(WORD src);
	void POP(WORD & dest);

	// 8-Bit Arithmetic
	void ADD_A_Rn(BYTE & src);
	void ADD_A_n();
	void ADC_A_Rn(BYTE & src);
	void ADC_A_n();
	void SUB_A(BYTE & src);
	void SUB_A_n();
	void SBC_A(BYTE & src);
	void SBC_A_n();
	void AND_A(BYTE & src);
	void AND_A_n();
	void OR_A(BYTE & src);
	void OR_A_n();
	void XOR_A(BYTE & src);
	void XOR_A_n();
	void CP_A(BYTE & src);
	void CP_A_n();
	void INC(BYTE & src);
	void DEC(BYTE & src);

	// 16-Bit Arithmetic
	void ADD_HL_Rn(WORD src);
	void ADD_SP_n();
	void INC_Rd(WORD & reg);
	void DEC_Rd(WORD & reg);

	// Miscellaneous
	void SWAP();
	void DAA();
	void CPL_A();
	void CCF();
	void SCF();
	void NOP();
	void HALT();
	void STOP();
	void DI();
	void EI();

	// Rotates & Shifts
	void RLC_A();
	void RL_A();
	void RRC_A();
	void RR_A();
	void RLC_n();
	void RL_n();
	void RRC_n();
	void RR_n();
	void SLA_n();
	void SRA_n();
	void SRL_n();

	// Bit Operations
	void BIT_b_r();
	void SET_b_r();
	void RES_b_r();

	// Jumps
	void JP_nn();
	void JP_cc_nn();
	void JP_HL();
	void JR_n(); // SIGNED VALUE
	void JR_cc_n();

	// Calls
	void CALL_nn();
	void CALL_cc_nn();

	// Restarts
	void RST_n();

	// Returns
	void RET();
	void RET_cc();
	void RETI();
};
