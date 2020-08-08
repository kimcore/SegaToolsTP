#include <windows.h>

#include "board/io4.h"
#include "board/sg-reader.h"
#include "board/vfd.h"

#include "hook/process.h"

#include "hooklib/serial.h"
#include "hooklib/spike.h"

#include "hotdhook/config.h"
#include "hotdhook/io4.h"

#include "platform/platform.h"

#include "util/dprintf.h"

static HMODULE hotd_hook_mod;
static process_entry_t hotd_startup;
static struct hotd_hook_config hotd_hook_cfg;

static DWORD CALLBACK hotd_pre_startup(void)
{
    HRESULT hr;

    dprintf("--- Begin hotd_pre_startup ---\n");

    /* Load config */

    hotd_hook_config_load(&hotd_hook_cfg, L".\\segatools.ini");

    /* Hook Win32 APIs */

    serial_hook_init();

    /* Initialize emulation hooks */

    hr = platform_hook_init(
            &hotd_hook_cfg.platform,
            "SDET",
            "AAV2",
            hotd_hook_mod);

    if (FAILED(hr)) {
        return hr;
    }

    hr = sg_reader_hook_init(&hotd_hook_cfg.aime, 1);

    if (FAILED(hr)) {
        return hr;
    }

    hr = vfd_hook_init(2);

    if (FAILED(hr)) {
        return hr;
    }

    hr = hotd_io4_hook_init();

    if (FAILED(hr)) {
        return hr;
    }

    /* Initialize debug helpers */

    spike_hook_init(L".\\segatools.ini");

    dprintf("---  End  hotd_pre_startup ---\n");

    /* Jump to EXE start address */

    return hotd_startup();
}

BOOL WINAPI DllMain(HMODULE mod, DWORD cause, void *ctx)
{
    HRESULT hr;

    if (cause != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    hotd_hook_mod = mod;

    hr = process_hijack_startup(hotd_pre_startup, &hotd_startup);

    if (!SUCCEEDED(hr)) {
        dprintf("Failed to hijack process startup: %x\n", (int) hr);
    }

    return SUCCEEDED(hr);
}
