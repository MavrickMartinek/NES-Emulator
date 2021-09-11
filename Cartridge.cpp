#include "Cartridge.h"

Cartridge::Cartridge(const std::string& sFileName)
{
	//iNES Format Header
	struct sHeader
	{
		char name[4];
		uint16_t prgRomUnits;
		uint8_t chrRomUnits;
		uint8_t mapper1;
		uint8_t mapper2;
		uint8_t ram_size;
		uint8_t tvSystem1;
		uint8_t tvSystem2;
		char unused[5];
	} header;
	bImageValid = false;
	std::ifstream ifs;
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		// Read header
		ifs.read((char*)&header, sizeof(sHeader));

		if (header.mapper1 & 0x04)
			ifs.seekg(512, std::ios_base::cur);

		// Get mapper ID
		mapperID = ((header.mapper2 >> 4) >> 4) | (header.mapper1 >> 4);
		mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

		uint8_t fileType = 1;

		if (fileType == 0)
		{
			
		}

		if (fileType == 1)
		{
			//cartHeader = header.name + " " + header.prgRomUnits + " " + header.chrRomUnits + " " + header.mapper1 + " " + header.mapper2 + " " + header.ram_size + " " + header.tvSystem1 + " " + header.tvSystem2 + " " + header.unused;
			//cartHeader = header.prgRomUnits;
			prgBanks = header.prgRomUnits;
			prgMemory.resize(prgBanks * 16384);
			ifs.read((char*)prgMemory.data(), prgMemory.size());

			chrBanks = header.chrRomUnits;

			if (chrBanks == 0)
			{
				//Create CHR ram
				chrMemory.resize(8192);
			}
			else
			{
				//Allocate for rom
				chrMemory.resize(chrBanks * 8192);
			}
			ifs.read((char*)chrMemory.data(), chrMemory.size());
		}

		if (fileType == 2)
		{

		}

		//Load mapper
		switch (mapperID)
		{
		case 0: 
			pMapper = std::make_shared<Mapper_000>(prgBanks, chrBanks); 
			break;
		}

		bImageValid = true;
		ifs.close();
	}
}


Cartridge::~Cartridge()
{
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t & data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapRead(addr, mapped_addr))
	{
		data = prgMemory[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_adr = 0;
	if (pMapper->cpuMapWrite(addr, mapped_adr, data))
	{
		prgMemory[mapped_adr] = data;
		return true;
	}
	else
	{
		return false;
	}
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t & data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->ppuMapRead(addr, mapped_addr))
	{
		data = chrMemory[mapped_addr];
		return true;
	}
	else
	{
		return false;
	}

}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->ppuMapWrite(addr, mapped_addr))
	{
		chrMemory[mapped_addr] = data;
		return true;
	}
	else
		return false;
}

bool Cartridge::ImageValid()
{
	return bImageValid;
}

void Cartridge::reset()
{
	if (pMapper != nullptr)
	{
		pMapper->reset();
	}
}
