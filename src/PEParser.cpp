#include "PEParser.h"

#include <cstring>

namespace PewParser {

	PEType PEParser::ValidatePE(const RawFile& raw_file)
	{
		BYTE* buffer = raw_file.Buffer();
		uintmax_t size = raw_file.Size();

		size_t hdrs_size32 = sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS32) + sizeof(IMAGE_SECTION_HEADER);
		size_t hdrs_size64 = sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS64) + sizeof(IMAGE_SECTION_HEADER);

		if (!buffer)
			return PEType::NotPE;

		if (size < (sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS32::Signature)))
			return PEType::NotPE;

		IMAGE_DOS_HEADER dos_hdr;
		std::memcpy(&dos_hdr, buffer, sizeof(IMAGE_DOS_HEADER));

		if (dos_hdr.e_magic == IMAGE_DOS_SIGNATURE)
		{
			DWORD signature;
			std::memcpy(&signature, buffer + dos_hdr.e_lfanew, sizeof(DWORD));
			if (signature == IMAGE_NT_SIGNATURE)
			{
				DWORD file_hdr_offset = dos_hdr.e_lfanew + sizeof(DWORD);
				DWORD optional_hdr_offset = file_hdr_offset + sizeof(IMAGE_FILE_HEADER);

				WORD magic;
				std::memcpy(&magic, buffer + optional_hdr_offset, sizeof(WORD));
				if (magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC && size > hdrs_size32)
						return PEType::x32PE;
				else if (magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC && size > hdrs_size64)
					return PEType::x64PE;
				else
					return PEType::Corrupted;
			}
			return PEType::NotPE;
		}
		else
			return PEType::NotPE;
	}

	PEFile* PEParser::MakePE(const RawFile& raw_file, PEType type)
	{
		if(type == PEType::NotPE || type == PEType::Corrupted)
			return nullptr;

		if(!raw_file)
			return nullptr;

		return new PEFile(raw_file, type);
	}
}
