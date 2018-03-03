#ifndef CHIP8_H
#define CHIP8_H
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#pragma once

using namespace std;

class chip8
{
	public:
		chip8();
		~chip8();

		void loadGame(string name);
		
		void eCycle();
		

		bool checkDrawFlag();

		bool checkKey();

		unsigned char display[2048];
		unsigned char key[16];

	private:
		unsigned short opcode;
		unsigned char memory[4096];
		unsigned char registers[16];
		unsigned short indexReg;
		unsigned short pc;

		//unsigned char display[2048];

		unsigned char dtimer;
		unsigned char stimer;

		unsigned short stack[16];
		unsigned short sp;

		bool dFlag;
		void decode();
};
#endif