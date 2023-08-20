//
// Created by tomer on 8/20/23.
//
#include "cpu.h"
#include "emu.h"
#include "stack.h"

// Processing CPU instructions
static void proc_none(cpu_context *ctx) {
    printf("Invalid Instraction! \n");
    exit(-7);
}

static bool is_16_bit(reg_type rt) {
    return rt >= RT_AF;
}

static void proc_ld(cpu_context *ctx) {
    if (ctx->dest_is_mem) {
        //LD (BC), A for instance...

        if (is_16_bit(ctx->cur_inst->reg_2)) {
            //if 16 bit register...
            emu_cycles(1);
            bus_write16(ctx->mem_dest, ctx->fetched_data);
        } else {
            bus_write(ctx->mem_dest, ctx->fetched_data);
        }

        return;
    }
    if (ctx->cur_inst->mode == AM_HL_SPR) {
        u8 hflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xF) +
                   (ctx->fetched_data & 0xF) >= 0x10;

        u8 cflag = (cpu_read_reg(ctx->cur_inst->reg_2) & 0xFF) +
                   (ctx->fetched_data & 0xFF) >= 0x100;

        cpu_set_flags(ctx, 0, 0, hflag, cflag);
        cpu_set_reg(ctx->cur_inst->reg_1,
                    cpu_read_reg(ctx->cur_inst->reg_2) + (char) ctx->fetched_data);

        return;
    }

    cpu_set_reg(ctx->cur_inst->reg_1, ctx->fetched_data);

}

static void proc_nop(cpu_context *ctx) {
    //TODO...
}

static bool check_cond(cpu_context *ctx) {
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch (ctx->cur_inst->cond) {
        case CT_NONE:
            return true;
        case CT_C:
            return c;
        case CT_NC:
            return !c;
        case CT_Z:
            return z;
        case CT_NZ:
            return !z;
    }
    return false;
}


static void proc_di(cpu_context *ctx) {
    ctx->int_master_enabled = false;
}

void cpu_set_flags(cpu_context *ctx, char z, char n, char h, char c) {
    if (z != -1) {
        BIT_SET(ctx->registers.f, 7, z);
    }
    if (n != -1) {
        BIT_SET(ctx->registers.f, 6, n);
    }
    if (h != -1) {
        BIT_SET(ctx->registers.f, 5, h);
    }
    if (c != -1) {
        BIT_SET(ctx->registers.f, 4, c);
    }

}

static void proc_xor(cpu_context *ctx) {
    ctx->registers.a ^= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->registers.a, 0, 0, 0);
}

static void proc_ldh(cpu_context *ctx) {
    if (ctx->cur_inst->reg_1 == RT_A) {
        cpu_set_reg(ctx->cur_inst->reg_1, bus_read(0xFF00 | ctx->fetched_data));
    } else {
        bus_write(0xFF00 | ctx->fetched_data, ctx->registers.a);
    }
    emu_cycles(1);
}

static void proc_pop(cpu_context *ctx) {
    u16 lo = stack_pop();
    emu_cycles(1);
    u16 hi = stack_pop();
    emu_cycles(1);

    u16 n = (hi << 8) | lo;

    cpu_set_reg(ctx->cur_inst->reg_1, n);

    if (ctx->cur_inst->reg_1 == RT_AF) {
        cpu_set_reg(ctx->cur_inst->reg_1, n & 0xFFF0);
    }
}

static void goto_addr(cpu_context *ctx, u16 addr, bool pushPC) {
    if (check_cond(ctx)) {
        if (pushPC) {
            emu_cycles(2);
            stack_push16(ctx->registers.pc);
        }
        ctx->registers.pc = addr;
        emu_cycles(1);
    }
}

static void proc_call(cpu_context *ctx) {
    goto_addr(ctx, ctx->fetched_data, true);
}

static void proc_jp(cpu_context *ctx) {
    goto_addr(ctx, ctx->fetched_data, false);
}

static void proc_ret(cpu_context *ctx) {
    if (ctx->cur_inst->cond != CT_NONE) {
        emu_cycles(1);
    }
    if (check_cond(ctx)) {
        u16 lo = stack_pop();
        emu_cycles(1);
        u16 hi = stack_pop();
        emu_cycles(1);
        u16 n = (hi << 8) | lo;
        ctx->registers.pc = n;
        emu_cycles(1);
    }
}

static void proc_reti(cpu_context *ctx) {
    ctx->int_master_enabled = true;
    proc_ret(ctx);
}

static void proc_rst(cpu_context *ctx) {
    goto_addr(ctx, ctx->cur_inst->param, true);
}

static void proc_jr(cpu_context *ctx) {
    char rel = (char) (ctx->fetched_data & 0xFF);
    u16 addr = ctx->registers.pc + rel;
    goto_addr(ctx, addr, false);
}

static void proc_push(cpu_context *ctx) {
    u16 hi = (cpu_read_reg(ctx->cur_inst->reg_1) >> 8) & 0xFF;
    emu_cycles(1);
    stack_push(hi);

    u16 lo = cpu_read_reg(ctx->cur_inst->reg_1) & 0xFF;
    emu_cycles(1);
    stack_push(lo);

    emu_cycles(1);
}

static void proc_inc(cpu_context *ctx) {
    u16 val = cpu_read_reg(ctx->cur_inst->reg_1) + 1;
    if (is_16_bit(ctx->cur_inst->reg_1)) {
        emu_cycles(1);
    }
    if (ctx->cur_inst->reg_1 == RT_HL && ctx->cur_inst->mode == AM_MR) {
        val = bus_read(cpu_read_reg(RT_HL)) + 1;
        val &= 0xFF;
        bus_write(cpu_read_reg(RT_HL), val);
    } else {
        cpu_set_reg(ctx->cur_inst->reg_1, val);
        val = cpu_read_reg(ctx->cur_inst->reg_1);
    }
    if ((ctx->cur_opcode & 0x03) == 0x03) {
        return;
    }
    cpu_set_flags(ctx,val==0,0,(val&0x0F)==0,-1);
}
static void proc_dec(cpu_context *ctx) {
    u16 val = cpu_read_reg(ctx->cur_inst->reg_1) - 1;
    if (is_16_bit(ctx->cur_inst->reg_1)) {
        emu_cycles(1);
    }
    if (ctx->cur_inst->reg_1 == RT_HL && ctx->cur_inst->mode == AM_MR) {
        val = bus_read(cpu_read_reg(RT_HL)) - 1;
        bus_write(cpu_read_reg(RT_HL), val);
    } else {
        cpu_set_reg(ctx->cur_inst->reg_1, val);
        val = cpu_read_reg(ctx->cur_inst->reg_1);
    }
    if ((ctx->cur_opcode & 0x03) == 0x03) {
        return;
    }
    cpu_set_flags(ctx,val==0,1,(val&0x0F)==0x0F,-1);
}
static IN_PROC procrssors[] = {
        [IN_NONE]=proc_none,
        [IN_NOP]=proc_nop,
        [IN_LD]=proc_ld,
        [IN_JP]=proc_jp,
        [IN_DI]=proc_di,
        [IN_XOR]=proc_xor,
        [IN_LDH]=proc_ldh,
        [IN_POP]=proc_pop,
        [IN_PUSH]=proc_push,
        [IN_CALL]=proc_call,
        [IN_RET]=proc_ret,
        [IN_JR]=proc_jr,
        [IN_RETI]=proc_reti,
        [IN_RST]=proc_rst,
        [IN_INC]=proc_inc,
        [IN_DEC]=proc_dec

};

IN_PROC inst_get_processor(in_type type) {
    return procrssors[type];
}