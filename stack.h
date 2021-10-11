#ifndef STACK_H
#define STACK_H

#include <stdio.h>


#define errprint(value) printf(#value"\n")

#define ptr_ver(ptr); if((ptr) == nullptr) \
                        return PTR_ERROR;

#ifndef DEFAULT_CAPACITY
    #define DEFAULT_CAPACITY 32
#endif

#ifndef SET_DEF_LEVEL
    #define SET_DEF_LEVEL 2
#endif

#ifndef LEFT_CANARY
    #define LEFT_CANARY 111111.111111
#endif

#ifndef RIGHT_CANARY
    #define RIGHT_CANARY 999999.999999
#endif

enum defence_level
{
    ONE   = 1,
    TWO   = 2,
    THREE = 3
};

enum loop_index
{
    STOP = 0,
    RUN  = 1
};

enum mem_reset
{
    INCREASE       = 1,
    DECREASE       = 0,
    MEM_RESET_COEF = 2
};

enum error_t
{
    FILE_ERROR    = 1,
    NO_ERROR      = 0,
    POP_ERROR     = 10,
    CTOR_ERROR    = 11,
    DTOR_ERROR    = 12,
    PUSH_ERROR    = 13,
    RECTOR_ERROR  = 14,
    PTR_ERROR     = 15,
    VERIF_ERROR   = 16,
    HASH_ERROR    = 17,
    CANARY_ERROR  = 32,
    POP_ERROR_NUM = 123456
};

typedef int data_t;
typedef int canary_t;

struct stack_t
{
    canary_t l_canary         = LEFT_CANARY;
    data_t* data            = nullptr;
    size_t capacity         = 0;
    size_t size_            = 0;
    long   hash_            = 0;
    defence_level def_level = (defence_level)SET_DEF_LEVEL;
    canary_t r_canary         = RIGHT_CANARY;
};

//-----------------------------------------------------------------------------

error_t Stack_Ctor(stack_t* st, size_t capacity = DEFAULT_CAPACITY);

error_t Stack_Push(stack_t* st, int x);

data_t Stack_Pop(stack_t* st, error_t* error = nullptr);

error_t Stack_Dtor(stack_t* st);

error_t Stack_ReCtor(stack_t* st, bool change);

error_t Stack_Verificator(stack_t* st);

//-----------------------------------------------------------------------------

error_t Error_Log(error_t error);

error_t Stack_Interface(stack_t* st);

error_t Canary_Verificator(stack_t* st);

long Stack_Hash_Count(stack_t* st);

#endif // STACK_H
