#pragma once
#include <PEFormat.h>
#include <PewTypes.h>

#include <string>
#include <map>

namespace PewParser {

    class PEFile;

    class SectionHdrsWrapper
    {
    public:
        enum Fields {
            NAME = 0,
            VIRTUAL_SIZE,
            VIRTUAL_PTR,
            RAW_SIZE,
            RAW_PTR,
            RELOC_PTR,
            LINE_NUM_PTR,
            RELOC_NUM,
            LINE_NUM_NUMS,
            CHARACTERISTICS,
            FIELDS_COUNT
        };

    public:
        SectionHdrsWrapper(PEFile* pe);

        FieldOffset GetFieldOffset() const { return field_offset_; }
        std::string GetFieldName() const;
        BYTE* GetFieldValue() const;
        FieldType GetFieldType() const { return field_type_; }
        size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }

        void SetCurrentSectionHdr(index_t section_index);

        std::string GetSectionName(index_t section_index) const;

        size_t GetNumOfSections() const;

        std::map<DWORD, std::string> GetCharacteristics(index_t section_index) const;

        void LoadNextField();
        void Reset();

        IMAGE_SECTION_HEADER* GetRootSectionHdr() const { return root_section_hdr_; }
        IMAGE_SECTION_HEADER* GetCurrentSectionHdr() const { return current_section_hdr_; }
        offset_t GetRootSectionHdrOffset() const { return root_section_hdr_offset_; }
        offset_t GetCurrentSectionHdrOffset() const { return current_section_hdr_offset_; }
        size_t GetSectionHdrSize() const { return sizeof(IMAGE_SECTION_HEADER); }
        size_t GetAllSectionsSize() const;
    private:
        IMAGE_SECTION_HEADER* root_section_hdr_;
        IMAGE_SECTION_HEADER* current_section_hdr_;
        offset_t root_section_hdr_offset_;
        offset_t current_section_hdr_offset_;

        FieldOffset field_offset_;
        FieldIndex field_index_;
        FieldType field_type_;

        PEFile* related_pe_;
    };
}