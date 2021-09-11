#pragma once
#include "Mapper.h"
class Mapper_000 :
	public Mapper
{
public:
	Mapper_000(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_000();

	virtual bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	virtual bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) override;
	virtual bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	virtual bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
	void reset() override;
};

