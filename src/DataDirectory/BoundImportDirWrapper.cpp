#include "BoundImportDirWrapper.h"

#include <PEFile.h>
#include <PEUtils.h>

#include <cstring>

namespace PewParser {

    BoundImportDirWrapper::BoundImportDirWrapper(PEFile* pe)
        : related_pe_(pe), root_descriptor_(nullptr), current_descriptor_(nullptr), root_descriptor_offset_(0)
    {
        Init();

        field_offset_ = root_descriptor_offset_;
        field_index_ = Fields::TIMESTAMP;
        field_type_ = FieldType::DWORD;
        forwarder_ref_counter_ = 0;
    }

    void BoundImportDirWrapper::Init()
    {
        offset_t bound_import_dir_rva = related_pe_->GetDataDirectory()[DataDirEntries::BOUNDIMP].VirtualAddress;
        offset_t bound_import_dir_raw = related_pe_->RvaToRaw(bound_import_dir_rva);

        if (bound_import_dir_raw)
        {
            root_descriptor_ = (IMAGE_BOUND_IMPORT_DESCRIPTOR*)related_pe_->GetContentAt(bound_import_dir_raw, OffsetType::RAW);
            current_descriptor_ = root_descriptor_;
            root_descriptor_offset_ = bound_import_dir_raw;
        }
        else if ((bound_import_dir_rva + GetDescriptorSize()) <= related_pe_->GetRawFileSize())
        {
            root_descriptor_ = (IMAGE_BOUND_IMPORT_DESCRIPTOR*)related_pe_->GetContentAt(bound_import_dir_rva, OffsetType::RAW);
            current_descriptor_ = root_descriptor_;
            root_descriptor_offset_ = bound_import_dir_rva;
        }
    }

    std::string BoundImportDirWrapper::GetFieldName() const
    {
        if (IsForwarderRef() && field_index_ == MODULE_FORWARDERS_NUM)
            return "Reserverd";

        switch (field_index_)
        {
            case Fields::TIMESTAMP:                return "TimeDateStamp";
            case Fields::MODULE_NAME_OFFSET:       return "OffsetModuleName";
            case Fields::MODULE_FORWARDERS_NUM:    return "NumberOfModuleForwarderRefs";
            default:                               return "UnKnown";
        }
    }

    BYTE* BoundImportDirWrapper::GetFieldValue() const
    {
        return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
    }

    std::string BoundImportDirWrapper::GetFieldDescription() const
    {
        switch (field_index_)
        {
            case Fields::TIMESTAMP:    return PEUtils::TimeDateStampConverter(current_descriptor_->TimeDateStamp);
            default:                   return std::string();
        }
    }

    size_t BoundImportDirWrapper::GetLiberiresCount() const
    {
        size_t descriptors_count = 0;

        IMAGE_BOUND_IMPORT_DESCRIPTOR* descriptor = root_descriptor_;
        IMAGE_BOUND_IMPORT_DESCRIPTOR null_descriptor = { 0 };

        while (std::memcmp(descriptor, &null_descriptor, sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR)) != 0)
        {
            descriptors_count++;
            descriptor++;
        }

        return descriptors_count;
    }

    std::string BoundImportDirWrapper::GetLiberaryName() const
    {
        offset_t lib_name_offset = root_descriptor_offset_ + current_descriptor_->OffsetModuleName;

        char* library_name = (char*)related_pe_->GetContentAt(lib_name_offset, OffsetType::RAW);

        if (library_name)
            return std::string(library_name);

        return std::string();
    }

    bool BoundImportDirWrapper::IsForwarderRef() const
    {
        if (forwarder_ref_counter_ > 0)
            return true;

        return false;
    }

    size_t BoundImportDirWrapper::GetAllDescriptorsSize() const
    {
        size_t descriptors_count = GetLiberiresCount();

        return descriptors_count * sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR);
    }

    void BoundImportDirWrapper::LoadNextLiberary()
    {
        if (forwarder_ref_counter_ == 0)
            forwarder_ref_counter_ = current_descriptor_->NumberOfModuleForwarderRefs;
        else
            forwarder_ref_counter_--;

        current_descriptor_++;
        field_index_ = Fields::TIMESTAMP;
        field_type_ = FieldType::DWORD;
    }

    bool BoundImportDirWrapper::IsValidWrapper() const
    {
        if (root_descriptor_)
            return true;

        return false;
    }

    void BoundImportDirWrapper::LoadNextField()
    {
        if (field_type_ == FieldType::WORD)
            field_offset_ += sizeof(WORD);
        else if (field_type_ == FieldType::DWORD)
            field_offset_ += sizeof(DWORD);

        field_index_++;
        if (field_index_ == Fields::TIMESTAMP)
            field_type_ = FieldType::DWORD;
        else
            field_type_ = FieldType::WORD;
    }

    void BoundImportDirWrapper::Reset()
    {
        current_descriptor_ = root_descriptor_;
        field_offset_ = root_descriptor_offset_;
        field_index_ = Fields::TIMESTAMP;
        field_type_ = FieldType::DWORD;
        forwarder_ref_counter_ = 0;
    }
}