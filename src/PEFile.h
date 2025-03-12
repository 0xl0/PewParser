#pragma once
#include "PEFormat.h"
#include "PewTypes.h"

#include "Headers/Headers.h"
#include "DataDirectory/DataDirectory.h"

#include "RawFile.h"

#include <array>

namespace PewParser {

	class PEFile
	{
	public:
		PEFile(const RawFile& raw_file, PEType type);
		~PEFile();

		DosHdrWrapper* GetDosHdrWrapper() const { return dos_hdr_wrapper_; }
		FileHdrWrapper* GetFileHdrWrapper() const { return file_hdr_wrapper_; }
		OptionalHdrWrapper* GetOptionalHdrWrapper() const { return optional_hdr_wrapper_; }
		SectionHdrsWrapper* GetSectionHdrsWrapper() { return section_hdrs_wrapper_; }
		const void* GetDataDirEntryWrapper(uint32_t entry) const { return data_dir_wrappers_[entry]; }

		offset_t GetNtHdrsOffset() const { return dos_hdr_wrapper_->GetNtHdrsOffset(); }
		offset_t GetFileHdrOffset() const { return GetNtHdrsOffset() + sizeof(DWORD); }
		offset_t GetOptionalHdrOffset() const { return GetFileHdrOffset() + GetFileHdrSize(); }
		offset_t GetSectionHdrsOffset() const { return GetOptionalHdrOffset() + GetOptionalHdrSize(); }

		size_t GetNtHdrsSize() const;
		size_t GetFileHdrSize() const { return file_hdr_wrapper_->GetFileHdrSize(); }
		size_t GetOptionalHdrSize() const { return file_hdr_wrapper_->GetOptionalHdrSize(); }
		size_t GetSectionHdrsSize() const { return section_hdrs_wrapper_->GetAllSectionsSize(); }

		size_t GetNumOfSections() const { return file_hdr_wrapper_->GetNumOfSctions(); }
		size_t GetNumOfDataDirEntries() const { return data_dir_wrappers_.size(); }

		offset_t RvaToRaw(offset_t rva) const;

		BYTE* GetContentAt(offset_t offset, OffsetType type) const;

		IMAGE_DATA_DIRECTORY* GetDataDirectory() const;

		PEType GetPEType() const { return pe_type_; }

		RawFile& GetRawFile() { return raw_file_; }
		const RawFile& GetRawFile() const { return raw_file_; }
		uintmax_t GetRawFileSize() const { return raw_file_.Size(); }
	private:
		void NullWrappers();
		void InitWrappers();
		void InitDataDirWrappers();
	private:
		RawFile raw_file_;
		PEType pe_type_;

		DosHdrWrapper* dos_hdr_wrapper_;
		FileHdrWrapper* file_hdr_wrapper_;
		OptionalHdrWrapper* optional_hdr_wrapper_;
		SectionHdrsWrapper* section_hdrs_wrapper_;

		std::array<void*, IMAGE_NUMBEROF_DIRECTORY_ENTRIES> data_dir_wrappers_;
	};

}
