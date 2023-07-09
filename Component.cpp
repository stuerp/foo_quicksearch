
/** $VER: Component.cpp (2023.07.09) P. Stuer **/

#include "framework.h"

#pragma hdrstop

namespace
{
    DECLARE_COMPONENT_VERSION
    (
        STR_COMPONENT_NAME,
        STR_COMPONENT_VERSION,
        STR_COMPONENT_BASENAME " " STR_COMPONENT_VERSION "\n"
            STR_COMPONENT_COPYRIGHT "\n"
            STR_COMPONENT_COMMENTS "\n"
            "\n"
            STR_COMPONENT_DESCRIPTION "\n"
            "\n"
            "Built with foobar2000 SDK " TOSTRING(FOOBAR2000_SDK_VERSION) "\n"
            "on " __DATE__ " " __TIME__ ".\n"
    )
    VALIDATE_COMPONENT_FILENAME(STR_COMPONENT_FILENAME)
}
