#include <assert.h>
#include <stddef.h>

#include "board/config.h"

#include "hotdhook/config.h"

#include "platform/config.h"

void hotd_hook_config_load(
        struct hotd_hook_config *cfg,
        const wchar_t *filename)
{
    assert(cfg != NULL);
    assert(filename != NULL);

    platform_config_load(&cfg->platform, filename);
    aime_config_load(&cfg->aime, filename);
}
