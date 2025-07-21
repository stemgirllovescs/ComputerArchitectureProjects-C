/**************************************************************************
 * C S 429 system emulator
 * 
 * instr_Fetch.c - Fetch stage of instruction processing pipeline.
 **************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "err_handler.h"
#include "instr.h"
#include "instr_pipeline.h"
#include "machine.h"
#include "hw_elts.h"

extern machine_t guest;
extern mem_status_t dmem_status;

extern uint64_t F_PC;

/*
 * Select PC logic.
 * STUDENT TO-DO:
 * Write the next PC to *current_PC.
 */

static comb_logic_t 
select_PC(uint64_t pred_PC,                                     // The predicted PC
          opcode_t D_opcode, uint64_t val_a,                    // Possible correction from RET
          opcode_t M_opcode, bool M_cond_val, uint64_t seq_succ,// Possible correction from B.cond
          uint64_t *current_PC) {
    /* 
     * Students: Please leave this code
     * at the top of this function. 
     * You may modify below it. 
     */
    if (D_opcode == OP_RET && val_a == RET_FROM_MAIN_ADDR) {
        *current_PC = 0; // PC can't be 0 normally.
        return;
    }    
    // Modify starting here.
    if(M_opcode == OP_B_COND && !M_cond_val) {
        *current_PC = seq_succ;
        return;
    } //happens before RET in case both true
    if(D_opcode == OP_RET) {
        *current_PC = val_a;
        return;
    }
    //otherwise, pass predPC
    *current_PC = pred_PC;
    return;
}

/*
 * Predict PC logic. Conditional branches are predicted taken.
 * STUDENT TO-DO:
 * Write the predicted next PC to *predicted_PC
 * and the next sequential pc to *seq_succ.
 */

static comb_logic_t 
predict_PC(uint64_t current_PC, uint32_t insnbits, opcode_t op, 
           uint64_t *predicted_PC, uint64_t *seq_succ) {
    /* 
     * Students: Please leave this code
     * at the top of this function. 
     * You may modify below it. 
     */
    if (!current_PC) {
        return; // We use this to generate a halt instruction.
    }
    // Modify starting here.
    if(op == OP_B_COND) {
        //predict taken
  //     label Is the program label to be conditionally branched to. Its offset from the address of this instruction,
  // in the range +/-1MB, is encoded as "imm19" times 4. (from arm manual)
        *seq_succ = current_PC + 4;
        *predicted_PC = current_PC + bitfield_s64(insnbits, 5, 19) * 4;
        
        return;
    }
    else if(op == OP_B || op == OP_BL) {
        //  (insnbits & 0x3FFFFFF) * 4;
        *predicted_PC = bitfield_s64(insnbits, 0, 26) * 4;
        *predicted_PC += current_PC;
        *seq_succ = current_PC + 4;
        return;
    }
    
    *predicted_PC = current_PC + 4;
    *seq_succ = current_PC + 4;
    return;
}

/*
 * Helper function to recognize the aliased instructions:
 * LSL, LSR, CMP, and TST. We do this only to simplify the 
 * implementations of the shift operations (rather than having
 * to implement UBFM in full).
 */

static
void fix_instr_aliases(uint32_t insnbits, opcode_t *op) {
    if(*op == OP_UBFM) {
        //check if lsl or lsr
        if(bitfield_u32(insnbits, 10, 6) == 0x3F) { //lsr
            *op = OP_LSR;
        }
        else {
            *op = OP_LSL;
        }
    }
    return;
}


//im error set stat ins
//otherwise stat ok

//decode: invalid instr: bogus op code, set to stat ins
//memory can get a dmem err, stat adr
//halt in fetch, stat hlt
//coming in with adk and get dmem=


/*
 * Fetch stage logic.
 * STUDENT TO-DO:
 * Implement the fetch stage.
 * 
 * Use in as the input pipeline register,
 * and update the out pipeline register as output.
 * Additionally, update F_PC for the next
 * cycle's predicted PC.
 * 
 * You will also need the following helper functions:
 * select_pc, predict_pc, and imem.
 */

comb_logic_t fetch_instr(f_instr_impl_t *in, d_instr_impl_t *out) {
    bool imem_err = 0;
    uint64_t current_PC;
    //predicted pc, D_opcode, val_a, M_opcode, M_cond_val, seq_succ, current_pc
    select_PC(in->pred_PC, X_out->op, X_out->val_a, M_out->op, M_out->cond_holds, M_out->seq_succ_PC, &current_PC);
    

    
    /* 
     * Students: This case is for generating HLT instructions
     * to stop the pipeline. Only write your code in the **else** case. 
     */
    if (!current_PC) {
        out->insnbits = 0xD4400000U;
        out->op = OP_HLT;
        out->print_op = OP_HLT;
        imem_err = false;
    }
    else {
        if(in->status != STAT_BUB) {
            in->status = STAT_AOK;
            out->status = in->status;
        }
        // printf("%x ", current_PC);
        //fetch isntruction from the imem
        // in->status = STAT_AOK;
        // out->status = in->status;
        uint32_t instruction;
        imem(current_PC, &instruction, &imem_err);


        if(imem_err) {
            in->status = STAT_INS;
            out->status = STAT_INS;
        }

        out->insnbits = instruction;
     //   printf("%x %x %x \n", out->insnbits, current_PC, in->pred_PC);
        uint32_t opcode = itable[bitfield_u32(out->insnbits, 21, 11)];

        if( (opcode == OP_ERROR) ) {
            in->status = STAT_INS;
            out->status = STAT_INS;
        }

        fix_instr_aliases(instruction, (opcode_t*) &opcode);
        out->op = opcode;
        out->print_op = out->op;
        if(out->op == OP_SUBS_RR && bitfield_u32(instruction, 0, 5) == 31) {
            out->op = OP_CMP_RR;
            out->print_op = OP_CMP_RR;
        }
        else if(out->op == OP_ANDS_RR && bitfield_u32(instruction, 0, 5) == 31) {
            out->print_op = OP_TST_RR;
            out->op = OP_TST_RR;
        }
        predict_PC(current_PC, instruction, out->op, &F_PC, &out->seq_succ_PC);
        //printf("%x\n", current_PC);
        if(out->status != STAT_INS) {
            out->status = in->status;
        }
        // printf("%x %x %x \n", out->insnbits, current_PC, in->pred_PC);

    }
    if (out->op == OP_HLT) {
        in->status = STAT_HLT;
        out->status = STAT_HLT;
    }
    return;
}
