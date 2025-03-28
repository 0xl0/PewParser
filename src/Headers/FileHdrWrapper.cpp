#include "FileHdrWrapper.h"

#include <PEFile.h>
#include <PEUtils.h>

namespace PewParser {

    FileHdrWrapper::FileHdrWrapper(PEFile* pe)
        : related_pe_(pe), file_hdr_offset_(pe->GetFileHdrOffset())
    {
        file_hdr_ = (IMAGE_FILE_HEADER*)pe->GetContentAt(file_hdr_offset_, OffsetType::RAW);
        field_offset_= file_hdr_offset_;
        field_index_ = Fields::MACHINE;
        field_type_ = FieldType::WORD;

        UpdateCharacteristics();
    }

    std::string FileHdrWrapper::GetFieldName() const
    {
        switch (field_index_)
        {
            case Fields::MACHINE:            return "Machine";
            case Fields::SEC_NUM:            return "Sections Count";
            case Fields::TIMESTAMP:          return "Time Date Stamp";
            case Fields::SYMBOL_PTR:         return "Ptr to Symbol Table";
            case Fields::SYMBOL_NUM:         return "Num of Symbols";
            case Fields::OPTHDR_SIZE:        return "Size of OptionalHeader";
            case Fields::CHARACTERISTICS:    return "Characteristics";
            default:                         return "Unknown";
        }
    }

    BYTE* FileHdrWrapper::GetFieldValue() const
    {
        return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
    }

    std::string FileHdrWrapper::GetFieldDescription() const
    {
        switch (field_index_)
        {
            case Fields::MACHINE:      return MachineDescription();
            case Fields::TIMESTAMP:    return PEUtils::TimeDateStampConverter(file_hdr_->TimeDateStamp);
            default:                   return std::string();
        }
    }

    bool FileHdrWrapper::IsFieldDescribed() const
    {
        if (field_index_ == Fields::MACHINE || field_index_ == Fields::TIMESTAMP)
            return true;

        return false;
    }

    std::string FileHdrWrapper::MachineDescription() const
    {
        switch (file_hdr_->Machine)
        {
            case IMAGE_FILE_MACHINE_TARGET_HOST:    return "Target Host";
            case IMAGE_FILE_MACHINE_I386:           return "Intel 386";
            case IMAGE_FILE_MACHINE_R3000:          return "MIPS little-endian";
            case IMAGE_FILE_MACHINE_R4000:          return "MIPS little-endian";
            case IMAGE_FILE_MACHINE_R10000:         return "MIPS little-endian";
            case IMAGE_FILE_MACHINE_WCEMIPSV2:      return "MIPS little-endian WCE v2";
            case IMAGE_FILE_MACHINE_ALPHA:          return "Alpha_AXP";
            case IMAGE_FILE_MACHINE_SH3:            return "SH3 little-endian";
            case IMAGE_FILE_MACHINE_SH3DSP:         return "SH3DSP";
            case IMAGE_FILE_MACHINE_SH3E:           return "SH3E little-endian";
            case IMAGE_FILE_MACHINE_SH4:            return "SH4 little-endian";
            case IMAGE_FILE_MACHINE_SH5:            return "SH5";
            case IMAGE_FILE_MACHINE_ARM:            return "ARM Little-Endian";
            case IMAGE_FILE_MACHINE_THUMB:          return "ARM Thumb/Thumb-2 Little-Endian";
            case IMAGE_FILE_MACHINE_ARMNT:          return "ARM Thumb-2 Little-Endian";
            case IMAGE_FILE_MACHINE_AM33:           return "TAM33BD";
            case IMAGE_FILE_MACHINE_POWERPC:        return "IBM PowerPC Little-Endian";
            case IMAGE_FILE_MACHINE_POWERPCFP:      return "POWERPCFP";
            case IMAGE_FILE_MACHINE_IA64:           return "Intel 64";
            case IMAGE_FILE_MACHINE_MIPS16:         return "MIPS";
            case IMAGE_FILE_MACHINE_ALPHA64:        return "ALPHA64";
            case IMAGE_FILE_MACHINE_MIPSFPU:        return "MIPS";
            case IMAGE_FILE_MACHINE_MIPSFPU16:      return "MIPS";
            //case IMAGE_FILE_MACHINE_AXP64:        return "AXP64";
            case IMAGE_FILE_MACHINE_TRICORE:        return "Infineon";
            case IMAGE_FILE_MACHINE_CEF:            return "CEF";
            case IMAGE_FILE_MACHINE_EBC:            return "EFI Byte Code";
            case IMAGE_FILE_MACHINE_AMD64:          return "AMD64 (K8)";
            case IMAGE_FILE_MACHINE_M32R:           return "M32R little-endian";
            case IMAGE_FILE_MACHINE_ARM64:          return "ARM64 Little-Endian";
            case IMAGE_FILE_MACHINE_CEE:            return "CEE";
            default:                                return "Unknown";
        }
    }

    void FileHdrWrapper::UpdateCharacteristics()
    {
        if(!characteristics_.empty())
            characteristics_.clear();

        WORD characteristics = file_hdr_->Characteristics;

        if (characteristics & IMAGE_FILE_RELOCS_STRIPPED)
            characteristics_[IMAGE_FILE_RELOCS_STRIPPED] = "Relocation info stripped from file";

        if (characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)
            characteristics_[IMAGE_FILE_EXECUTABLE_IMAGE] = "File is executable";

        if (characteristics & IMAGE_FILE_LINE_NUMS_STRIPPED)
            characteristics_[IMAGE_FILE_LINE_NUMS_STRIPPED] = "COFF line numbers stripped";

        if (characteristics & IMAGE_FILE_LOCAL_SYMS_STRIPPED)
            characteristics_[IMAGE_FILE_LOCAL_SYMS_STRIPPED] = "COFF local symbols stripped";

        if (characteristics & IMAGE_FILE_AGGRESIVE_WS_TRIM)
            characteristics_[IMAGE_FILE_AGGRESIVE_WS_TRIM] = "Aggressively trim working set";

        if (characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE)
            characteristics_[IMAGE_FILE_LARGE_ADDRESS_AWARE] = "Application can handle > 2GB addresses";

        if (characteristics & IMAGE_FILE_BYTES_REVERSED_LO)
            characteristics_[IMAGE_FILE_BYTES_REVERSED_LO] = "LE: LSB precedes MSB (deprecated)";

        if (characteristics & IMAGE_FILE_32BIT_MACHINE)
            characteristics_[IMAGE_FILE_32BIT_MACHINE] = "32bit word machine";

        if (characteristics & IMAGE_FILE_DEBUG_STRIPPED)
            characteristics_[IMAGE_FILE_DEBUG_STRIPPED] = "Debugging stripped from file in .DBG";

        if (characteristics & IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP)
            characteristics_[IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP] = "If image on removable media, load it and copy to swap";

        if (characteristics & IMAGE_FILE_NET_RUN_FROM_SWAP)
            characteristics_[IMAGE_FILE_NET_RUN_FROM_SWAP] =  "If image on network media, load it and copy to swap";

        if (characteristics & IMAGE_FILE_SYSTEM)
            characteristics_[IMAGE_FILE_SYSTEM] = "System file, not a user program";

        if (characteristics & IMAGE_FILE_DLL)
            characteristics_[IMAGE_FILE_DLL] = "File is a (DLL)";

        if (characteristics & IMAGE_FILE_UP_SYSTEM_ONLY)
            characteristics_[IMAGE_FILE_UP_SYSTEM_ONLY] = "Run only on uniprocessor machine";

        if (characteristics & IMAGE_FILE_BYTES_REVERSED_HI)
            characteristics_[IMAGE_FILE_BYTES_REVERSED_HI] = "BE: MSB precedes LSB (deprecated)";
    }

    void FileHdrWrapper::LoadNextField()
    {
        if (field_type_ == FieldType::WORD)
            field_offset_ += sizeof(WORD);
        else if (field_type_ == FieldType::DWORD)
            field_offset_ += sizeof(DWORD);

        field_index_++;
        if (field_index_ == Fields::TIMESTAMP || field_index_ == Fields::SYMBOL_PTR || field_index_ == Fields::SYMBOL_NUM)
            field_type_ = FieldType::DWORD;
        else
            field_type_ = FieldType::WORD;
    }

    void FileHdrWrapper::Reset()
    {
        field_offset_ = file_hdr_offset_;
        field_index_ = Fields::MACHINE;
        field_type_ = FieldType::WORD;
    }
}
