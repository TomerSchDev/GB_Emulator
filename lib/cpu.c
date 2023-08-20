#include <cpu.h>
cpu_context ctx = {0};

void cpu_init() {
    ctx.registers.pc = 0x100;
    ctx.registers.a = 0x01;
    ctx.halted=false;
}

static void fetch_instruction() {
    ctx.cur_opcode = bus_read(ctx.registers.pc++);
    ctx.cur_inst = instruction_by_opcode(ctx.cur_opcode);
    if(ctx.cur_inst==NULL){
        printf("Unknown instraction! %02X\n",ctx.cur_opcode);
        exit(-7);
    }
}


static void execute() {
    IN_PROC proc = inst_get_processor(ctx.cur_inst->type);
    if (!proc) {
        NO_IMPL
    }
    proc(&ctx);
}
bool cpu_step() {

    if (!ctx.halted) {
        u16 pc = ctx.registers.pc;

        fetch_instruction();
        fetch_data();

        printf("%04X: %-7s (%02X %02X %02X) A: %02X BC: %02X%02X DE: %02X%02X HL: %02X%02X\n",
               pc, inst_name(ctx.cur_inst->type), ctx.cur_opcode,
               bus_read(pc + 1), bus_read(pc + 2), ctx.registers.a, ctx.registers.b, ctx.registers.c,
               ctx.registers.d, ctx.registers.e, ctx.registers.h, ctx.registers.l);


        if (ctx.cur_inst == NULL) {
            printf("Unknown Instruction! %02X\n", ctx.cur_opcode);
            exit(-7);
        }

        execute();
    }

    return true;
}
u8 cpu_get_ie_register() {
    return ctx.ie_register;
}

void cpu_set_ie_register(u8 n) {
    ctx.ie_register = n;
}
