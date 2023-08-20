#pragma once

#include <common.h>
#include "emu.h"
#include "bus.h"

#include "instructions.h"

typedef struct {
    u8 a;
    u8 f;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 pc;
    u16 sp;
} cpu_registers;
cpu_registers *cpu_get_regs();
typedef struct {
    cpu_registers registers;
    //current fetch
    u16 fetched_data;
    u16 mem_dest;
    bool dest_is_mem;
    u8 cur_opcode;
    instruction *cur_inst;
    bool halted;
    bool stepping;
    u8 ie_register;

    bool int_master_enabled;

} cpu_context;

void cpu_init();

bool cpu_step();

void fetch_data();
void cpu_set_reg(reg_type rt, u16 val);
void cpu_set_flags(cpu_context *ctx, char z, char n, char h, char c) ;

typedef void (*IN_PROC)(cpu_context *);
u8 cpu_get_ie_register();
void cpu_set_ie_register(u8 n);
IN_PROC inst_get_processor(in_type type);

#define CPU_FLAG_Z BIT(ctx->registers.f, 7)
#define CPU_FLAG_C BIT(ctx->registers.f, 4)

u16 cpu_read_reg(reg_type rt);;