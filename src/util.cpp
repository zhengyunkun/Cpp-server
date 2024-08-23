#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void errIf(bool condition, const char* msg)
{
    if (condition)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}