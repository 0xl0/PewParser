#pragma once
#include <PEFormat.h>
#include <PewTypes.h>

#include <string>
#include <map>

namespace PewParser {

    class PEFile;

    class FileHdrWrapper
    {
    public:
        enum Fields {
            MACHINE = 0,
            SEC_NUM,
            TIMESTAMP,
            SYMBOL_PTR,
            SYMBOL_NUM,
            OPTHDR_SIZE,
            CHARACTERISTICS,
            FIELDS_COUNT
        };
    public:
        FileHdrWrapper(PEFile* pe);

        FieldOffset GetFieldOffset() const { return field_offset_; }
        std::string GetFieldName() const;
        BYTE* GetFieldValue() const;
        std::string GetFieldDescription() const;
        FieldType GetFieldType() const { return field_type_; }
        size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }

        bool IsFieldDescribed() const;

        const auto& GetCharacteristics() const { return characteristics_; }
        void UpdateCharacteristics();

        void LoadNextField();
        void Reset();

        size_t GetOptionalHdrSize() const { return file_hdr_->SizeOfOptionalHeader; }
        size_t GetNumOfSctions() const { return file_hdr_->NumberOfSections; }

        IMAGE_FILE_HEADER* GetFileHdr() const { return file_hdr_; }
        offset_t GetFileHdrOffset() const { return file_hdr_offset_; }
        size_t GetFileHdrSize() const { return sizeof(IMAGE_FILE_HEADER); }
    private:
        std::string MachineDescription() const;
    private:
        IMAGE_FILE_HEADER* file_hdr_;
        offset_t file_hdr_offset_;

        FieldOffset field_offset_;
        FieldIndex field_index_;
        FieldType field_type_;

        std::map<WORD, std::string_view> characteristics_;

        PEFile* related_pe_;
    };
}