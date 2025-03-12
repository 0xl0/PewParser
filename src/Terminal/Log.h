#pragma once
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <cstdint>

namespace PewParser {

    class Logger
    {
    public:
        enum class Level
        {
            Info,
            Warn,
            Error,
            Fatal
        };

        enum class Color
        {
            NONE = -1,
            BLACK,
            RED,
            GREEN,
            YELLOW,
            BLUE,
            MAGENTA,
            CYAN,
            WHITE,
            BRIGHT_BLACK,
            BRIGHT_RED,
            BRIGHT_GREEN,
            BRIGHT_YELLOW,
            BRIGHT_BLUE,
            BRIGHT_MAGENTA,
            BRIGHT_CYAN,
            BRIGHT_WHITE,
        };

        enum CustomPEColors
        {
            RAW = 2,
            RVA = 33,
            IMAGE_BASE = 93,
            INVALID_VALUE = 160,
            TABLE_HEADER = 236,
            COLUMN_EVEN = 254,
            COLUMN_ODD = 250,
            DISABLED_COLUMN = 52,
            DESCRIPTION = 57,
            RSRC_ENTRY_ARROW = 166,
            LONG_STR_DOTS = 20
        };
    public:
        static std::string TextColor(Color color)
        {
            switch (color)
            {
                case Color::BLACK:                return "\x1b[30m";
                case Color::RED:                return "\x1b[31m";
                case Color::GREEN:                return "\x1b[32m";
                case Color::YELLOW:                return "\x1b[33m";
                case Color::BLUE:                return "\x1b[34m";
                case Color::MAGENTA:            return "\x1b[35m";
                case Color::CYAN:                return "\x1b[36m";
                case Color::WHITE:                return "\x1b[37m";
                case Color::BRIGHT_BLACK:        return "\x1b[90m";
                case Color::BRIGHT_RED:            return "\x1b[91m";
                case Color::BRIGHT_GREEN:        return "\x1b[92m";
                case Color::BRIGHT_YELLOW:        return "\x1b[93m";
                case Color::BRIGHT_BLUE:        return "\x1b[94m";
                case Color::BRIGHT_MAGENTA:        return "\x1b[95m";
                case Color::BRIGHT_CYAN:        return "\x1b[96m";
                case Color::BRIGHT_WHITE:        return "\x1b[97m";
                default:                        return std::string();
            }
        }
        static std::string BgColor(Color color)
        {
            switch (color)
            {
                case Color::BLACK:                return "\x1b[40m";
                case Color::RED:                return "\x1b[41m";
                case Color::GREEN:                return "\x1b[42m";
                case Color::YELLOW:                return "\x1b[43m";
                case Color::BLUE:                return "\x1b[44m";
                case Color::MAGENTA:            return "\x1b[45m";
                case Color::CYAN:                return "\x1b[46m";
                case Color::WHITE:                return "\x1b[47m";
                case Color::BRIGHT_BLACK:        return "\x1b[100m";
                case Color::BRIGHT_RED:            return "\x1b[101m";
                case Color::BRIGHT_GREEN:        return "\x1b[102m";
                case Color::BRIGHT_YELLOW:        return "\x1b[103m";
                case Color::BRIGHT_BLUE:        return "\x1b[104m";
                case Color::BRIGHT_MAGENTA:        return "\x1b[105m";
                case Color::BRIGHT_CYAN:        return "\x1b[106m";
                case Color::BRIGHT_WHITE:        return "\x1b[107m";
                default:                        return std::string();
            }
        }
        static std::string CustomTextColor(uint32_t color_id)
        {
            if (color_id > 255)
                return std::string();

            char fmt[24] = {};
            sprintf(fmt, "\x1b[38;5;%dm", color_id);
            return fmt;
        }
        static std::string CustomBgColor(uint32_t color_id)
        {
            if (color_id > 255)
                return std::string();

            char fmt[24] = {};
            sprintf(fmt, "\x1b[48;5;%dm", color_id);
            return fmt;
        }
        static std::string ResetColor()
        {
            return "\x1b[0m";
        }

        template<typename... Args>
        static void Log(Color color, bool display_time, const char* fmt, Args&&... args)
        {
            std::stringstream format;

            if (color != Color::NONE)
                format << TextColor(color);

            if (display_time)
                format << "[" << GetTime() << "] ";

            format << fmt;

            if (color != Color::NONE)
                format << ResetColor();

            std::printf(format.str().c_str(), std::forward<Args&&>(args)...);
        }

        template<typename... Args>
        static void Log(const char* prefix, Color color, const char* fmt, Args&&... args)
        {
            char fmt_buffer[64] = {};

            std::sprintf(fmt_buffer, "%s[%s] %s %s%s", TextColor(color).c_str(), GetTime().c_str(), prefix, fmt, ResetColor().c_str());
            std::printf(fmt_buffer, std::forward<Args&&>(args)...);
        }

        template<typename... Args>
        static void Log(Level lvl, const char* fmt, Args&&... args)
        {
            if (lvl == Level::Info)
                Log("INFO:", Color::NONE, fmt, std::forward<Args&&>(args)...);
            else if (lvl == Level::Warn)
                Log("WARN:", Color::YELLOW, fmt, std::forward<Args&&>(args)...);
            else if (lvl == Level::Error)
                Log("ERROR:", Color::RED, fmt, std::forward<Args&&>(args)...);
            else if (lvl == Level::Fatal)
                Log("FATAL:", Color::RED, fmt, std::forward<Args&&>(args)...);
        }

    private:
        static std::string GetTime()
        {
            time_t t = time(NULL);
            tm* local = localtime(&t);

            char buffer[32] = { 0 };
            strftime(buffer, 32, "%T", local);

            return std::string(buffer);
        }
    };

#define PEW_LOG(color, bTime, fmt, ...) Logger::Log(color, bTime, fmt, ##__VA_ARGS__)
#define PEW_INFO(fmt, ...) Logger::Log(Logger::Level::Info, fmt, ##__VA_ARGS__)
#define PEW_WARN(fmt, ...) Logger::Log(Logger::Level::Warn, fmt, ##__VA_ARGS__)
#define PEW_ERROR(fmt, ...) Logger::Log(Logger::Level::Error, fmt, ##__VA_ARGS__)
#define PEW_FATAL(fmt, ...) Logger::Log(Logger::Level::Fatal, fmt, ##__VA_ARGS__)
}
