swdcio_dll = shared_library(
    'swdcio',
    name_prefix : '',
    include_directories : inc,
    implicit_include_directories : false,
    vs_module_defs : 'swdcio.def',
    c_pch : '../precompiled.h',
    dependencies : [
        xinput_lib,
    ],
    link_with : [
        util_lib,
    ],
    sources : [
        'swdcio.c',
        'swdcio.h',
    ],
)
