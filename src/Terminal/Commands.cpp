#include "Commands.h"
#include "Log.h"

#include "Tables.h"

#include <cstring>

namespace PewParser {

	static std::string StrLower(const std::string& cmd)
	{
		std::string lower;

		for (size_t i = 0; i < cmd.size(); i++)
		{
			if (cmd[i] >= 'A' && cmd[i] <= 'Z')
				lower += cmd[i] + 0x20;
			else lower += cmd[i];
		}

		return lower;
	}

	static std::string GetTrancatedStr(size_t max_size, const std::string& str)
	{
		if(str.length() > max_size)
			return std::string(str.substr(0, max_size));

		return str;
	}

	Commands::Commands(PEFile* pe)
		: listening_(false), loaded_pe_(pe)
	{
	}

	Commands::Command Commands::ParseCommands(const std::string& cmd)
	{
		std::string lower = StrLower(cmd);

		if (lower == "doshdr")				return Command::DOS_HDR;
		else if (lower == "filehdr")		return Command::FILE_HDR;
		else if (lower == "opthdr")			return Command::OPT_HDR;
		else if (lower == "sechdrs")		return Command::SEC_HDRS;
		else if (lower == "exportdir")		return Command::EXPORT_DIR;
		else if (lower == "exports")		return Command::EXPORTS;
		else if (lower == "imports")		return Command::IMPORTS;
		else if (lower == "rsrc")			return Command::RSRC_DIR;
		else if (lower == "boundimports")	return Command::BOUND_IMPORTS;
		else if (lower == "debug")			return Command::DEBUG_DIR;
		else								return Command::INVALID;
	}

	void Commands::PrintDosHdr()
	{
		DosHdrWrapper* dos_hdr_wrapper = loaded_pe_->GetDosHdrWrapper();
		IMAGE_DOS_HEADER* dos_hdr = dos_hdr_wrapper->GetDosHdr();

		std::stringstream Res;
		std::stringstream Res2;
		Res << dos_hdr->e_res[0] << "," << dos_hdr->e_res[1] << "," << dos_hdr->e_res[2] << "," << dos_hdr->e_res[3];
		Res2 << dos_hdr->e_res2[0] << "," << dos_hdr->e_res2[1] << "," << dos_hdr->e_res2[2] << "," << dos_hdr->e_res2[3] << "," << dos_hdr->e_res2[4] << "," << dos_hdr->e_res2[5] << "," << dos_hdr->e_res2[6] << "," << dos_hdr->e_res2[7] << "," << dos_hdr->e_res2[8] << "," << dos_hdr->e_res2[9];

		std::cout << std::left << std::uppercase << std::hex << "\n";
		DisplayTable<kDosHdrTable.size()>(kDosHdrTable);

		std::cout << std::uppercase << std::hex;
		for (size_t field = 0; field < dos_hdr_wrapper->GetFieldsCount(); field++)
		{
			std::cout << " " << Logger::CustomBgColor((field % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
			std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << dos_hdr_wrapper->GetFieldOffset();
			std::cout << Logger::TextColor(Logger::Color::BLACK) << std::setw(DOS_HDR_NAME_W) << dos_hdr_wrapper->GetFieldName();
			if (!(field == DosHdrWrapper::Fields::LFARLC || field == DosHdrWrapper::Fields::LFANEW))
				std::cout << std::setw(DOS_HDR_VALUE_W);

			if (field == DosHdrWrapper::Fields::LFARLC)
				std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(DOS_HDR_VALUE_W) << *(WORD*)dos_hdr_wrapper->GetFieldValue() << Logger::TextColor(Logger::Color::BLACK);
			else if (field == DosHdrWrapper::Fields::RES)
				std::cout << Res.str();
			else if (field == DosHdrWrapper::Fields::RES2)
				std::cout << Res2.str();
			else if (field == DosHdrWrapper::Fields::LFANEW)
				std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(DOS_HDR_VALUE_W) << *(DWORD*)dos_hdr_wrapper->GetFieldValue() << Logger::TextColor(Logger::Color::BLACK);
			else
				std::cout << *(WORD*)dos_hdr_wrapper->GetFieldValue();

			std::cout << Logger::ResetColor() << std::endl;
			dos_hdr_wrapper->LoadNextField();
		}
		dos_hdr_wrapper->Reset();

		std::cout << std::endl;
	}

	void Commands::PrintFileHdr()
	{
		FileHdrWrapper* file_hdr_wrapper = loaded_pe_->GetFileHdrWrapper();

		std::cout << std::left << std::uppercase << std::hex << "\n";
		DisplayTable<kFileHdrTable.size()>(kFileHdrTable);

		for (size_t field = 0; field < file_hdr_wrapper->GetFieldsCount(); field++)
		{
			std::cout << " " << Logger::CustomBgColor((field % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
			std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << file_hdr_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
			std::cout << std::setw(FILE_HDR_NAME_W) << file_hdr_wrapper->GetFieldName();
			std::cout << std::setw(FILE_HDR_VALUE_W);

			if (file_hdr_wrapper->GetFieldType() == FieldType::WORD)
				std::cout << *(WORD*)file_hdr_wrapper->GetFieldValue();
			else if (file_hdr_wrapper->GetFieldType() == FieldType::DWORD)
				std::cout << *(DWORD*)file_hdr_wrapper->GetFieldValue();

			std::cout << std::setw(FILE_HDR_DESCRIPTION_W);

			if (file_hdr_wrapper->IsFieldDescribed())
				std::cout << file_hdr_wrapper->GetFieldDescription();

			std::cout << "" << Logger::ResetColor() << std::endl;

			file_hdr_wrapper->LoadNextField();
		}

		auto characteristics = file_hdr_wrapper->GetCharacteristics();
		for (const auto& [value, description] : characteristics)
		{
			std::cout << " " << std::setw(OFFSET_W + FILE_HDR_NAME_W) << "";
			std::cout << Logger::CustomBgColor(Logger::CustomPEColors::DESCRIPTION) << std::setw(FILE_HDR_VALUE_W) << value << std::setw(FILE_HDR_DESCRIPTION_W) << description << Logger::ResetColor() << std::endl;
		}
		file_hdr_wrapper->Reset();

		std::cout << std::endl;
	}

	void Commands::PrintOptHdr()
	{
		OptionalHdrWrapper* optional_hdr_wrapper = loaded_pe_->GetOptionalHdrWrapper();
		OptHdrType opt_hdr_type = optional_hdr_wrapper->GetOptionalHdrType();

		std::cout << std::left << std::uppercase << std::hex << "\n";
		DisplayTable<kOptionalHdrTable.size()>(kOptionalHdrTable);

		for (size_t field = 0; field < optional_hdr_wrapper->GetFieldsCount(); field++)
		{
			if (field == OptionalHdrWrapper::Fields::DATA_BASE && opt_hdr_type == OptHdrType::x64)
			{
				std::cout << " " << Logger::CustomBgColor(Logger::CustomPEColors::DISABLED_COLUMN) << std::setw(OFFSET_W + OPTIONAL_HDR_NAME_W + OPTIONAL_HDR_VALUE_W + OPTIONAL_HDR_DESCRIPTION_W) << "" << Logger::ResetColor() << std::endl;
				optional_hdr_wrapper->LoadNextField();
				continue;
			}
			else if (field == OptionalHdrWrapper::Fields::DLL_CHARACT)
			{
				std::cout << " " << Logger::CustomBgColor((field % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
				std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << optional_hdr_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
				std::cout << std::setw(OPTIONAL_HDR_NAME_W) << optional_hdr_wrapper->GetFieldName();
				std::cout << std::setw(OPTIONAL_HDR_VALUE_W) << *(WORD*)optional_hdr_wrapper->GetFieldValue() << std::setw(OPTIONAL_HDR_DESCRIPTION_W) << "" << Logger::ResetColor() << std::endl;

				auto dll_characteristics = optional_hdr_wrapper->GetDllCharacteristics();
				for (const auto& [value, description] : dll_characteristics)
				{
					std::cout << " " << std::setw(OFFSET_W + OPTIONAL_HDR_NAME_W) << "";
					std::cout << Logger::CustomBgColor(Logger::CustomPEColors::DESCRIPTION) << std::setw(OPTIONAL_HDR_VALUE_W) << value << std::setw(OPTIONAL_HDR_DESCRIPTION_W) << description << Logger::ResetColor() << std::endl;
				}
				optional_hdr_wrapper->LoadNextField();
				continue;
			}
			else if (field == OptionalHdrWrapper::Fields::DATA_DIR)
			{
				DisplayTable<kDataDirTable.size()>(kDataDirTable);

				for (size_t entry = 0; entry < optional_hdr_wrapper->GetDataDirEntriesCount(); entry++)
				{
					std::cout << " " << Logger::CustomBgColor((entry % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << optional_hdr_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
					std::cout << std::setw(OPTIONAL_HDR_NAME_W) << optional_hdr_wrapper->GetDataDirEntryName();

					IMAGE_DATA_DIRECTORY* data_dir_entry = (IMAGE_DATA_DIRECTORY*)optional_hdr_wrapper->GetFieldValue();
					if ((entry == DataDirEntries::SECU))
						std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OPTIONAL_HDR_VALUE_W) << data_dir_entry->VirtualAddress << Logger::TextColor(Logger::Color::BLACK) << std::setw(OPTIONAL_HDR_DESCRIPTION_W) << data_dir_entry->Size;
					else
						std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RVA) << std::setw(OPTIONAL_HDR_VALUE_W) << data_dir_entry->VirtualAddress << Logger::TextColor(Logger::Color::BLACK) << std::setw(OPTIONAL_HDR_DESCRIPTION_W) << data_dir_entry->Size;

					std::cout << Logger::ResetColor() << std::endl;

					optional_hdr_wrapper->LoadNextField();
				}
				std::cout << std::endl;
				continue;
			}

			std::cout << " " << Logger::CustomBgColor((field % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
			std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << optional_hdr_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
			std::cout << std::setw(OPTIONAL_HDR_NAME_W) << optional_hdr_wrapper->GetFieldName();
			if (!(field == OptionalHdrWrapper::Fields::EP || field == OptionalHdrWrapper::Fields::CODE_BASE || field == OptionalHdrWrapper::Fields::DATA_BASE || field == OptionalHdrWrapper::Fields::IMAGE_BASE))
				std::cout << std::setw(OPTIONAL_HDR_VALUE_W);

			if (optional_hdr_wrapper->GetFieldType() == FieldType::BYTE)
				std::cout << (int32_t)*optional_hdr_wrapper->GetFieldValue();
			else if (optional_hdr_wrapper->GetFieldType() == FieldType::WORD)
				std::cout << *(WORD*)optional_hdr_wrapper->GetFieldValue();
			else if (optional_hdr_wrapper->GetFieldType() == FieldType::DWORD)
			{
				if (field == OptionalHdrWrapper::Fields::EP || field == OptionalHdrWrapper::Fields::CODE_BASE || field == OptionalHdrWrapper::Fields::DATA_BASE)
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RVA) << std::setw(OPTIONAL_HDR_VALUE_W) << *(DWORD*)optional_hdr_wrapper->GetFieldValue() << Logger::TextColor(Logger::Color::BLACK);
				else if (field == OptionalHdrWrapper::Fields::IMAGE_BASE)
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::IMAGE_BASE) << std::setw(OPTIONAL_HDR_VALUE_W) << *(DWORD*)optional_hdr_wrapper->GetFieldValue() << Logger::TextColor(Logger::Color::BLACK);
				else
					std::cout << *(DWORD*)optional_hdr_wrapper->GetFieldValue();
			}
			else if (optional_hdr_wrapper->GetFieldType() == FieldType::ULONGLONG)
			{
				if (field == OptionalHdrWrapper::Fields::IMAGE_BASE)
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::IMAGE_BASE) << std::setw(OPTIONAL_HDR_VALUE_W) << *(ULONGLONG*)optional_hdr_wrapper->GetFieldValue() << Logger::TextColor(Logger::Color::BLACK);
				else
					std::cout << *(ULONGLONG*)optional_hdr_wrapper->GetFieldValue();
			}
			std::cout << std::setw(OPTIONAL_HDR_DESCRIPTION_W);

			if (optional_hdr_wrapper->IsFieldDescribed())
				std::cout << optional_hdr_wrapper->GetFieldDescription();

			std::cout << "" << Logger::ResetColor() << std::endl;

			optional_hdr_wrapper->LoadNextField();
		}
		optional_hdr_wrapper->Reset();
	}

	void Commands::PrintSecHdrs()
	{
		SectionHdrsWrapper* section_hdrs_wrapper = loaded_pe_->GetSectionHdrsWrapper();

		if (section_hdrs_wrapper->GetNumOfSections() > 0)
		{
			std::cout << std::left << std::uppercase << std::hex << "\n";
			DisplayTable<kSectionHdrsTable.size()>(kSectionHdrsTable);

			IMAGE_SECTION_HEADER* section_hdr = section_hdrs_wrapper->GetRootSectionHdr();

			for (size_t i = 0; i < section_hdrs_wrapper->GetNumOfSections(); i++)
			{
				char sec_name[IMAGE_SIZEOF_SHORT_NAME + 1] = { 0 };
				std::memcpy(sec_name, section_hdr->Name, IMAGE_SIZEOF_SHORT_NAME);

				std::cout << " " << Logger::CustomBgColor((i % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD) << Logger::TextColor(Logger::Color::BLACK);
				std::cout << std::setw(SECTION_HDRS_NAME_W) << sec_name;
				std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(SECTION_HDRS_R_ADDR_W) << section_hdr->PointerToRawData << Logger::TextColor(Logger::Color::BLACK);
				std::cout << std::setw(SECTION_HDRS_R_SIZE_W) << section_hdr->SizeOfRawData;
				std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RVA) << std::setw(SECTION_HDRS_V_ADDR_W) << section_hdr->VirtualAddress << Logger::TextColor(Logger::Color::BLACK);
				std::cout << std::setw(SECTION_HDRS_V_SIZE_W) << section_hdr->Misc.VirtualSize;
				std::cout << std::setw(SECTION_HDRS_CHARAC_W) << section_hdr->Characteristics;
				std::cout << std::setw(SECTION_HDRS_REL_PTR_W) << section_hdr->PointerToRelocations;
				std::cout << std::setw(SECTION_HDRS_LNUM_PTR_W) << section_hdr->PointerToLinenumbers;
				std::cout << std::setw(SECTION_HDRS_NUM_OF_REL_W) << section_hdr->NumberOfRelocations;
				std::cout << std::setw(SECTION_HDRS_NUM_OF_LNUM_W) << section_hdr->NumberOfLinenumbers << Logger::ResetColor() << std::endl;
				section_hdr++;
			}
			std::cout << std::endl;
		}
		else
			PEW_ERROR("PE has no sections");
	}

	void Commands::PrintExportDir()
	{
		ExportDirWrapper* export_dir_wrapper = (ExportDirWrapper*)loaded_pe_->GetDataDirEntryWrapper(DataDirEntries::EXP);

		if (export_dir_wrapper)
		{
			if (export_dir_wrapper->IsValidWrapper())
			{
				std::cout << std::left << std::uppercase << std::hex << "\n";
				DisplayTable<kExportDirTable.size()>(kExportDirTable);

				for (size_t field = 0; field < export_dir_wrapper->GetFieldsCount(); field++)
				{
					std::cout << " " << Logger::CustomBgColor((field % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << export_dir_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
					std::cout << std::setw(EXPORT_DIR_NAME_W) << export_dir_wrapper->GetFieldName();
					if (!(field == ExportDirWrapper::Fields::NAME_RVA || field == ExportDirWrapper::Fields::FUNCTIONS_RVA || field == ExportDirWrapper::Fields::FUNC_NAMES_RVA || field == ExportDirWrapper::Fields::NAMES_ORDINALS_RVA))
						std::cout << std::setw(EXPORT_DIR_VALUE_W);

					if (field == ExportDirWrapper::Fields::NAME_RVA || field == ExportDirWrapper::Fields::FUNCTIONS_RVA || field == ExportDirWrapper::Fields::FUNC_NAMES_RVA || field == ExportDirWrapper::Fields::NAMES_ORDINALS_RVA)
						std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RVA) << std::setw(EXPORT_DIR_VALUE_W) << *(DWORD*)export_dir_wrapper->GetFieldValue() << Logger::TextColor(Logger::Color::BLACK);
					else if (export_dir_wrapper->GetFieldType() == FieldType::WORD)
						std::cout << *(WORD*)export_dir_wrapper->GetFieldValue();
					else if (export_dir_wrapper->GetFieldType() == FieldType::DWORD)
						std::cout << *(DWORD*)export_dir_wrapper->GetFieldValue();

					std::cout << std::setw(EXPORT_DIR_DESCRIPTION_W);

					if (export_dir_wrapper->IsFieldDescribed())
						std::cout << export_dir_wrapper->GetFieldDescription();

					std::cout << "" << Logger::ResetColor() << std::endl;

					export_dir_wrapper->LoadNextField();
				}
				export_dir_wrapper->Reset();

				std::cout << std::endl;
			}
			else
				PEW_ERROR("Invalid Export Directory\n");
		}
		else
			PEW_ERROR("PE has no Export Directory\n");
	}

	void Commands::PrintExports()
	{
		ExportDirWrapper* export_dir_wrapper = (ExportDirWrapper*)loaded_pe_->GetDataDirEntryWrapper(DataDirEntries::EXP);

		if (export_dir_wrapper)
		{
			if (export_dir_wrapper->IsValidWrapper())
			{
				if (!export_dir_wrapper->GetNumOfFunctions())
				{
					PEW_ERROR("PE has 0 exported functions\n");
					return;
				}

				std::cout << "\n Exports [" << std::dec << export_dir_wrapper->GetNumOfFunctions() << " entiries]" << std::endl;

				std::cout << std::left << std::uppercase << std::hex << "\n";
				DisplayTable<kExportsTable.size()>(kExportsTable);
				
				for (size_t i = 0; i < export_dir_wrapper->GetNumOfFunctions(); i++)
				{
					std::cout << " " << Logger::CustomBgColor((i % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << export_dir_wrapper->GetOffset() << Logger::TextColor(Logger::Color::BLACK);
					std::cout << std::setw(EXPORTS_ORDINAL_W) << export_dir_wrapper->GetOrdinal();
					if (!export_dir_wrapper->IsByOrdinal())
					{
						if(export_dir_wrapper->GetFuncName().size() > EXPORTS_NAME_W - 3)
							std::cout << std::setw(EXPORTS_NAME_W - 3) << GetTrancatedStr(EXPORTS_NAME_W - 3, export_dir_wrapper->GetFuncName()) << Logger::CustomTextColor(Logger::CustomPEColors::LONG_STR_DOTS) << std::setw(3) << "..." << Logger::TextColor(Logger::Color::BLACK);
						else
						std::cout << std::setw(EXPORTS_NAME_W) <<  export_dir_wrapper->GetFuncName();
					}
					else
						std::cout << Logger::CustomBgColor(Logger::CustomPEColors::DISABLED_COLUMN) << std::setw(EXPORTS_NAME_W ) << "" << Logger::CustomBgColor((i % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);

					std::cout << Logger::ResetColor() << std::endl;

					export_dir_wrapper->LoadNextEATEntry();
				}
				export_dir_wrapper->ResetEATEntry();

				std::cout << std::endl;
			}
			else
				PEW_ERROR("Invalid Export Directory\n");
		}
		else
			PEW_ERROR("PE has no Export Directory\n");
	}

	void Commands::PrintImports()
	{
		ImportDirWrapper* import_dir_wrapper = (ImportDirWrapper*)loaded_pe_->GetDataDirEntryWrapper(DataDirEntries::IMP);

		if (import_dir_wrapper)
		{
			if (import_dir_wrapper->IsValidWrapper())
			{
				std::cout << "\n Imports [" << std::dec << import_dir_wrapper->GetLiberiresCount() << " entiries]" << std::endl;

				std::cout << std::left << std::uppercase << std::hex << "\n";
				DisplayTable<kImportsTable.size()>(kImportsTable);

				for (size_t i = 0; i < import_dir_wrapper->GetLiberiresCount(); i++)
				{
					std::cout << " " << Logger::CustomBgColor((i % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << import_dir_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
					std::cout << std::setw(IMPORT_DIR_FUNCTIONSCOUNT_W) << std::dec << import_dir_wrapper->GetFuncCount(i) << std::hex;
					if(import_dir_wrapper->GetLibraryName(i).size() > IMPORT_DIR_NAME_W - 3)
						std::cout << std::setw(IMPORT_DIR_NAME_W - 3) << GetTrancatedStr(IMPORT_DIR_NAME_W - 3, import_dir_wrapper->GetLibraryName(i)) << Logger::CustomTextColor(Logger::CustomPEColors::LONG_STR_DOTS) << std::setw(3) << "..." << Logger::TextColor(Logger::Color::BLACK);
					else
						std::cout << std::setw(IMPORT_DIR_NAME_W) << import_dir_wrapper->GetLibraryName(i);

					std::cout << Logger::ResetColor() << std::endl;
						
					import_dir_wrapper->SkipEntry();
				}
				import_dir_wrapper->Reset();

				std::cout << std::endl;
			}
			else
				std::cerr << "Invalid Import Directory" << std::endl;
		}
		else
			std::cerr << "PE has no Import Directory" << std::endl;
	}

	void Commands::PrintBoundImportsDir()
	{
		BoundImportDirWrapper* bound_import_dir_wrapper = (BoundImportDirWrapper*)loaded_pe_->GetDataDirEntryWrapper(DataDirEntries::BOUNDIMP);

		if (bound_import_dir_wrapper)
		{
			if (bound_import_dir_wrapper->IsValidWrapper())
			{
				if (!bound_import_dir_wrapper->GetLiberiresCount())
				{
					PEW_ERROR("PE has 0 bound imports\n");
					return;
				}

				std::cout << std::left << std::uppercase << std::hex << "\n";
				DisplayTable<kBoundImportsTable.size()>(kBoundImportsTable);

				for (size_t i = 0; i < bound_import_dir_wrapper->GetLiberiresCount(); i++)
				{
					std::cout << " " << Logger::CustomBgColor((i % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << bound_import_dir_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
					std::cout << std::setw(BOUND_IMPORT_NAME_W) << bound_import_dir_wrapper->GetLiberaryName();
					std::cout << std::setw(BOUND_IMPORT_TIMEDATESTAMP_W);

					for (size_t field = 0; field < bound_import_dir_wrapper->GetFieldsCount(); field++)
					{
						if (field == BoundImportDirWrapper::Fields::TIMESTAMP)
							std::cout << std::setw(BOUND_IMPORT_TIMEDATESTAMP_W) << *(DWORD*)bound_import_dir_wrapper->GetFieldValue();
						else if (field == BoundImportDirWrapper::Fields::MODULE_NAME_OFFSET)
							std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(BOUND_IMPORT_OFFSETMODULENAME_W) << *(WORD*)bound_import_dir_wrapper->GetFieldValue() << Logger::TextColor(Logger::Color::BLACK);
						else
							std::cout << std::setw(BOUND_IMPORT_REFS_W) << *(WORD*)bound_import_dir_wrapper->GetFieldValue();

						bound_import_dir_wrapper->LoadNextField();
					}
					std::cout << Logger::ResetColor() << std::endl;

					bound_import_dir_wrapper->LoadNextLiberary();
				}
				bound_import_dir_wrapper->Reset();

				std::cout << Logger::ResetColor() << std::endl;
			}
			else
				PEW_ERROR("Invalid Bound Import Directory\n");
		}
		else
			PEW_ERROR("PE has no Bound Import Directory\n");
	}


	void Commands::PrintRsrcDir()
	{
		ResourceDirWrapper* rsrc_dir_wrapper = (ResourceDirWrapper*)loaded_pe_->GetDataDirEntryWrapper(DataDirEntries::RSRC);

		if (rsrc_dir_wrapper)
		{
			if (rsrc_dir_wrapper->IsValidWrapper())
			{
				std::cout << std::left << std::uppercase << std::hex << "\n";
				DisplayTable<kRsrcDirTable.size()>(kRsrcDirTable);

				for (size_t field = 0; field < rsrc_dir_wrapper->GetFieldsCount(); field++)
				{
					std::cout << " " << Logger::CustomBgColor((field % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << rsrc_dir_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
					std::cout << std::setw(RSRC_DIR_NAME_W) << rsrc_dir_wrapper->GetFieldName();
					std::cout << std::setw(RSRC_DIR_VALUE_W);

					if (rsrc_dir_wrapper->GetFieldType() == FieldType::WORD)
						std::cout << *(WORD*)rsrc_dir_wrapper->GetFieldValue();
					else if (rsrc_dir_wrapper->GetFieldType() == FieldType::DWORD)
						std::cout << *(DWORD*)rsrc_dir_wrapper->GetFieldValue();

					std::cout << std::setw(RSRC_DIR_DESCRIPTION_W);

					if (rsrc_dir_wrapper->IsFieldDescribed())
						std::cout << rsrc_dir_wrapper->GetFieldDescription();

					std::cout << "" << Logger::ResetColor() << std::endl;

					rsrc_dir_wrapper->LoadNextField();
				}
				rsrc_dir_wrapper->Reset();

				if (rsrc_dir_wrapper->GetEntriesCount())
				{
					DisplayTable<kRsrcDirEntriesTable.size()>(kRsrcDirEntriesTable);

					for (size_t entry = 0; entry < rsrc_dir_wrapper->GetEntriesCount(); entry++)
					{
						std::cout << " " << Logger::CustomBgColor((entry % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
						if (rsrc_dir_wrapper->IsDirectory())
						{
							std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RSRC_ENTRY_ARROW) << "> " << Logger::TextColor(Logger::Color::BLACK);
							if (rsrc_dir_wrapper->IsString())
								std::cout << std::setw(RSRC_DIR_ENTRY_TYPE_W - 2) << rsrc_dir_wrapper->GetName();
							else
							{
								WORD rsrc_id = rsrc_dir_wrapper->GetId(); /// check user-defined

								std::cout << std::setw(RSRC_DIR_ENTRY_TYPE_W - 2) << rsrc_dir_wrapper->GetType();
							}
						}
						else
							std::cout << std::setw(RSRC_DIR_ENTRY_TYPE_W - 2) << rsrc_dir_wrapper->GetId();

						if (rsrc_dir_wrapper->IsString())
						{
							std::stringstream ss;
							ss << "Name[" << entry << "]";
							std::cout << std::setw(RSRC_DIR_ENTRY_ENTRIES_W) << ss.str();
							std::cout << std::setw(RSRC_DIR_ENTRY_NAME_ID_W) << rsrc_dir_wrapper->GetNameValue();
							std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(RSRC_DIR_ENTRY_OFFSET_W) << rsrc_dir_wrapper->GetNameOffset() << Logger::TextColor(Logger::Color::BLACK);
						}
						else
						{
							WORD rsrc_id = rsrc_dir_wrapper->GetId();
							{
								std::stringstream ss;
								ss << "Id[" << entry << "]";
								std::cout << std::setw(RSRC_DIR_ENTRY_ENTRIES_W) << ss.str();
								std::cout << std::setw(RSRC_DIR_ENTRY_NAME_ID_W) << rsrc_id;
								std::cout << Logger::CustomBgColor(Logger::CustomPEColors::DISABLED_COLUMN) << std::setw(RSRC_DIR_ENTRY_OFFSET_W) << "" << Logger::CustomBgColor((entry % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
							}
						}

						std::cout << std::setw(RSRC_DIR_ENTRY_DIR_DATA_W) << rsrc_dir_wrapper->GetDataValue();
						std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(RSRC_DIR_ENTRY_OFFSET_W) << rsrc_dir_wrapper->GetDataOffset() << Logger::TextColor(Logger::Color::BLACK);
						std::cout << std::setw(RSRC_DIR_ENTRY_ENTRIES_COUNT_W) << std::dec << rsrc_dir_wrapper->GetEntriesCount(entry) << std::hex;

						std::cout << Logger::ResetColor() << std::endl;

						rsrc_dir_wrapper->LoadNextEntry();
					}
					rsrc_dir_wrapper->ClearEntry();

					std::cout << std::endl;
				}
			}
			else
				PEW_ERROR("Invalid Resource Directory\n");
		}
		else
			PEW_ERROR("PE has no Resource Directory\n");
	}

	void Commands::PrintDebugDir()
	{
		DebugDirWrapper* debug_dir_wrapper = (DebugDirWrapper*)loaded_pe_->GetDataDirEntryWrapper(DataDirEntries::DBG);

		if (debug_dir_wrapper)
		{
			if (debug_dir_wrapper->IsValidWrapper())
			{
				std::cout << std::left << std::uppercase << std::hex << "\n";
				DisplayTable<kDebugDirTable.size()>(kDebugDirTable);

				for (size_t field = 0; field < debug_dir_wrapper->GetFieldsCount(); field++)
				{
					std::cout << " " << Logger::CustomBgColor((field % 2 == 0) ? Logger::CustomPEColors::COLUMN_EVEN : Logger::CustomPEColors::COLUMN_ODD);
					std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(OFFSET_W) << debug_dir_wrapper->GetFieldOffset() << Logger::TextColor(Logger::Color::BLACK);
					std::cout << std::setw(DEBUG_DIR_NAME_W) << debug_dir_wrapper->GetFieldName();
					if (!(field == DebugDirWrapper::Fields::RAW_DATA_ADDR || field == DebugDirWrapper::Fields::RAW_DATA_PTR))
						std::cout << std::setw(DEBUG_DIR_VALUE_W);

					if (field == DebugDirWrapper::Fields::RAW_DATA_ADDR || field == DebugDirWrapper::Fields::RAW_DATA_PTR)
						std::cout << Logger::CustomTextColor(Logger::CustomPEColors::RAW) << std::setw(DEBUG_DIR_VALUE_W) << *(DWORD*)debug_dir_wrapper->GetFieldValue() << Logger::TextColor(Logger::Color::BLACK);
					else if (debug_dir_wrapper->GetFieldType() == FieldType::DWORD)
						std::cout << *(DWORD*)debug_dir_wrapper->GetFieldValue();
					else if (debug_dir_wrapper->GetFieldType() == FieldType::WORD)
						std::cout << *(WORD*)debug_dir_wrapper->GetFieldValue();

					std::cout << std::setw(DEBUG_DIR_DESCRIPTION_W);

					if (debug_dir_wrapper->IsFieldDescribed())
						std::cout << debug_dir_wrapper->GetFieldDescription();

					std::cout << "" << Logger::ResetColor() << std::endl;

					debug_dir_wrapper->LoadNextField();
				}
				debug_dir_wrapper->Reset();

				std::cout << std::endl;
			}
			else
				PEW_ERROR("Invalid Debug Directory\n");
		}
		else
			PEW_ERROR("PE has no Debug Directory\n");
	}

	void Commands::Listen()
	{
		listening_ = true;
		std::string command;

		while (listening_)
		{
			PEW_LOG(Logger::Color::GREEN, true, "PewParser: ");
			std::cin >> command;
			Command c = ParseCommands(command);

			switch (c)
			{
			case Command::DOS_HDR:				PrintDosHdr();					break;
			case Command::FILE_HDR:				PrintFileHdr();					break;
			case Command::OPT_HDR:				PrintOptHdr();					break;
			case Command::SEC_HDRS:				PrintSecHdrs();					break;
			case Command::EXPORT_DIR:			PrintExportDir();				break;
			case Command::EXPORTS:				PrintExports();					break;
			case Command::IMPORTS:				PrintImports();					break;
			case Command::RSRC_DIR:				PrintRsrcDir();					break;
			case Command::DEBUG_DIR:			PrintDebugDir();				break;
			case Command::BOUND_IMPORTS:		PrintBoundImportsDir();			break;
			default:
				PEW_ERROR("Invalid Command\n");
			}
		}
	}

}