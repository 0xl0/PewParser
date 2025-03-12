#pragma once
#include <PEFile.h>
#include <PewTypes.h>

namespace PewParser {

	class DebugDirWrapper
	{
	public:
		enum Fields {
			CHARACTERISTICS = 0,
			TIMESTAMP,
			MAJOR_VER,
			MINOR_VER,
			TYPE,
			DATA_SIZE,
			RAW_DATA_ADDR,
			RAW_DATA_PTR,
			FIELDS_COUNT
		};
	public:
		DebugDirWrapper(PEFile* pe);

		FieldOffset GetFieldOffset() const { return field_offset_; }
		std::string GetFieldName() const;
		BYTE* GetFieldValue() const;
		std::string GetFieldDescription() const;
		FieldType GetFieldType() const { return field_type_; }
		size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }

		bool IsFieldDescribed() const;

		void LoadNextField();
		void Reset();

		bool IsValidWrapper() const;

		IMAGE_DEBUG_DIRECTORY* GetDebugDir() { return debug_dir_; }
		offset_t GetDebugDirOffset() { return debug_dir_offset_; }
		size_t GetDebugDirSize() { return sizeof(IMAGE_DEBUG_DIRECTORY); }
	private:
		std::string GetTypeDescription() const;
	private:
		void Init();
	private:
		IMAGE_DEBUG_DIRECTORY* debug_dir_;
		offset_t debug_dir_offset_;

		FieldOffset field_offset_;
		FieldIndex field_index_;
		FieldType field_type_;

		PEFile* related_pe_;
	};

}