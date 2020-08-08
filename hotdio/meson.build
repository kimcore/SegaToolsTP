hotdio_dll = shared_library(
    'hotdio',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,
    vs_module_defs : 'hotdio.def',
    c_pch : '../precompiled.h',
    dependencies : [
        xinput_lib,
    ],
    sources : [
        'hotdio.c',
        'hotdio.h',
    ],
)
