
/** $VER: main.cpp (2022.12.11) P. Stuer **/

#include "framework.h"

#pragma hdrstop

DECLARE_COMPONENT_VERSION
(
    _ComponentName,
    STR_COMPONENT_VERSION,
    STR_COMPONENT_FILENAME " " STR_COMPONENT_VERSION "\n"
        "Copyright (c) 2022 LionTech. All rights reserved.\n"
        "Written by P. Stuer\n"
        "\n"
        "Implements quick search patterns.\n"
        "\n"
        "Build with foobar2000 SDK " TOSTRING(FOOBAR2000_SDK_VERSION) ".\n"
        "Build: " __TIME__ ", " __DATE__
);

VALIDATE_COMPONENT_FILENAME(STR_COMPONENT_FILENAME);
