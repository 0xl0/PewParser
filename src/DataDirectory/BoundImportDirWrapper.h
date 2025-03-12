#pragma once
#include <PEFile.h>
#include <PewTypes.h>

namespace PewParser {

	class BoundImportDirWrapper
	{
	public:
		enum Fields {
			TIMESTAMP = 0,
			MODULE_NAME_OFFSET,
			MODULE_FORWARDERS_NUM,
			FIELDS_COUNT
		};
	public:
		BoundImportDirWrapper(PEFile* pe);

		FieldOffset GetFieldOffset() const { return field_offset_; }
		std::string GetFieldName() const;
		BYTE* GetFieldValue() const;
		std::string GetFieldDescription() const;
		FieldType GetFieldType() const { return field_type_; }
		size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }

		size_t GetLiberiresCount() const;

		std::string GetLiberaryName() const;

		bool IsForwarderRef() const;

		void LoadNextLiberary();
		void LoadNextField();
		void Reset();

		bool IsValidWrapper() const;

		IMAGE_BOUND_IMPORT_DESCRIPTOR* GetRootDescriptor() const { return root_descriptor_; }
		offset_t GetRootDescriptorOffset() const { return root_descriptor_offset_; }
		size_t GetDescriptorSize() const { return sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR); }
		size_t GetAllDescriptorsSize() const;
	private:
		void Init();
	private:
		IMAGE_BOUND_IMPORT_DESCRIPTOR* root_descriptor_;
		IMAGE_BOUND_IMPORT_DESCRIPTOR* current_descriptor_;
		offset_t root_descriptor_offset_;

		FieldOffset field_offset_;
		FieldIndex field_index_;
		FieldType field_type_;

		WORD forwarder_ref_counter_;

		PEFile* related_pe_;
	};

}