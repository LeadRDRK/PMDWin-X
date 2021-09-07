project(PMDWin)
cmake_minimum_required(VERSION 3.13)

set(PMDWIN_OPNA_EMU "fmgen" CACHE STRING "OPNA emulator to use (fmgen/nuked)")
option(PMDWIN_BUILD_EXAMPLE "Build the sample program" OFF)

set(PMDWIN_SRC
    pmdwin/opnaw.cpp
    pmdwin/p86drv.cpp
    pmdwin/pmdwin.cpp
    pmdwin/ppsdrv.cpp
    pmdwin/ppz8l.cpp
    pmdwin/table.cpp
    pmdwin/util.cpp
)

set(PMDWIN_HEADERS
    pmdwin/opnaw.h
    pmdwin/pmdwin.h
    pmdwin/ppsdrv.h
    pmdwin/util.h
    pmdwin/ipmdwin.h
    pmdwin/p86drv.h
    pmdwin/ppz8l.h
    pmdwin/ippz8.h
    pmdwin/pcmmusdriver.h
    pmdwin/table.h
)

if(WIN32)
    set(PMDWIN_SRC
        ${PMDWIN_SRC}
        dll/pmdwin.cpp
        dll/pmdwin.def
    )
endif()

# fmgen file i/o interface
set(FMFILEIO_SRC
    fmfileio/fileio.cpp
    fmfileio/sjis2utf.cpp
)

set(FMFILEIO_HEADERS
    fmfileio/fileio.h
    fmfileio/ifileio.h
    fmfileio/sjis2utf.h
)

if(PMDWIN_OPNA_EMU STREQUAL "nuked")
    set(OPNA_SRC
        nuked/opna.cpp
        nuked/ym3438.c
        nuked/ymdeltat.c
        nuked/emu2149/emu2149.c
        common/fmtimer.cpp
    )

    set(OPNA_HEADERS
        nuked/mamedefs.h
        nuked/opna.h
        nuked/ym3438.h
        nuked/ymdeltat.h
        nuked/emu2149/emu2149.h
        common/fmtimer.h
    )
elseif(PMDWIN_OPNA_EMU STREQUAL "fmgen")
    set(OPNA_SRC
        fmgen/fmgen.cpp
        common/fmtimer.cpp
        fmgen/opna.cpp
        fmgen/psg.cpp
    )

    set(OPNA_HEADERS
        fmgen/diag.h
        fmgen/fmgen.h
        fmgen/fmgeninl.h
        common/fmtimer.h
        fmgen/headers_fmgen.h
        fmgen/opna.h
        fmgen/psg.h
    )
else()
    message(FATAL_ERROR "Invalid OPNA emulator specified (expected fmgen or nuked, got ${PMDWIN_OPNA_EMU})")
endif()

add_library(PMDWin
    ${PMDWIN_SRC} ${PMDWIN_HEADERS}
    ${FMFILEIO_SRC} ${FMFILEIO_HEADERS}
    ${OPNA_SRC} ${OPNA_HEADERS}
)
target_include_directories(PMDWin PUBLIC . ${PMDWIN_OPNA_EMU})
if(WIN32)
    target_link_options(PMDWin PRIVATE "LINKER:--enable-stdcall-fixup")
    set_target_properties(PMDWin PROPERTIES PREFIX "")
endif()

if(PMDWIN_BUILD_EXAMPLE)
    find_package(Threads REQUIRED)
    add_executable(pmdplayer examples/pmdplayer.cpp)
    target_link_libraries(pmdplayer PRIVATE PMDWin Threads::Threads ${CMAKE_DL_LIBS})
endif()