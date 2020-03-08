#include <windows.h>
#include <xinput.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "idzio/backend.h"
#include "idzio/config.h"
#include "idzio/idzio.h"
#include "idzio/shifter.h"
#include "idzio/tp.h"

#include "util/dprintf.h"

static void idz_tp_jvs_read_buttons(uint8_t *gamebtn_out);
static void idz_tp_jvs_read_shifter(uint8_t *gear);
static void idz_tp_jvs_read_shifter_pos(uint8_t* gear);
static void idz_tp_jvs_read_shifter_virt(uint8_t* gear);
static void idz_tp_jvs_read_analogs(struct idz_io_analog_state *out);
static bool idz_tp_jvs_shifter_pos;
static HANDLE hSection;
static int* secData;
int* ffbOffset;
int* ffbOffset2;
int* ffbOffset3;
int* ffbOffset4;
static HRESULT idz_tp_config_apply(const struct idz_tp_config *cfg);

static const struct idz_io_backend idz_tp_backend = {
    .jvs_read_buttons   = idz_tp_jvs_read_buttons,
    .jvs_read_shifter   = idz_tp_jvs_read_shifter,
    .jvs_read_analogs   = idz_tp_jvs_read_analogs,
};

HRESULT idz_tp_init(const struct idz_tp_config *cfg, const struct idz_io_backend **backend)
{
    hSection = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 64, "TeknoParrot_JvsState");
    secData = (int*)MapViewOfFile(hSection, FILE_MAP_ALL_ACCESS, 0, 0, 64);
    // Battle gear
    ffbOffset = &secData[2];
    ffbOffset2 = &secData[3];
    ffbOffset3 = &secData[4];
    ffbOffset4 = &secData[5];
    HRESULT hr;
    assert(cfg != NULL);
    assert(backend != NULL);
    hr = idz_tp_config_apply(cfg);

    if (FAILED(hr)) {
        return hr;
    }

    dprintf("Using TeknoParrot Input Support\n");
    *backend = &idz_tp_backend;

    return S_OK;
}

static HRESULT idz_tp_config_apply(const struct idz_tp_config *cfg)
{
    dprintf("TeknoParrot SegaTools Support by nzgamer41\n");
    if (cfg->pos_shifter) {
        dprintf("Individual Gear Shifter is enabled!\n");
        idz_tp_jvs_shifter_pos = cfg->pos_shifter;
    }
    else
    {
        dprintf("Using Shift Up and Down!\n");
        idz_tp_jvs_shifter_pos = false;
    }
    return S_OK;
}

static void idz_tp_jvs_read_buttons(uint8_t *gamebtn_out)
{
    
    uint8_t gamebtn;
    

    assert(gamebtn_out != NULL);

    gamebtn = 0;

    if (*ffbOffset & 0x02) {
        gamebtn |= IDZ_IO_GAMEBTN_UP;
    }

    if (*ffbOffset & 0x04) {
        gamebtn |= IDZ_IO_GAMEBTN_DOWN;
    }

    if (*ffbOffset & 0x08) {
        gamebtn |= IDZ_IO_GAMEBTN_LEFT;
    }

    if (*ffbOffset & 0x10) {
        gamebtn |= IDZ_IO_GAMEBTN_RIGHT;
    }

    if (*ffbOffset & 0x01) {
        gamebtn |= IDZ_IO_GAMEBTN_START;
    }

    if (*ffbOffset & 0x0200) {
        gamebtn |= IDZ_IO_GAMEBTN_VIEW_CHANGE;
    }

    *gamebtn_out = gamebtn;
}

static void idz_tp_jvs_read_shifter(uint8_t *gear)
{
    if (idz_tp_jvs_shifter_pos != false) {
        idz_tp_jvs_read_shifter_pos(gear);
    }
    else {
        idz_tp_jvs_read_shifter_virt(gear);
    }
}

void idz_tp_jvs_read_shifter_pos(uint8_t* gear)
{
    assert(gear != NULL);

    if (*ffbOffset & 0x0400)
    {
        *gear = 1;
    }
    else if (*ffbOffset & 0x0800)
    {
        *gear = 2;
    }
    else if (*ffbOffset & 0x1000)
    {
        *gear = 3;
    }
    else if (*ffbOffset & 0x2000)
    {
        *gear = 4;
    }
    else if (*ffbOffset & 0x4000)
    {
        *gear = 5;
    }
    else if (*ffbOffset & 0x8000)
    {
        *gear = 6;
    }
}


static void idz_tp_jvs_read_shifter_virt(uint8_t *gear)
{
    bool shift_dn;
    bool shift_up;

    assert(gear != NULL);



    if (*ffbOffset & 0x01) {
        /* Reset to Neutral when start is pressed */
        idz_shifter_reset();
    }

    shift_dn = *ffbOffset & 0x0100;
    shift_up = *ffbOffset & 0x20;

    idz_shifter_update(shift_dn, shift_up);

    *gear = idz_shifter_current_gear();
}

static void idz_tp_jvs_read_analogs(struct idz_io_analog_state *out)
{
    //XINPUT_STATE tp;
    //int left;
    //int right;

    assert(out != NULL);


    //left = tp.Gamepad.sThumbLX;

    //if (left < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
    //    left += XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
    //} else if (left > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
    //    left -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
    //} else {
    //    left = 0;
    //}

    //right = tp.Gamepad.sThumbRX;

    //if (right < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
    //    right += XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
    //} else if (right > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
    //    right -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
    //} else {
    //    right = 0;
    //}
    
	int iWheel = ((float)* ffbOffset2);
	int gas = ((float)* ffbOffset3);
	int brake = ((float)* ffbOffset4);
    iWheel = iWheel - 128;
	iWheel = iWheel * 195;
	gas = gas * 74;
	brake = brake * 74;
    out->wheel = iWheel;
    out->accel = gas;
    out->brake = brake;
}
