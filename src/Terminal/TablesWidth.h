#pragma once

#define OFFSET_W 10

#define DOS_HDR_NAME_W 39
#define DOS_HDR_VALUE_W 22

#define FILE_HDR_NAME_W 24
#define FILE_HDR_VALUE_W 10
#define FILE_HDR_DESCRIPTION_W 55

#define OPTIONAL_HDR_NAME_W 29
#define OPTIONAL_HDR_VALUE_W 18
#define OPTIONAL_HDR_DESCRIPTION_W 49

#define SECTION_HDRS_NAME_W 11
#define SECTION_HDRS_R_ADDR_W 10
#define SECTION_HDRS_R_SIZE_W 10
#define SECTION_HDRS_V_ADDR_W 10
#define SECTION_HDRS_V_SIZE_W 10
#define SECTION_HDRS_CHARAC_W 17
#define SECTION_HDRS_REL_PTR_W 11
#define SECTION_HDRS_LNUM_PTR_W 14
#define SECTION_HDRS_NUM_OF_REL_W 11
#define SECTION_HDRS_NUM_OF_LNUM_W 11

#define EXPORT_DIR_NAME_W 23
#define EXPORT_DIR_VALUE_W 10
#define EXPORT_DIR_DESCRIPTION_W 40

#define EXPORTS_ORDINAL_W 9
#define EXPORTS_FUNCTION_RVA_W 14
#define EXPORTS_NAME_RVA_W 10
#define EXPORTS_NAME_W 60
#define EXPORTS_FORWARDER_W 30

#define IMPORT_DIR_NAME_W 57
#define IMPORT_DIR_FUNCTIONSCOUNT_W 12
#define IMPORT_DIR_Bound_W 7
#define IMPORT_DIR_TIMEDATESTAMP_W 15
#define IMPORT_DIR_FORWARDER_W 10
#define IMPORT_DIR_OFT_NAMERVA_FT_W 10

#define BOUND_IMPORT_OFFSET_W 8
#define BOUND_IMPORT_NAME_W 20
#define BOUND_IMPORT_TIMEDATESTAMP_W 15
#define BOUND_IMPORT_OFFSETMODULENAME_W 18
#define BOUND_IMPORT_REFS_W 27

#define DEBUG_DIR_NAME_W 18
#define DEBUG_DIR_VALUE_W 10
#define DEBUG_DIR_DESCRIPTION_W 40

#define RSRC_DIR_NAME_W 22
#define RSRC_DIR_VALUE_W 10
#define RSRC_DIR_DESCRIPTION_W 40

#define RSRC_DIR_ENTRY_TYPE_W 17
#define RSRC_DIR_ENTRY_ENTRIES_W 12
#define RSRC_DIR_ENTRY_NAME_ID_W 15
#define RSRC_DIR_ENTRY_OFFSET_W 10
#define RSRC_DIR_ENTRY_DIR_DATA_W 16
#define RSRC_DIR_ENTRY_ENTRIES_COUNT_W 15
