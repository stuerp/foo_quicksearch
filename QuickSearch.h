
/** $VER: QuickSearch.h (2023.07.09) P. Stuer **/

#include "framework.h"

#pragma once

class QuickSearch
{
public:
    bool Execute(unsigned int itemIndex, const metadb_handle_list_cref & handles, const pfc::string8 fieldName, const pfc::string8 verb) const;

private:
    pfc::string8 GetFieldValue(pfc::string8 fieldName, const metadb_handle_ptr & handle) const noexcept;
    size_t CreateAutoPlaylist(const pfc::string8 & queryText) const;

private:
    bool _SendToAutoPlaylist = false;
};
