#include<stdio.h>
#include"linear/linear.h"
#include<stdlib.h>
#include<string.h>
int main()
{
    li_ctx_t ctx;
    li_setup_mem(&ctx,4096,malloc(4096));
    pointer_t a,b,c;
    printf("0x%x->0x%x 0x%x\n",ctx.mem_header,(char *)ctx.mem_header+ctx.memsz,a=li_malloc(&ctx,100));
    printf("0x%x->0x%x 0x%x\n",ctx.mem_header,(char *)ctx.mem_header+ctx.memsz,b=li_malloc(&ctx,100));
    li_free(&ctx,a);
    li_free(&ctx,b);
    printf("0x%x->0x%x 0x%x\n",ctx.mem_header,(char *)ctx.mem_header+ctx.memsz,a=li_malloc(&ctx,100));
    char codes[100];
    codes[0] = LI_MOV;
    codes[1] = LI_EAX;
    uint32_t t = 0x2023;
    memcpy(&codes[2],&t,sizeof(int));
    codes[6]=LI_MOV;
    codes[7] = LI_EBX;
    t = 0x6666;
    memcpy(&codes[8],&t,sizeof(int));
    codes[12]=LI_JIT;
    t=0;
    memset(&codes[13],0,8);
    //memcpy(&codes[17],&t,sizeof(int));
    codes[21]=LI_MOV;
    codes[22]=LI_EDX;
    t=123;
    memcpy(&codes[23],&t,sizeof(int));
    codes[27]=LI_END;
    codes[28]=LI_END;
    li_setup_code(&ctx,codes,29);
    li_start_vm(&ctx);
    ctx.code_now=0;
    li_start_vm(&ctx);
}