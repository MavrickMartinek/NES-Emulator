#include <stdafx.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <bitset>

#include "olcPixelGameEngine.h"

#include "Bus.h"

using namespace std;

class Emulation : public olc::PixelGameEngine
{
public:
		Emulation() { sAppName = "Mavrick Demo"; } //Sets app name for OLC's PixelGameEngine

public:
	bool OnUserCreate() override
	{
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
		return true;
	}

	/*bool RunEmu(float elapsedTime)
	{
		if (time > 0.0f)
			time -= elapsedTime;
		else
		{
			time += (1.0f / 60.0f) - elapsedTime;
			do { nes.clock(); } while (!nes.ppu.frame_done);
			nes.ppu.frame_done = false;
		}
		return true;
	}*/
};

int main()
{
	Emulation demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	Bus nes;
	std::shared_ptr<Cartridge> cart;
	float time;
	int result;
	int resp = 0;
	
	int addr;
	int data;

	/*do {
		nes.cpu.clock();
		cout << "Address: ";
		cin >> std::hex >> addr;
		cout << "Data: ";
		cin >> std::hex >> data;
		nes.write((uint16_t)addr, (uint8_t)data);

		cout << "\nLook for address: ";
		cin >> hex >> addr;
		//cin.ignore();
		data = nes.read((uint16_t)addr);
		cout << "Address 0x" << std::hex << addr << " contains 0x" << std::hex << data << "\n\n";
	} while(resp = 1);*/

	//Load cart
	cart = std::make_shared<Cartridge>("test.nes");
	//Insert into nes
	nes.insertCartridge(cart);

	nes.cpu.reset();
	std::stringstream ss;
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
		nes.mainRam[nOffset++] = (uint8_t)std::stoul(b, nullptr, 16);
	}
	nes.cpu.pc = 0x000F;

	do
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
	cout << "\n\n" << (int)nes.mainRam[0000]<< " x " << (int)nes.mainRam[0001] << " = " << (int)nes.mainRam[0002] << "\n\n";

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
	cin.get();
	
	return 0;
}