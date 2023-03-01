#include"linear.h"
#include<string.h>
char* li_next(li_ctx_t *ctx, int size)
{
    char *r = &ctx->codes[ctx->code_now];
    if(ctx->code_now + size > ctx->code_sz)
        return NULL;
    ctx->code_now += size;
    if(r)
    {
        //LI_INFO("op:%x\n",*(uint32_t*)r);
    }
    return r;
}

#define saf(e)  do{e}while(0);
#define chkif(ptr,fal,tr) if(!ptr){fal;}else{tr;}
#define chkop(ptr,type,name) type name =0;chkif(ptr,LI_INFO("BROKEN OPCODES");break;,name = *(type*)ptr);
#define expect(expr,el) if(!(expr)){el;}
#define valreg(reg) ctx->stack.regs[reg]

void li_dump_regs(li_ctx_t *ctx)
{
    LI_INFO("EAX: %d; EBX: %d; ECX: %d\n",ctx->stack.regs[LI_EAX],
                                            ctx->stack.regs[LI_EBX],
                                            ctx->stack.regs[LI_ECX]);
}

void li_dump_jit(li_ctx_t *ctx)
{
    printf("dump jit asm:\n");
    for (int i = 0; i < ctx->jitbuf.max; i+=4)
    {
        printf("%02x ",*(uint32_t*)(ctx->jitbuf.buffer+i));
    }
    printf("\n");
}

int li_start_jit(li_ctx_t *ctx)
{
    LI_INFO("jit in\n");
    uint32_t *jitptr = li_next(ctx,4);
    uint32_t jitendoff = 0,jitfunc = 0;
    chkif(jitptr,LI_INFO("BROKEN JIT  %d",jitendoff);,jitendoff = *jitptr);
    jitptr = li_next(ctx,4);
    chkif(jitptr,LI_INFO("BROKEN JIT  %d",jitfunc);,jitfunc = *jitptr);
    int (*func)() = jitfunc;
    LI_INFO("jit: %x %x\n",jitendoff,jitfunc);
    if(func)
    {
        LI_INFO("Already generated codes!");
        ctx->code_now+=jitendoff;
        return func();
    }
    li_jit_cmd(ctx,LI_PUSHAD,0,0);

    li_jit_cmd(ctx,LI_SETUP_STACK,0,0);
    while (1)
    {
        char *ptr = li_next(ctx,1);
        char opcode = 0;
        chkif(ptr,LI_INFO("BROKEN OPCODES %d",opcode);break;,opcode = *ptr);
        if(opcode == (char)LI_END)
        {
            LI_INFO("JIT END!\n");
            //li_jit_cmd(ctx,LI_POPAD,0,0);
            li_jit_cmd(ctx,LI_END,0,0);
            break;
        }
        else if(opcode == (char)LI_RET)
        {
            LI_INFO("JIT RET!\n");
            //li_jit_cmd(ctx,LI_POPAD,0,0);
            li_jit_cmd(ctx,LI_RET,0,0);
            break;
        }
        else if(opcode == (char)LI_MOV)
        {
            LI_INFO("JIT MOV\n");
            ptr = li_next(ctx,1);
            chkop(ptr,char,dst);
            ptr = li_next(ctx,4);
            chkop(ptr,uint32_t,src);
            printf("%d --> %d\n",src,dst);
            expect(dst>=0 && dst<LI_EBP,LI_INFO("Bad register number:%d!",dst);break;)
            li_jit_cmd(ctx,LI_MOV,dst,src);
        }
        else
        {
            LI_INFO("UNKNOWN JIT: %d\n",opcode);
        }
    }
    
    //li_dump_jit(ctx);
    //char *ops=li_malloc(ctx,64);
    //memcpy(ops,ctx->jitbuf.buffer,64);
    //li_map_exemem(ops,64);
    void (*funccc)()=ctx->jitbuf.buffer;
    funccc();
    *jitptr = funccc;
    printf("call jit!");

}

int li_start_vm(li_ctx_t *ctx)
{
    while (1)
    {
        char *ptr = li_next(ctx,1);
        char opcode = 0;
        chkif(ptr,LI_INFO("BROKEN OPCODES %d",opcode);break;,opcode = *ptr);
        if(opcode == (char)LI_END)
            break; // end of vm
        else if(opcode == (char)LI_MOV)
        {
            // mov dst,src
            LI_INFO("MOV");
            ptr = li_next(ctx,1);
            chkop(ptr,char,dst);
            ptr = li_next(ctx,4);
            chkop(ptr,uint32_t,src);
            printf("%d --> %d\n",src,dst);
            expect(dst>=0 && dst<LI_EBP,LI_INFO("Bad register number:%d!",dst);break;)
            //li_jit_cmd(ctx,LI_MOV,dst,src);
            li_dump_regs(ctx);
        }else if(opcode == (char)LI_JIT)
        {
            li_start_jit(ctx);
            //LI_INFO("666");
            return 1;
        }
    }
    
}