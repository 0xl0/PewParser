#pragma once
#include <PEFile.h>
#include <PewTypes.h>

namespace PewParser {

	class ImportDirWrapper
	{
	public:
		enum Fields {
			ORIG_FIRST_THUNK = 0,
			TIMESTAMP,
			FORWARDER,
			NAME,
			FIRST_THUNK,
			FIELDS_COUNT
		};
	public:
		ImportDirWrapper(PEFile* pe);

		FieldOffset GetFieldOffset() const { return field_offset_; }
		std::string GetFieldName() const;
		BYTE* GetFieldValue() const;
		size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }

		size_t GetLiberiresCount() const;
		std::string GetLibraryName(index_t descriptor_index) const;
		size_t GetFuncCount(index_t descriptor_index) const;
		Binding BindingStyle(index_t descriptor_index) const;
		bool IsValidForwarderChain(index_t descriptor_index) const;

		offset_t CallFuncVia() const;
		bool IsByOrdinal() const;
		WORD GetOrdinal() const;
		void* GetOriginalThunk() const;
		void* GetThunk() const;
		WORD GetHint() const;
		std::string GetName() const;

		bool HasForwarders(index_t descriptor_index) const;
		bool IsForwarder() const;
		bool IsValidNextChainIndex() const;
		bool IsEndOfChain() const;
		void LoadNextChainIndex();
		offset_t GetForwarder() const;

		void SkipEntry();
		void LoadNextField();
		void Reset();

		void LoadNextFunc() { current_ft_array_index_++; }
		void Clear();

		int32_t GetSelectedLibrary() const { return selected_library_; }
		void SetSelectedLibrary(int32_t selected_library);

		ThunkType GetThunkType() const { return thunk_type_; }

		bool IsValidWrapper() const;

		IMAGE_IMPORT_DESCRIPTOR* GetRootDescriptor() const { return root_descriptor_; }
		offset_t GetRootDescriptorOffset() const { return root_descriptor_offset_; }
		size_t GetDescriptorSize() const { return sizeof(IMAGE_IMPORT_DESCRIPTOR); }
		size_t GetAllDescriptorsSize() const;
		size_t GetThunkDataSize() const;
	private:
		bool HasINT(index_t descriptor_index) const;
		bool HasIAT(index_t descriptor_index) const;
		IMAGE_IMPORT_BY_NAME* GetHintAndName() const;
	private:
		void Init();
	private:
		IMAGE_IMPORT_DESCRIPTOR* root_descriptor_;
		IMAGE_IMPORT_DESCRIPTOR* selected_descriptor_;
		offset_t root_descriptor_offset_;

		FieldOffset field_offset_;
		FieldIndex field_index_;

		int32_t selected_library_;
		DWORD current_forwarderchain_;
		ThunkType thunk_type_;
		index_t current_ft_array_index_;

		PEFile* related_pe_;
	};
}