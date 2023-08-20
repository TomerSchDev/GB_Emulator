//
// Created by tomer on 8/20/23.
//

#ifndef GBEMU_STACK_H
#define GBEMU_STACK_H
#pragma once
#include "common.h"

void stack_push(u8 data);
void stack_push16(u16 data);

u8 stack_pop();
u16 stack_pop16();
#endif //GBEMU_STACK_H
