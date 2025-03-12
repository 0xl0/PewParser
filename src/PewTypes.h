#pragma once

#include <cstdint>
#include <cstddef>

namespace PewParser {

	typedef uint64_t offset_t;
	typedef uint64_t FieldOffset;
	typedef uint32_t FieldIndex;
	typedef size_t index_t;

	enum class OffsetType
	{
		RAW = 0,
		RVA
	};

	enum class FieldType
	{
		BYTE = 0,
		WORD,
		DWORD,
		ULONGLONG
	};

	enum class PEType
	{
		NotPE = 0,
		Corrupted,
		x32PE,
		x64PE,
	};

	enum class OptHdrType
	{
		x32 = 0,
		x64
	};

	enum DataDirEntries
	{
		EXP = 0,
		IMP,
		RSRC,
		EXPTN,
		SECU,
		BRELOC,
		DBG,
		ARC,
		GBLPTR,
		TLS,
		LDCFG,
		BOUNDIMP,
		IAT,
		DLYIMP,
		COMDESC,
		DATA_DIR_ENTRIES_COUNT
	};

	enum class ThunkType
	{
		THUNK32 = 0,
		THUNK64
	};

	enum class Binding
	{
		NEWBIND = 0,
		OLDBIND,
		NOTBOUND
	};
}
