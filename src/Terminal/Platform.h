#pragma once
#include <iostream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

#if defined(_MSC_VER)
#define PewMain wmain
#define arg_t wchar_t
#define PewCin std::wcin

typedef std::wstring PewPath;
#else
#define PewMain main
#define arg_t char
#define PewCin std::cin

typedef std::string PewPath;
#endif