#include <cstdio>
#include <fstream>

#include "sim86_shared.h"
#pragma comment (lib, "sim86_shared_debug.lib")

#include "Cpu.h"
#include "DataTransferExec.h"
#include "ArithmeticExec.h"
#include "ControlExec.h"


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << format("Usage: {} <input binary file>", argv[0]) << endl;
		return -1;
	}

	const string filename = argv[1];

	ifstream inFile(filename, ios::binary);

	if (!inFile.is_open())
	{
		cout << "Error opening file: " << filename << endl;
		return -1;
	}

	inFile.seekg(0, inFile.end);
	const u32 mCodeLen = inFile.tellg();
	inFile.seekg(0, inFile.beg);

	// read the machine code into vector
	u8 *mCode = new u8[mCodeLen];
	inFile.read(reinterpret_cast<char*>(mCode), mCodeLen);

	u32 Version = Sim86_GetVersion();
	printf("Sim86 Version: %u (expected %u)\n", Version, SIM86_VERSION);
	if (Version != SIM86_VERSION)
	{
		printf("ERROR: Header file version doesn't match DLL.\n");
		return -1;
	}

	instruction_table Table;
	Sim86_Get8086InstructionTable(&Table);
	printf("8086 Instruction Instruction Encoding Count: %u\n", Table.EncodingCount);

	//u32 Offset = 0;
	while (cpu.ip < mCodeLen)
	{
		instruction Decoded;
		Sim86_Decode8086Instruction(mCodeLen - cpu.ip, mCode + cpu.ip, &Decoded);
		//Offset += Decoded.Size;

		// advance the PC counter
		cpu.ip += Decoded.Size;


		switch (Decoded.Op)
		{
		case Op_mov:
			dataTransferExec(&Decoded);
			break;
		case Op_add:
		case Op_sub:
		case Op_cmp:
			ArithmeticExec(&Decoded);
			break;
		case Op_jne:
			ControlExec(&Decoded);
			break;
		}

		//printRegs();
		//getchar();
	}

	printRegs();
	printMem(1000, 5);

	return 0;
}
