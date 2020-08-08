shared_library(
    'hotdhook',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,
    vs_module_defs : 'hotdhook.def',
    c_pch : '../precompiled.h',
    dependencies : [
        capnhook.get_variable('hook_dep'),
        capnhook.get_variable('hooklib_dep'),
        xinput_lib,
    ],
    link_with : [
        aimeio_dll,
        board_lib,
        hooklib_lib,
        hotdio_dll,
        platform_lib,
        util_lib,
    ],
    sources : [
        'config.c',
        'config.h',
        'dllmain.c',
        'io4.c',
        'io4.h',
    ],
)
