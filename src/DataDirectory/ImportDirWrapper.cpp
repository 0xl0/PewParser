#include "ImportDirWrapper.h"

#include <PEFile.h>

#include <cstring>

namespace PewParser {

	ImportDirWrapper::ImportDirWrapper(PEFile* pe)
		: related_pe_(pe), root_descriptor_(nullptr), selected_descriptor_(nullptr), root_descriptor_offset_(0), current_forwarderchain_(-1)
	{
		Init();

		field_offset_ = root_descriptor_offset_;
		field_index_ = Fields::ORIG_FIRST_THUNK;
		current_ft_array_index_ = 0;
		selected_library_ = 0;

		if (related_pe_->GetPEType() == PEType::x32PE)
			thunk_type_ = ThunkType::THUNK32;
		else
			thunk_type_ = ThunkType::THUNK64;
	}

	void ImportDirWrapper::Init()
	{
		offset_t root_descriptor_rva = related_pe_->GetDataDirectory()[DataDirEntries::IMP].VirtualAddress;
		offset_t root_descriptor_raw = related_pe_->RvaToRaw(root_descriptor_rva);

		if (root_descriptor_raw)
		{
			root_descriptor_ = (IMAGE_IMPORT_DESCRIPTOR*)related_pe_->GetContentAt(root_descriptor_raw, OffsetType::RAW);
			selected_descriptor_ = root_descriptor_;
			root_descriptor_offset_ = root_descriptor_raw;
			current_forwarderchain_ = selected_descriptor_->ForwarderChain;
		}
		else if ((root_descriptor_raw + GetDescriptorSize()) <= related_pe_->GetRawFileSize())
		{
			root_descriptor_ = (IMAGE_IMPORT_DESCRIPTOR*)related_pe_->GetContentAt(root_descriptor_raw, OffsetType::RAW);
			selected_descriptor_ = root_descriptor_;
			root_descriptor_offset_ = root_descriptor_raw;
			current_forwarderchain_ = selected_descriptor_->ForwarderChain;
		}
	}

	std::string ImportDirWrapper::GetFieldName() const
	{
		switch (field_index_)
		{
			case Fields::ORIG_FIRST_THUNK:	return "OriginalFirstThunk";
			case Fields::TIMESTAMP:			return "TimeDateStamp";
			case Fields::FORWARDER:			return "Forwarder";
			case Fields::NAME:				return "NameRVA";
			case Fields::FIRST_THUNK:		return "FirstThunk";
			default:						return "UnKnown";
		}
	}

	BYTE* ImportDirWrapper::GetFieldValue() const
	{
		return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
	}

	size_t ImportDirWrapper::GetLiberiresCount() const
	{
		size_t descriptors_count = 0;

		IMAGE_IMPORT_DESCRIPTOR* descriptor = root_descriptor_;
		IMAGE_IMPORT_DESCRIPTOR null_descriptor = { 0 };

		while (std::memcmp(descriptor, &null_descriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR)) != 0)
		{
			descriptors_count++;
			descriptor++;
		}

		return descriptors_count;
	}

	std::string ImportDirWrapper::GetLibraryName(index_t descriptor_index) const
	{
		char* libraryName = (char*)related_pe_->GetContentAt((root_descriptor_ + descriptor_index)->Name, OffsetType::RVA);

		if(libraryName)
			return std::string(libraryName);
		
		return std::string();
	}

	size_t ImportDirWrapper::GetFuncCount(index_t descriptor_index) const
	{
		size_t functions_count = 0;
		offset_t symbols_array_offset = 0;

		if (HasINT(descriptor_index))
			symbols_array_offset = (root_descriptor_ + descriptor_index)->OriginalFirstThunk;
		else if (HasIAT(descriptor_index))
			symbols_array_offset = (root_descriptor_ + descriptor_index)->FirstThunk;

		if (thunk_type_ == ThunkType::THUNK32)
		{
			DWORD* symbols_array_ptr = (DWORD*)related_pe_->GetContentAt(symbols_array_offset, OffsetType::RVA);
			while (*symbols_array_ptr)
			{
				functions_count++;
				symbols_array_ptr++;
			}
		}
		else
		{
			ULONGLONG* symbols_array_ptr = (ULONGLONG*)related_pe_->GetContentAt(symbols_array_offset, OffsetType::RVA);
			while (*symbols_array_ptr)
			{
				functions_count++;
				symbols_array_ptr++;
			}
		}

		return functions_count;
	}

	Binding ImportDirWrapper::BindingStyle(index_t descriptor_index) const
	{
		if ((root_descriptor_ + descriptor_index)->TimeDateStamp == -1)
			return Binding::NEWBIND;
		if ((root_descriptor_ + descriptor_index)->TimeDateStamp > 0)
			return Binding::OLDBIND;

		return Binding::NOTBOUND;
	}

	bool ImportDirWrapper::IsValidForwarderChain(index_t descriptor_index) const
	{
		DWORD first_chain_index = (root_descriptor_ + descriptor_index)->ForwarderChain;
		size_t func_count = GetFuncCount(descriptor_index);

		if (first_chain_index == -1 || first_chain_index < func_count)
			return true;

		return false;
	}

	offset_t ImportDirWrapper::CallFuncVia() const
	{
		offset_t callvia = 0;

		callvia = selected_descriptor_->FirstThunk + (current_ft_array_index_ * GetThunkDataSize());

		return callvia;
	}

	bool ImportDirWrapper::IsByOrdinal() const
	{
		offset_t entry_offset = 0;

		if (HasINT(selected_library_))
			entry_offset = selected_descriptor_->OriginalFirstThunk + (current_ft_array_index_ * GetThunkDataSize());
		else if (HasIAT(selected_library_))
			entry_offset = selected_descriptor_->FirstThunk + (current_ft_array_index_ * GetThunkDataSize());

		if (thunk_type_ == ThunkType::THUNK32)
		{
			IMAGE_THUNK_DATA32 thunk = *(IMAGE_THUNK_DATA32*)related_pe_->GetContentAt(entry_offset, OffsetType::RVA);
			if (IMAGE_SNAP_BY_ORDINAL32(thunk.u1.Ordinal))
				return true;
		}
		else
		{
			IMAGE_THUNK_DATA64 thunk = *(IMAGE_THUNK_DATA64*)related_pe_->GetContentAt(entry_offset, OffsetType::RVA);
			if (IMAGE_SNAP_BY_ORDINAL64(thunk.u1.Ordinal))
				return true;
		}

		return false;
	}

	WORD ImportDirWrapper::GetOrdinal() const
	{
		offset_t entry_offset = 0;
		WORD ordinal = 0;

		if (HasINT(selected_library_))
			entry_offset = selected_descriptor_->OriginalFirstThunk + (current_ft_array_index_ * GetThunkDataSize());
		else if (HasIAT(selected_library_))
			entry_offset = selected_descriptor_->FirstThunk + (current_ft_array_index_ * GetThunkDataSize());

		if (thunk_type_ == ThunkType::THUNK32)
		{
			IMAGE_THUNK_DATA32 thunk = *(IMAGE_THUNK_DATA32*)related_pe_->GetContentAt(entry_offset, OffsetType::RVA);
			ordinal = IMAGE_ORDINAL32(thunk.u1.Ordinal);
		}
		else
		{
			IMAGE_THUNK_DATA64 thunk = *(IMAGE_THUNK_DATA64*)related_pe_->GetContentAt(entry_offset, OffsetType::RVA);
			ordinal = IMAGE_ORDINAL64(thunk.u1.Ordinal);
		}

		return ordinal;
	}

	void* ImportDirWrapper::GetOriginalThunk() const
	{
		offset_t oft_offset = selected_descriptor_->OriginalFirstThunk + (current_ft_array_index_ * GetThunkDataSize());

		void* oft_ptr = related_pe_->GetContentAt(oft_offset, OffsetType::RVA);

		if (!oft_ptr)
			return nullptr;

		return oft_ptr;
	}

	void* ImportDirWrapper::GetThunk() const
	{
		offset_t ft_offset = selected_descriptor_->FirstThunk + (current_ft_array_index_ * GetThunkDataSize());

		void* ft_ptr = related_pe_->GetContentAt(ft_offset, OffsetType::RVA);

		if (!ft_ptr)
			return nullptr;

		return ft_ptr;
	}

	WORD ImportDirWrapper::GetHint() const
	{
		_IMAGE_IMPORT_BY_NAME* hint_and_name = GetHintAndName();

		return hint_and_name->Hint;
	}

	std::string ImportDirWrapper::GetName() const
	{
		IMAGE_IMPORT_BY_NAME* hint_and_name = GetHintAndName();

		return (char*)hint_and_name->Name;
	}

	IMAGE_IMPORT_BY_NAME* ImportDirWrapper::GetHintAndName() const
	{
		offset_t entry_offset = 0;
		IMAGE_IMPORT_BY_NAME* hint_and_name = nullptr;

		if (HasINT(selected_library_))
			entry_offset = selected_descriptor_->OriginalFirstThunk + (current_ft_array_index_ * GetThunkDataSize());
		else if (HasIAT(selected_library_))
			entry_offset = selected_descriptor_->FirstThunk + (current_ft_array_index_ * GetThunkDataSize());

		if (thunk_type_ == ThunkType::THUNK32)
		{
			IMAGE_THUNK_DATA32 thunk = *(IMAGE_THUNK_DATA32*)related_pe_->GetContentAt(entry_offset, OffsetType::RVA);
			hint_and_name = (IMAGE_IMPORT_BY_NAME*)related_pe_->GetContentAt(thunk.u1.AddressOfData, OffsetType::RVA);
		}
		else
		{
			IMAGE_THUNK_DATA64 thunk = *(IMAGE_THUNK_DATA64*)related_pe_->GetContentAt(entry_offset, OffsetType::RVA);
			hint_and_name = (IMAGE_IMPORT_BY_NAME*)related_pe_->GetContentAt(thunk.u1.AddressOfData, OffsetType::RVA);
		}

		return hint_and_name;
	}

	bool ImportDirWrapper::HasForwarders(index_t descriptor_index) const
	{
		if (BindingStyle(descriptor_index) == Binding::OLDBIND && IsValidForwarderChain(descriptor_index))
			return true;

		return false;
	}

	bool ImportDirWrapper::IsForwarder() const
	{
		if (current_ft_array_index_ == current_forwarderchain_)
			return true;

		return false;
	}

	bool ImportDirWrapper::IsValidNextChainIndex() const
	{
		DWORD next_chanin_index = *(DWORD*)GetThunk();
		size_t func_count = GetFuncCount(selected_library_);

		if (next_chanin_index == -1)
			return true;

		if (next_chanin_index > current_forwarderchain_ && next_chanin_index < func_count)
			return true;

		return false;
	}

	bool ImportDirWrapper::IsEndOfChain() const
	{
		DWORD next_chanin_index = *(DWORD*)GetThunk();

		if (next_chanin_index == -1)
			return true;

		return false;
	}

	void ImportDirWrapper::LoadNextChainIndex()
	{
		DWORD next_chanin_index = *(DWORD*)GetThunk();

		current_forwarderchain_ = next_chanin_index;
	}

	offset_t ImportDirWrapper::GetForwarder() const
	{
		offset_t entry_offset = 0;

		DWORD next_chanin_index = *(DWORD*)GetThunk();

		entry_offset = selected_descriptor_->FirstThunk + (next_chanin_index * GetThunkDataSize());
		return entry_offset;
	}

	size_t ImportDirWrapper::GetAllDescriptorsSize() const
	{
		size_t descriptors_count = GetLiberiresCount();

		return descriptors_count * sizeof(IMAGE_IMPORT_DESCRIPTOR);
	}

	size_t ImportDirWrapper::GetThunkDataSize() const
	{
		if (thunk_type_ == ThunkType::THUNK32)
			return sizeof(IMAGE_THUNK_DATA32);
		else
			return sizeof(IMAGE_THUNK_DATA64);
	}

	void ImportDirWrapper::SetSelectedLibrary(int32_t selectedLibrary)
	{
		selected_library_ = selectedLibrary;
		selected_descriptor_ = root_descriptor_ + selectedLibrary;
		current_forwarderchain_ = selected_descriptor_->ForwarderChain;
	}

	bool ImportDirWrapper::HasINT(index_t descriptor_index) const
	{
		if ((root_descriptor_ + descriptor_index)->OriginalFirstThunk)
			return true;

		return false;
	}

	bool ImportDirWrapper::HasIAT(index_t descriptor_index) const
	{
		if ((root_descriptor_ + descriptor_index)->FirstThunk)
			return true;

		return false;
	}

	bool ImportDirWrapper::IsValidWrapper() const
	{
		if (root_descriptor_)
			return true;

		return false;
	}

	void ImportDirWrapper::SkipEntry()
	{
		if(field_index_ == Fields::ORIG_FIRST_THUNK)
			field_offset_ += sizeof(DWORD) * FIELDS_COUNT;
		else if (field_index_ == Fields::TIMESTAMP)
			field_offset_ += sizeof(DWORD) * FIELDS_COUNT -1;
		else if (field_index_ == Fields::FORWARDER)
			field_offset_ += sizeof(DWORD) * FIELDS_COUNT -2;
		else if (field_index_ == Fields::NAME)
			field_offset_ += sizeof(DWORD) * FIELDS_COUNT -3;
		else if (field_index_ == Fields::FIRST_THUNK)
			field_offset_ += sizeof(DWORD) * FIELDS_COUNT -4;

		field_index_ = Fields::ORIG_FIRST_THUNK;
	}

	void ImportDirWrapper::LoadNextField()
	{
		field_index_++;
		field_offset_ += sizeof(DWORD);
	}

	void ImportDirWrapper::Reset()
	{
		field_offset_ = root_descriptor_offset_;
		field_index_ = Fields::ORIG_FIRST_THUNK;
	}

	void ImportDirWrapper::Clear()
	{
		current_ft_array_index_ = 0;
		current_forwarderchain_ = selected_descriptor_->ForwarderChain;
	}

}