#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "amex/amex.h"

#include "board/sg-reader.h"

#include "swdchook/zinput.h"

#include "platform/platform.h"

struct swdc_hook_config {
    struct platform_config platform;
    struct amex_config amex;
    struct aime_config aime;
    struct zinput_config zinput;
};

void swdc_hook_config_load(
        struct swdc_hook_config *cfg,
        const wchar_t *filename);

void zinput_config_load(struct zinput_config *cfg, const wchar_t *filename);
