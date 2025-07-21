/**************************************************************************
 * C S 429 system emulator
 * 
 * instr_Execute.c - Execute stage of instruction processing pipeline.
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

extern bool X_condval;

extern comb_logic_t copy_m_ctl_sigs(m_ctl_sigs_t *, m_ctl_sigs_t *);
extern comb_logic_t copy_w_ctl_sigs(w_ctl_sigs_t *, w_ctl_sigs_t *);

/*
 * Execute stage logic.
 * STUDENT TO-DO:
 * Implement the execute stage.
 * 
 * Use in as the input pipeline register,
 * and update the out pipeline register as output.
 * 
 * You will need the following helper functions:
 * copy_m_ctl_signals, copy_w_ctl_signals, and alu.
 */

comb_logic_t execute_instr(x_instr_impl_t *in, m_instr_impl_t *out) {
    copy_m_ctl_sigs(&out->M_sigs, &in->M_sigs);
    copy_w_ctl_sigs(&out->W_sigs, &in->W_sigs);
    out->dst = in->dst;
    out->op = in->op;

    //choose val b, either val b or imm
    uint64_t selected_val_b = in->val_b;
    if(!in->X_sigs.valb_sel) {
        selected_val_b = in->val_imm;
    }
    if(in->op == OP_BL) {
        in->val_a = in->seq_succ_PC;
    }

    //val a, val b, val hw, aluop, setCC, cond, *vale, *condval
    out->cond_holds = true;
    alu(in->val_a, selected_val_b, in->val_hw, in->ALU_op, in->X_sigs.set_CC, in->cond, &out->val_ex, &out->cond_holds);
    out->val_b = in->val_b;
    out->print_op = in->print_op;
    M_in->val_ex = out->val_ex;
    out->seq_succ_PC = in->seq_succ_PC;
   // printf("EXECUTE SEQ SUCC: %x\n", out->seq_succ_PC);
    
    if(in->print_op != OP_B_COND && in->op == OP_B_COND) {
        out->op = in->print_op;
    }

    out->status = in->status;
    X_condval = out->cond_holds;
    return;
}
