/**************************************************************************
 * C S 429 system emulator
 * 
 * instr_Memory.c - Memory stage of instruction processing pipeline.
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

extern comb_logic_t copy_w_ctl_sigs(w_ctl_sigs_t *, w_ctl_sigs_t *);

/*
 * Memory stage logic.
 * STUDENT TO-DO:
 * Implement the memory stage.
 * 
 * Use in as the input pipeline register,
 * and update the out pipeline register as output.
 * 
 * You will need the following helper functions:
 * copy_w_ctl_signals and dmem.
 */

comb_logic_t memory_instr(m_instr_impl_t *in, w_instr_impl_t *out) {

    //dmem addr, dmem wval, dmem read, bool dmem write, *dmem rval, *dmem err
    copy_w_ctl_sigs(&out->W_sigs, &in->W_sigs);
    out->dst = in->dst;
    out->val_ex = in->val_ex;
    out->print_op = in->print_op;
    out->op = in->op;
    out->val_b = in->val_b;
    bool error;
    dmem(in->val_ex, in->val_b, in->M_sigs.dmem_read, in->M_sigs.dmem_write, &out->val_mem, &error);
    if(error && (in->M_sigs.dmem_read || in->M_sigs.dmem_write) ) {
        out->status = STAT_ADR;
    } else {
        out->status = in->status;
    }

    return;
}
