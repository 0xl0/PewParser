#pragma once
#include "Log.h"
#include "TablesWidth.h"

#include <string>

namespace PewParser {

    struct TableRow
    {
        uint32_t width;
        std::string_view name;
    };

    template<size_t N>
    static void DisplayTable(const std::array<TableRow, N>& table)
    {
        std::cout << " " << Logger::CustomBgColor(Logger::CustomPEColors::TABLE_HEADER);
        for (size_t i = 0; i < N; i++)
            std::cout << std::setw(table[i].width) << table[i].name;
        std::cout << Logger::ResetColor() << std::endl;
    }

    constexpr std::array<TableRow, 3> kDosHdrTable =
    {
        {{OFFSET_W, "Offset"},
        {DOS_HDR_NAME_W, "Name"},
        {DOS_HDR_VALUE_W, "Value"}}
    };

    constexpr std::array<TableRow, 4> kFileHdrTable =
    {
        {{OFFSET_W, "Offset"},
        {FILE_HDR_NAME_W, "Name"},
        {FILE_HDR_VALUE_W, "Value"},
        {FILE_HDR_DESCRIPTION_W, "Description"}}
    };

    constexpr std::array<TableRow, 4> kOptionalHdrTable =
    {
        {{OFFSET_W, "Offset"},
        {OPTIONAL_HDR_NAME_W, "Name"},
        {OPTIONAL_HDR_VALUE_W, "Value"},
        {OPTIONAL_HDR_DESCRIPTION_W, "Description"}}
    };

    constexpr std::array<TableRow, 4> kDataDirTable =
    {
        {{OFFSET_W, ""},
        {OPTIONAL_HDR_NAME_W, "Data Directory"},
        {OPTIONAL_HDR_VALUE_W, "Address"},
        {OPTIONAL_HDR_DESCRIPTION_W, "Size"}}
    };

    constexpr std::array<TableRow, 10> kSectionHdrsTable =
    {
        {{SECTION_HDRS_NAME_W, "Name"},
        {SECTION_HDRS_R_ADDR_W, "RAW"},
        {SECTION_HDRS_R_SIZE_W, "RAW Size"},
        {SECTION_HDRS_V_ADDR_W, "RVA"},
        {SECTION_HDRS_V_SIZE_W, "RVA Size"},
        {SECTION_HDRS_CHARAC_W, "Characteristics"},
        {SECTION_HDRS_REL_PTR_W, "Reloc Ptr"},
        {SECTION_HDRS_LNUM_PTR_W, "Line Num Ptr"},
        {SECTION_HDRS_NUM_OF_REL_W, "Reloc Num"},
        {SECTION_HDRS_NUM_OF_LNUM_W, "Line Nums"}}
    };

    constexpr std::array<TableRow, 4> kExportDirTable =
    {
        {{OFFSET_W, "Offset"},
        {EXPORT_DIR_NAME_W, "Name"},
        {EXPORT_DIR_VALUE_W, "Value"},
        {EXPORT_DIR_DESCRIPTION_W, "Description"}}
    };

    constexpr std::array<TableRow, 3> kExportsTable =
    {
        {{OFFSET_W, "Offset"},
        {EXPORTS_ORDINAL_W, "Ordinal"},
        {EXPORTS_NAME_W, "Name"}}
    };

    constexpr std::array<TableRow, 3> kImportsTable =
    {
        {{OFFSET_W, "Offset"},
        {IMPORT_DIR_FUNCTIONSCOUNT_W, "Func Count"},
        {IMPORT_DIR_NAME_W, "Name"}},
    };
/*
constexpr std::array<TableRow, 6> kExportsTableFull =
{
    {{OFFSET_W, "Offset"},
    {EXPORTS_ORDINAL_W, "Ordinal"},
    {EXPORTS_FUNCTION_RVA_W, "Function RVA"},
    {EXPORTS_NAME_RVA_W, "Name RVA"},
    {EXPORTS_NAME_W, "Name"},
    {EXPORTS_FORWARDER_W, "Forwarder"}}
};

/*
constexpr std::array<TableRow, 9> kImportsTableFull =
{
    {{OFFSET_W, "Offset"},
    {IMPORT_DIR_NAME_W, "Name"},
    {IMPORT_DIR_FUNCTIONSCOUNT_W, "Func Count"},
    {IMPORT_DIR_Bound_W, "Bound?"},
    {IMPORT_DIR_OFT_NAMERVA_FT_W, "OFT"},
    {IMPORT_DIR_TIMEDATESTAMP_W, "TimeDateStamp"},
    {IMPORT_DIR_FORWARDER_W, "Forwarder"},
    {IMPORT_DIR_OFT_NAMERVA_FT_W, "NameRVA"},
    {IMPORT_DIR_OFT_NAMERVA_FT_W, "FT"}}
};
*/

    constexpr std::array<TableRow, 5> kBoundImportsTable =
    {
        {{OFFSET_W, "Offset"},
        {BOUND_IMPORT_NAME_W, "Name"},
        {BOUND_IMPORT_TIMEDATESTAMP_W, "TimeDateStamp"},
        {BOUND_IMPORT_OFFSETMODULENAME_W, "OffsetModuleName"},
        {BOUND_IMPORT_REFS_W, "NumberOfModuleForwarderRefs"}}
    };

    constexpr std::array<TableRow, 4> kRsrcDirTable =
    {
        {{OFFSET_W, "Offset"},
        {RSRC_DIR_NAME_W, "Name"},
        {RSRC_DIR_VALUE_W, "Value"},
        {RSRC_DIR_DESCRIPTION_W, "Description"}}
    };

    constexpr std::array<TableRow, 7> kRsrcDirEntriesTable =
    {
        {{RSRC_DIR_ENTRY_TYPE_W, "Type"},
        {RSRC_DIR_ENTRY_ENTRIES_W, "Entries"},
        {RSRC_DIR_ENTRY_NAME_ID_W, "Name/Id Value"},
        {RSRC_DIR_ENTRY_OFFSET_W, "Offset"},
        {RSRC_DIR_ENTRY_DIR_DATA_W, "Dir/Data Value"},
        {RSRC_DIR_ENTRY_OFFSET_W, "Offset"},
        {RSRC_DIR_ENTRY_ENTRIES_COUNT_W, "Entries Count"}}
    };

    constexpr std::array<TableRow, 4> kDebugDirTable =
    {
        {{OFFSET_W, "Offset"},
        {DEBUG_DIR_NAME_W, "Name"},
        {DEBUG_DIR_VALUE_W, "Value"},
        {DEBUG_DIR_DESCRIPTION_W, "Description"}}
    };
}

