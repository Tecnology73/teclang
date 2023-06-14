#pragma once

#include <string>

struct Color {
    int R;
    int G;
    int B;

    std::string ToString() const {
        return std::to_string(R) + ";" + std::to_string(G) + ";" + std::to_string(B);
    }
};

struct Console {
    static const std::string EscapeCode;
    static const std::string ResetCode;

    static const unsigned Reset = 0;
    static const unsigned Bold = 1;
    static const unsigned Dim = 2;
    static const unsigned Underscore = 4;
    static const unsigned Blink = 5;
    static const unsigned Reverse = 7;
    static const unsigned Hidden = 8;

    constexpr static const Color Black{0, 0, 0};
    constexpr static const Color Red{255, 83, 112};
    constexpr static const Color Green{195, 232, 141};
    constexpr static const Color Yellow{255, 203, 107};
    constexpr static const Color Blue{130, 170, 255};
    constexpr static const Color Magenta{199, 146, 234};
    constexpr static const Color Cyan{137, 221, 255};
    constexpr static const Color White{248, 248, 248};
    constexpr static const Color Orange{244, 138, 107};
    constexpr static const Color Lime{195, 232, 141};

    static std::string PrintColoredType(const std::string &type);

    static void Print(const std::string &str);
};
