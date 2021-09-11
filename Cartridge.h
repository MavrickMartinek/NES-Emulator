#pragma once
#include <cstdint>
#include <fstream>
#include <vector>
#include <string>

#include "Mapper_000.h"

class Cartridge
{
public:
	Cartridge(const std::string& sFileName);
	~Cartridge();

	std::string cartHeader;

	//Comm with main bus
	bool cpuRead(uint16_t addr, uint8_t &data);
	bool cpuWrite(uint16_t addr, uint8_t data);

	//Comm with ppu bus
	bool ppuRead(uint16_t addr, uint8_t &data);
	bool ppuWrite(uint16_t addr, uint8_t data);

	bool ImageValid();

	void reset();

	enum MIRROR
	{
		HORIZONTAL,
		VERTICAL,
		ONSCREEN_LO,
		ONSCREEN_HI,
	} mirror = HORIZONTAL;

private:

	bool bImageValid = false;

	std::vector<uint8_t> prgMemory;
	std::vector<uint8_t> chrMemory;

	uint8_t mapperID = 0;
	uint8_t prgBanks = 0;
	uint8_t chrBanks = 0;

	std::shared_ptr<Mapper> pMapper;
};

