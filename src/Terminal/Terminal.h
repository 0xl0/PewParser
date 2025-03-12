#pragma once
#include "Log.h"
#include <PEFile.h>

#include <filesystem>

namespace PewParser {

    class Terminal
    {
    public:
        enum class InputModes
        {
            ASCII = 0,
            UTF8,
            UTF16
        };

        Terminal();

        void Listen(PEFile* pe);

        void SetInputMode(InputModes mode);

        void PrintFilepath();

        void NewFilepath();
        void SetFilepath(const std::filesystem::path& path) { filepath_ = path; }
        std::filesystem::path GetFilepath() { return filepath_; }
    private:
        void Init();
    private:
        std::filesystem::path filepath_;
    };

}