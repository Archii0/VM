/* VM.c*/

#include "VM.h"
#include "utils.h"

void __mov(VM *vm, Args a1, Args a2) { vm $ax = (Reg)a1; }

void execinstr(VM *vm, Instruction *i) {
    Args a1, a2;
    int16 size;
    a1 = a2 = 0;

    size = map(i->o);
    switch (size) {
    case 0:
        break;
    case 1:
        a1 = i->a[0];
        break;
    case 2:
        a1 = i->a[0];
        a2 = i->a[1];
        break;
    default:
        segfault(vm);
        break;
    }

    switch (i->o) {
    case mov:
        __mov(vm, a1, a2);
        break;
    case nop:
        break;
    case hlt:
        error(vm, SysHlt);
        break;
    }
}

void error(VM *vm, Errorcode e) {
    int8 exitcode;
    exitcode = -1;
    switch (e) {
    case ErrSegv:
        fprintf(stderr, "%s\n", "VM Segmentation fault");
        break;
    case SysHlt:
        fprintf(stderr, "%s\n", "System halted");
        exitcode = 0;
        break;

    default:
        break;
    }
    if (vm) {
        free(vm);
    }
    exit((int8 *)exitcode);
}

void execute(VM *vm) {
    Program *pp;
    int16 size;
    Instruction *ip;
    assert(vm && *vm->m);
    size = 0;
    pp = vm->m;

    while ((*pp != (Opcode)hlt) && (pp <= (Program *)vm->b)) {
        ip = (Instruction *)pp;
        size = map(ip->o);
        execinstr(vm, ip);

        vm $ip += size;
        pp += size;
    }
    if (pp > (Program *)vm->b) {
        segfault(vm);
    }
}

int8 map(Opcode o) {
    int8 n, ret;
    IM *p;

    ret = 0;
    for (n = IMs, p = instrmap; n; --n, ++p) {
        if (p->o == o) {
            ret = p->size;
            break;
        }
    }
    return ret;
}

VM *virtualmachine() {
    VM *p;
    int16 size;

    size = (int16 *)sizeof(VM);
    p = (VM *)malloc((int8 *)size);

    if (!p) {
        errno = ErrMem;
        return (VM *)0;
    }

    zero((int8 *)p, size);

    return p;
}

Program *exampleprogram(VM *vm) {
    Program *p;
    Instruction *i1, *i2, *i3;
    Args a1;
    int16 s1, s2, sa1;

    a1 = 0;
    s1 = map(mov);
    s2 = map(nop);

    i1 = (Instruction *)malloc((int8 *)s1);
    i2 = (Instruction *)malloc((int8 *)s2);
    i3 = (Instruction *)malloc((int8 *)s2);
    assert(i1 && i2);
    zero((int8 *)i1, s1);
    zero((int8 *)i2, s2);
    zero((int8 *)i3, s2);

    i1->o = mov;
    sa1 = (s1 - 1);

    a1 = 0x0005;
    p = vm->m;
    copy((int8 *)p, (int8 *)i1, 1);
    p++;
    if (a1) {
        copy((int8 *)p, (int8 *)&a1, sa1);
        p += sa1;
    }
    i2->o = nop;
    copy((int8 *)p, (int8 *)i2, 1);
    i3->o = hlt;
    copy((int8 *)p, (int8 *)i3, 1);
    free(i1);
    free(i2);
    free(i3);
    vm->b = (s1 + sa1 + s2 + s2);
    vm $ip = (Reg)vm->m;
    vm $sp = (Reg)-1;

    return (Program *)&vm->m;
}

int main(int argc, char *argv[]) {
    Program *prog;
    VM *vm;

    vm = virtualmachine();
    printf("vm = %p (sz: %d)\n", vm, sizeof(struct s_vm));
    prog = exampleprogram(vm);
    printf("prog = %p\n", prog);

    execute(vm);
    printf("ax = %.04hx\n", (int8 *)vm $ax);

    printhex((int8 *)prog, (map(mov) + map(nop)), ' ');
    return 0;
}