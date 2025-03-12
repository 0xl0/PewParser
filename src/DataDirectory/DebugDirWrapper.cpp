#include "DebugDirWrapper.h"

#include <PEFile.h>
#include <PEUtils.h>

namespace PewParser {

    DebugDirWrapper::DebugDirWrapper(PEFile* pe)
        : related_pe_(pe), debug_dir_(nullptr), debug_dir_offset_(0)
    {
        Init();

        field_offset_ = debug_dir_offset_;
        field_index_ = Fields::CHARACTERISTICS;
        field_type_ = FieldType::DWORD;
    }

    void DebugDirWrapper::Init()
    {
        offset_t debug_dir_rva = related_pe_->GetDataDirectory()[DataDirEntries::DBG].VirtualAddress;
        offset_t debug_dir_raw = related_pe_->RvaToRaw(debug_dir_rva);

        if (debug_dir_raw)
        {
            debug_dir_ = (IMAGE_DEBUG_DIRECTORY*)related_pe_->GetContentAt(debug_dir_raw, OffsetType::RAW);
            debug_dir_offset_ = debug_dir_raw;
        }
        else if ((debug_dir_rva + GetDebugDirSize()) <= related_pe_->GetRawFileSize())
        {
            debug_dir_ = (IMAGE_DEBUG_DIRECTORY*)related_pe_->GetContentAt(debug_dir_rva, OffsetType::RAW);
            debug_dir_offset_ = debug_dir_rva;
        }
    }

    std::string DebugDirWrapper::GetFieldName() const
    {
        switch (field_index_)
        {
            case Fields::CHARACTERISTICS:    return "Characteristics";
            case Fields::TIMESTAMP:          return "TimeDateStamp";
            case Fields::MAJOR_VER:          return "MajorVersion";
            case Fields::MINOR_VER:          return "MinorVersion";
            case Fields::TYPE:               return "Type";
            case Fields::DATA_SIZE:          return "SizeOfData";
            case Fields::RAW_DATA_ADDR:      return "AddressOfRawData";
            case Fields::RAW_DATA_PTR:       return "PointerToRawData";
            default:                         return "UnKnown";
        }
    }

    BYTE* DebugDirWrapper::GetFieldValue() const
    {
        return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
    }

    std::string DebugDirWrapper::GetFieldDescription() const
    {
        switch (field_index_)
        {
            case Fields::TIMESTAMP:    return PEUtils::TimeDateStampConverter(debug_dir_->TimeDateStamp);
            case Fields::TYPE:         return GetTypeDescription();
            default:                   return std::string();
        }
    }

    std::string DebugDirWrapper::GetTypeDescription() const
    {
        switch (debug_dir_->Type)
        {
            case IMAGE_DEBUG_TYPE_UNKNOWN:     return "UnKnown";
            case IMAGE_DEBUG_TYPE_COFF:        return "COFF";
            case IMAGE_DEBUG_TYPE_CODEVIEW:    return "Visual C++ (CODEVIEW)";
            case IMAGE_DEBUG_TYPE_FPO:         return "Frame pointer omission (FPO)";
            case IMAGE_DEBUG_TYPE_MISC:        return "DBG file";
                /*
            case IMAGE_DEBUG_TYPE_EXCEPTION:
            case IMAGE_DEBUG_TYPE_FIXUP:
            case IMAGE_DEBUG_TYPE_OMAP_TO_SRC:
            case IMAGE_DEBUG_TYPE_OMAP_FROM_SRC:
            case IMAGE_DEBUG_TYPE_BORLAND:
            case IMAGE_DEBUG_TYPE_RESERVED10:
            case IMAGE_DEBUG_TYPE_BBT:
            case IMAGE_DEBUG_TYPE_CLSID:
            case IMAGE_DEBUG_TYPE_VC_FEATURE:
            case IMAGE_DEBUG_TYPE_POGO:
            case IMAGE_DEBUG_TYPE_ILTCG:
            case IMAGE_DEBUG_TYPE_MPX:
            case IMAGE_DEBUG_TYPE_REPRO:
            case IMAGE_DEBUG_TYPE_SPGO:
            case IMAGE_DEBUG_TYPE_EX_DLLCHARACTERISTICS:
                */
            default:                            return std::string();
        }
    }

    bool DebugDirWrapper::IsFieldDescribed() const
    {
        if (field_index_ == Fields::TIMESTAMP || field_index_ == Fields::TYPE)
            return true;

        return false;
    }

    bool DebugDirWrapper::IsValidWrapper() const
    {
        if (debug_dir_)
            return true;

        return false;
    }

    void DebugDirWrapper::LoadNextField()
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

    void DebugDirWrapper::Reset()
    {
        field_offset_ = debug_dir_offset_;
        field_index_ = Fields::CHARACTERISTICS;
        field_type_ = FieldType::DWORD;
    }
}