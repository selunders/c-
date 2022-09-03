#include <stdio.h>

enum class PRINTCOLOR
{
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    PURPLE,
    CYAN,
    WHITE,
};

void setPrintColor(PRINTCOLOR color)
{
    switch (color)
    {
        case PRINTCOLOR::BLACK:
            printf("\033[0;30m");
        break;

        case PRINTCOLOR::RED:
            printf("\033[0;31m");
        break;

        case PRINTCOLOR::GREEN:
            printf("\033[0;32m");
        break;

        case PRINTCOLOR::YELLOW:
            printf("\033[0;33m");
        break;

        case PRINTCOLOR::BLUE:
            printf("\033[0;34m");
        break;

        case PRINTCOLOR::PURPLE:
            printf("\033[0;35m");
        break;

        case PRINTCOLOR::CYAN:
            printf("\033[0;36m");
        break;

        case PRINTCOLOR::WHITE:
            printf("\033[0;37m");
        break;
    }
}

void resetPrintColor()
{
    printf("\033[0m");
}