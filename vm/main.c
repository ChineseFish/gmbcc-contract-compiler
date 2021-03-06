#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "./common.h"

FILE *codfp, *errfp;

char codname[FILE_NAME_LEN];
char errname[FILE_NAME_LEN];

void clear();
void signup();

extern union header *arena[LASTARENA];
extern int byte_sequence_size;
extern unsigned char byte_sequence[];

Interface x64_vm_interface = {
    {1, 4},     /* charmetric */
    {2, 4},     /* shortmetric */
    {4, 4},     /* intmetric */
    {4, 4},     /* floatmetric */
    {4, 8},     /* doublemetric */
    {4, 4},     /* pointermetric */
    {4, 4},     /* structmetric */
};

Interface *IR = &x64_vm_interface;

union {
    int number;
    char s;
} _host_byte_order_example_;
 
boolean testBigEndin() {
    _host_byte_order_example_.number = 0x01000002;
    return (_host_byte_order_example_.s == 0x02);
}

static void finalize()
{
    /* clear all memory. */
    deallocate(PERM);
    deallocate(FUNC);
}

void init_spl()
{
    memset(arena, 0, sizeof(arena));
    signup();
}

void prepare_file(char *fname)
{
    char *p;

    for (p = fname; *p; p++)
        *p = tolower(*p);

    if (strstr(fname, ".cod"))
    {
        for (; p > fname; p--)
        {
            if (*p == '.')
                break;

        }
        *p = '\0';
    }

    snprintf(codname, sizeof(codname), "%s.cod", fname);
    snprintf(errname, sizeof(errname), "%s.err", fname);
    
    codfp = fopen(codname, "r");
    errfp = fopen(errname, "wt");

    if (!codfp || !errfp)
    {
        printf("File open error!\n");
        exit(1);
    }
}

int main(int argc, char **argv)
{
    int dargc;
    char **arg, **dargv;

    if (testBigEndin()) {
        g_is_big_endian = true;
    } else {
        g_is_big_endian = false;
    }

    printf("host byte sequence is %sn", g_is_big_endian ? "big_endian" : "little_endian");
    
    if (argc == 1)
    {
        printf("\nUsage :%s [-d stad] filename[.pas]\n\n", argv[0]);
        return 1;
    }

    init_op_code();

    init_spl();

    arg = argv + 1;
    dargc = 0;
    dargv = malloc(argc * sizeof(char *));

    /*
     * arguments not recognized by main is pased to 
     * target program_begin
     */
    while(arg)
    {
        if (**(arg) == '-')
        {
            switch(arg[0][1])
            {
            case 'd':
                {
                    char *p = arg[1];
                    while (*p)
                    {
                        switch(*p++)
                        {
                        case 's':
                            
                            break;
                        case 'a':
                            
                            break;
                        case 't':
                            
                            break;
                        case 'd':
                            
                            break;
                        default:
                            printf("Unkown dump option %c.\n", *(p - 1));
                            break;
                        }
                    }
                }
                arg++;
                arg++;
                break;
            default:
                dargv[dargc++] = *arg++;
                dargv[dargc++] = *arg++;
                break;
            }
        }
        else
        { 
            prepare_file(arg[0]);
            break;
        }
    }

    int c;
    while((c = fgetc(codfp)) != EOF)
    {
        byte_sequence[byte_sequence_size++] = (unsigned char)c;
    }

    interpret();

    clear();

    finalize();

    free(dargv);

    return 0;
}

void clear()
{
    fclose(codfp);
    fclose(errfp);
}

void signup()
{
    printf("\n");
    printf("Simple Pascal Language Vm Version %s\n", _PC_VER_);
}

