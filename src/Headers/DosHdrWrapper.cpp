#include "DosHdrWrapper.h"

#include <PEFile.h>

namespace PewParser {

    DosHdrWrapper::DosHdrWrapper(PEFile* pe)
        : related_pe_(pe)
    {
        dos_hdr_ = (IMAGE_DOS_HEADER*)pe->GetRawFile().Buffer();
        field_offset_ = 0;
        field_index_ = Fields::MAGIC;
    }

    std::string DosHdrWrapper::GetFieldName() const
    {
        switch (field_index_)
        {
            case Fields::MAGIC:       return "Magic Number";
            case Fields::CBLP:        return "Bytes on last page of file";
            case Fields::CP:          return "Pages in file";
            case Fields::CRLC:        return "Relocations";
            case Fields::CPARHDR:     return "Size of header in paragraphs";
            case Fields::MINALLOC:    return "Minimum extra paragraphs needed";
            case Fields::MAXALLOC:    return "Maximum extra paragraphs needed";
            case Fields::SS:          return "Initial (relative) SS value";
            case Fields::SP:          return "Initial SP value";
            case Fields::CSUM:        return "Checksum";
            case Fields::IP:          return "Initial IP value";
            case Fields::CS:          return "Initial (relative) CS value";
            case Fields::LFARLC:      return "File address of relocation table";
            case Fields::OVNO:        return "Overlay number";
            case Fields::RES:         return "Reserved words[4]";
            case Fields::OEMID:       return "OEM identifier (for e_oeminfo)";
            case Fields::OEMINFO:     return "OEM information; e_oemid specific";
            case Fields::RES2:        return "Reserved words[10]";
            case Fields::LFANEW:      return "Offset to extended header";
            default:                  return "Unknown";
        };
    }

    BYTE* DosHdrWrapper::GetFieldValue() const
    {
        return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
    }

    void DosHdrWrapper::LoadNextField()
    {
        if (field_index_ == Fields::RES)
            field_offset_ += sizeof(dos_hdr_->e_res);
        else if (field_index_ == Fields::RES2)
            field_offset_ += sizeof(dos_hdr_->e_res2);
        else if (field_index_ == Fields::LFANEW)
            field_offset_ += sizeof(dos_hdr_->e_lfanew);
        else
            field_offset_ += sizeof(WORD);

        field_index_++;
    }

    void DosHdrWrapper::Reset()
    {
        field_offset_ = 0;
        field_index_ = Fields::MAGIC;
    }
}