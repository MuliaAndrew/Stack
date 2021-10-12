#include "stack.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

data_t* PTR_POISION = (int*) 13;

error_t Stack_Ctor(stack_t* st, size_t capacity)
{
    st->capacity = capacity;

    if(st->def_level == ONE)
    {
        st->data = (data_t*)calloc(capacity, sizeof(data_t));
        ptr_ver(st->data);
    }

    if(st->def_level >= TWO)
    {
        st->data = (data_t*)calloc(capacity * sizeof(data_t) + 2 * sizeof(double), 1);
        ptr_ver(st->data);

        *((double*) st->data) = LEFT_CANARY;

        st->data = (data_t*)((char*)st->data + sizeof(canary_t));
        data_t* temp_ptr = st->data;
        st->data = st->data + capacity;
        *((double*) st->data) = RIGHT_CANARY;
        st->data = temp_ptr;

        if(st->def_level == THREE)
            st->hash_ = Stack_Hash_Count(st);
    }
    return Stack_Verificator(st);
}

error_t Stack_Push(stack_t* st, data_t elem)
{
    ptr_ver(st);

    error_t temp = NO_ERROR;

    if(st->size_ == st->capacity - 1)
        temp = Stack_ReCtor(st, INCREASE);

    if(temp != NO_ERROR)
        return temp;

    if(st->def_level >= THREE)
    {
        if(st->hash_ != Stack_Hash_Count(st))
            return HASH_ERROR;
    }

    st->data[st->size_] = elem;

    st->size_++;

    if(st->def_level >= THREE)
        st->hash_ = Stack_Hash_Count(st);

    return Stack_Verificator(st);
}

data_t Stack_Pop(stack_t* st, error_t* error)
{
    if(st->def_level >= THREE)
    {
        if(st->hash_ != Stack_Hash_Count(st))
        {
            if(error)
                *error = HASH_ERROR;
        }
    }

    st->size_--;

    if(int(st->size_) < 0)
    {
        if(error)
            *error = POP_ERROR;

        st->size_++;

        return POP_ERROR_NUM;
    }
    else
    {
        if(st->size_ <= st->capacity / 4 && st->capacity > DEFAULT_CAPACITY)
            Stack_ReCtor(st, DECREASE);

        if(error)
            *error = Stack_Verificator(st);

        if(st->def_level >= THREE)
            st->hash_ = Stack_Hash_Count(st);

        return st->data[st->size_];
    }
}

error_t Stack_Dtor(stack_t* st)
{
    ptr_ver(st);

    error_t error = Stack_Verificator(st);

    if(st->def_level == ONE)
    {
        free(st->data);
        st->data = PTR_POISION;
    }
    else if(st->def_level >= TWO)
    {
        st->data = (data_t*)((char*)st->data - sizeof(canary_t));
        free(st->data);
        st->data = PTR_POISION;
    }
    else return DTOR_ERROR;
    st->size_ = 0;
    st->capacity = 0;

    return error;
}

error_t Stack_ReCtor(stack_t* st, bool change)
{
    ptr_ver(st);

    if(st->def_level == ONE)
    {
        if(change == INCREASE)
        {
            st->capacity *= MEM_RESET_COEF;
            st->data = (data_t*)realloc(st->data, st->capacity);
        }
        else if(change == DECREASE)
        {
            st->capacity /= MEM_RESET_COEF;
            st->data = (data_t*)realloc(st->data, st->capacity);
        }
        else return RECTOR_ERROR;
    }
    else if(st->def_level >= TWO)
    {
        if(change == INCREASE)
        {
            *((canary_t*)st->data + st->capacity) = 0;
            st->data = (data_t*)((char*)st->data - sizeof(canary_t));

            st->capacity *= MEM_RESET_COEF;
            st->data = (data_t*)realloc(st->data, st->capacity + 2 * sizeof(canary_t));

            st->data = (data_t*)((char*)st->data + sizeof(canary_t));

            *((canary_t*)st->data + st->capacity) = RIGHT_CANARY;

            if(st->def_level >= THREE)
                Stack_Hash_Count(st);
        }
        else if(change == DECREASE)
        {
            st->data = (data_t*)((char*)st->data - sizeof(canary_t));

            st->capacity /= MEM_RESET_COEF;
            st->data = (data_t*)realloc(st->data, st->capacity + 2 * sizeof(canary_t));

            st->data = (data_t*)((char*)st->data + sizeof(canary_t));

            *((canary_t*)st->data + st->capacity) = RIGHT_CANARY;

            if(st->def_level >= THREE)
                Stack_Hash_Count(st);
        }
        else return RECTOR_ERROR;
    }
    else return RECTOR_ERROR;

    ptr_ver(st->data);

    return Stack_Verificator(st);
}

error_t Stack_Verificator(stack_t* st)
{
    ptr_ver(st);

    if(st->data == nullptr)
        return PTR_ERROR;

    if(st->capacity < 0)
        return VERIF_ERROR;

    if(st->capacity < st->size_)
        return VERIF_ERROR;

    if(st->def_level >= 2)
    {
        if(st->l_canary != LEFT_CANARY)
            return CANARY_ERROR1;

        else if(st->r_canary != RIGHT_CANARY)
            return CANARY_ERROR2;

        else if(*((canary_t*)st->data - 1) != LEFT_CANARY)
            return CANARY_ERROR3;

        else if(*((canary_t*)st->data + st->capacity) != RIGHT_CANARY)
            return CANARY_ERROR4;

        if(st->def_level == 3)
        {
            if(Stack_Hash_Count(st) != st->hash_)
                return HASH_ERROR;
        }
    }
    return NO_ERROR;
}

error_t Error_Log(error_t error)
{
    int fwrite_check = FILE_ERROR;

    FILE* log = nullptr;
    if(!(log = fopen("log.txt", "ab+")))
    {
        errprint(Cant open the log file);
        return NO_ERROR;
    }

    switch(error)
    {
        case NO_ERROR:
        {
            break;
        }
        case POP_ERROR:
        {
            fwrite_check = fwrite("\n Stack_Pop() error.\nCheck the enter number", sizeof(char), 44, log);
            break;
        }
        case CTOR_ERROR:
        {
            fwrite_check = fwrite("\n Stack_Ctor() error.\n", sizeof(char), 23, log);
            break;
        }
        case DTOR_ERROR:
        {
            fwrite_check = fwrite("\n Stack_Dtor() error.\n", sizeof(char), 23, log);
            break;
        }
        case PUSH_ERROR:
        {
            fwrite_check = fwrite("\n Stack_Push() error.\n", sizeof(char), 23, log);
            break;
        }
        case RECTOR_ERROR:
        {
            fwrite_check = fwrite("\n Stack_ReCtor() error.\n", sizeof(char), 25, log);
            break;
        }
        case PTR_ERROR:
        {
            fwrite_check = fwrite("\n Stack pointer error.\n", sizeof(char), 23, log);
            break;
        }
        case VERIF_ERROR:
        {
            fwrite_check = fwrite("\n Stack parameters error.\n", sizeof(char), 27, log);
            break;
        }
        case CANARY_ERROR1 || CANARY_ERROR2 || CANARY_ERROR3 || CANARY_ERROR4:
        {
            fwrite_check = fwrite("\n Stack canaries error.\n", sizeof(char), 25, log);
            break;
        }
        case HASH_ERROR:
        {
            fwrite_check = fwrite("\n Stack hash error.\n", sizeof(char), 20, log);
            break;
        }
        default:
            fwrite_check = fwrite("\n Unprocessed error.\n", sizeof(char), 23, log);
    }

    fclose(log);
    log = nullptr;

    if(!fwrite_check)
    {
        errprint(File write error);
    }

    return NO_ERROR;
}

error_t Stack_Interface(stack_t* st)
{
    ptr_ver(st->data);

    loop_index loop = RUN;
    error_t error = NO_ERROR;

    while(loop == RUN)
    {
        char cmd[5] = {0};
        data_t num  = 0;

        scanf("%s", cmd);

        if(!(strcmp(cmd, "push")))
        {
            scanf("%d", &num);

            error = Stack_Push(st, num);
            printf("error = %d\n", error);
            error = Error_Log(error);
        }
        else if(!(strcmp(cmd, "pop")))
        {
            num = Stack_Pop(st, &error);
            printf("error = %d\n", error);
            error = Error_Log(error);

            printf("%d\n", num);
        }
        else if(!(strcmp(cmd, "ver")))
            printf("error = %d\n,"
                   "Stack params now: capacity = %d, size_ = %d, def_level = %d, "
                   "hash = %d\n", error, st->capacity, st->size_, st->def_level, st->hash_);

        else if(!(strcmp(cmd, "exit")))
            loop = STOP;

        else
            printf("input error, write command again\n");

        fflush(stdin);
    }

    return Stack_Verificator(st);
}

long Stack_Hash_Count(stack_t* st)
{
    long temp_hash = 0;
    for(unsigned int i = 0; i < st->size_; i++)
    {
        temp_hash += st->data[i];
        temp_hash += temp_hash << 10;
        temp_hash ^= temp_hash >> 6;
    }
    temp_hash += st->def_level;
    temp_hash += temp_hash << 10;
    temp_hash ^= temp_hash >> 6;

    temp_hash += st->capacity;
    temp_hash += temp_hash << 10;
    temp_hash ^= temp_hash >> 6;

    temp_hash += st->size_;
    temp_hash += temp_hash << 10;
    temp_hash ^= temp_hash >> 6;

    temp_hash += temp_hash << 3;
    temp_hash ^= temp_hash >> 11;
    temp_hash += temp_hash << 15;

    return temp_hash;
}
