#include "chip8.h"

chip8::chip8()	//clear registers and memory
{
	pc = 0x200;
	opcode = 0;
	indexReg = 0;
	sp = 0;

	memset(registers, 0, 16 * sizeof(char));	
	memset(memory, 0, 4096 * sizeof(char));
	memset(display, 0, 2048 * sizeof(char));
	memset(stack, 0, 16 * sizeof(short));
	
	unsigned char chip8ft[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int i = 0; i < 80; i++)
		memory[i] = chip8ft[i];

	srand((unsigned)time(0));			//initializes to create a random number

	dtimer = 0;
	stimer = 0;
}

chip8::~chip8()
{
	memset(registers, 0, 16 * sizeof(char));
	memset(memory, 0, 4096 * sizeof(char));
	memset(display, 0, 2048 * sizeof(char));
	memset(stack, 0, 16 * sizeof(short));
}

void chip8::loadGame(string name)
{
	FILE * game;
	fopen_s(&game, name.c_str(), "rb");		//opens game

	int size = 0; 

	fseek(game, 0, SEEK_END);
	size = ftell(game);
	rewind(game);

	char *buffer = new char[size];

	fread(buffer, size, 1, game);

	for (int i = 0; i < size; i++)		//loads game into memory starting at 512
		memory[i + 512] = buffer[i];

	delete buffer;

	fclose(game);

}

void chip8::eCycle()
{

	opcode = (memory[pc] << 8) | memory[pc + 1];

	dFlag = false;

	decode();

	if (dtimer > 0)
	{
		--dtimer;
	}

	if (stimer > 0)
	{
		--stimer;
	}

}

void chip8::decode()
{	
	short regX, regY;

	
	if (opcode == 0x00E0)
	{

		memset(display, 0, 2048 * sizeof(char));

		pc += 2;

	}
	else if (opcode == 0x00EE)
	{
		sp--;		//back up to last address
		pc = stack[sp];		//set program counter to previous address

		pc += 2;
	}
	else if ((opcode & 0xF000) == 0x0000)
	{
		cout << "0NNN calls RCA 1802 program\n";

	}
	else if ((opcode & 0xF000) == 0x1000)
	{
		pc = opcode & 0x0FFF;			//jump to specified address
	}
	else if ((opcode & 0xF000) == 0x2000)
	{
		stack[sp] = pc;		//save current address on stack
		sp++;

		pc = opcode & 0x0FFF;		//get address of subroutine
	}
	else if ((opcode & 0xF000) == 0x3000)
	{
		regX = (opcode & 0x0F00) >> 8;

		//check if register value is NN
		if (registers[regX] == (opcode & 0x00FF))
		{
			pc += 2;			//skip ins
		}
		pc += 2;
		
	}
	else if ((opcode & 0xF000) == 0x4000)
	{

		regX = (opcode & 0x0F00) >> 8;

		//check if register value is NN
		if (registers[regX] != (opcode & 0x00FF))
		{
			pc += 2;		//skip ins
		}

		pc += 2;
	}
	else if ((opcode & 0xF00F) == 0x5000)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		//check if register value equal to one another
		if (registers[regX] == registers[regY])
		{
			pc += 2;			//skip ins
		}

		pc += 2;
	}
	else if ((opcode & 0xF000) == 0x6000)
	{

		regX = (opcode & 0x0F00) >> 8;

		registers[regX] = opcode & 0x00FF;

		pc += 2;
		
	}
	else if ((opcode & 0xF000) == 0x7000)
	{

		regX = (opcode & 0x0F00) >> 8;

		registers[regX] += opcode & 0x00FF;

		pc += 2;

	}
	else if ((opcode & 0xF00F) == 0x8000)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		registers[regX] = registers[regY];

		pc += 2;


	}
	else if ((opcode & 0xF00F) == 0x8001)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		registers[regX] = registers[regX] | registers[regY];

		pc += 2;

	}
	else if ((opcode & 0xF00F) == 0x8002)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		registers[regX] = registers[regX] & registers[regY];

		pc += 2;


	}
	else if ((opcode & 0xF00F) == 0x8003)
	{
		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		registers[regX] = registers[regX] ^ registers[regY];

		pc += 2;

	}
	else if ((opcode & 0xF00F) == 0x8004)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		if (registers[regX] > (0xFF - registers[regY]))		//x + y > 255 then there is a carry
			registers[15] = 1;
		else
			registers[15] = 0;

		registers[regX] = registers[regX] + registers[regY];

		pc += 2;

	}
	else if ((opcode & 0xF00F) == 0x8005)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		if (registers[regY] > registers[regX])		//if x - y < 0 then there is a borrow
			registers[15] = 0;						//set flag to 0 when there is a borrow
		else
			registers[15] = 1;

		registers[regX] = registers[regX] - registers[regY];

		pc += 2;
	}
	else if ((opcode & 0xF00F) == 0x8006)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;
	
		registers[15] = registers[regY] & 0x01;	//saves least significant bit

		registers[regY] = registers[regY] >> 1;
		registers[regX] = registers[regY];

		pc += 2;
	}
	else if ((opcode & 0xF00F) == 0x8007)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		if (registers[regY] < registers[regX])		//y - x < 0 then there is a borrow
			registers[15] = 0;
		else
			registers[15] = 1;

		registers[regX] = registers[regY] - registers[regX];

		pc += 2;
	}
	else if ((opcode & 0xF00F) == 0x800E)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		registers[15] = (registers[regY] & 0x80) >> 7;	//saves most significant bit

		registers[regY] = registers[regY] << 1;
		registers[regX] = registers[regY];

		pc += 2;
	}
	else if ((opcode & 0xF00F) == 0x9000)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		//check if register value are NOT equal to one another
		if (registers[regX] != registers[regY])
		{
			pc += 2;		//skip ins
		}

		pc += 2;

	}
	else if ((opcode & 0xF000) == 0xA000)
	{
		indexReg = opcode & 0x0FFF;
		
		pc += 2;
	}
	else if ((opcode & 0xF000) == 0xB000)
	{

		stack[sp] = pc;		//store program counter on stack
		
		sp++;

		pc = registers[0] + (opcode & 0x0FFF);
	}
	else if ((opcode & 0xF000) == 0xC000)
	{

		regX = (opcode & 0x0F00) >> 8;

		registers[regX] = (rand() % 255) & (opcode & 0x00FF);	//creates random number and does bitwise and

		pc += 2;

	}
	else if ((opcode & 0xF000) == 0xD000)
	{

		regX = (opcode & 0x0F00) >> 8;

		regY = (opcode & 0x00F0) >> 4;

		short height = opcode & 0x000F;

		registers[15] = 0;

		//start drawing by reading bits at indexReg
		int start = registers[regY] * 64 + registers[regX];
		for (int i = 0; i < height; i++)
		{
			char block = memory[indexReg + i];

			for (int j = 0; j < 8; j++)			
			{
				if ((block & (0x80 >> j)) != 0)	//bit is 1
				{
					if (display[start + i * 64 + j] == 1)	//bit is already 1 in display
						registers[15] = 1;

					display[start + i * 64 + j] ^= 1;
				}
			}
		}


		dFlag = true;

		pc += 2;


	}
	else if ((opcode & 0xF0FF) == 0xE09E)
	{
		regX = (opcode & 0x0F00) >> 8;
		
		
		if (key[registers[regX]])	 //key is pressed
		{
			pc += 2;		//key
		}

		pc += 2;
	}
	else if ((opcode & 0xF0FF) == 0xE0A1)
	{

		regX = (opcode & 0x0F00) >> 8;

		
		if (!key[registers[regX]])	 //key is not pressed
		{
			pc += 2;			//skip ins
		}

		pc += 2;
	}
	else if ((opcode & 0xF0FF) == 0xF007)
	{

		regX = (opcode & 0x0F00) >> 8;

		registers[regX] = dtimer;	//sets regX to value of delay timer

		pc += 2;
	}
	else if ((opcode & 0xF0FF) == 0xF00A)
	{

		regX = (opcode & 0x0F00) >> 8;

		for (int i = 0; i < 16; i++)
		{
			if (key[i])	//if it is pressed
			{
				registers[regX] = i;		//puts in register x which key was pressed
				pc += 2;					//move on to next instruction
			}
		}

	}
	else if ((opcode & 0xF0FF) == 0xF015)
	{

		regX = (opcode & 0x0F00) >> 8;

		dtimer = registers[regX];	//sets delay timer to value in regX

		pc += 2;


	}
	else if ((opcode & 0xF0FF) == 0xF018)
	{

		regX = (opcode & 0x0F00) >> 8;

		stimer = registers[regX];	//sets sound timer to value in regX

		pc += 2;

	}
	else if ((opcode & 0xF0FF) == 0xF01E)
	{

		regX = (opcode & 0x0F00) >> 8;

		indexReg += registers[regX];	//adds regX to indexReg

		pc += 2;
	}
	else if ((opcode & 0xF0FF) == 0xF029)
	{

		regX = (opcode & 0x0F00) >> 8;

		indexReg = (registers[regX]) * 5;		//stores location of sprite into index

		pc += 2;
	}
	else if ((opcode & 0xF0FF) == 0xF033)
	{

		regX = (opcode & 0x0F00) >> 8;

		memory[indexReg] = registers[regX] / 100;				//gets most significant digit
		memory[indexReg + 1] = (registers[regX] / 10) % 10;		//gets middle digit
		memory[indexReg + 2] = registers[regX] % 10;			//get least significant digit

		pc += 2;
	}
	else if ((opcode & 0xF0FF) == 0xF055)
	{

		regX = (opcode & 0x0F00) >> 8;

		for (int i = 0; i <= regX; i++)					//stores all the registers up to regX into memory
		{
			memory[indexReg] = registers[i];
			indexReg++;
		}

		pc += 2;
	}
	else if ((opcode & 0xF0FF) == 0xF065)
	{

		regX = (opcode & 0x0F00) >> 8;

		for (int i = 0; i <= regX; i++)					//stores number of bytes (up to regX) of memory into registers
		{
			registers[i] = memory[indexReg];
			indexReg++;
		}



		pc += 2;
	}
	else
	{
		cout << "Error couldn't find opcode\n";
	}
}

bool chip8:: checkKey()
{
	
	for (int i = 0; i < 16; i++)	//clear keys
		key[i] = 0;

	if (GetKeyState('1') & 0x8000)		//1 is pressed
	{
		key[1] = 1;
		return true;
	}
	else if (GetKeyState('2') & 0x8000)		//2 is pressed
	{
		key[2] = 1;
		return true;
	}
	else if (GetKeyState('3') & 0x8000)		
	{
		key[3] = 1;
		return true;
	}
	else if (GetKeyState('4') & 0x8000)		
	{
		key[12] = 1;
		return true;
	}
	else if (GetKeyState('Q') & 0x8000)		
	{
		key[4] = 1;
		return true;
	}
	else if (GetKeyState('W') & 0x8000)		
	{
		key[5] = 1;
		return true;
	}
	else if (GetKeyState('E') & 0x8000)		
	{
		key[6] = 1;
		return true;
	}
	else if (GetKeyState('R') & 0x8000)		
	{
		key[13] = 1;
		return true;
	}
	else if (GetKeyState('A') & 0x8000)		
	{
		key[7] = 1;
		return true;
	}
	else if (GetKeyState('S') & 0x8000)		
	{
		key[8] = 1;
		return true;
	}
	else if (GetKeyState('D') & 0x8000)		
	{
		key[9] = 1;
		return true;
	}
	else if (GetKeyState('F') & 0x8000)	
	{
		key[14] = 1;
		return true;
	}
	else if (GetKeyState('Z') & 0x8000)	
	{
		key[10] = 1;
		return true;
	}
	else if (GetKeyState('X') & 0x8000)	
	{
		key[0] = 1;
		return true;
	}
	else if (GetKeyState('C') & 0x8000)
	{
		key[11] = 1;
		return true;
	}
	else if (GetKeyState('V') & 0x8000)	
	{
		key[15] = 1;
		return true;
	}

	return false;
}

bool chip8::checkDrawFlag()
{
	return dFlag;
}