#include"linear.h"
#include<stdio.h>
#include<string.h>
#include<sys/mman.h>
#include<stdlib.h>
#include<errno.h>
void li_setup_mem(li_ctx_t *ctx, size_t buf_size, pointer_t buf)
{
    ctx->mem_header=buf;
    ctx->memsz=buf_size;
    ctx->mem_header->prev=(char*)buf;
    ctx->mem_header->next=(char*)buf+buf_size;
    ctx->stack.stack_size = 1024;
    ctx->stack.stacks = li_malloc(ctx,1024);
    //ctx->stack.stack_size = 1024;
    ctx->stack.ebp = ctx->stack.esp = &ctx->stack.stacks[1024];
    memset(ctx->stack.regs,0,4*4);
    ctx->jitbuf.buffer= mmap(0,             // address
                      4096,             // size
                      PROT_READ | PROT_WRITE | PROT_EXEC,
                      MAP_PRIVATE|MAP_ANONYMOUS,
                      -1,               // fd (not used here)
                      0);               // offset (not used here)
    //ctx->jitbuf.buffer = li_malloc(ctx,256);
    printf("0x%x %s\n",ctx->jitbuf.buffer,strerror(errno));
    ctx->jitbuf.cur = 0;
    ctx->jitbuf.max = 4096;
}


static li_mem_block_hearder* li_split(li_mem_block_hearder *header,size_t sz)
{
    pointer_t nxt=header->next;
    header->next=(char*)header+sz;
    header->next->next=nxt;
    header->next->prev=header;
    return header;
}

static void li_merge(li_ctx_t *ctx,li_mem_block_hearder *head)
{
    size_t maxptr=(char *)ctx->mem_header + ctx->memsz;
    if(head->next == maxptr)
        return;
    if(head->next->used == 0)
    {
        printf("merge: %x %x\n",head,head->next);
        head->next=head->next->next;
        head->next->prev=head;
        
    }

}
#define li_msz(x) x+sizeof(li_mem_block_hearder)
pointer_t li_malloc(li_ctx_t *ctx,size_t size)
{
    li_mem_block_hearder *cur=ctx->mem_header;
    size_t maxptr=(char *)ctx->mem_header + ctx->memsz;
    while (1)
    {
        if(cur == maxptr)
            return 0;
        if(!cur->used)
        {
            size_t req=li_msz(size);
            size_t dif=(char *)cur->next - (char *)cur->prev;
            cur->used=1;
            if(dif > req)
            {
                char *addr= (char *)li_split(cur,req)+sizeof(li_mem_block_hearder);
                memset(addr,0,size);
                return addr;
            }else if(dif == req)
            {
                char *addr = (char *)cur+sizeof(li_mem_block_hearder);
                memset(addr,0,size);
                return addr;
            }
            cur->used=0;
        }
        cur=cur->next;
    }
    
}

void li_setup_code(li_ctx_t *ctx,char *codes, size_t sz)
{
    ctx->codes=codes;
    ctx->code_sz=sz;
    ctx->code_now=0;
}

void li_free(li_ctx_t *ctx,pointer_t ptr)
{
    if(!ptr)
        return;
    li_mem_block_hearder *head=(char*)ptr-sizeof(li_mem_block_hearder);
    head->used=0;
    li_merge(ctx,head);
}

int li_push(li_ctx_t *ctx,char type, pointer_t value,uint32_t sz)
{
    ctx->stack.esp -= sz;
    if(ctx->stack.esp < ctx->stack.stacks)
    {
        LI_INFO("[ERROR] Stack Overflow!");
        return -1;
    }
    memcpy(ctx->stack.esp,value,sz);
    return 1;
}

char *li_get_str(li_ctx_t *ctx, size_t *len, char *dst)
{
    li_str_pool *pool=&ctx->str_pool;
    char *now=pool->codes;
    while (1)
    {
        if(now[0] == '\0')
            return NULL;
        /* abcde012345 */
        *len=strlen(now);
        if(!strcmp(now,dst))
            return now;
        now=now+*len+1;
    }
    
}

char *li_push_str(li_ctx_t *ctx, char *str, size_t len)
{
    li_str_pool *pool=&ctx->str_pool;
    size_t lens = 0;
    char *r = li_get_str(ctx,&lens,str);
    if(r)
        return r;
    if(pool->codes+pool->cur+len > pool->codes+pool->total_sz)
        return -1; // full
    memcpy(pool->codes+pool->cur,str,len);
    r=pool->codes+pool->cur;
    pool->cur+=len+1;
    return r;
}

pointer_t li_pop(li_ctx_t *ctx, uint32_t sz)
{
    if(ctx->stack.esp >= ctx->stack.stacks + ctx->stack.stack_size)
    {
        LI_INFO("Stack Underflow!");
        return NULL;
    }
    pointer_t out = ctx->stack.esp;
    ctx->stack.esp += sz;
    return out;
}

