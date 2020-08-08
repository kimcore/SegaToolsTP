#include <windows.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "board/io4.h"

#include "swdcio/swdcio.h"

#include "util/dprintf.h"

#include "MinHook.h"
#include "Xinput.h"

//
// Stricture used for our Xbox One Controller
//

uint16_t gamebtn;
static HANDLE hSection;
static int* secData;
int* ffbOffset;
int* ffbOffset2;
int* ffbOffset3;
int* ffbOffset4;
struct XBOXONE_STATE
{
	int guideButton;
	int view;
	int menu;
	int rightShoulder;
	int rightTrigger;
	int rightThumb;
	int leftShoulder;
	int leftTrigger;
	int leftThumb;
	int thumbRX;
	int thumbRY;
	int thumbLX;
	int thumbLY;
	int up;
	int down;
	int left;
	int right;
	int yButton;
	int bButton;
	int aButton;
	int xButton;
};

//
// Device types available in XINPUT_CAPABILITIES
//
#define XINPUT_DEVTYPE_GAMEPAD          0x01

//
// Device subtypes available in XINPUT_CAPABILITIES
//
#define XINPUT_DEVSUBTYPE_GAMEPAD       0x01

//
// Flags for XINPUT_CAPABILITIES
//
#define XINPUT_CAPS_VOICE_SUPPORTED     0x0004

//
// Constants for gamepad buttons
//
#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y                0x8000

#define XINPUT_GAMEPAD_GUIDE			0x400

//
// Flags to pass to XInputGetCapabilities
//
#define XINPUT_FLAG_GAMEPAD             0x00000001

//
// Devices that support batteries
//
#define BATTERY_DEVTYPE_GAMEPAD         0x00
#define BATTERY_DEVTYPE_HEADSET         0x01

//
// Flags for battery status level
//
#define BATTERY_TYPE_DISCONNECTED       0x00    // This device is not connected
#define BATTERY_TYPE_WIRED              0x01    // Wired device, no battery
#define BATTERY_TYPE_ALKALINE           0x02    // Alkaline battery source
#define BATTERY_TYPE_NIMH               0x03    // Nickel Metal Hydride battery source
#define BATTERY_TYPE_UNKNOWN            0xFF    // Cannot determine the battery type

// These are only valid for wireless, connected devices, with known battery types
// The amount of use time remaining depends on the type of device.
#define BATTERY_LEVEL_EMPTY             0x00
#define BATTERY_LEVEL_LOW               0x01
#define BATTERY_LEVEL_MEDIUM            0x02
#define BATTERY_LEVEL_FULL              0x03

// User index definitions
#define XUSER_MAX_COUNT                 4

typedef struct _XINPUT_VIBRATION_EX
{
	WORD                                wLeftMotorSpeed;
	WORD                                wRightMotorSpeed;
	WORD                                wLeftTriggerMotorSpeed;
	WORD                                wRightTriggerMotorSpeed;
} XINPUT_VIBRATION_EX, *PXINPUT_VIBRATION_EX;

extern bool daytonaPressStart;


struct XboxOneControllerHandler
{
	struct usb_dev_handle* handle;
	bool isConnected;
	struct XBOXONE_STATE controller;

	uint8_t lastState[64];
	unsigned int tickCount;

	XINPUT_GAMEPAD lastGamepadState;
};

struct XboxOneControllerHandler* controllerHandler[4] = { NULL, NULL, NULL, NULL };
HANDLE XboxOneControllerThread[4] = { 0 };
HANDLE XboxOneControllerMutex[4] = { 0 };

static unsigned short idVendor = 0x045E;
static unsigned short idProduct = 0x02D1;

int configuration = 1;
int _interface = 0;
int endpointIn = 0x81;
int endpointOut = 0x01;
int timeout = 2000; // milliseconds 
bool controllerInit = false;
bool runThread = true;

// Structure we receive from the controller
struct XboxOneControllerState
{
	char eventCount;
	char unknown;
	char buttons1;
	char buttons2;
	short leftTrigger;  // Triggers are 0 - 1023
	short rightTrigger;
	short thumbLX;      // Axes are -32767 - 32767
	short thumbLY;
	short thumbRX;
	short thumbRY;
};

bool connectController(bool enable)
{
	controllerInit = enable;
	dprintf("connectController\n");
	return true;
}

int iround(double num) {
	return (num > 0.0) ? (int)floor(num + 0.5) : (int)ceil(num - 0.5);
}

DWORD WINAPI XInputSetStateHook
(
	__in DWORD             dwUserIndex,						// Index of the gamer associated with the device
	__in XINPUT_VIBRATION* pVibration						// The vibration information to send to the controller
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		// We're receiving as XInput [0 ~ 65535], need to be [0 ~ 255] !!
		int leftVal = iround(((float)pVibration->wLeftMotorSpeed / 65535) * 255);
		int rightVal = iround(((float)pVibration->wRightMotorSpeed / 65535) * 255);

		//*ffbOffset5 = leftVal;
		//*ffbOffset6 = rightVal;

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputGetCapabilitiesHook
(
	__in  DWORD                dwUserIndex,					// Index of the gamer associated with the device
	__in  DWORD                dwFlags,						// Input flags that identify the device type
	__out XINPUT_CAPABILITIES* pCapabilities				// Receives the capabilities
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (dwFlags > XINPUT_FLAG_GAMEPAD)
	{
		return ERROR_BAD_ARGUMENTS;
	}

	if (controllerInit && dwUserIndex == 0)
	{
		pCapabilities->Flags = XINPUT_CAPS_VOICE_SUPPORTED;
		pCapabilities->Type = XINPUT_DEVTYPE_GAMEPAD;
		pCapabilities->SubType = XINPUT_DEVSUBTYPE_GAMEPAD;

		pCapabilities->Gamepad.wButtons = 0xF3FF;

		pCapabilities->Gamepad.bLeftTrigger = 0xFF;
		pCapabilities->Gamepad.bRightTrigger = 0xFF;

		pCapabilities->Gamepad.sThumbLX = (SHORT)0xFFC0;
		pCapabilities->Gamepad.sThumbLY = (SHORT)0xFFC0;
		pCapabilities->Gamepad.sThumbRX = (SHORT)0xFFC0;
		pCapabilities->Gamepad.sThumbRY = (SHORT)0xFFC0;

		pCapabilities->Vibration.wLeftMotorSpeed = 0xFF;
		pCapabilities->Vibration.wRightMotorSpeed = 0xFF;

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

void WINAPI XInputEnableHook
(
	__in bool enable										// [in] Indicates whether xinput is enabled or disabled. 
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && !enable)
	{
		XINPUT_VIBRATION Vibration = { 0, 0 };
		int xboxControllerCounter = 0;

		while (xboxControllerCounter < 4)
		{
			if (controllerHandler[xboxControllerCounter])
			{
				XInputSetStateHook(xboxControllerCounter, &Vibration);
			}
			xboxControllerCounter++;
		}
	}
}

DWORD WINAPI XInputGetDSoundAudioDeviceGuidsHook
(
	__in  DWORD dwUserIndex,								// Index of the gamer associated with the device
	__out GUID* pDSoundRenderGuid,							// DSound device ID for render
	__out GUID* pDSoundCaptureGuid							// DSound device ID for capture
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		pDSoundRenderGuid = NULL;
		pDSoundCaptureGuid = NULL;

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD XInputGetBatteryInformationHook
(
	__in  DWORD                       dwUserIndex,			// Index of the gamer associated with the device
	__in  BYTE                        devType,				// Which device on this user index
	__out XINPUT_BATTERY_INFORMATION* pBatteryInformation	// Contains the level and types of batteries
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		pBatteryInformation->BatteryType = BATTERY_TYPE_WIRED;
		pBatteryInformation->BatteryLevel = BATTERY_LEVEL_FULL;
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputGetKeystrokeHook
(
	__in       DWORD dwUserIndex,							// Index of the gamer associated with the device
	SAL__reserved DWORD dwReserved,							// Reserved for future use
	__out      PXINPUT_KEYSTROKE pKeystroke					// Pointer to an XINPUT_KEYSTROKE structure that receives an input event.
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		return ERROR_EMPTY; // or ERROR_SUCCESS
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputGetStateExHook
(
	__in  DWORD         dwUserIndex,						// Index of the gamer associated with the device
	__out XINPUT_STATE* pState								// Receives the current state
)
{
	if (!controllerInit)
	{
		connectController(true);
	}
	if (controllerInit && dwUserIndex == 0)
	{
		XINPUT_GAMEPAD gamepadState = { 0 };
		    // START 
			if (gamebtn & swdc_IO_GAMEBTN_STEERING_BLUE)
			{
				gamepadState.wButtons = XINPUT_GAMEPAD_X;
			}
			// BUTTON1
			if (gamebtn & swdc_IO_GAMEBTN_STEERING_RED)
			{
				gamepadState.wButtons = XINPUT_GAMEPAD_B;
			}
			// BUTTON2
			if (gamebtn & swdc_IO_GAMEBTN_STEERING_GREEN)
			{
				gamepadState.wButtons = XINPUT_GAMEPAD_A;
			}
			// BUTTON3
			if (gamebtn & swdc_IO_GAMEBTN_STEERING_YELLOW)
			{
				gamepadState.wButtons = XINPUT_GAMEPAD_Y;
			}
			// BUTTON4
			if (gamebtn & swdc_IO_GAMEBTN_STEERING_PADDLE_LEFT)
			{
				gamepadState.wButtons = XINPUT_GAMEPAD_LEFT_SHOULDER;
			}
            if (gamebtn & swdc_IO_GAMEBTN_STEERING_PADDLE_RIGHT)
			{
				gamepadState.wButtons = XINPUT_GAMEPAD_RIGHT_SHOULDER;
			}

		if (pState->dwPacketNumber == UINT_MAX)
			pState->dwPacketNumber = 0;
		else
			pState->dwPacketNumber++;
		pState->Gamepad = gamepadState;
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

DWORD WINAPI XInputSetStateExHook
(
	__in DWORD             dwUserIndex,						// Index of the gamer associated with the device
	__in XINPUT_VIBRATION_EX* pVibration					// The vibration information to send to the controller
)
{
	if (!controllerInit)
	{
		connectController(true);
	}

	if (controllerInit && dwUserIndex == 0)
	{
		int leftTriggerVal = iround(((float)pVibration->wLeftTriggerMotorSpeed / 65535) * 255);
		int rightTriggerVal = iround(((float)pVibration->wRightTriggerMotorSpeed / 65535) * 255);
		int leftVal = iround(((float)pVibration->wLeftMotorSpeed / 65535) * 255);
		int rightVal = iround(((float)pVibration->wRightMotorSpeed / 65535) * 255);

		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}


DWORD WINAPI XInputGetStateHook
(
	__in  DWORD         dwUserIndex,						// Index of the gamer associated with the device
	__out XINPUT_STATE* pState								// Receives the current state
)
{
	if (!controllerInit)
	{
		connectController(true);
	}
	if (controllerInit && dwUserIndex == 0)
	{
		XINPUT_GAMEPAD gamepadState = { 0 };
			if (*ffbOffset & 0x20) {
				gamepadState.wButtons = XINPUT_GAMEPAD_LEFT_SHOULDER;
			}

			if (*ffbOffset & 0x100) {
				gamepadState.wButtons = XINPUT_GAMEPAD_RIGHT_SHOULDER;
			}

			if (*ffbOffset & 0x0400){
				gamepadState.wButtons = XINPUT_GAMEPAD_X;
			}

			if (*ffbOffset & 0x0800){
				gamepadState.wButtons = XINPUT_GAMEPAD_B;
			}

			if (*ffbOffset & 0x01000){
				gamepadState.wButtons = XINPUT_GAMEPAD_A;
			}

			if (*ffbOffset & 0x02000){
				gamepadState.wButtons = XINPUT_GAMEPAD_Y;
			}
		if (pState->dwPacketNumber == UINT_MAX)
			pState->dwPacketNumber = 0;
		else
			pState->dwPacketNumber++;

		pState->Gamepad = gamepadState;
		return ERROR_SUCCESS;
	}
	else
	{
		return ERROR_DEVICE_NOT_CONNECTED;
	}
}

static HRESULT swdc_io4_poll(void *ctx, struct io4_state *state);

static const struct io4_ops swdc_io4_ops = {
    .poll = swdc_io4_poll,
};

HRESULT swdc_io4_hook_init(void)
{
    HRESULT hr;

    hr = io4_hook_init(&swdc_io4_ops, NULL);

    if (FAILED(hr)) {
        return hr;
    }

    hSection = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 64, "TeknoParrot_JvsState");
    secData = (int*)MapViewOfFile(hSection, FILE_MAP_ALL_ACCESS, 0, 0, 64);
    // Battle gear
    ffbOffset = &secData[2];
    ffbOffset2 = &secData[3];
    ffbOffset3 = &secData[4];
    ffbOffset4 = &secData[5];

	LPCWSTR ptrHook = L"xinput1_3.dll";
    if (MH_Initialize() == MH_OK){
		controllerInit = true;
		dprintf("mh init\n");
		if (MH_CreateHookApi(ptrHook, "XInputGetState", XInputGetStateHook, NULL) == MH_OK){
			dprintf("xinputgetstate\n");
		}
		if (MH_CreateHookApi(ptrHook, "XInputSetState", XInputSetStateHook, NULL) == MH_OK){
			dprintf("xinputsetstate\n");
		}
		if (MH_CreateHookApi(ptrHook, "XInputGetCapabilities", XInputGetCapabilitiesHook, NULL) == MH_OK){
			dprintf("XInputGetCapabilities\n");
		}
		if (MH_CreateHookApi(ptrHook, "XInputEnable", XInputEnableHook, NULL) == MH_OK){
			dprintf("XInputEnable\n");
		}
		if (MH_CreateHookApi(ptrHook, "XInputGetDSoundAudioDeviceGuids", XInputGetDSoundAudioDeviceGuidsHook, NULL) == MH_OK){
			dprintf("XInputGetDSoundAudioDeviceGuids\n");
		}
		if (MH_CreateHookApi(ptrHook, "XInputGetBatteryInformation", XInputGetBatteryInformationHook, NULL) == MH_OK){
			dprintf("XInputGetBatteryInformation\n");
		}
		if (MH_CreateHookApi(ptrHook, "XInputGetKeystroke", XInputGetKeystrokeHook, NULL) == MH_OK){
			dprintf("XInputGetKeystroke\n");
		}
		if (MH_CreateHookApiEx(ptrHook, "XInputGetStateEx", XInputGetStateExHook, NULL, NULL) == MH_OK){
			dprintf("XInputGetStateEx\n");
		}
		else{
			dprintf("failed getstateex\n");
		}
		if(MH_CreateHookApiEx(ptrHook, "XInputSetStateEx", XInputSetStateExHook, NULL, NULL) == MH_OK){
			dprintf("XInputSetStateEx\n");
		}
		else{
			dprintf("failed setstateex\n");
		}
		if (MH_EnableHook(MH_ALL_HOOKS) == MH_OK){
			dprintf("hooks enabled\n");
		}
	}


    return swdc_io_init();
}

static HRESULT swdc_io4_poll(void *ctx, struct io4_state *state)
{
    uint8_t opbtn;

    int16_t wheel;
    int16_t gas;
    int16_t brake;
    HRESULT hr;
    memset(state, 0, sizeof(*state));

    hr = swdc_io_poll();

    if (FAILED(hr)) {
        dprintf("fail\n");
        return hr;
    }

    opbtn = 0;
    gamebtn = 0;
    wheel = 0;
    gas = 0;
    brake = 0;

    swdc_io_get_opbtns(&opbtn);
    swdc_io_get_gamebtns(&gamebtn);
    swdc_io_get_wheel(&wheel);
    swdc_io_get_gas(&gas);
    swdc_io_get_brake(&brake);

    if (opbtn & swdc_IO_OPBTN_TEST) {
        state->buttons[0] |= IO4_BUTTON_TEST;
    }

    if (opbtn & swdc_IO_OPBTN_SERVICE) {
        state->buttons[0] |= IO4_BUTTON_SERVICE;
    }

    //Player 0

    if (gamebtn & swdc_IO_GAMEBTN_UP) {
        state->buttons[0] |= 1 << 5;
    }

    if (gamebtn & swdc_IO_GAMEBTN_DOWN) {
        state->buttons[0] |= 1 << 4;
    }

    if (gamebtn & swdc_IO_GAMEBTN_LEFT) {
        state->buttons[0] |= 1 << 3;
    }

    if (gamebtn & swdc_IO_GAMEBTN_RIGHT) {
        state->buttons[0] |= 1 << 2;
    }

    if (gamebtn & swdc_IO_GAMEBTN_START) {
        state->buttons[0] |= 1 << 7;
    }

    if (gamebtn & swdc_IO_GAMEBTN_VIEW_CHANGE) {
        state->buttons[0] |= 1 << 1;
    }





    // non-working inputs

    if (gamebtn & swdc_IO_GAMEBTN_STEERING_BLUE) {
        dprintf("blue\n");
        state->buttons[1] |= 1 << 15;
    }

    if (gamebtn & swdc_IO_GAMEBTN_STEERING_GREEN) {
        dprintf("green\n");
        state->buttons[1] |= 1 << 14;
    }

    if (gamebtn & swdc_IO_GAMEBTN_STEERING_RED) {
        dprintf("red\n");
        state->buttons[1] |= 1 << 13;
    }

    if (gamebtn & swdc_IO_GAMEBTN_STEERING_YELLOW) {
        dprintf("yellow\n");
        state->buttons[1] |= 1 << 12;
    }

    if (gamebtn & swdc_IO_GAMEBTN_STEERING_PADDLE_LEFT) {
        dprintf("left\n");
        state->buttons[1] |= 1 << 1;
    }

    if (gamebtn & swdc_IO_GAMEBTN_STEERING_PADDLE_RIGHT) {
        dprintf("right\n");
        state->buttons[1] |= 1 << 0;

    }

    state->adcs[0] = wheel;
    state->adcs[1] = gas;
    state->adcs[2] = brake;
    return S_OK;
}
