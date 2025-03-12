#include "Terminal.h"
#include "Platform.h"
#include "Log.h"
#include "Commands.h"

#ifdef _WIN32
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <Windows.h>
#endif

namespace PewParser {

    Terminal::Terminal()
    {
        Init();
        std::cout << "\n\n";
    }

    void Terminal::Init()
    {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);

        DWORD out_mode = 0;
        HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

        if (stdout_handle == INVALID_HANDLE_VALUE)
            exit(GetLastError());

        if(!GetConsoleMode(stdout_handle, &out_mode))
            exit(GetLastError());

        DWORD out_mode_init = out_mode;

        out_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        if(!SetConsoleMode(stdout_handle, out_mode))
            exit(GetLastError());
#endif
    }

    void Terminal::SetInputMode(InputModes mode)
    {
#ifdef _WIN32
        if (mode == InputModes::ASCII)
            _setmode(_fileno(stdin), _O_TEXT);
        else if (mode == InputModes::UTF8)
            _setmode(_fileno(stdin), _O_U8TEXT);
        else if (mode == InputModes::UTF16)
            _setmode(_fileno(stdin), _O_U16TEXT);
#endif
    }

    void Terminal::NewFilepath()
    {
        PewPath path;

        SetInputMode(InputModes::UTF16);
        PEW_LOG(Logger::Color::YELLOW, true, "Filepath: ");
        std::getline(PewCin, path);
        SetInputMode(InputModes::ASCII);

        filepath_ = path;
    }

    void Terminal::PrintFilepath()
    {
        PEW_LOG(Logger::Color::YELLOW, true, "Filepath: ");
        PEW_LOG(Logger::Color::NONE, false, "%s\n\n", filepath_.u8string().c_str());
    }

    void Terminal::Listen(PEFile* pe)
    {
        Commands commands(pe);
        commands.Listen();
    }

}