#include <windows.h>
#include <xinput.h>

#include <limits.h>
#include <stdint.h>

#include "hotdio/hotdio.h"

static uint8_t hotd_opbtn;
static uint8_t hotd_left_btn;
static uint8_t hotd_right_btn;
static int16_t hotd_lever_pos;
static int16_t hotd_lever_xpos;

HRESULT hotd_io_init(void)
{
    return S_OK;
}

HRESULT hotd_io_poll(void)
{
    int lever;
    int xlever;
    XINPUT_STATE xi;
    WORD xb;

    hotd_opbtn = 0;
    hotd_left_btn = 0;
    hotd_right_btn = 0;

    if (GetAsyncKeyState('1') & 0x8000) {
        hotd_opbtn |= hotd_IO_OPBTN_TEST;
    }

    if (GetAsyncKeyState('2') & 0x8000) {
        hotd_opbtn |= hotd_IO_OPBTN_SERVICE;
    }

    memset(&xi, 0, sizeof(xi));
    XInputGetState(0, &xi);
    xb = xi.Gamepad.wButtons;

    if (xb & XINPUT_GAMEPAD_DPAD_LEFT) {
        hotd_left_btn |= hotd_IO_GAMEBTN_1;
    }

    if (xb & XINPUT_GAMEPAD_DPAD_UP) {
        hotd_left_btn |= hotd_IO_GAMEBTN_2;
    }

    if (xb & XINPUT_GAMEPAD_DPAD_RIGHT) {
        hotd_left_btn |= hotd_IO_GAMEBTN_3;
    }

    if (xb & XINPUT_GAMEPAD_X) {
        hotd_right_btn |= hotd_IO_GAMEBTN_1;
    }

    if (xb & XINPUT_GAMEPAD_Y) {
        hotd_right_btn |= hotd_IO_GAMEBTN_2;
    }

    if (xb & XINPUT_GAMEPAD_B) {
        hotd_right_btn |= hotd_IO_GAMEBTN_3;
    }

    if (xb & XINPUT_GAMEPAD_BACK) {
        hotd_left_btn |= hotd_IO_GAMEBTN_MENU;
    }

    if (xb & XINPUT_GAMEPAD_START) {
        hotd_right_btn |= hotd_IO_GAMEBTN_MENU;
    }

    if (xb & XINPUT_GAMEPAD_LEFT_SHOULDER) {
        hotd_left_btn |= hotd_IO_GAMEBTN_SIDE;
    }

    if (xb & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
        hotd_right_btn |= hotd_IO_GAMEBTN_SIDE;
    }

    lever = hotd_lever_pos;

    if (abs(xi.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
        lever += xi.Gamepad.sThumbLX / 24;
    }

    if (abs(xi.Gamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
        lever += xi.Gamepad.sThumbRX / 24;
    }

    if (lever < INT16_MIN) {
        lever = INT16_MIN;
    }

    if (lever > INT16_MAX) {
        lever = INT16_MAX;
    }

    hotd_lever_pos = lever;

    xlever = hotd_lever_pos
                    - xi.Gamepad.bLeftTrigger * 64
                    + xi.Gamepad.bRightTrigger * 64;

    if (xlever < INT16_MIN) {
        xlever = INT16_MIN;
    }

    if (xlever > INT16_MAX) {
        xlever = INT16_MAX;
    }

    hotd_lever_xpos = xlever;

    return S_OK;
}

void hotd_io_get_opbtns(uint8_t *opbtn)
{
    if (opbtn != NULL) {
        *opbtn = hotd_opbtn;
    }
}

void hotd_io_get_gamebtns(uint8_t *left, uint8_t *right)
{
    if (left != NULL) {
        *left = hotd_left_btn;
    }

    if (right != NULL ){
        *right = hotd_right_btn;
    }
}

void hotd_io_get_lever(int16_t *pos)
{
    if (pos != NULL) {
        *pos = hotd_lever_xpos;
    }
}
