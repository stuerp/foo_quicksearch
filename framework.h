
/** $VER: framework.h (2023.02.25) P. Stuer **/

#pragma once

#define TOSTRING_IMPL(x) #x
#define TOSTRING(x) TOSTRING_IMPL(x)

#include <SDKDDKVer.h>

#include <sdk/foobar2000.h>

#include <strsafe.h>

#include <array>

#include "Preferences.h"
#include "Resources.h"

constexpr const char * _ComponentName = STR_COMPONENT_NAME;
