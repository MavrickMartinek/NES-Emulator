#pragma once
#include <cstdint>
#include <array>

class Mapper
{
public:
	Mapper(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper();

	virtual void reset() = 0;

	virtual bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) = 0;
	virtual bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) = 0;
	virtual bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) = 0;
	virtual bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) = 0;

protected:
	uint8_t MPrgBanks = 0;
	uint8_t MChrBanks = 0;

};

