#include <windows.h>

#include <assert.h>
#include <limits.h>
#include <stdint.h>

#include "swdcio/swdcio.h"

#include "util/dprintf.h"

static uint8_t swdc_opbtn;
static uint16_t swdc_gamebtn;
static int16_t swdc_wheel_pos;
static int16_t swdc_gas_pos;
static int16_t swdc_brake_pos;
static HANDLE hSection;
static int* secData;
int* ffbOffset;
int* ffbOffset2;
int* ffbOffset3;
int* ffbOffset4;

HRESULT swdc_io_init(void)
{


    dprintf("Using TeknoParrot Input Support\n");
    return S_OK;
}

HRESULT swdc_io_poll(void)
{
    hSection = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 64, "TeknoParrot_JvsState");
    secData = (int*)MapViewOfFile(hSection, FILE_MAP_ALL_ACCESS, 0, 0, 64);
    // Battle gear
    ffbOffset = &secData[2];
    ffbOffset2 = &secData[3];
    ffbOffset3 = &secData[4];
    ffbOffset4 = &secData[5];


    int wheel;
    int gas;
    int brake;

    swdc_opbtn = 0;
    swdc_gamebtn = 0;

    if (GetAsyncKeyState('1') & 0x8000 || *ffbOffset & 0x010000) {
        swdc_opbtn |= swdc_IO_OPBTN_TEST;
    }

    if (GetAsyncKeyState('2') & 0x8000|| *ffbOffset & 0x020000) {
        swdc_opbtn |= swdc_IO_OPBTN_SERVICE;
    }

    if (*ffbOffset & 0x01) {
        swdc_gamebtn |= swdc_IO_GAMEBTN_START;
    }

    if (*ffbOffset & 0x02) {
        swdc_gamebtn |= swdc_IO_GAMEBTN_UP;
    }

    if (*ffbOffset & 0x04) {
        swdc_gamebtn |= swdc_IO_GAMEBTN_DOWN;
    }

    if (*ffbOffset & 0x08) {
        swdc_gamebtn |= swdc_IO_GAMEBTN_LEFT;
    }

    if (*ffbOffset & 0x10) {
        swdc_gamebtn |= swdc_IO_GAMEBTN_RIGHT;
    }

    if (*ffbOffset & 0x0200) {
        swdc_gamebtn |= swdc_IO_GAMEBTN_VIEW_CHANGE;
    }

    // this stuff is handled by the XInput Emu because it's not actually part of the USB I/O board.
    // no point reading it here when its not used.
    /*if (*ffbOffset & 0x0100) {
        swdc_gamebtn |= swdc_IO_GAMEBTN_STEERING_PADDLE_LEFT;
    }

    if (*ffbOffset & 0x20) {
        swdc_gamebtn |= swdc_IO_GAMEBTN_STEERING_PADDLE_RIGHT;
    }

    if (*ffbOffset & 0x0400){
        swdc_gamebtn |= swdc_IO_GAMEBTN_STEERING_BLUE;
    }

    if (*ffbOffset & 0x0800){
        swdc_gamebtn |= swdc_IO_GAMEBTN_STEERING_RED;
    }

    if (*ffbOffset & 0x01000){
        swdc_gamebtn |= swdc_IO_GAMEBTN_STEERING_GREEN;
    }

    if (*ffbOffset & 0x02000){
        swdc_gamebtn |= swdc_IO_GAMEBTN_STEERING_YELLOW;
    }*/
    
    wheel = ((float)* ffbOffset2);
	gas = ((float)* ffbOffset3);
	brake = ((float)* ffbOffset4);
    wheel = wheel * 257;
	gas = gas * 257;
	brake = brake * 257;
    swdc_wheel_pos = wheel;
    swdc_gas_pos = gas;
    swdc_brake_pos = brake;   
    

    return S_OK;
}

void swdc_io_get_opbtns(uint8_t *opbtn)
{
    if (opbtn != NULL) {
        *opbtn = swdc_opbtn;
    }
}

void swdc_io_get_gamebtns(uint16_t *gamebtn)
{
    if (gamebtn != NULL){
        *gamebtn = swdc_gamebtn;
    }
}

void swdc_io_get_wheel(int16_t *pos)
{
    if (pos != NULL) {
        *pos = swdc_wheel_pos;
    }
}

void swdc_io_get_gas(int16_t *pos)
{
    if (pos != NULL) {
        *pos = swdc_gas_pos;
    }
}

void swdc_io_get_brake(int16_t *pos)
{
    if (pos != NULL) {
        *pos = swdc_brake_pos;
    }
}
