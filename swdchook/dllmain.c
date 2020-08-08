#include <windows.h>

#include <stddef.h>
#include <stdlib.h>

#include "board/sg-reader.h"
#include "board/io4.h"
#include "board/vfd.h"

#include "hook/process.h"

#include "hooklib/gfx.h"
#include "hooklib/serial.h"
#include "hooklib/spike.h"

#include "swdchook/config.h"
#include "swdcio/swdcio.h"
#include "swdchook/io4.h"
#include "swdchook/zinput.h"

#include "platform/platform.h"

#include "util/dprintf.h"
#include "MinHook.h"

static HMODULE swdc_hook_mod;
static process_entry_t swdc_startup;
static struct swdc_hook_config swdc_hook_cfg;

static DWORD CALLBACK swdc_pre_startup(void)
{
    HRESULT hr;

    dprintf("--- Begin swdc_pre_startup ---\n");

    /* Config load */

    swdc_hook_config_load(&swdc_hook_cfg, L".\\segatools.ini");

    /* Hook Win32 APIs */

    serial_hook_init();
    zinput_hook_init(&swdc_hook_cfg.zinput);
    /* Initialize emulation hooks */

    hr = swdc_io4_hook_init();

    if (FAILED(hr)) {
        return hr;
    }

    hr = platform_hook_init(
            &swdc_hook_cfg.platform,
            "SDDS",
            "AAV2",
            swdc_hook_mod);

    if (FAILED(hr)) {
        return hr;
    }

    hr = vfd_hook_init(4);

    if (FAILED(hr)) {
        return hr;
    }

    hr = sg_reader_hook_init(&swdc_hook_cfg.aime, 3);

    if (FAILED(hr)) {
        return hr;
    }

    /* Initialize debug helpers */

    spike_hook_init(L".\\segatools.ini");

    dprintf("---  End  swdc_pre_startup ---\n");

    /* Jump to EXE start address */

    return swdc_startup();
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
    HRESULT hr;

    if (cause != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    swdc_hook_mod = mod;

    hr = process_hijack_startup(swdc_pre_startup, &swdc_startup);

    if (!SUCCEEDED(hr)) {
        dprintf("Failed to hijack process startup: %x\n", (int) hr);
    }

    return SUCCEEDED(hr);
}
