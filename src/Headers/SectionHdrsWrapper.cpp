#include "SectionHdrsWrapper.h"

#include <PEFile.h>

#include <cstring>

namespace PewParser {

	SectionHdrsWrapper::SectionHdrsWrapper(PEFile* pe)
		: related_pe_(pe), root_section_hdr_offset_(pe->GetSectionHdrsOffset())
	{
		root_section_hdr_ = (IMAGE_SECTION_HEADER*)pe->GetContentAt(root_section_hdr_offset_, OffsetType::RAW);
		current_section_hdr_ = root_section_hdr_;
		current_section_hdr_offset_ = root_section_hdr_offset_;
		field_offset_ = root_section_hdr_offset_;
		field_index_ = Fields::NAME;
		field_type_ = FieldType::BYTE;
	}

	std::string SectionHdrsWrapper::GetFieldName() const
	{
		switch (field_index_)
		{
			case Fields::NAME:				return "Name";
			case Fields::VIRTUAL_SIZE:		return "Virtual Size";
			case Fields::VIRTUAL_PTR:		return "Virtual Address";
			case Fields::RAW_SIZE:			return "Size Of Raw Data";
			case Fields::RAW_PTR:			return "Pointer To Raw Data";
			case Fields::RELOC_PTR:			return "Pointer To Relocations";
			case Fields::LINE_NUM_PTR:		return "Pointer To Line numbers";
			case Fields::RELOC_NUM:			return "Number Of Relocations";
			case Fields::LINE_NUM_NUMS:		return "Number Of Line numbers";
			case Fields::CHARACTERISTICS:	return "Characteristics";
			default:						return "UnKnown";
		}
	}

	BYTE* SectionHdrsWrapper::GetFieldValue() const
	{
		return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
	}

	void SectionHdrsWrapper::SetCurrentSectionHdr(index_t section_index)
	{
		current_section_hdr_ = root_section_hdr_ + section_index;
		current_section_hdr_offset_ = root_section_hdr_offset_ + (GetSectionHdrSize() * section_index);
		field_offset_ = current_section_hdr_offset_;
		field_index_ = Fields::NAME;
		field_type_ = FieldType::BYTE;
	}

	std::string SectionHdrsWrapper::GetSectionName(index_t section_index) const
	{
		IMAGE_SECTION_HEADER* target_section_hdr = root_section_hdr_ + section_index;

		char section_name[IMAGE_SIZEOF_SHORT_NAME + 1] = { 0 };

		memcpy(section_name, target_section_hdr->Name, IMAGE_SIZEOF_SHORT_NAME);

		return std::string(section_name);
	}

	size_t SectionHdrsWrapper::GetNumOfSections() const
	{
		return related_pe_->GetNumOfSections();
	}

	std::map<DWORD, std::string> SectionHdrsWrapper::GetCharacteristics(index_t section_index) const
	{
		IMAGE_SECTION_HEADER* target_section_hdr = root_section_hdr_ + section_index;

		DWORD characteristics = target_section_hdr->Characteristics;

		std::map<DWORD, std::string> result;

		if (characteristics & IMAGE_SCN_CNT_CODE)
			result[IMAGE_SCN_CNT_CODE] = "Contains executable code";

		if (characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA)
			result[IMAGE_SCN_CNT_INITIALIZED_DATA] = "Contains initialized data";

		if (characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
			result[IMAGE_SCN_CNT_UNINITIALIZED_DATA] = "Contains uninitialized data";

		if (characteristics & IMAGE_SCN_LNK_NRELOC_OVFL)
			result[IMAGE_SCN_LNK_NRELOC_OVFL] = "Contains extended relocations";

		if (characteristics & IMAGE_SCN_MEM_DISCARDABLE)
			result[IMAGE_SCN_MEM_DISCARDABLE] = "Can be discarded as needed";

		if (characteristics & IMAGE_SCN_MEM_NOT_CACHED)
			result[IMAGE_SCN_MEM_NOT_CACHED] = "Cannot be cached";

		if (characteristics & IMAGE_SCN_MEM_NOT_PAGED)
			result[IMAGE_SCN_MEM_NOT_PAGED] = "Not pageable";

		if (characteristics & IMAGE_SCN_MEM_SHARED)
			result[IMAGE_SCN_MEM_SHARED] = "Can be shared in memory";

		if (characteristics & IMAGE_SCN_MEM_EXECUTE)
			result[IMAGE_SCN_MEM_EXECUTE] = "Can be executed as code";

		if (characteristics & IMAGE_SCN_MEM_READ)
			result[IMAGE_SCN_MEM_READ] = "Can be read";

		if (characteristics & IMAGE_SCN_MEM_WRITE)
			result[IMAGE_SCN_MEM_WRITE] = "Can be written to";

		return result;
	}

	size_t SectionHdrsWrapper::GetAllSectionsSize() const
	{
		return related_pe_->GetNumOfSections() * GetSectionHdrSize();
	}

	void SectionHdrsWrapper::LoadNextField()
	{
		if (field_type_ == FieldType::BYTE)
			field_offset_ += IMAGE_SIZEOF_SHORT_NAME;
		else if (field_type_ == FieldType::WORD)
			field_offset_ += sizeof(WORD);
		else if (field_type_ == FieldType::DWORD)
			field_offset_ += sizeof(DWORD);

		field_index_++;
		if (field_index_ == Fields::RELOC_NUM || field_index_ == Fields::LINE_NUM_NUMS)
			field_type_ = FieldType::WORD;
		else
			field_type_ = FieldType::DWORD;
	}

	void SectionHdrsWrapper::Reset()
	{
		current_section_hdr_ = root_section_hdr_;
		current_section_hdr_offset_ = root_section_hdr_offset_;
		field_offset_ = root_section_hdr_offset_;
		field_index_ = Fields::NAME;
		field_type_ = FieldType::BYTE;
	}
}