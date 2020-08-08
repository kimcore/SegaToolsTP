#pragma once

#include <windows.h>

#include <stdint.h>

enum {
    hotd_IO_OPBTN_TEST = 0x01,
    hotd_IO_OPBTN_SERVICE = 0x02,
};

enum {
    hotd_IO_GAMEBTN_1 = 0x01,
    hotd_IO_GAMEBTN_2 = 0x02,
    hotd_IO_GAMEBTN_3 = 0x04,
    hotd_IO_GAMEBTN_SIDE = 0x08,
    hotd_IO_GAMEBTN_MENU = 0x10,
};

HRESULT hotd_io_init(void);

HRESULT hotd_io_poll(void);

void hotd_io_get_opbtns(uint8_t *opbtn);

void hotd_io_get_gamebtns(uint8_t *left, uint8_t *right);

void hotd_io_get_lever(int16_t *pos);
