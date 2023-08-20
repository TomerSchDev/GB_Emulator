//
// Created by tomer on 8/20/23.
//
#include <cpu.h>

extern cpu_context ctx;

u16 reverse(u16 n) {
    return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
}

u16 cpu_read_reg(reg_type rt) {
    switch (rt) {
        case RT_A:
            return ctx.registers.a;
        case RT_F:
            return ctx.registers.f;
        case RT_B:
            return ctx.registers.b;
        case RT_C:
            return ctx.registers.c;
        case RT_D:
            return ctx.registers.d;
        case RT_E:
            return ctx.registers.e;
        case RT_H:
            return ctx.registers.h;
        case RT_L:
            return ctx.registers.l;

        case RT_AF:
            return reverse(*((u16 *) &ctx.registers.a));
        case RT_BC:
            return reverse(*((u16 *) &ctx.registers.b));
        case RT_DE:
            return reverse(*((u16 *) &ctx.registers.d));
        case RT_HL:
            return reverse(*((u16 *) &ctx.registers.h));

        case RT_PC:
            return ctx.registers.pc;
        case RT_SP:
            return ctx.registers.sp;
        default:
            return 0;
    }
}

void cpu_set_reg(reg_type rt, u16 val) {
    switch (rt) {
        case RT_A:
            ctx.registers.a = val & 0xFF;
            break;
        case RT_F:
            ctx.registers.f = val & 0xFF;
            break;
        case RT_B:
            ctx.registers.b = val & 0xFF;
            break;
        case RT_C: {
            ctx.registers.c = val & 0xFF;
        }
            break;
        case RT_D:
            ctx.registers.d = val & 0xFF;
            break;
        case RT_E:
            ctx.registers.e = val & 0xFF;
            break;
        case RT_H:
            ctx.registers.h = val & 0xFF;
            break;
        case RT_L:
            ctx.registers.l = val & 0xFF;
            break;

        case RT_AF:
            *((u16 *) &ctx.registers.a) = reverse(val);
            break;
        case RT_BC:
            *((u16 *) &ctx.registers.b) = reverse(val);
            break;
        case RT_DE:
            *((u16 *) &ctx.registers.d) = reverse(val);
            break;
        case RT_HL: {
            *((u16 *) &ctx.registers.h) = reverse(val);
            break;
        }

        case RT_PC:
            ctx.registers.pc = val;
            break;
        case RT_SP:
            ctx.registers.sp = val;
            break;
        case RT_NONE:
            break;
    }
}
cpu_registers *cpu_get_regs() {
    return &ctx.registers;
}