
/** $VER: QuickSearch.cpp (2023.07.09) P. Stuer **/

#include "QuickSearch.h"

#pragma hdrstop

/// <summary>
/// Executes a search.
/// </summary>
bool QuickSearch::Execute(unsigned int itemIndex, const metadb_handle_list_cref & handles, const pfc::string8 fieldName, const pfc::string8 verb) const
{
    pfc::string8 FieldValue = GetFieldValue(fieldName, handles[0]);

    FieldValue.replace_char(',', ' ');

    if (FieldValue.is_empty())
    {
        FB2K_console_print(STR_COMPONENT_NAME, ": Search was cancelled. ", fieldName, " was not set or empty.");

        return false;
    }

    const pfc::string8 QueryText = pfc::format("%", fieldName, "% ", verb.toUpper(), " ", FieldValue.toLower());

    FB2K_console_print(STR_COMPONENT_NAME, ": ", QueryText);

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

    return true;
}

/// <summary>
/// Gets the value of the specified field.
/// </summary>
pfc::string8 QuickSearch::GetFieldValue(pfc::string8 fieldName, const metadb_handle_ptr & handle) const noexcept
{
    titleformat_object_ptr CompiledPattern;

    {
        const pfc::string8 Pattern = pfc::format("[%", fieldName, "%]");

        titleformat_compiler::get()->compile_safe(CompiledPattern, Pattern);
    }

    {
        pfc::string8 Result;

        auto pbc = playback_control::get();

        metadb_handle_ptr TrackNowPlaying;

        if (!pbc->get_now_playing(TrackNowPlaying) || (!TrackNowPlaying.is_empty() && (TrackNowPlaying->get_location() != handle->get_location())))
            handle->format_title(nullptr, Result, CompiledPattern, nullptr);
        else
            pbc->playback_format_title(nullptr, Result, CompiledPattern, nullptr, playback_control::display_level_all);

        return Result;
    }
}

size_t QuickSearch::CreateAutoPlaylist(const pfc::string8 & queryText) const
{
    search_filter_v2::ptr Filter;

    {
        try
        {
            Filter = search_filter_manager_v2::get()->create_ex(queryText, fb2k::service_new<completion_notify_dummy>(), search_filter_manager_v2::KFlagSuppressNotify);
        }
        catch (...) { }

        if (!Filter.is_valid())
            return (size_t)pfc_infinite;
    }

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
        if (core_version_info_v2::get()->test_version(2, 0, 0, 0))
        {
            fb2k::arrayRef Items;

            try
            {
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
