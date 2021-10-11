#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "Txlib.h"

#define SET_DEF_LEVEL 2

int main()
{
    error_t error = NO_ERROR;

    stack_t st = {};

    error = Stack_Ctor(&st);
    error = Error_Log(error);

    error = Stack_Interface(&st);
    error = Error_Log(error);

    error = Stack_Dtor(&st);
    error = Error_Log(error);
}
