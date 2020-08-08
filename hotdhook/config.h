#pragma once

#include <stddef.h>

#include "board/config.h"

#include "platform/config.h"

struct hotd_hook_config {
    struct platform_config platform;
    struct aime_config aime;
};

void hotd_hook_config_load(
        struct hotd_hook_config *cfg,
        const wchar_t *filename);
