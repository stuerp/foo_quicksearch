
/** $VER: ContextMenuHandler.cpp (2023.05.14) P. Stuer **/

#include "framework.h"

#pragma hdrstop

namespace
{
#pragma region("GUIDs")
static constexpr GUID _ContextMenuGUID = { 0x74ea6352, 0x3a06, 0x4ae7, { 0x92, 0x88, 0x1c, 0xaa, 0x6e, 0xf5, 0xd6, 0xc7 } };
    static constexpr GUID _ArtistGUID = { 0xcf64efae, 0x7c27, 0x4f44, { 0x88, 0xcf, 0xf6, 0xd, 0x1a, 0x2a, 0x90, 0x17 } };
    static constexpr GUID _TitleGUID = { 0xc3043a77, 0x114a, 0x41ec, { 0x99, 0x64, 0x34, 0x50, 0x66, 0x9f, 0xc8, 0x77 } };
    static constexpr GUID _ComposerGUID = {0xc5bf853b,0x354f,0x478a,{0xa0,0xf5,0x2a,0x67,0x58,0xd4,0x9f,0xd9}};
    static constexpr GUID _AlbumGUID = { 0x4bd0ae21, 0x3e48, 0x4ec4, { 0x90, 0xc, 0x3c, 0x14, 0x25, 0xc6, 0x28, 0x1e } };
#pragma endregion

#pragma region("Context Menu")
static constexpr std::array _ItemGUIDs =
{
    &_ArtistGUID,
    &_TitleGUID,
    &_ComposerGUID,
    &_AlbumGUID,
};

/// <summary>
/// Implements the context menu handler.
/// </summary>
class ContextMenuHandler : public contextmenu_item_simple
{
public:
#pragma region("contextmenu_item interface")
    /// <summary>
    /// Retrieves number of menu items provided by this contextmenu_item implementation.
    /// </summary>
    virtual unsigned get_num_items() final
    {
        return static_cast<unsigned>(_ItemGUIDs.size());
    }

    /// <summary>
    /// Retrieves human-readable name of the context menu item.
    /// </summary>
    virtual void get_item_name(unsigned int itemIndex, pfc::string_base & out) final
    {
        if (itemIndex >= _ItemGUIDs.size())
            FB2K_BugCheck();

        bool IsCtrlPressed = ::IsKeyPressed(VK_CONTROL);

        out = pfc::format(GetVerb(itemIndex, IsCtrlPressed), " same ", GetFieldName(itemIndex));
    }

    /// <summary>
    /// 
    /// </summary>
    virtual bool context_get_display(unsigned int itemIndex, metadb_handle_list_cref handles, pfc::string_base & out, uint32_t &, const GUID &) final
    {
        if (itemIndex >= _ItemGUIDs.size())
            FB2K_BugCheck();

        get_item_name(itemIndex, out);

        return handles.get_count() == 1;
    }

    /// <summary>
    /// Retrieves GUID of the context menu item.
    /// </summary>
    virtual GUID get_item_guid(unsigned int itemIndex) final
    {
        if (itemIndex >= _ItemGUIDs.size())
            FB2K_BugCheck();

        return *_ItemGUIDs[itemIndex];
    }

    /// <summary>
    /// Retrieves item's description to show in the status bar. Set p_out to the string to be displayed and return true if you provide a description, return false otherwise.
    /// </summary>
    virtual bool get_item_description(unsigned int itemIndex, pfc::string_base & out) final
    {
        if (itemIndex >= _ItemGUIDs.size())
            FB2K_BugCheck();

        out = pfc::format("Search for tracks with the same ", GetFieldName(itemIndex).get_ptr(), ".");

        return true;
    }
#pragma endregion

#pragma region("contextmenu_item_v2 interface")
    /// <summary>
    /// Gets the parent of all our menu items.
    /// </summary>
    virtual GUID get_parent() final
    {
        return _ContextMenuGUID;
    }
#pragma endregion

#pragma region("contextmenu_item_simple interface")
    /// <summary>
    /// Executes the specified command.
    /// </summary>
    virtual void context_command(unsigned int itemIndex, metadb_handle_list_cref handles, const GUID &) final
    {
        if (itemIndex >= _ItemGUIDs.size())
            FB2K_BugCheck();

        if (handles.get_count() != 1)
        {
            FB2K_console_print(_ComponentName, ": Search was cancelled. Too many tracks selected.");

            return;
        }

        bool IsCtrlPressed = ::IsKeyPressed(VK_CONTROL);

        const pfc::string8 FieldName = GetFieldName(itemIndex);
        const pfc::string8 FieldValue = GetFieldValue(FieldName, handles[0]);

        if (FieldValue.is_empty())
        {
            FB2K_console_print(_ComponentName, ": Search was cancelled. ", FieldName, " was not set or empty.");

            return;
        }

        const pfc::string8 QueryText = pfc::format(FieldName, " ", GetVerb(itemIndex, IsCtrlPressed).toUpper(), " ", FieldValue.toLower());

        FB2K_console_print(_ComponentName, ": ", QueryText);

        if (!ShouldOpenMediaLibrarySearch())
        {
            size_t PlaylistIndex = CreateAutoPlaylist(QueryText);

            if ((PlaylistIndex != pfc_infinite) && ShouldActivatePlaylist())
            {
                auto plm = playlist_manager::get();

                plm->set_active_playlist(PlaylistIndex);
            }
        }
        else
            library_search_ui::get()->show(QueryText);
    }
#pragma endregion

private:
    /// <summary>
    /// Gets the value of the specified field.
    /// </summary>
    pfc::string8 GetFieldValue(pfc::string8 fieldName, const metadb_handle_ptr & handle) const noexcept
    {
        const pfc::string8 Pattern = pfc::format("[%", fieldName, "%]");

        titleformat_object_ptr CompiledPattern;

        titleformat_compiler::get()->compile_safe(CompiledPattern, Pattern);

        pfc::string8 Result;

        auto pbc = playback_control::get();

        metadb_handle_ptr TrackNowPlaying;

        if (!pbc->get_now_playing(TrackNowPlaying) || (!TrackNowPlaying.is_empty() && (TrackNowPlaying->get_location() != handle->get_location())))
            handle->format_title(nullptr, Result, CompiledPattern, nullptr);
        else
            pbc->playback_format_title(nullptr, Result, CompiledPattern, nullptr, playback_control::display_level_all);

        return Result;
    }

    /// <summary>
    /// Gets the verb of the predicate.
    /// </summary>
    pfc::string8 GetVerb(uint32_t, bool isCtrlPressed) const noexcept
    {
        return !isCtrlPressed ? "Is" : "Has";
    }

    /// <summary>
    /// Gets the field name.
    /// </summary>
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

    size_t CreateAutoPlaylist(const pfc::string8 & queryText)
    {
        search_filter_v2::ptr Filter;

        try
        {
            Filter = search_filter_manager_v2::get()->create_ex(queryText, fb2k::service_new<completion_notify_dummy>(), search_filter_manager_v2::KFlagSuppressNotify);
        }
        catch (...) { }

        if (!Filter.is_valid())
            return (size_t)pfc_infinite;

        auto plm = playlist_manager::get();

        const size_t PlaylistIndex = plm->create_playlist(queryText, queryText.get_length(), SIZE_MAX);

        if (PlaylistIndex == pfc_infinite)
            return (size_t)pfc_infinite;

        if (_SendToAutoPlaylist)
        {
            // Add the items to an autoplaylist.
            auto aplm = autoplaylist_manager::get();

            aplm->add_client_simple(queryText, "", PlaylistIndex, 0);
        }
        else
        {
            // Add the items to a regular playlist.
            if (core_version_info_v2::get()->test_version(2,0,0,0))
            {
                fb2k::arrayRef Items;

                try
                {
//                  auto li = library_index::get();
                    auto sim = search_index_manager::get();
                    auto li = sim->get_library_index();

                    Items = li->search(Filter, 0, search_index::flag_sort, fb2k::noAbort);
                }
                catch (...) {}

                if (Items.is_valid())
                    plm->playlist_insert_items(PlaylistIndex, 0, Items->as_list_of<metadb_handle>(), pfc::bit_array_false());
            }
            else
            {
                metadb_handle_list Items;

                library_manager::get()->get_all_items(Items);

                pfc::array_t<bool> Mask;

                Mask.set_size(Items.get_count());

                Filter->test_multi(Items, Mask.get_ptr());

                Items.filter_mask(Mask.get_ptr());

                plm->playlist_insert_items(PlaylistIndex, 0, Items, pfc::bit_array_false());
            }
        }

        return PlaylistIndex;
    }

private:
    bool _SendToAutoPlaylist = false;
};

// Embed the command in the root of the context menu but separated from other commands.
static contextmenu_group_popup_factory _ContextMenuGroup(_ContextMenuGUID, contextmenu_groups::root, _ComponentName, 0);

FB2K_SERVICE_FACTORY(ContextMenuHandler);
#pragma endregion
}
