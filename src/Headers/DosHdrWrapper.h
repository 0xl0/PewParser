#pragma once
#include <PEFormat.h>
#include <PewTypes.h>

#include <string>

namespace PewParser {

    class PEFile;

    class DosHdrWrapper
    {
    public:
        enum Fields {
            MAGIC = 0,
            CBLP,
            CP,
            CRLC,
            CPARHDR,
            MINALLOC,
            MAXALLOC,
            SS,
            SP,
            CSUM,
            IP,
            CS,
            LFARLC,
            OVNO,
            RES,
            OEMID,
            OEMINFO,
            RES2,
            LFANEW,
            FIELDS_COUNT
        };
    public:
        DosHdrWrapper(PEFile* pe);

        FieldOffset GetFieldOffset() const { return field_offset_; }
        std::string_view GetFieldName() const;
        BYTE* GetFieldValue() const;
        size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }
        
        void LoadNextField();
        void Reset();

        offset_t GetNtHdrsOffset() const { return dos_hdr_->e_lfanew; }

        IMAGE_DOS_HEADER* GetDosHdr() const { return dos_hdr_; }
        size_t GetDosHdrSize() const { return sizeof(IMAGE_DOS_HEADER); }
    private:
        IMAGE_DOS_HEADER* dos_hdr_;

        FieldOffset field_offset_;
        FieldIndex field_index_;

        PEFile* related_pe_;
    };
}