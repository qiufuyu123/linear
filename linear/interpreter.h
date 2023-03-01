#pragma once
#include<string>
/**
 * 
 * Interpreter Impl
 * 
 * Examples of codes:
 *  * 4 5 
 *  + 8 9
 * 
 * (Operation) (Value 1) (Value 2)
 * 
 * 
*/

enum{
    LI_ERR_OK,
    LI_ERR_OPER = -1,
    LI_ERR_VAL = -2
};

typedef struct 
{
    bool regs_used[4];
    
}li_vm_info_t;


class LI_Interpreter
{
private:
    std::string codes;
    int now_pos;
    char *vm_codes;
    int vm_size;
    int vm_now;
public:
    LI_Interpreter(std::string codes, char *vmbuf, int bufsize);
    int generateIL();
}
