#pragma once
#include <cstdint>
#include <array>

#include "ppu2C02.h"
#include "cpu6502.h"
#include "Cartridge.h"

class Bus
{
public:
		Bus();
		~Bus();

	//Devices on the bus
	cpu6502 cpu;

	//2C02 PPU
	ppu2C02 ppu;

	//RAM
	uint8_t mainRam[2048];

	uint8_t controller[2];

	//Cart
	std::shared_ptr<Cartridge> cart;

	//Write to ram.
	void cpuWrite(uint16_t addr, uint8_t data);
	//Read ram.
	uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

	//System interface
	void insertCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void reset();
	void clock();

private:
	uint32_t nSystemClockCounter = 0;
};

