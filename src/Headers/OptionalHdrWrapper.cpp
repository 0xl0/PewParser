#include "OptionalHdrWrapper.h"

#include <PEFile.h>
#include <PEUtils.h>

namespace PewParser {

	OptionalHdrWrapper::OptionalHdrWrapper(PEFile* pe)
		: related_pe_(pe), optional_hdr32_(nullptr), optional_hdr64_(nullptr), optional_hdr_offset_(pe->GetOptionalHdrOffset())
	{
		if(pe->GetPEType() == PEType::x32PE)
			optional_hdr32_ = (IMAGE_OPTIONAL_HEADER32*)pe->GetContentAt(optional_hdr_offset_, OffsetType::RAW);
		else if (pe->GetPEType() == PEType::x64PE)
			optional_hdr64_ = (IMAGE_OPTIONAL_HEADER64*)pe->GetContentAt(optional_hdr_offset_, OffsetType::RAW);

		field_offset_ = optional_hdr_offset_;
		field_index_ = Fields::MAGIC;
		field_type_ = FieldType::WORD;
		data_dir_entry_ = DataDirEntries::EXP;
	}

	std::string OptionalHdrWrapper::GetFieldName() const
	{
		switch (field_index_)
		{
			case Fields::MAGIC:					return "Magic";
			case Fields::LINKER_MAJOR:			return "Linker Ver. (Major)";
			case Fields::LINKER_MINOR:			return "Linker Ver. (Minor)";
			case Fields::CODE_SIZE:				return "Size of code";
			case Fields::INITDATA_SIZE:			return "Size of Initialized Data";
			case Fields::UNINITDATA_SIZE:		return "Size of Uninitialized Data";
			case Fields::EP:					return "Entry Point";
			case Fields::CODE_BASE:				return "Base of Code";
			case Fields::DATA_BASE:				return "Base of Data";
			case Fields::IMAGE_BASE:			return "Image Base";
			case Fields::SEC_ALIGN:				return "Section Alignment";
			case Fields::FILE_ALIGN:			return "File Alignment";
			case Fields::OSVER_MAJOR:			return "OS Ver. (Major)";
			case Fields::OSVER_MINOR:			return "OS Ver. (Mainor)";
			case Fields::IMGVER_MAJOR:			return "Image Ver. (Major)";
			case Fields::IMGVER_MINOR:			return "Image Ver. (Mainor)";
			case Fields::SUBSYSVER_MAJOR:		return "Subsystem Ver. (Mainor)";
			case Fields::SUBSYSVER_MINOR:		return "Subsystem Ver. (Mainor)";
			case Fields::WIN32_VER:				return "Win32 Version Value";
			case Fields::IMAGE_SIZE:			return "Size of Image";
			case Fields::HDRS_SIZE:				return "Size of Headers";
			case Fields::CHECKSUM:				return "Checksum";
			case Fields::SUBSYS:				return "Subsystem";
			case Fields::DLL_CHARACT:			return "DLL Characteristics";
			case Fields::STACK_RSRV_SIZE:		return "Size of Stack Reserve";
			case Fields::STACK_COMMIT_SIZE:		return "Size of Stack Commit";
			case Fields::HEAP_RSRV_SIZE:		return "Size of Heap Reserve";
			case Fields::HEAP_COMMIT_SIZE:		return "Size of Heap Commit";
			case Fields::LDR_FLAGS:				return "Loader Flags";
			case Fields::RVAS_SIZES_NUM:		return "Number of RVAs and Sizes";
			case Fields::DATA_DIR:				return "Data Directory";
			default:							return "Unknown";
		}
	}

	BYTE* OptionalHdrWrapper::GetFieldValue() const
	{
		return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
	}

	std::string OptionalHdrWrapper::GetFieldDescription() const
	{
		switch (field_index_)
		{
			case Fields::MAGIC:				return MagicDescription();
			case Fields::SUBSYS:			return SubsystemDescription();
			default:						return std::string();
		}
	}

	bool OptionalHdrWrapper::IsFieldDescribed() const
	{
		if (field_index_ == Fields::MAGIC || field_index_ == Fields::SUBSYS)
			return true;

		return false;
	}

	std::map<WORD, std::string> OptionalHdrWrapper::GetDllCharacteristics() const
	{
		WORD dll_characteristics = 0;

		if (optional_hdr32_)
			dll_characteristics = optional_hdr32_->DllCharacteristics;
		else
			dll_characteristics = optional_hdr64_->DllCharacteristics;

		std::map<WORD, std::string> result;

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE)
			result[IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE] = "DLL can be relocated at load time";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY)
			result[IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY] = "Code Integrity checks are enforced";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_NX_COMPAT)
			result[IMAGE_DLLCHARACTERISTICS_NX_COMPAT] = "Image is NX compatible";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_NO_ISOLATION)
			result[IMAGE_DLLCHARACTERISTICS_NO_ISOLATION] = "Isolation aware, but do not isolate the image";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_NO_SEH)
			result[IMAGE_DLLCHARACTERISTICS_NO_SEH] = "No structured exception (SEH) handling";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_NO_BIND)
			result[IMAGE_DLLCHARACTERISTICS_NO_BIND] = "Do not bind the image";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_APPCONTAINER)
			result[IMAGE_DLLCHARACTERISTICS_APPCONTAINER] = "Image must execute in an AppContainer";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_WDM_DRIVER)
			result[IMAGE_DLLCHARACTERISTICS_WDM_DRIVER] = "A WDM driver";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_GUARD_CF)
			result[IMAGE_DLLCHARACTERISTICS_GUARD_CF] = "Image supports Control Flow Guard";

		if (dll_characteristics & IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE)
			result[IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE] = "Terminal Server aware";

		return result;
	}

	std::string OptionalHdrWrapper::GetDataDirEntryName() const
	{
		switch (data_dir_entry_)
		{
			case DataDirEntries::EXP:			return "Export Directory";
			case DataDirEntries::IMP:			return "Import Directory";
			case DataDirEntries::RSRC:			return "Resource Directory";
			case DataDirEntries::EXPTN:			return "Exeption Directory";
			case DataDirEntries::SECU:			return "Security Directory";
			case DataDirEntries::BRELOC:		return "Base Relocation Table";
			case DataDirEntries::DBG:			return "Debug Directory";
			case DataDirEntries::ARC:			return "Architecture Specific Data";
			case DataDirEntries::GBLPTR:		return "RVA of GlobalPtr";
			case DataDirEntries::TLS:			return "TLS Directory";
			case DataDirEntries::LDCFG:			return "Load Configuration Directory";
			case DataDirEntries::BOUNDIMP:		return "Bound Import Directory";
			case DataDirEntries::IAT:			return "Import Table Address";
			case DataDirEntries::DLYIMP:		return "Delay Load Import Descriptor";
			case DataDirEntries::COMDESC:		return ".NET Header";
			default:							return "Unknown";
		}
	}

	std::string OptionalHdrWrapper::MagicDescription() const
	{
		WORD magic = 0;

		if (optional_hdr32_)
			magic = optional_hdr32_->Magic;
		else
			magic = optional_hdr64_->Magic;

		switch (magic)
		{
			case IMAGE_NT_OPTIONAL_HDR32_MAGIC:		return "PE32";
			case IMAGE_NT_OPTIONAL_HDR64_MAGIC:		return "PE32+(x64)";
			case IMAGE_ROM_OPTIONAL_HDR_MAGIC:		return "ROM";
			default:								return "Unknown";
		}
	}

	std::string OptionalHdrWrapper::SubsystemDescription() const
	{
		WORD subsystem = 0;

		if (optional_hdr32_)
			subsystem = optional_hdr32_->Subsystem;
		else
			subsystem = optional_hdr64_->Subsystem;

		switch (subsystem)
		{
			case IMAGE_SUBSYSTEM_UNKNOWN:					return "Unknown sybsystem";
			case IMAGE_SUBSYSTEM_NATIVE:					return "Driver";
			case IMAGE_SUBSYSTEM_WINDOWS_GUI:				return "Windows Graphical (GUI)";
			case IMAGE_SUBSYSTEM_WINDOWS_CUI:				return "Windows Character (Console)";
			case IMAGE_SUBSYSTEM_OS2_CUI:					return "OS/2 (Console)";
			case IMAGE_SUBSYSTEM_POSIX_CUI:					return "POSIX (Console)";
			case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:			return "Native Win9x driver";
			case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:			return "Windows CE";
			case IMAGE_SUBSYSTEM_EFI_APPLICATION:			return "Extensible Firmware Interface (EFI)";
			case IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER:	return "EFI driver with boot services";
			case IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER:		return "EFI driver with run-time services";
			case IMAGE_SUBSYSTEM_EFI_ROM:					return "EFI ROM image";
			case IMAGE_SUBSYSTEM_XBOX:						return "Xbox system";
			case IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION:	return "Boot application";
			default:										return "Unknown";
		}
	}

	void* OptionalHdrWrapper::GetOptionalHdr() const
	{
		if (optional_hdr32_)
			return optional_hdr32_;
		else
			return optional_hdr64_;
	}

	size_t OptionalHdrWrapper::GetOptionalHdrSize() const
	{
		if (optional_hdr32_)
			return sizeof(IMAGE_OPTIONAL_HEADER32);
		else
			return sizeof(IMAGE_OPTIONAL_HEADER64);
	}

	OptHdrType OptionalHdrWrapper::GetOptionalHdrType() const
	{
		if (optional_hdr32_)
			return OptHdrType::x32;
		else
			return OptHdrType::x64;
	}

	IMAGE_DATA_DIRECTORY* OptionalHdrWrapper::GetDataDir() const
	{
		IMAGE_DATA_DIRECTORY* dataDir = nullptr;

		if (optional_hdr32_)
			dataDir = optional_hdr32_->DataDirectory;
		else
			dataDir = optional_hdr64_->DataDirectory;

		return dataDir;
	}

	void OptionalHdrWrapper::LoadNextField()
	{
		if (field_index_ == OptionalHdrWrapper::Fields::DATA_BASE && GetOptionalHdrType() == OptHdrType::x64)
		{
			field_index_++;
			field_type_ = FieldType::ULONGLONG;
			return;
		}

		if (field_index_ == OptionalHdrWrapper::Fields::DATA_DIR)
		{
			data_dir_entry_++;
			field_offset_ += sizeof(IMAGE_DATA_DIRECTORY);
			field_type_ = FieldType::ULONGLONG;
			return;
		}

		if (field_type_ == FieldType::BYTE)
			field_offset_ += sizeof(BYTE);
		else if (field_type_ == FieldType::WORD)
			field_offset_ += sizeof(WORD);
		else if (field_type_ == FieldType::DWORD)
			field_offset_ += sizeof(DWORD);
		else if (field_type_ == FieldType::ULONGLONG)
			field_offset_ += sizeof(ULONGLONG);

		field_index_++;
		if (field_index_ == Fields::LINKER_MAJOR || field_index_ == Fields::LINKER_MINOR)
			field_type_ = FieldType::BYTE;
		else if (field_index_ == Fields::MAGIC || field_index_ == Fields::OSVER_MAJOR || field_index_ == Fields::OSVER_MINOR || field_index_ == Fields::IMGVER_MAJOR || field_index_ == Fields::IMGVER_MINOR || field_index_ == Fields::SUBSYSVER_MAJOR || field_index_ == Fields::SUBSYSVER_MINOR || field_index_ == Fields::SUBSYS || field_index_ == Fields::DLL_CHARACT)
			field_type_ = FieldType::WORD;
		else if ((field_index_ == Fields::IMAGE_BASE || field_index_ == Fields::STACK_RSRV_SIZE || field_index_ == Fields::STACK_COMMIT_SIZE || field_index_ == Fields::HEAP_RSRV_SIZE || field_index_ == Fields::HEAP_COMMIT_SIZE) && GetOptionalHdrType() == OptHdrType::x64)
			field_type_ = FieldType::ULONGLONG;
		else
			field_type_ = FieldType::DWORD;
	}

	void OptionalHdrWrapper::Reset()
	{
		field_offset_ = optional_hdr_offset_;
		field_index_ = Fields::MAGIC;
		field_type_ = FieldType::WORD;
		data_dir_entry_ = DataDirEntries::EXP;
	}
}
