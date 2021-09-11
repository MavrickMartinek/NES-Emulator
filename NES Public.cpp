//Experimental NES emulator. 
//Author: Mavrick Martinek
//Inspired by: OneLonelyCoder (
// (https://github.com/OneLoneCoder/olcNES)
// (https://www.youtube.com/watch?v=F8kx56OZQhg))
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <bitset>
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "Bus.h"

using namespace std;

class Emulation : public olc::PixelGameEngine
{
public:
	Emulation() { sAppName = "Mavrick Demo"; } //Sets up OLC's PixelGameEngine renderer

private: 

	//The NES
	Bus NES;
	std::shared_ptr<Cartridge> cart;


	bool emulationRun = true;
	float fResidualTime = 0.0f;

	uint8_t nSelectedPallete = 0x00;

	//Utilities
	std::map<uint16_t, std::string> mapAsm;

	std::string hex(uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	void DrawRam(int x, int y, uint16_t rAddr, int rRows, int rCollumns)
	{
		int rRamX = x, rRamY = y;
		for (int row = 0; row < rRows; row++)
		{
			std::string sOffset = "$" + hex(rAddr, 4) + ":";
			for (int col = 0; col < rCollumns; col++)
			{
				sOffset += " " + hex(NES.cpuRead(rAddr, true), 2);
				rAddr += 1;
			}
			DrawString(rRamX, rRamY, sOffset);
			rRamY += 10;
		}
	}

	void DrawCpu(int x, int y)
	{
		std::string status = "STATUS";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x + 64, y, "N", NES.cpu.status & cpu6502::N ? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", NES.cpu.status & cpu6502::V ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", NES.cpu.status & cpu6502::U ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B", NES.cpu.status & cpu6502::B ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D", NES.cpu.status & cpu6502::D ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I", NES.cpu.status & cpu6502::I ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z", NES.cpu.status & cpu6502::Z ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C", NES.cpu.status & cpu6502::C ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(NES.cpu.pc, 4));
		DrawString(x + 75, y + 10, "Cycles: " + std::to_string(NES.cpu.cycle_count));
		DrawString(x, y + 20, "A: $" + hex(NES.cpu.a, 2) + "  [" + std::to_string(NES.cpu.a) + "]");
		DrawString(x, y + 30, "X: $" + hex(NES.cpu.x, 2) + "  [" + std::to_string(NES.cpu.x) + "]");
		DrawString(x, y + 40, "Y: $" + hex(NES.cpu.y, 2) + "  [" + std::to_string(NES.cpu.y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(NES.cpu.stkp, 4));
	}
	
	void DrawCode(int x, int y, int nLines)
	{
		auto it_a = mapAsm.find(NES.cpu.pc);
		int nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			DrawString(x, nLineY, (*it_a).second, olc::CYAN);
			while (nLineY < (nLines * 10) + y)
			{
				nLineY += 10;
				if (++it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}

		it_a = mapAsm.find(NES.cpu.pc);
		nLineY = (nLines >> 1) * 10 + y;
		if (it_a != mapAsm.end())
		{
			while (nLineY > y)
			{
				nLineY -= 10;
				if (--it_a != mapAsm.end())
				{
					DrawString(x, nLineY, (*it_a).second);
				}
			}
		}
	}

public:
	bool OnUserCreate() 
	{
		//Load Cartridge (rom not included)
		cart = std::make_shared<Cartridge>("ROM.nes");

		if (!cart->ImageValid())
			return false;

		NES.insertCartridge(cart);
		//NES.cpu.pc = 0x4020;
		mapAsm = NES.cpu.disassemble(0x0000, 0xFFFF);
		
		NES.cpu.reset();

		//NES.cpu.pc = 0xC79E;
		/*std::stringstream ss;
		//10 x 3
		// *=8000 LDX #10 STX $0000 LDX #3 STX $0001 LDY $0000 LDA #0 CLC loop ADC $0001 DEY BNE loop STA $0002 NOP NOP NOP
		//ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
		//3 x 10
		//ss << "A2 0A 8E 00 00 A0 03 8C 01 00 A9 00 18 6D 00 00 88 D0 FA 8D 02 00 EA EA EA";
		//11 x 5
		ss << "A2 0B 8E 00 00 A0 05 8C 01 00 6D 00 00 88 D0 FA 8D 02 00 EA";
		//100 / 4
		//ss << "A2 64 8E 00 00 A0 04 8C 01 00 AD 00 00 18 ED 01 00 88 D0 FA 8D 02 00 EA EA EA";

		uint16_t nOffset = 0x000F;
		while (!ss.eof())
		{
			std::string b;
			ss >> b;
			NES.mainRam[nOffset++] = (uint8_t)std::stoul(b, nullptr, 16);
		}
		NES.cpu.pc = 0x000F;*/


		return true;
	}
	bool OnUserUpdate(float fElapsedTime)
	{
		Clear(olc::DARK_BLUE);

		if (GetKey(olc::Key::SPACE).bPressed) emulationRun = !emulationRun;
		if (GetKey(olc::Key::R).bPressed) NES.reset();
		if (GetKey(olc::Key::P).bPressed) (++nSelectedPallete) &= 0x07;

		if (emulationRun)
		{
			if (fResidualTime > 0.0f)
				fResidualTime -= fElapsedTime;
			else
			{
				fResidualTime -= (1.0f / 60.0f) - fElapsedTime;
				do { NES.clock(); } while (!NES.ppu.frame_done);
				NES.ppu.frame_done = false;
			}
		}
		else
		{
			//Emulate code step by step
			if (GetKey(olc::Key::C).bPressed)
			{
				do { NES.clock(); } while (!NES.cpu.complete());
				do { NES.clock(); } while (NES.cpu.complete()); //Drains out additions clocks.
			}

			// Emulate 1 frame
			if (GetKey(olc::Key::F).bPressed)
			{
				do { NES.clock(); } while (!NES.ppu.frame_done); //Clock enough times to draw 1 frame
				do { NES.clock(); } while (!NES.cpu.complete()); // Use residual locks to complete instruction
				NES.ppu.frame_done = false; //Reset frame
			}
		}

		DrawCpu(516, 2);
		DrawCode(516, 72, 26);
		//DrawRam(516, 182, 0x4020, 16, 16);
		//DrawRam(2, 182, 0x8000, 16, 16);

		// Draw Palettes & Pattern Tables
		const int nSwatchSize = 6;
		for (int p = 0; p < 8; p++) // For each palette
			for (int s = 0; s < 4; s++) // For each index
				FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340,
					nSwatchSize, nSwatchSize, NES.ppu.GetColorFromPaletteRam(p, s));

		// Draw around selected pallettes.
		DrawRect(516 + nSelectedPallete * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4), nSwatchSize, olc::WHITE);
		//Pattern tables
		DrawSprite(516, 400, &NES.ppu.GetPatternTable(0, nSelectedPallete));
		DrawSprite(648, 400, &NES.ppu.GetPatternTable(1, nSelectedPallete));
		//DrawString(516, 350, cart->cartHeader);
		DrawSprite(0, 0, &NES.ppu.GetScreen(), 2);

		return true;

			/*//Draw palletes & pattern tables
			const int nSwatchSize = 6;
			for (int p = 0; p < 8; p++) //For each palette
				for(int s = 0; s < 4; s++) //For each index
					FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340, nSwatchSize, nSwatchSize, NES.ppu.GetColourFromPaletteRam(p, s))

			// Draw selection reticule around selected palette
			DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4), nSwatchSize, olc::WHITE);

			// Generate Pattern Tables
			DrawSprite(516, 348, &NES.ppu.GetPatternTable(0, nSelectedPalette));
			DrawSprite(648, 348, &NES.ppu.GetPatternTable(1, nSelectedPalette));

			// Draw rendered output ========================================================
			DrawSprite(0, 0, &NES.ppu.GetScreen(), 2);
			return true;*/

			// called once per frame, draws random coloured pixels
			/*for (int x = 0; x < ScreenWidth(); x++)
				for (int y = 0; y < ScreenHeight(); y++)
					Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
			return true;*/
	}
};

int main()
{
	//Bus nes;
	//std::shared_ptr<Cartridge> cart;
	//float time;
	//int result;
	//int resp = 0;

	//int addr;
	//int data;

	//Load cart
	//cart = std::make_shared<Cartridge>("test.nes");
	//Insert into nes
	//nes.insertCartridge(cart);

	//nes.cpu.reset();

	//std::stringstream ss;
	//10 x 3
	// *=8000 LDX #10 STX $0000 LDX #3 STX $0001 LDY $0000 LDA #0 CLC loop ADC $0001 DEY BNE loop STA $0002 NOP NOP NOP
	//ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
	//3 x 10
	//ss << "A2 0A 8E 00 00 A0 03 8C 01 00 A9 00 18 6D 00 00 88 D0 FA 8D 02 00 EA EA EA";
	//11 x 5
	//ss << "A2 0B 8E 00 00 A0 05 8C 01 00 6D 00 00 88 D0 FA 8D 02 00 EA";
	//100 / 4
	//ss << "A2 64 8E 00 00 A0 04 8C 01 00 AD 00 00 18 ED 01 00 88 D0 FA 8D 02 00 EA EA EA";

	//uint16_t nOffset = 0x000F;
	//while (!ss.eof())
	//{
	//	std::string b;
	//	ss >> b;
	//	nes.mainRam[nOffset++] = (uint8_t)std::stoul(b, nullptr, 16);
	//}

	//nes.cpu.pc = 0x000F;


	Emulation demo;
	demo.Construct(950, 480, 2, 2);
	demo.Start();
	

	/*do
	{
		do
		{
			//nes.clock();
			nes.cpu.clock();

		} while (!nes.cpu.complete());
		cout << "\nA register: " << (int)nes.cpu.a;
		cout << "    X register: " << (int)nes.cpu.x;
		cout << "    Y register: " << (int)nes.cpu.y;
		cout << "    PC: " << (int)nes.cpu.pc;
		std::bitset<8> x((int)nes.cpu.status);
		cout << "    Flags: " << x;
		cout << "    Clocks: " << nes.cpu.clock_count;
	} while (nes.cpu.pc != 0x0000);
	cout << "\n\n" << (int)nes.mainRam[0000] << " x " << (int)nes.mainRam[0001] << " = " << (int)nes.mainRam[0002] << "\n\n";
	*/

	/*do
	{
		do
		{
			nes.clock();

		} while (!nes.ppu.frame_done);

		for (int i = 0; i < 261; i++)
		{
			for (int j = 0; j < 341; j++)
			{
				cout << (int)nes.ppu.display[j][i];
			}
			cout << "\n";
		}
		system("CLS");
	} while (resp == 0);*/
	
	/*Bus nes;
	std::shared_ptr<Cartridge> cart;
	//Load cart
	cart = std::make_shared<Cartridge>("SMB.nes");
	//Insert into nes
	nes.insertCartridge(cart);
	cout << cart->cartName;

	cin.get();*/

	return 0;
}