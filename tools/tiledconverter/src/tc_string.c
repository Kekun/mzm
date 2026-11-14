#include <ctype.h>
#include <string.h>
#include "tc_string.h"

char* tcStrlwr(char* str)
{
    for (char* p = str; *p; p++)
    {
        *p = tolower(*p);
    }

    return str;
}
