#pragma once
#include <PEFormat.h>
#include <PewTypes.h>

#include <string>
#include <map>

namespace PewParser {

    class PEFile;

    class OptionalHdrWrapper
    {
    public:
        enum Fields
        {
            MAGIC = 0,
            LINKER_MAJOR,
            LINKER_MINOR,
            CODE_SIZE,
            INITDATA_SIZE,
            UNINITDATA_SIZE,
            EP,
            CODE_BASE,
            DATA_BASE,
            IMAGE_BASE,
            SEC_ALIGN,
            FILE_ALIGN,
            OSVER_MAJOR,
            OSVER_MINOR,
            IMGVER_MAJOR,
            IMGVER_MINOR,
            SUBSYSVER_MAJOR,
            SUBSYSVER_MINOR,
            WIN32_VER,
            IMAGE_SIZE,
            HDRS_SIZE,
            CHECKSUM,
            SUBSYS,
            DLL_CHARACT,
            STACK_RSRV_SIZE,
            STACK_COMMIT_SIZE,
            HEAP_RSRV_SIZE,
            HEAP_COMMIT_SIZE,
            LDR_FLAGS,
            RVAS_SIZES_NUM,
            DATA_DIR,
            FIELDS_COUNT
        };
    public:
        OptionalHdrWrapper(PEFile* pe);

        FieldOffset GetFieldOffset() const { return field_offset_; }
        std::string_view GetFieldName() const;
        BYTE* GetFieldValue() const;
        std::string_view GetFieldDescription() const;
        FieldType GetFieldType() const { return field_type_; }
        size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }

        bool IsFieldDescribed() const;

        const auto& GetDllCharacteristics() const { return dll_characteristics_; }
        void UpdateDllCharacteristics();

        std::string_view GetDataDirEntryName() const;
        size_t GetDataDirEntriesCount() const { return DataDirEntries::DATA_DIR_ENTRIES_COUNT; }

        IMAGE_DATA_DIRECTORY* GetDataDir() const;

        void LoadNextField();
        void Reset();

        void* GetOptionalHdr() const;
        offset_t GetOptionalHdrOffset() const { return optional_hdr_offset_; }
        size_t GetOptionalHdrSize() const;
        OptHdrType GetOptionalHdrType() const;
    private:
        std::string_view MagicDescription() const;
        std::string_view SubsystemDescription() const;
    private:
        IMAGE_OPTIONAL_HEADER32* optional_hdr32_;
        IMAGE_OPTIONAL_HEADER64* optional_hdr64_;
        offset_t optional_hdr_offset_;

        FieldOffset field_offset_;
        FieldIndex field_index_;
        FieldType field_type_;

        index_t data_dir_entry_;

        std::map<WORD, std::string_view> dll_characteristics_;

        PEFile* related_pe_;
    };
}