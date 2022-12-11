
/** $VER: Preferences.cpp (2022.12.11) P. Stuer **/

#include "framework.h"

#pragma hdrstop

#pragma region("GUIDs")
static constexpr GUID GUIDAdvancedConfig = { 0x57cd1f9d, 0xb4fc, 0x46a9, { 0xad, 0x34, 0x20, 0x72, 0x39, 0x9c, 0x37, 0xa6 } };
    static constexpr GUID GUIDCreatePlaylist = { 0x64b8d088, 0xcfa6, 0x472e, { 0x89, 0x35, 0x2a, 0x8c, 0x9d, 0x56, 0xdd, 0xe2 } };
    static constexpr GUID GUIDActivatePlaylist = { 0x9944325d, 0xd4e8, 0x4267, { 0x90, 0xf3, 0xfd, 0x66, 0x1b, 0xad, 0x2c, 0xd1 } };
    static constexpr GUID GUIDOpenMediaLibrarySearch = { 0x525edd00, 0xc292, 0x4704, { 0xb6, 0xb8, 0xe, 0xca, 0x9c, 0xe8, 0x17, 0xb4 } };
#pragma endregion

#pragma region("Preferences")
static advconfig_branch_factory _AdvancedConfig(_ComponentName, GUIDAdvancedConfig, advconfig_branch::guid_branch_tools, 0);
    static advconfig_radio_factory _CreatePlaylistSettings("Create playlist", GUIDCreatePlaylist, GUIDAdvancedConfig, 0, true);
    static advconfig_radio_factory _ActivatePlaylistSettings("Create and activate playlist", GUIDActivatePlaylist, GUIDAdvancedConfig, 1, false);
    static advconfig_radio_factory _OpenMediaLibrarySearchSettings("Open Media Library search window", GUIDOpenMediaLibrarySearch, GUIDAdvancedConfig, 2, false);
#pragma endregion

/// <summary>
/// Returns true if the search result should create a new playlist.
/// </summary>
bool ShouldCreatePlayList()
{
    return _CreatePlaylistSettings;
}

/// <summary>
/// Returns true if the search result playlist should be activated.
/// </summary>
bool ShouldActivatePlaylist()
{
    return _ActivatePlaylistSettings;
}

/// <summary>
/// Returns true if the search result should open the media libary search.
/// </summary>
bool ShouldOpenMediaLibrarySearch()
{
    return _OpenMediaLibrarySearchSettings;
}
