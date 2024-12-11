/* VM.h */
#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NoErr 0x00
#define SysHlt 0x01
#define ErrMem 0x02
#define ErrSegv 0x04
#define segfault(x) error((x), ErrSegv)

#define $ax ->c.r.ax
#define $bx ->c.r.bx
#define $cx ->c.r.cx
#define $dx ->c.r.dx
#define $sp ->c.r.sp
#define $ip ->c.r.ip

/*
    16 bit
    Registers: AX, BX, CX, DX
    Stack pointer: SP
    Instruction pointer: IP
    65 Kb memory
    (Serial COM port)
    (Floppy drive)

*/
typedef unsigned char Errorcode;

typedef unsigned char int8;
typedef unsigned short int int16;

typedef unsigned short int Reg;

struct s_registers {
    Reg ax;
    Reg bx;
    Reg cx;
    Reg dx;
    Reg sp;
    Reg ip;
} typedef Registers;

struct s_cpu {
    Registers r;
} typedef CPU;

enum e_opcode { mov = 0x01, nop = 0x02, hlt = 0x03 } typedef Opcode;

struct s_instrmap {
    Opcode o;
    int8 size;
} typedef IM;

typedef int16 Args;

struct s_instruction {
    Opcode o;
    Args a[];
};
typedef struct s_instruction Instruction;

typedef int8 Memory[(int16)(-1)];

typedef int8 Program;

struct s_vm {
    CPU c;
    Memory m;
    int16 b; // break line
    Program *p;
} typedef VM;

typedef Memory *Stack;

static IM instrmap[] = {{mov, 0x03}, {nop, 0x01}, {hlt, 0x01}};
#define IMs (sizeof(instrmap) / (sizeof(struct s_instrmap)))

void __mov(VM *, Args, Args);
void error(VM *, Errorcode);
void execinstr(VM *, Instruction *);
void execute(VM *);
Program *exampleprogram(VM *);
int8 map(Opcode);
VM *virtualmachine();
int main(int, char **);
