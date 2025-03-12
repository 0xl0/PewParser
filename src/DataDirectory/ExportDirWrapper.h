#pragma once
#include <PEFile.h>
#include <PewTypes.h>

#include <unordered_map>

namespace PewParser {

	class ExportDirWrapper
	{
	public:
		enum Fields {
			CHARACTERISTICS = 0,
			TIMESTAMP,
			MAJOR_VER,
			MINOR_VER,
			NAME_RVA,
			BASE,
			FUNCTIONS_NUM,
			NAMES_NUM,
			FUNCTIONS_RVA,
			FUNC_NAMES_RVA,
			NAMES_ORDINALS_RVA,
			FIELDS_COUNT
		};
	public:
		ExportDirWrapper(PEFile* pe);

		FieldOffset GetFieldOffset() const { return field_offset_; }
		std::string GetFieldName() const;
		BYTE* GetFieldValue() const;
		std::string GetFieldDescription() const;
		FieldType GetFieldType() const { return field_type_; }
		size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }

		bool IsFieldDescribed() const;

		void LoadNextField();
		void Reset();

		size_t GetNumOfFunctions() const { return export_dir_->NumberOfFunctions; }

		std::string GetLibraryName() const;

		offset_t GetOffset() const;
		WORD GetOrdinal() const;
		DWORD GetFuncRVA() const;
		DWORD GetFuncNameRVA() const;
		std::string GetFuncName() const;
		std::string GetForwarderName() const;

		bool IsByOrdinal() const;
		bool IsForwarder() const;

		void LoadNextEATEntry() { EAT_entry_++; }
		void ResetEATEntry() { EAT_entry_ = 0; }

		void CacheNames();

		bool IsValidWrapper() const;

		IMAGE_EXPORT_DIRECTORY* GetExportDir() { return export_dir_; }
		offset_t GetExportDirOffset() { return export_dir_offset_; }
		size_t GetExportDirSize() { return sizeof(IMAGE_EXPORT_DIRECTORY); }
	private:
		void Init();
	private:
		IMAGE_EXPORT_DIRECTORY* export_dir_;
		offset_t export_dir_offset_;

		FieldOffset field_offset_;
		FieldIndex field_index_;
		FieldType field_type_;

		WORD EAT_entry_;

		std::unordered_map<WORD, index_t> ordinal_to_name_;

		PEFile* related_pe_;
	};

}