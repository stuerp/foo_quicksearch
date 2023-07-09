
/** $VER: framework.h (2023.07.09) P. Stuer **/

#pragma once

#define TOSTRING_IMPL(x) #x
#define TOSTRING(x) TOSTRING_IMPL(x)

#include <SDKDDKVer.h>

#include <sdk/foobar2000.h>

#include <strsafe.h>

#include <array>

#include "Preferences.h"
#include "Resources.h"
