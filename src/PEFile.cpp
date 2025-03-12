#include "PEFile.h"

namespace PewParser {

	PEFile::PEFile(const RawFile& raw_file, PEType type)
		: raw_file_(raw_file), pe_type_(type)
	{
		NullWrappers();
		InitWrappers();
	}

	PEFile::~PEFile()
	{
		raw_file_.Delete();
		NullWrappers();
	}


	size_t PEFile::GetNtHdrsSize() const
	{
		size_t size = 0;

		if (pe_type_ == PEType::x32PE)
			size = sizeof(IMAGE_NT_HEADERS32);
		else if (pe_type_ == PEType::x64PE)
			size = sizeof(IMAGE_NT_HEADERS64);

		return size;
	}

	offset_t PEFile::RvaToRaw(offset_t rva) const
	{
		offset_t raw = 0;

		DWORD sum_addr = 0;
		IMAGE_SECTION_HEADER* section_hdr = section_hdrs_wrapper_->GetRootSectionHdr();

		for (size_t i = 0; i < GetNumOfSections(); i++)
		{
			sum_addr = section_hdr->VirtualAddress + section_hdr->SizeOfRawData;

			if (rva >= section_hdr->VirtualAddress && (rva <= sum_addr))
				return raw = section_hdr->PointerToRawData + (rva - section_hdr->VirtualAddress);
			section_hdr++;
		}

		return raw;
	}

	IMAGE_DATA_DIRECTORY* PEFile::GetDataDirectory() const
	{
		return optional_hdr_wrapper_->GetDataDir();
	}

	BYTE* PEFile::GetContentAt(offset_t offset, OffsetType type) const
	{
		if (type == OffsetType::RVA)
		{
			offset_t raw_offset = RvaToRaw(offset);
			if (raw_offset)
				return (raw_file_.Buffer() + raw_offset);
		}
		else if (offset <= GetRawFileSize())
			return (raw_file_.Buffer() + offset);

		return nullptr;
	}

	void PEFile::NullWrappers()
	{
		dos_hdr_wrapper_ = nullptr;
		file_hdr_wrapper_= nullptr;
		optional_hdr_wrapper_= nullptr;
		section_hdrs_wrapper_= nullptr;

		data_dir_wrappers_.fill(nullptr);
	}

	void PEFile::InitWrappers()
	{
		dos_hdr_wrapper_ = new DosHdrWrapper(this);
		file_hdr_wrapper_ = new FileHdrWrapper(this);
		optional_hdr_wrapper_ = new OptionalHdrWrapper(this);
		section_hdrs_wrapper_ = new SectionHdrsWrapper(this);

		InitDataDirWrappers();
	}

	void PEFile::InitDataDirWrappers()
	{
		if (GetDataDirectory()[DataDirEntries::EXP].VirtualAddress > 0)
			data_dir_wrappers_[DataDirEntries::EXP] = new ExportDirWrapper(this);

		if (GetDataDirectory()[DataDirEntries::IMP].VirtualAddress > 0)
			data_dir_wrappers_[DataDirEntries::IMP] = new ImportDirWrapper(this);

		if (GetDataDirectory()[DataDirEntries::RSRC].VirtualAddress > 0)
			data_dir_wrappers_[DataDirEntries::RSRC] = new ResourceDirWrapper(this);

		if (GetDataDirectory()[DataDirEntries::BOUNDIMP].VirtualAddress > 0)
			data_dir_wrappers_[DataDirEntries::BOUNDIMP] = new BoundImportDirWrapper(this);

		if (GetDataDirectory()[DataDirEntries::DBG].VirtualAddress > 0)
			data_dir_wrappers_[DataDirEntries::DBG] = new DebugDirWrapper(this);
	}
}
