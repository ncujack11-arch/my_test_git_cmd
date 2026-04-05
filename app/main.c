#include <stdint.h>
//0x8010000 -> SP
//0x0810004 -> PC
#define APP_BASE_ADDRESS 0x08010000
extern void JumpApp(uint32_t base);
extern void board_level_init(void);
extern void bootloader_main(void);
int main(void)
{
    board_level_init();

    bootloader_main();

	// JumpApp(APP_BASE_ADDRESS);

	return 0;
}



