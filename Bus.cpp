
#include "Bus.h"

Bus::Bus()
{
	cpu.ConnectBus(this);
}


Bus::~Bus()
{
}

//Writes to main RAM
void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	if (cart->cpuWrite(addr, data))
	{
		
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		mainRam[addr & 0x07FF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		ppu.cpuWrite(addr & 0x0007, data);
	}
}

//Reads main RAM
uint8_t Bus::cpuRead(uint16_t addr, bool readOnly)
{
	uint8_t data = 0x00;


	if (cart->cpuRead(addr, data))
	{
		
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		data = mainRam[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		data = ppu.cpuRead(addr & 0x0007, readOnly);
	}
	else if (addr >= 0x4016 && addr <= 4017)
	{

	}

	return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	this->cart = cartridge;
	ppu.ConnectCartridge(cartridge);
}

void Bus::reset()
{
	cart->reset();
	cpu.reset();
	ppu.reset();
	nSystemClockCounter = 0;
}

//Perform one system clock
void Bus::clock()
{
	ppu.clock();
	if (nSystemClockCounter % 3 == 0)
	{
		cpu.clock(); //Cpu clocks 3 times slower than ppu.
	}

	if (ppu.nmi)
	{
		ppu.nmi = false;
		cpu.nmi();
	}
	nSystemClockCounter++;
}
