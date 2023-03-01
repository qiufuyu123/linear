#pragma once

#include<stdint.h>
#include<stddef.h>
/**
 * Root Nodes:
 *  declaration:
 *      <Type> <Name> [ = <Expr>]
 *      <Type> <Name> <Args> '{' <RootNodes> '}'
 *  Equal:
 *      <Refer> <=> <Expr>
 * Sub Nodes:
 *  Variable:
 *      <words>
 *  Function:
 *      <words> <Use-Args>
 *  Refer:
 *      <Variable>
 *      <Function>
 * 
 *  Expr:
 *      <subexpr> <==>/<!=>/<&&>/<||> <subexpr>
 *  Subexpr:
 *      <term> <+>/<-> <term>
 *  term:
 *      <factor> <*>/</> <factor>
 *  factor:
 *      <+/-> <factor>
 *      <number>
 *      <Refer>
 * 
*/
typedef void* pointer_t;
typedef struct li_mem_block
{
    struct li_mem_block *next;
    struct li_mem_block *prev;
    char used : 1;
}li_mem_block_hearder;

typedef struct 
{
    char *stacks;
    pointer_t esp;
    pointer_t ebp;
    size_t stack_size;
    uint32_t regs[4];
}li_stacks;


typedef struct 
{
    char *codes;
    size_t total_sz;
    size_t cur;
}li_str_pool;

enum
{
    LI_EAX=0,
    LI_ECX,
    LI_EDX,
    LI_EBX,
    LI_ESP,
    LI_EBP,
    LI_NOP,
    LI_SETUP_STACK,
    LI_JIT,
    LI_END,
    LI_MOV=0xb8,
    LI_MOV_REG=0x89,
    LI_PUSH_IM=0x68,
    LI_PUSH_REG=0x50,
    LI_POP_REG=0x58,
    LI_RET=0xc3,
    LI_PUSHAD=0x60,
    LI_POPAD=0x61,
    LI_ADD,
    LI_MINUS,
    LI_MUL,
    LI_DIV
};
typedef struct
{
    char *buffer;
    size_t cur;
    size_t max;
}li_jit_buf;

typedef struct 
{
    size_t memsz;
    li_mem_block_hearder *mem_header;
    li_str_pool str_pool;
    li_stacks stack;
    li_jit_buf jitbuf;
    char *codes;
    size_t code_sz;
    size_t code_now;
}li_ctx_t;



#define LI_INFO printf

void li_map_exemem(pointer_t addr,size_t sz);

void li_setup_code(li_ctx_t *ctx,char *codes, size_t sz);

void li_setup_mem(li_ctx_t *ctx, size_t buf_size, pointer_t buf);

pointer_t li_malloc(li_ctx_t *ctx,size_t size);

void li_free(li_ctx_t *ctx,pointer_t ptr);

int li_push(li_ctx_t *ctx,char type, pointer_t value,uint32_t sz);

char *li_push_str(li_ctx_t *ctx, char *str, size_t len);

int li_start_vm(li_ctx_t *ctx);

int li_jit_cmd(li_ctx_t *ctx,char opcode,uint32_t v1,uint32_t v2);