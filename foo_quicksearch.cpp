
/** $VER: foo_quicksearch.cpp (2022.12.05) P. Stuer **/

#include "framework.h"

#include "Resources.h"

#pragma hdrstop

static constexpr const char * _ComponentName = STR_COMPONENT_NAME;

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
        "Based on marc2003's foo_qsearch.\n"
        "\n"
        "Build with foobar2000 SDK " TOSTRING(FOOBAR2000_SDK_VERSION) ".\n"
        "Build: " __TIME__ ", " __DATE__
);

VALIDATE_COMPONENT_FILENAME(STR_COMPONENT_FILENAME);

#pragma region("GUIDs")

static constexpr GUID GUIDContextMenu = { 0x74ea6352, 0x3a06, 0x4ae7, { 0x92, 0x88, 0x1c, 0xaa, 0x6e, 0xf5, 0xd6, 0xc7 } };
    static constexpr GUID GUIDArtist = { 0xcf64efae, 0x7c27, 0x4f44, { 0x88, 0xcf, 0xf6, 0xd, 0x1a, 0x2a, 0x90, 0x17 } };
    static constexpr GUID GUIDTitle = { 0xc3043a77, 0x114a, 0x41ec, { 0x99, 0x64, 0x34, 0x50, 0x66, 0x9f, 0xc8, 0x77 } };
    static constexpr GUID GUIDComposer = {0xc5bf853b,0x354f,0x478a,{0xa0,0xf5,0x2a,0x67,0x58,0xd4,0x9f,0xd9}}; // {c5bf853b-354f-478a-a0f5-2a6758d49fd9}
    static constexpr GUID GUIDAlbum = { 0x4bd0ae21, 0x3e48, 0x4ec4, { 0x90, 0xc, 0x3c, 0x14, 0x25, 0xc6, 0x28, 0x1e } };

static constexpr GUID GUIDAdvancedConfig = { 0x57cd1f9d, 0xb4fc, 0x46a9, { 0xad, 0x34, 0x20, 0x72, 0x39, 0x9c, 0x37, 0xa6 } };
    static constexpr GUID GUIDCreatePlaylist = { 0x64b8d088, 0xcfa6, 0x472e, { 0x89, 0x35, 0x2a, 0x8c, 0x9d, 0x56, 0xdd, 0xe2 } };
    static constexpr GUID GUIDActivatePlaylist = { 0x9944325d, 0xd4e8, 0x4267, { 0x90, 0xf3, 0xfd, 0x66, 0x1b, 0xad, 0x2c, 0xd1 } };
    static constexpr GUID GUIDOpenMediaLibrarySearch = { 0x525edd00, 0xc292, 0x4704, { 0xb6, 0xb8, 0xe, 0xca, 0x9c, 0xe8, 0x17, 0xb4 } };

static constexpr std::array ItemGUIDs =
{
    &GUIDArtist,
    &GUIDTitle,
    &GUIDComposer,
    &GUIDAlbum,
};

#pragma endregion

#pragma region("Settings")

static advconfig_branch_factory _AdvancedConfig(_ComponentName, GUIDAdvancedConfig, advconfig_branch::guid_branch_tools, 0);
    static advconfig_radio_factory _CreatePlaylistSettings("Create playlist", GUIDCreatePlaylist, GUIDAdvancedConfig, 0, true);
    static advconfig_radio_factory _ActivatePlaylistSettings("Create and activate playlist", GUIDActivatePlaylist, GUIDAdvancedConfig, 1, false);
    static advconfig_radio_factory _OpenMediaLibrarySearchSettings("Open Media Library search window", GUIDOpenMediaLibrarySearch, GUIDAdvancedConfig, 2, false);

#pragma endregion

#pragma region("Context Menu")

class ContextMenu : public contextmenu_item_simple
{
public:
    GUID get_parent() final
    {
        return GUIDContextMenu;
    }

    GUID get_item_guid(uint32_t itemIndex) final
    {
        if (itemIndex >= ItemGUIDs.size())
            FB2K_BugCheck();

        return *ItemGUIDs[itemIndex];
    }

    void get_item_name(uint32_t itemIndex, pfc::string_base & out) final
    {
        if (itemIndex >= ItemGUIDs.size())
            FB2K_BugCheck();

        bool IsCtrlPressed = ::IsKeyPressed(VK_CONTROL);

        out = pfc::format(GetPredicate(itemIndex, IsCtrlPressed), " same ", GetFieldName(itemIndex));
    }

    bool get_item_description(uint32_t itemIndex, pfc::string_base & out) final
    {
        if (itemIndex >= ItemGUIDs.size())
            FB2K_BugCheck();

        out = pfc::format("Search for tracks with the same ", GetFieldName(itemIndex).get_ptr(), ".");

        return true;
    }

    uint32_t get_num_items() final
    {
        return static_cast<uint32_t>(ItemGUIDs.size());
    }

    bool context_get_display(uint32_t index, metadb_handle_list_cref handles, pfc::string_base & out, uint32_t &, const GUID &) final
    {
        if (index >= ItemGUIDs.size())
            FB2K_BugCheck();

        get_item_name(index, out);

        return handles.get_count() == 1;
    }

    void context_command(uint32_t itemIndex, metadb_handle_list_cref handles, const GUID &) final
    {
        if (itemIndex >= ItemGUIDs.size())
            FB2K_BugCheck();

        if (handles.get_count() != 1)
            return;

        bool IsCtrlPressed = ::IsKeyPressed(VK_CONTROL);

        const pfc::string8 FieldName = GetFieldName(itemIndex);
        const pfc::string8 FieldValue = GetFieldValue(FieldName, handles[0]);

        if (FieldValue.is_empty())
        {
            FB2K_console_print(_ComponentName, ": Search cancelled. ", FieldName, " was not set or empty.");

            return;
        }

        const pfc::string8 QueryText = pfc::format(FieldName, " ", GetPredicate(itemIndex, IsCtrlPressed).toUpper(), " ", FieldValue);

        if (!_OpenMediaLibrarySearchSettings.get())
        {
            search_filter_v2::ptr Filter;

            try
            {
                Filter = search_filter_manager_v2::get()->create_ex(QueryText, fb2k::service_new<completion_notify_dummy>(), search_filter_manager_v2::KFlagSuppressNotify);
            }
            catch (...)
            {
            }

            if (Filter.is_valid())
            {
                auto plm = playlist_manager::get();

                const size_t Playlist = plm->create_playlist(QueryText, QueryText.get_length(), SIZE_MAX);

                if (Playlist != pfc_infinite)
                {
                    auto aplm = autoplaylist_manager::get();

                    aplm->add_client_simple(QueryText, "", Playlist, 0);

                    if (_ActivatePlaylistSettings.get())
                        plm->set_active_playlist(Playlist);
                }
            }
        }
        else
            library_search_ui::get()->show(QueryText);
    }

private:
    pfc::string8 GetFieldValue(const char * field, const metadb_handle_ptr & handle) const noexcept
    {
        const pfc::string8 Pattern = pfc::format("[%", field, "%]");

        titleformat_object_ptr CompiledPattern;

        titleformat_compiler::get()->compile_safe(CompiledPattern, Pattern);

        pfc::string8 Result;

        auto pbc = playback_control::get();

        metadb_handle_ptr NowPlaying;

        if (!pbc->get_now_playing(NowPlaying))
            handle->format_title(nullptr, Result, CompiledPattern, nullptr);
        else
            if (NowPlaying->get_location() == handle->get_location())
                pbc->playback_format_title(nullptr, Result, CompiledPattern, nullptr, playback_control::display_level_all);

        return Result;
    }

    pfc::string8 GetPredicate(uint32_t index, bool isCtrlPressed) const noexcept
    {
        return !isCtrlPressed ? "Is" : "Has";
    }

    pfc::string8 GetFieldName(uint32_t index) const noexcept
    {
        if (index == 0)
            return "artist";

        if (index == 1)
            return "title";

        if (index == 2)
            return "composer";

        if (index == 3)
            return "album";

        return "";
    }
};

static contextmenu_group_popup_factory g_context_group(GUIDContextMenu, contextmenu_groups::root, _ComponentName, 0);

FB2K_SERVICE_FACTORY(ContextMenu);

#pragma endregion
