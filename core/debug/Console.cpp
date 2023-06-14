#include <iostream>
#include "Console.h"

// const std::string Console::EscapeCode = "\x1b[";
const std::string Console::EscapeCode = "\u001b[38;2;";
const std::string Console::ResetCode = "\u001b[0;00m";

std::string Console::PrintColoredType(const std::string &type) {
    if (type == "bool" || type == "int" || type == "double" || type == "string")
        return "%m" + type + "%/";

    return "%y" + type + "%/";
}

void Console::Print(const std::string &str) {
    std::string tmp = str;
    size_t strPos = 0;
    while (strPos < tmp.size()) {
        if (tmp[strPos] != '%') {
            strPos++;
            continue;
        }

        unsigned startPos = strPos;
        Color color = White;

        switch (tmp[++strPos]) {
            case 'n':
                color = Black;
                break;
            case 'r':
                color = Red;
                break;
            case 'g':
                color = Green;
                break;
            case 'y':
                color = Yellow;
                break;
            case 'b':
                color = Blue;
                break;
            case 'm':
                color = Magenta;
                break;
            case 'c':
                color = Cyan;
                break;
            case 'w':
                color = White;
                break;
            case 'o':
                color = Orange;
                break;
            case 'l':
                color = Lime;
                break;
            case '/':
                tmp.replace(startPos, strPos + 1 - startPos, ResetCode);
                strPos += ResetCode.size() - 1;
                continue;
        }

        std::string colorCode = EscapeCode + color.ToString() + 'm';
        tmp.replace(startPos, strPos + 1 - startPos, colorCode);
        strPos += colorCode.size();
    }

    std::cout << tmp << std::endl;
}