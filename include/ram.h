//
// Created by tomer on 8/20/23.
//

#ifndef GBEMU_RAM_H
#define GBEMU_RAM_H
#pragma once
#include "common.h"
u8 wram_read(u16 address);
void wram_write(u16 address,u8 value);


u8 hram_read(u16 address);
void hram_write(u16 address,u8 value);



#endif //GBEMU_RAM_H
