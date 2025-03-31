#include "ResourceDirWrapper.h"

#include <PEUtils.h>

#if defined(__linux__)
#include <iconv.h>
#elif defined(_WIN32)
#include <Windows.h>
#endif

namespace PewParser {

    std::string Utf16ToUtf8(WCHAR* src, int len)
    {
#if defined(__linux__)
        size_t src_len = len * sizeof(WCHAR);
        size_t dst_len = 256;
        char dst[dst_len] = {0};

        char* pIn = (char*)src;
        char* pOut = dst;

        iconv_t conv = iconv_open("UTF-8", "UTF-16");
        iconv(conv, &pIn, &src_len, &pOut, &dst_len);
        iconv_close(conv);
#elif defined(_WIN32)
        if (!src) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, src, len, NULL, 0, NULL, NULL);
        std::string dst(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, src, len, &dst[0], size_needed, NULL, NULL);
#endif
        return dst;
    }

    ResourceDirWrapper::ResourceDirWrapper(PEFile* pe)
        : related_pe_(pe), root_rsrc_dir_(nullptr), current_rsrc_dir_(nullptr), root_rsrc_dir_offset_(0), current_entry_(0), current_tree_level_(0)
    {
        Init();

        field_offset_ = root_rsrc_dir_offset_;
        field_index_ = Fields::CHARACTERISTICS;
        field_type_ = FieldType::DWORD;
    }

    void ResourceDirWrapper::Init()
    {
        offset_t resource_dir_rva = related_pe_->GetDataDirectory()[DataDirEntries::RSRC].VirtualAddress;
        offset_t resource_dir_raw = related_pe_->RvaToRaw(resource_dir_rva);

        if (resource_dir_raw)
        {
            root_rsrc_dir_ = (IMAGE_RESOURCE_DIRECTORY*)related_pe_->GetContentAt(resource_dir_raw, OffsetType::RAW);
            current_rsrc_dir_ = root_rsrc_dir_;
            root_rsrc_dir_offset_ = resource_dir_raw;
        }
        else if ((resource_dir_rva + GetRsrcDirSize()) <= related_pe_->GetRawFileSize())
        {
            root_rsrc_dir_ = (IMAGE_RESOURCE_DIRECTORY*)related_pe_->GetContentAt(resource_dir_rva, OffsetType::RAW);
            current_rsrc_dir_ = root_rsrc_dir_;
            root_rsrc_dir_offset_ = resource_dir_rva;
        }
    }

    std::string_view ResourceDirWrapper::GetFieldName() const
    {
        switch (field_index_)
        {
            case Fields::CHARACTERISTICS:      return "Characteristics";
            case Fields::TIMESTAMP:            return "TimeDateStamp";
            case Fields::MAJOR_VER:            return "MajorVersion";
            case Fields::MINOR_VER:            return "MinorVersion";
            case Fields::NAMED_ENTRIES_NUM:    return "NumberOfNamedEntries";
            case Fields::ID_ENTRIES_NUM:       return "NumberOfIdEntries";
            default:                           return "UnKnown";
        }
    }

    BYTE* ResourceDirWrapper::GetFieldValue() const
    {
        return related_pe_->GetContentAt(field_offset_, OffsetType::RAW);
    }

    std::string ResourceDirWrapper::GetFieldDescription() const
    {
        switch (field_index_)
        {
            case Fields::TIMESTAMP:    return PEUtils::TimeDateStampConverter(root_rsrc_dir_->TimeDateStamp);
            default:                   return std::string();
        }
    }

    bool ResourceDirWrapper::IsFieldDescribed() const
    {
        if (field_index_ == Fields::TIMESTAMP)
            return true;

        return false;
    }

    offset_t ResourceDirWrapper::GetDataEntryOffset()
    {
        if (!IsDirectory())
        {
            IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize());
            return (root_rsrc_dir_offset_ + entry->OffsetToData);
        }

        current_rsrc_dir_ = GetEntryRsrcDir(0);
        return GetDataEntryOffset();
    }

    void ResourceDirWrapper::SetSelectedDataEntryOffset(offset_t offset)
    {
        selected_data_entry_offset_ = offset;
    }

    bool ResourceDirWrapper::IsDirectory() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));

        return (entry->DataIsDirectory == 1);
    }

    bool ResourceDirWrapper::IsString() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));

        return (entry->NameIsString == 1);
    }

    std::string ResourceDirWrapper::GetName() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));

        IMAGE_RESOURCE_DIR_STRING_U* str_u = (_IMAGE_RESOURCE_DIR_STRING_U*)related_pe_->GetContentAt(root_rsrc_dir_offset_ + entry->NameOffset, OffsetType::RAW);

        std::string name = Utf16ToUtf8(str_u->NameString, str_u->Length);

        return name;
    }


    std::string_view ResourceDirWrapper::GetType() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));

        switch (entry->Id)
        {
            case RT::CURSOR:          return "Cursor";
            case RT::BITMAP:          return "Bitmap";
            case RT::ICON:            return "Icon";
            case RT::MENU:            return "Menu";
            case RT::DIALOG:          return "Dialog";
            case RT::STRING:          return "String Table";
            case RT::FONT_DIR:        return "Font Directory";
            case RT::FONT:            return "Font";
            case RT::ACCELERATOR:     return "Accelerator";
            case RT::RCDATA:          return "RC Data";
            case RT::MESSAGETABLE:    return "Message Table";
            case RT::GROUP_CURSOR:    return "Cursor Group";
            case RT::GROUP_ICON:      return "Icon Group";
            case RT::VERSION:         return "Version";
            case RT::DLGINCLUDE:      return "Dlg Include";
            case RT::PLUGPLAY:        return "Plug & Play";
            case RT::VXD:             return "VXD";
            case RT::ANICURSOR:       return "Animated Cursor";
            case RT::ANIICON:         return "Animated Icon";
            case RT::HTML:            return "HTML";
            case RT::MANIFEST:        return "Manifest";
            default:                  return "UnKnown";
        }
    }

    DWORD ResourceDirWrapper::GetNameValue() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));

        return entry->Name;
    }

    offset_t ResourceDirWrapper::GetNameOffset() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));

        return root_rsrc_dir_offset_ + entry->NameOffset;
    }

    DWORD ResourceDirWrapper::GetDataValue() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));

        return entry->OffsetToData;
    }

    offset_t ResourceDirWrapper::GetDataOffset() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));

        return root_rsrc_dir_offset_ + entry->OffsetToDirectory;
    }

    WORD ResourceDirWrapper::GetId() const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * current_entry_));
        return entry->Id;
    }

    size_t ResourceDirWrapper::GetEntriesCount() const
    {
        if (!current_rsrc_dir_)
            return 0;

        size_t count = current_rsrc_dir_->NumberOfNamedEntries + current_rsrc_dir_->NumberOfIdEntries;

        return count;
    }

    size_t ResourceDirWrapper::GetEntriesCount(index_t entry_index) const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * entry_index));
        offset_t rsrc_dir_offset = root_rsrc_dir_offset_ + entry->OffsetToDirectory;
        IMAGE_RESOURCE_DIRECTORY* rsrc_dir = (IMAGE_RESOURCE_DIRECTORY*)related_pe_->GetContentAt(rsrc_dir_offset, OffsetType::RAW);
        if (!rsrc_dir)
            return 0;

        size_t count = rsrc_dir->NumberOfNamedEntries + rsrc_dir->NumberOfIdEntries;

        return count;
    }

    IMAGE_RESOURCE_DIRECTORY* ResourceDirWrapper::GetEntryRsrcDir(index_t entry_index) const
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY* entry = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)(current_rsrc_dir_) + GetRsrcDirSize() + (sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY) * entry_index));
        offset_t offset = root_rsrc_dir_offset_ + entry->OffsetToDirectory;
        IMAGE_RESOURCE_DIRECTORY* rsrc_dir = (IMAGE_RESOURCE_DIRECTORY*)related_pe_->GetContentAt(offset, OffsetType::RAW);
        return rsrc_dir;
    }

    bool ResourceDirWrapper::IsValidWrapper() const
    {
        if (root_rsrc_dir_)
            return true;

        return false;
    }

    void ResourceDirWrapper::LoadNextField()
    {
        if (field_type_ == FieldType::WORD)
            field_offset_ += sizeof(WORD);
        else if (field_type_ == FieldType::DWORD)
            field_offset_ += sizeof(DWORD);

        field_index_++;
        if (field_index_ == Fields::CHARACTERISTICS || field_index_ == Fields::TIMESTAMP)
            field_type_ = FieldType::DWORD;
        else
            field_type_ = FieldType::WORD;
    }

    void ResourceDirWrapper::Reset()
    {
        field_offset_ = root_rsrc_dir_offset_;
        field_index_ = Fields::CHARACTERISTICS;
        field_type_ = FieldType::DWORD;
    }

}