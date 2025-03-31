#pragma once
#include <PEFile.h>
#include <PewTypes.h>

namespace PewParser {

    class ResourceDirWrapper
    {
    public:
        enum Fields {
            CHARACTERISTICS = 0,
            TIMESTAMP,
            MAJOR_VER,
            MINOR_VER,
            NAMED_ENTRIES_NUM,
            ID_ENTRIES_NUM,
            FIELDS_COUNT
        };

        enum RT
        {
            CURSOR = 1,
            BITMAP = 2,
            ICON = 3,
            MENU = 4,
            DIALOG = 5,
            STRING = 6,
            FONT_DIR = 7,
            FONT = 8,
            ACCELERATOR = 9,
            RCDATA = 10,
            MESSAGETABLE = 11,
            GROUP_CURSOR = 12,
            GROUP_ICON = 14,
            VERSION = 16,
            DLGINCLUDE = 17,
            PLUGPLAY = 19,
            VXD = 20,
            ANICURSOR = 21,
            ANIICON = 22,
            HTML = 23,
            MANIFEST = 24
        };

        enum TreeLevel
        {
            ROOT = 0,
            TYPE,
            ID,
            LANGUAGE
        };
    public:
        ResourceDirWrapper(PEFile* pe);

        FieldOffset GetFieldOffset() const { return field_offset_; }
        std::string_view GetFieldName() const;
        BYTE* GetFieldValue() const;
        std::string GetFieldDescription() const;
        FieldType GetFieldType() const { return field_type_; }
        size_t GetFieldsCount() const { return Fields::FIELDS_COUNT; }

        bool IsFieldDescribed() const;

        void LoadNextField();
        void Reset();

        bool IsString() const;
        bool IsDirectory() const;

        std::string GetName() const;
        std::string_view GetType() const;
        DWORD GetNameValue() const;
        offset_t GetNameOffset() const;
        DWORD GetDataValue() const;
        offset_t GetDataOffset() const;
        WORD GetId() const;

        void SetCurrentEntry(index_t entry_index) { current_entry_ = entry_index; }
        void LoadNextEntry() { current_entry_++; }
        void ClearEntry() { current_entry_ = 0; }

        void SetTreeLevel(uint32_t level) { current_tree_level_ = level; }
        uint32_t GetTreeLevel() const { return current_tree_level_; }
        void LoadNextTreeLevel() { current_tree_level_++; }
        void ClearTreeLevel() { current_tree_level_ = 0; }

        size_t GetEntriesCount() const;
        size_t GetEntriesCount(index_t entry_index) const;

        void SetCurrentRsrcDir(IMAGE_RESOURCE_DIRECTORY* rsrc_dir) { current_rsrc_dir_ = rsrc_dir; }

        offset_t GetDataEntryOffset();
        offset_t GetSelectedDataEntryOffset() const { return selected_data_entry_offset_; };
        void SetSelectedDataEntryOffset(offset_t offset);

        bool IsValidWrapper() const;

        IMAGE_RESOURCE_DIRECTORY* GetRootRsrcDir() const { return root_rsrc_dir_; }
        IMAGE_RESOURCE_DIRECTORY* GetCurrentRsrcDir() const { return current_rsrc_dir_; }
        IMAGE_RESOURCE_DIRECTORY* GetEntryRsrcDir(index_t entry_index) const;
        offset_t GetRootRsrcDirOffset() const { return root_rsrc_dir_offset_; }
        size_t GetRsrcDirSize() const { return sizeof(IMAGE_RESOURCE_DIRECTORY); }
    private:
        void Init();
    private:
        IMAGE_RESOURCE_DIRECTORY* root_rsrc_dir_;
        IMAGE_RESOURCE_DIRECTORY* current_rsrc_dir_;
        offset_t root_rsrc_dir_offset_;

        FieldOffset field_offset_;
        FieldIndex field_index_;
        FieldType field_type_;

        index_t current_entry_;
        uint32_t current_tree_level_;
        offset_t selected_data_entry_offset_;

        PEFile* related_pe_;
    };

}