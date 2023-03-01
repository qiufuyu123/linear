#include"linear.h"
#include<sys/mman.h>
#include<string.h>
void li_map_exemem(pointer_t addr,size_t sz)
{
    mprotect(addr,sz,PROT_EXEC | PROT_READ | PROT_WRITE);
}

int jit_move(li_ctx_t *ctx, char *ptr, size_t sz)
{
    LI_INFO("%x %x %x\n",ctx->jitbuf.cur,sz,ctx->jitbuf.max);
    
    if(ctx->jitbuf.cur + sz > ctx->jitbuf.max)
        return -1;
    memcpy(ctx->jitbuf.buffer + ctx->jitbuf.cur,ptr,sz);
    ctx->jitbuf.cur += sz;
    return 1;
}

#define LI_GEN_REGS(dst,src) (char) (0xC0 |( (src << 3) | dst))

int li_jit_cmd(li_ctx_t *ctx, char opcode,uint32_t v1,uint32_t v2)
{
    if(opcode == (char)LI_PUSHAD || 
        opcode == (char)LI_POPAD)
    {
        jit_move(ctx,&opcode,1);
    }
    else if(opcode == (char)LI_SETUP_STACK)
    {
        LI_INFO("JIT IN PREPARE STACK");
        li_jit_cmd(ctx,LI_MOV_REG,LI_EAX,LI_ESP);
        li_jit_cmd(ctx,LI_MOV_REG,LI_EBX,LI_EBP);
        li_jit_cmd(ctx,LI_MOV,LI_ESP,ctx->stack.esp);
        li_jit_cmd(ctx,LI_MOV,LI_EBP,ctx->stack.ebp);
        li_jit_cmd(ctx,LI_PUSH_REG,LI_EBX,0);
        li_jit_cmd(ctx,LI_PUSH_REG,LI_EAX,0);
    }
    else if(opcode == (char)LI_MOV)
    {
        LI_INFO("CMD MOV\n");
        opcode += v1;
        jit_move(ctx,&opcode,1);
        jit_move(ctx,&v2,4);
    }else if(opcode == (char)LI_MOV_REG)
    {
        jit_move(ctx,&opcode,1);
        char data_buf=LI_GEN_REGS(v1,v2);
        jit_move(ctx,&data_buf,1);
    }
    else if(opcode ==(char)LI_PUSH_REG || opcode ==(char)LI_POP_REG)
    {
        opcode += v1;
        jit_move(ctx,&opcode,1);
    }
    else if(opcode == (char) LI_RET)
    {
        jit_move(ctx,&opcode,1);
    }
    else if(opcode == (char)LI_END)
    {
        li_jit_cmd(ctx,LI_POP_REG,LI_EAX,0);
        li_jit_cmd(ctx,LI_POP_REG,LI_EBX,0);
        li_jit_cmd(ctx,LI_MOV_REG,LI_ESP,LI_EAX);
        li_jit_cmd(ctx,LI_MOV_REG,LI_EBP,LI_EBX);
        li_jit_cmd(ctx,LI_POPAD,0,0);
        li_jit_cmd(ctx,LI_RET,0,0);
    }
}