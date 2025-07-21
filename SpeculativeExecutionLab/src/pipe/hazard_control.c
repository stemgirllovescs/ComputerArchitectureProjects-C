/**************************************************************************
 * C S 429 system emulator
 * 
 * Bubble and stall checking logic.
 * STUDENT TO-DO:
 * Implement logic for hazard handling.
 * 
 * handle_hazards is called from proc.c with the appropriate
 * parameters already set, you must implement the logic for it.
 * 
 * You may optionally use the other three helper functions to 
 * make it easier to follow your logic.
 **************************************************************************/ 

#include "machine.h"

extern machine_t guest;
extern mem_status_t dmem_status;

/* Use this method to actually bubble/stall a pipeline stage.
 * Call it in handle_hazards(). Do not modify this code. */
void pipe_control_stage(proc_stage_t stage, bool bubble, bool stall) {
    pipe_reg_t *pipe;
    switch(stage) {
        case S_FETCH: pipe = F_instr; break;
        case S_DECODE: pipe = D_instr; break;
        case S_EXECUTE: pipe = X_instr; break;
        case S_MEMORY: pipe = M_instr; break;
        case S_WBACK: pipe = W_instr; break;
        default: printf("Error: incorrect stage provided to pipe control.\n"); return;
    }
    if (bubble && stall) {
        printf("Error: cannot bubble and stall at the same time.\n");
        pipe->ctl = P_ERROR;
    }
    // If we were previously in an error state, stay there.
    if (pipe->ctl == P_ERROR) return;

    if (bubble) {
        pipe->ctl = P_BUBBLE;
    }
    else if (stall) {
        pipe->ctl = P_STALL;
    }
    else { 
        pipe->ctl = P_LOAD;
    }
}

// Stalling
bool check_ret_hazard(opcode_t D_opcode) {
    return D_opcode == OP_RET; //simple enough
}

// Squashing
bool check_mispred_branch_hazard(opcode_t X_opcode, bool X_condval) {
    return X_opcode == OP_B_COND && !X_condval;
}

// Data hazard
bool check_load_use_hazard(opcode_t D_opcode, uint8_t D_src1, uint8_t D_src2,
                           opcode_t X_opcode, uint8_t X_dst) {
    //D_opcode not in slides for what it should equally, but perhaps its also load hence
    //back to back load use
     return  X_opcode == OP_LDUR && (X_dst == D_src1 || X_dst == D_src2);
}

comb_logic_t handle_hazards(opcode_t D_opcode, uint8_t D_src1, uint8_t D_src2, 
                            opcode_t X_opcode, uint8_t X_dst, bool X_condval) {
    /* Students: Change this code */
    bool ret_hazard = check_ret_hazard(D_opcode);
    bool mispred_branch = check_mispred_branch_hazard(X_opcode, X_condval);
    bool load_use = check_load_use_hazard(D_opcode, D_src1, D_src2, X_opcode,X_dst);
     
    //if back to back load use, 
    // (F, D, X, M, W) = (Stall, Stall, Bubble, Normal, Normal)
    // mispredicted: (F, D, X, M, W) = (Normal, Bubble, Bubble, Normal, Normal)
    //ret: (F, D, X, M, W) = (Normal, Bubble, Normal, Normal, Normal)

    /* WARNING: both condition inputs can't be true at the same time */
    bool f_stall = F_out->status == STAT_HLT || F_out->status == STAT_INS;
    bool d_stall = D_out->status == STAT_HLT || D_out->status == STAT_INS;
    bool x_stall = X_out->status == STAT_HLT || X_out->status == STAT_INS;
    bool m_stall = M_out->status == STAT_HLT || M_out->status == STAT_INS || M_out->status == STAT_ADR;
    bool w_stall = W_out->status == STAT_HLT || W_out->status == STAT_INS || M_out->status == STAT_ADR;
    

    if(X_out->status == STAT_INS) {
        X_out->X_sigs.set_CC = false;
        X_out->W_sigs.w_enable = false;
    }
    
    if(W_out->status == STAT_ADR) {
        W_out->W_sigs.w_enable = false;
    }
    if(M_in->status == STAT_INS) {
        X_in->X_sigs.set_CC = false;
        pipe_control_stage(S_MEMORY, true, false);
    }
    if(W_in->status == STAT_INS) {
        pipe_control_stage(S_WBACK, false, true);
    }


    if(dmem_status == IN_FLIGHT) {
        pipe_control_stage(S_FETCH, false, true);
        pipe_control_stage(S_DECODE, false, true);
        pipe_control_stage(S_EXECUTE, false, true);
        pipe_control_stage(S_MEMORY, false, true);
        pipe_control_stage(S_WBACK, true, false);

        
    }


    
    else {
        if(w_stall) {
            pipe_control_stage(S_FETCH, false, true);
            pipe_control_stage(S_DECODE, false, true);
            pipe_control_stage(S_EXECUTE, false, true);
            pipe_control_stage(S_MEMORY, false, true);
            pipe_control_stage(S_WBACK, false, true);
                
        } else if(m_stall) {
            pipe_control_stage(S_FETCH, false, true);
            pipe_control_stage(S_DECODE, false, true);
            pipe_control_stage(S_EXECUTE, false, true);
            pipe_control_stage(S_MEMORY, false, true);
            pipe_control_stage(S_WBACK, false, false);
        } else if(x_stall) {
            pipe_control_stage(S_FETCH, false, true);
            pipe_control_stage(S_DECODE, false, true);
            pipe_control_stage(S_EXECUTE, false, true);
            pipe_control_stage(S_MEMORY, false, false);
            pipe_control_stage(S_WBACK, false, false);
        } else if(d_stall) {
            pipe_control_stage(S_FETCH, false, true);
            pipe_control_stage(S_DECODE, false, true);
            pipe_control_stage(S_EXECUTE, false, false);
            pipe_control_stage(S_MEMORY, false, false);
            pipe_control_stage(S_WBACK, false, false);
        }

        else {
            if(load_use) {
                pipe_control_stage(S_FETCH, false, true); //stall
                pipe_control_stage(S_DECODE, false, true); //stall
                pipe_control_stage(S_EXECUTE, true, false); //bubble
            } else if(mispred_branch) {
                pipe_control_stage(S_FETCH, false, f_stall); //normal
                pipe_control_stage(S_DECODE, true, false); //bubble
                pipe_control_stage(S_EXECUTE, true, false); //bubble
            } else if(ret_hazard) {
                pipe_control_stage(S_FETCH, false, f_stall); //normal
                pipe_control_stage(S_DECODE, true, false); //bubble
                pipe_control_stage(S_EXECUTE, false, false); //normal
            }
            else { //good to go
                pipe_control_stage(S_FETCH, false, f_stall);
                pipe_control_stage(S_DECODE, false, false);
                pipe_control_stage(S_EXECUTE, false, false);
            }
            pipe_control_stage(S_MEMORY, false, false);
            pipe_control_stage(S_WBACK, false, false);

            
        }
    }

}



