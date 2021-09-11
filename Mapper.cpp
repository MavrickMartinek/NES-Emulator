
#include "Mapper.h"


Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks)
{
	MPrgBanks = prgBanks;
	MChrBanks = chrBanks;

	reset();
}


Mapper::~Mapper()
{
}

void Mapper::reset()
{

}
