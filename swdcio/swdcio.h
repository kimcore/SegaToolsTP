#pragma once

#include <windows.h>

#include <stdint.h>


enum {
    swdc_IO_OPBTN_TEST = 0x01,
    swdc_IO_OPBTN_SERVICE = 0x02,
};

enum {
    swdc_IO_GAMEBTN_UP = 0x01,
    swdc_IO_GAMEBTN_DOWN = 0x02,
    swdc_IO_GAMEBTN_LEFT = 0x04,
    swdc_IO_GAMEBTN_RIGHT = 0x08,
    swdc_IO_GAMEBTN_START = 0x10,
    swdc_IO_GAMEBTN_VIEW_CHANGE = 0x20,
    swdc_IO_GAMEBTN_STEERING_BLUE = 0x40,
    swdc_IO_GAMEBTN_STEERING_GREEN = 0x80,
    swdc_IO_GAMEBTN_STEERING_RED = 0x100,
    swdc_IO_GAMEBTN_STEERING_YELLOW = 0x200,
    swdc_IO_GAMEBTN_STEERING_PADDLE_LEFT = 0x400,
    swdc_IO_GAMEBTN_STEERING_PADDLE_RIGHT = 0x800,
};

HRESULT swdc_io_init(void);

HRESULT swdc_io_poll(void);

void swdc_io_get_opbtns(uint8_t *opbtn);

void swdc_io_get_gamebtns(uint16_t *gamebtn);

void swdc_io_get_wheel(int16_t *pos);

void swdc_io_get_gas(int16_t *pos);

void swdc_io_get_brake(int16_t *pos);
