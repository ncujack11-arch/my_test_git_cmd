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
//SCBbase   E000ED00

//	uint32_t base = APP_BASE_ADDRESS;
//	uint32_t sp = *(uint32_t *)(base + 0);

//	uint32_t pc = *(uint32_t *)(base + 4);
//	typedef void (*app_func_t)(void);
//	app_func_t app_func = (app_func_t)pc;

//	__set_MSP(sp);//换栈操作，危险系数高，不建议使用
//	app_func();
/*
上一段代码
*/

