#include "ExportDirWrapper.h"

#include <PEFile.h>
#include <PEUtils.h>

namespace PewParser {

    ExportDirWrapper::ExportDirWrapper(PEFile* pe)
        : related_pe_(pe), export_dir_(nullptr), export_dir_offset_(0)
    {
        Init();

        field_offset_ = export_dir_offset_;
        field_index_ = Fields::CHARACTERISTICS;
        field_type_ = FieldType::DWORD;
        EAT_entry_ = 0;

        CacheNames();
    }

    void ExportDirWrapper::Init()
    {
        offset_t export_dir_rva = related_pe_->GetDataDirectory()[DataDirEntries::EXP].VirtualAddress;
        offset_t export_dir_raw = related_pe_->RvaToRaw(export_dir_rva);

        if (export_dir_raw)
        {
            export_dir_ = (IMAGE_EXPORT_DIRECTORY*)related_pe_->GetContentAt(export_dir_raw, OffsetType::RAW);
            export_dir_offset_ = export_dir_raw;
        }
        else if ((export_dir_rva + GetExportDirSize()) <= related_pe_->GetRawFileSize())
        {
            export_dir_ = (IMAGE_EXPORT_DIRECTORY*)related_pe_->GetContentAt(export_dir_rva, OffsetType::RAW);
            export_dir_offset_ = export_dir_rva;
        }
    }

    std::string_view ExportDirWrapper::GetFieldName() const
    {
        switch (field_index_)
        {
            case Fields::CHARACTERISTICS:       return "Characteristics";
            case Fields::TIMESTAMP:             return "TimeDateStamp";
            case Fields::MAJOR_VER:             return "MajorVersion";
            case Fields::MINOR_VER:             return "MinorVersion";
            case Fields::NAME_RVA:              return "Name";
            case Fields::BASE:                  return "Base";
            case Fields::FUNCTIONS_NUM:         return "NumberOfFunctions";
            case Fields::NAMES_NUM:             return "NumberOfNames";
            case Fields::FUNCTIONS_RVA:         return "AddressOfFunctions";
            case Fields::FUNC_NAMES_RVA:        return "AddressOfNames";
            case Fields::NAMES_ORDINALS_RVA:    return "AddressOfNameOrdinals";
            default:                            return "UnKnown";
        }
    }

    BYTE* ExportDirWrapper::GetFieldValue() const
    {
        return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
    }

    std::string ExportDirWrapper::GetFieldDescription() const
    {
        switch (field_index_)
        {
            case Fields::TIMESTAMP:    return PEUtils::TimeDateStampConverter(export_dir_->TimeDateStamp);
            case Fields::NAME_RVA:     return GetLibraryName();
            default:                   return std::string();
        }
    }

    bool ExportDirWrapper::IsFieldDescribed() const
    {
        if (field_index_ == Fields::TIMESTAMP || field_index_ == Fields::NAME_RVA)
            return true;

        return false;
    }

    std::string ExportDirWrapper::GetLibraryName() const
    {
        char* library_name = (char*)related_pe_->GetContentAt(export_dir_->Name, OffsetType::RVA);

        if (library_name)
            return std::string(library_name);

        return std::string();
    }

    offset_t ExportDirWrapper::GetOffset() const
    {
        offset_t offset = 0;

        offset_t current_func_rva = export_dir_->AddressOfFunctions + (EAT_entry_ * sizeof(DWORD));
        offset_t current_func_raw = related_pe_->RvaToRaw(current_func_rva);

        if (current_func_raw)
            offset = current_func_raw;

        return offset;
    }

    WORD ExportDirWrapper::GetOrdinal() const
    {
        WORD base_ordinal = (WORD)export_dir_->Base + EAT_entry_;
        WORD ordinal = base_ordinal;

        return ordinal;
    }

    DWORD ExportDirWrapper::GetFuncRVA() const
    {
        DWORD rva = 0;

        DWORD* EAT = (DWORD*)related_pe_->GetContentAt(export_dir_->AddressOfFunctions, OffsetType::RVA);

        if (EAT)
            rva = *(EAT + EAT_entry_);

        return rva;
    }

    DWORD ExportDirWrapper::GetFuncNameRVA() const
    {
        DWORD rva = 0;

        DWORD* ENT = (DWORD*)related_pe_->GetContentAt(export_dir_->AddressOfNames, OffsetType::RVA);

        if (ENT)
            rva = *(ENT + ordinal_to_name_.at(EAT_entry_));

        return rva;
    }

    std::string ExportDirWrapper::GetFuncName() const
    {
        DWORD func_name_rva = GetFuncNameRVA();

        if (func_name_rva)
        {
            char* func_name = (char*)related_pe_->GetContentAt(func_name_rva, OffsetType::RVA);

            if (func_name)
                return std::string(func_name);
        }

        return std::string();
    }

    std::string ExportDirWrapper::GetForwarderName() const
    {
        DWORD func_rva = GetFuncRVA();

        if (func_rva)
        {
            char* func_name = (char*)related_pe_->GetContentAt(func_rva, OffsetType::RVA);
            if (func_name)
                return std::string(func_name);
        }

        return std::string();
    }

    bool ExportDirWrapper::IsByOrdinal() const
    {
        if(ordinal_to_name_.find(EAT_entry_) == ordinal_to_name_.end())
            return true;
        else
            return false;
    }

    bool ExportDirWrapper::IsForwarder() const
    {
        DWORD func_rva = GetFuncRVA();

        DWORD export_dir_rva = related_pe_->GetDataDirectory()->VirtualAddress;
        DWORD export_dir_size = export_dir_rva + related_pe_->GetDataDirectory()->Size;

        return (func_rva >= export_dir_rva && func_rva <= export_dir_size);
    }

    void ExportDirWrapper::CacheNames()
    {
        WORD* ordinals_array = (WORD*)related_pe_->GetContentAt(export_dir_->AddressOfNameOrdinals, OffsetType::RVA);

        if (!ordinals_array)
            return;

        for (size_t i = 0; i < export_dir_->NumberOfNames; i++)
        {
            WORD ordinal = *ordinals_array;
            ordinal_to_name_[ordinal] = (WORD)i;
            ordinals_array++;
        }
    }

    bool ExportDirWrapper::IsValidWrapper() const
    {
        if (export_dir_)
            return true;

        return false;
    }

    void ExportDirWrapper::LoadNextField()
    {
        if (field_type_ == FieldType::WORD)
            field_offset_ += sizeof(WORD);
        else if (field_type_ == FieldType::DWORD)
            field_offset_ += sizeof(DWORD);

        field_index_++;
        if (field_index_ == Fields::MAJOR_VER || field_index_ == Fields::MINOR_VER)
            field_type_ = FieldType::WORD;
        else
            field_type_ = FieldType::DWORD;
    }

    void ExportDirWrapper::Reset()
    {
        field_offset_ = export_dir_offset_;
        field_index_ = Fields::CHARACTERISTICS;
        field_type_ = FieldType::DWORD;
    }

}