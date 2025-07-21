/**************************************************************************
 * C S 429 system emulator
 * 
 * hw_elts.h - A header file defining the prototypes of the hardware elements
 * used in the pipeline simulator.
 * 
 * All of the hardware elements are of return type void, so they write return
 * values in their out parameters (which must therefore be passed by reference).
 * The in and out parameters are indicated in the prototypes below.
 * 
 * Copyright (c) 2022, 2023. 
 * Author: S. Chatterjee. 
 * All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#ifndef _HW_ELTS_H_
#define _HW_ELTS_H_
#include <stdint.h>
#include <stdbool.h>
#include "instr.h"
#include "instr_pipeline.h"

/* Instruction memory. */
// Retrieving an INSTRUCTION from memory
// imem_addr  = The address of the instruction to retrieve
// imem_rval  = The address where you want the value to be stored at after
//              retrieving the function from memory.
// imem_error = Set to TRUE if there's any issue regarding the retrieval of
//              instruction memory.

extern comb_logic_t imem(uint64_t imem_addr,                                                    // in, data
                         uint32_t *imem_rval, bool *imem_err);                                  // out

/* 
 * Register file.
 * 
 * Remember that the inputs for writing are fed by the instruction in Writeback at this cycle,
 * while the inputs for reading are fed by the instruction in Decode at this cycle.
 * THESE ARE TWO DIFFERENT INSTRUCTIONS.
 * 
 * Register number 31 is always interpreted as SP.
 * Register number 32 and higher is interpreted as ZR.
 */

/* regfile function SUMMARY */

// Case 1: Read from a register
    // w_enable = FALSE
    // src1 = source/input register 1
    // src2 = source/input register 2
    // *val_a = value of what's in register src1
    // *val_b = value of what's in register src2

// Case 2: Write to a register
    // w_enable = TRUE
    // dst = destination register (register that you want to write to)
    // val_w = the value that you want to write to that register
    // NOTE: Try to give valid values for src1, src2, val_a, and val_b even though
    // you're writing -- it'll do the same thing as reading but prevent null pointer
    // errors
        // src1 = source/input register 1
        // src2 = source/input register 2
        // *val_a = value of what's in register src1
        // *val_b = value of what's in register src2

extern comb_logic_t regfile(uint8_t src1, uint8_t src2, uint8_t dst, uint64_t val_w,            // in, data
                            // bool src1_31isSP, bool src2_31isSP, bool dst_31isSP, 
                            bool w_enable, // in, control
                            uint64_t *val_a, uint64_t *val_b);                                  // out



/* Arithmetic and logic unit SUMMARY. Includes the NZCV register. */

// Does each of the operations ALUop on vala and valb and puts the result
// in val_e. (vala ALUop valb)

// If you set set_CC to true, it updates the conditional codes for the functions
// (ZF -- zero flag, SF -- sign flag, OF -- overflow flag)

// cond corresponds to the condition of the instruction. If you have b.LE,
// cond would correspond to LE
    // cond_val will be updated based on whether or not that condition holds.
    // If it does, *cond_val = TRUE, otherwise, *cond_val = FALSE

// Not too sure what the purpose of alu_valhw is -- set it to 0 for now when
// using it until you find a purpose for it.
    // For the ADD, PLUS, and MOV instructions, it shifts valb by valhw units
    // before performing the operation. Why?

// uint64_t val_e;
// bool cond_val;
// alu(vala, valb, valhw, ALUop, set_CC, cond, &val_e, &cond_val);

extern comb_logic_t alu(uint64_t alu_vala, uint64_t alu_valb, uint8_t alu_valhw,               // in, data
                        alu_op_t ALUop, bool set_CC, cond_t cond,                               // in, control
                        uint64_t *val_e, bool *cond_val);                                       // out

/* Data memory SUMMARY. */

// Allows you to retrieve and write things to memory.
// Case 1: Read from memory
    // dmem_addr = The memory address you want to read from
    // dmem_read = TRUE
    // dmem_rval = The address you want the value of the memory to be read
    //             to. *dmem_rval = M[dmem_addr]
    // dmem_err  = Determines whether there was an error when working with the
    //             memory.

// Case 2: Write to memory (Imagine it like M[dmem_addr] = dmem_wval)
    // dmem_addr  = The memory address you want to write to
    // dmem_wval  = The value that you want to write to that address
    // dmem_write = TRUE
    // dmem_err   = Determines whether there was an error when working with the
    //              memory.

extern comb_logic_t dmem(uint64_t dmem_addr, uint64_t dmem_wval,                                // in, data
                         bool dmem_read, bool dmem_write,                                       // in, control
                         uint64_t *dmem_rval, bool *dmem_err);                                   // out
#endif