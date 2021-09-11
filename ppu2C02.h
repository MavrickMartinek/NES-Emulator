#pragma once
#include <cstdint>
#include <array>
#include <memory>

#include "olcPixelGameEngine.h"

#include "Cartridge.h"

//#include "olcPixelGameEngine.h"

class ppu2C02
{
public:
	ppu2C02();
	~ppu2C02();

	//Comm with main bus
	uint8_t cpuRead(uint16_t addr, bool rdonly = false);
	void cpuWrite(uint16_t addr, uint8_t data);

	//Comm with ppu bus
	uint8_t ppuRead(uint16_t addr, bool rdonly = false);
	void ppuWrite(uint16_t addr, uint8_t data);

	void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void clock();
	void reset();

	bool frame_done = false;
	uint32_t display[341][261];
	uint8_t displayPixel();

	// Debugging Utilities
	olc::Sprite& GetScreen();
	olc::Sprite& GetNameTable(uint8_t i);
	olc::Sprite& GetPatternTable(uint8_t i, uint8_t palette);
	olc::Pixel& GetColorFromPaletteRam(uint8_t palette, uint8_t pixel);

	bool frame_complete = false;
	bool nmi = false;

private:
	//The Cart
	std::shared_ptr<Cartridge> cart;

	olc::Pixel  palletteArray[0x40];
	olc::Sprite mainScreen = olc::Sprite(256, 240);
	olc::Sprite sprNameTable[2] = { olc::Sprite(256, 240), olc::Sprite(256, 240) };
	olc::Sprite spritePatternTable[2] = { olc::Sprite(128, 128), olc::Sprite(128, 128) };

	uint8_t tblPattern[2][4096];
	uint8_t tblName[2][1024];
	uint8_t tblPallette[32];
	
	int16_t scanline = 0;
	int16_t cycle = 0;

	union
	{
		struct
		{
			uint8_t unused : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};
		uint8_t reg;
	} status;

	union
	{
		struct
		{
			uint8_t grayscale : 1;
			uint8_t render_background_left : 1;
			uint8_t render_sprites_left : 1;
			uint8_t render_background : 1;
			uint8_t render_sprites : 1;
			uint8_t enhance_red : 1;
			uint8_t enhance_green : 1;
			uint8_t enhance_blue : 1;
		};

		uint8_t reg;
	} mask;

	union PPUCTRL
	{
		struct
		{
			uint8_t nametable_x : 1;
			uint8_t nametable_y : 1;
			uint8_t increment_mode : 1;
			uint8_t pattern_sprite : 1;
			uint8_t pattern_background : 1;
			uint8_t sprite_size : 1;
			uint8_t slave_mode : 1; // unused
			uint8_t enable_nmi : 1;
		};

		uint8_t reg;
	} control;

	union loopy_register
	{
		struct
		{

			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};

		uint16_t reg = 0x0000;
	};

	loopy_register vram_addr; //Pointer address for extracting background tile data
	loopy_register tram_addr; //Temporary data to be transferred to pointer.

	uint8_t address_latch = 0x00;
	uint8_t ppu_data_buffer = 0x00;
	uint16_t ppu_address = 0x0000;

	uint8_t fine_x = 0x00;

	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_attrib = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;

	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo = 0x0000;
	uint16_t bg_shifter_attrib_hi = 0x0000;
};

