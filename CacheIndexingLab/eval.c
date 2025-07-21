/**************************************************************************
 * C S 429 EEL interpreter
 * 
 * eval.c - This file contains the skeleton of functions to be implemented by
 * you. When completed, it will contain the code used to evaluate an expression
 * based on its AST.
 * 
 * Copyright (c) 2021. S. Chatterjee, X. Shen, T. Byrd. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "ci.h"

/* Function interfaces */

/* Returns true if the given token is a binary operator and false otherwise */
extern bool is_binop(token_t);
/* Returns true if the given token is a unary operator and false otherwise */
extern bool is_unop(token_t);
/* It might be helpful to note that TOK_QUESTION is the only ternary operator. */

char *strrev(char *str);

/* infer_type() - set the type of a non-root node based on the types of children
 * Parameter: A node pointer, possibly NULL.
 * Return value: None.
 * Side effect: The type field of the node is updated.
 * (STUDENT TODO)
 */

static void infer_type(node_t *nptr) {
    // check running status - you can ignore this
    if (terminate || ignore_input) return;

    // Week 1 TODO: Implement a recursive post-order traversal of the AST. Remember to include a base case.
    if (nptr == NULL) {
        return;
    }
    // Traverse through the nodes
    for (int index = 0; index < 3; index++) {
        infer_type(nptr->children[index]);
    }
    // nptr->val.sval = 
    // malloc(sizeof())
    // str.cpy(destination, source) make deep copy
    switch (nptr->node_type) {
        // For each week, you will also need to include error checking for each type.
        // Week 1 TODO: Implement type inference for all operators on int and bool types.
        // Week 2 TODO: Extend type inference to handle operators on string types.
        // Week 3 TODO: Implement tpye evaluation for variables.
        case NT_INTERNAL:
            switch (nptr->tok) {
                // For reference, the identity (do nothing) operator is implemented for you.
                case TOK_IDENTITY:
                    nptr->type = nptr->children[0]->type;
                    break;
                // Ternary
                case TOK_QUESTION:
                    if (nptr->children[1]->type == nptr->children[2]->type){
                        if (nptr->children[1]->type == INT_TYPE){
                            nptr->type = INT_TYPE;
                        } else if (nptr->children[1]->type == BOOL_TYPE){
                            nptr->type = BOOL_TYPE;
                            // also for strings, if child 1 and 2 are strings
                        } else  if (nptr->children[1]->type == STRING_TYPE){
                            nptr->type = STRING_TYPE;
                        } else {
                            handle_error(ERR_TYPE);
                        }
                    } else {
                        handle_error(ERR_TYPE);
                    }
                    break;
                // Binary Operators
                case TOK_PLUS:
                    // string concatenation
                    if (nptr->children[0]->type == STRING_TYPE && nptr->children[1]->type == STRING_TYPE){
                        nptr->type = nptr->children[0]->type;                       
                    } else if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE){
                        nptr->type = nptr->children[0]->type; 
                    } else {
                        handle_error(ERR_TYPE);
                    }
                    break;
                case TOK_TIMES:
                    // make n copies of a string, concatenate a string to itself, n times
                    if (nptr->children[0]->type == STRING_TYPE && nptr->children[1]->type == INT_TYPE){
                        nptr->type = nptr->children[0]->type;      
                    } else  if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE){
                        nptr->type = nptr->children[0]->type;                       
                    } else {
                        handle_error(ERR_TYPE);
                    }
                    break;
                case TOK_BMINUS:            
                case TOK_DIV:
                case TOK_MOD:

                    if (nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE){
                        nptr->type = nptr->children[0]->type;
                        
                    } else {
                        handle_error(ERR_TYPE);
                    }
                    break;
                case TOK_AND:
                case TOK_OR:
                    if (nptr->children[0]->type == BOOL_TYPE && nptr->children[1]->type == BOOL_TYPE){
                        nptr->type = nptr->children[0]->type;
                    } else {
                        handle_error(ERR_TYPE);
                    }
                    break;
                case TOK_LT:
                case TOK_GT:
                    if ((nptr->children[0]->type == INT_TYPE && nptr->children[1]->type == INT_TYPE) ||  
                    (nptr->children[0]->type == STRING_TYPE && nptr->children[1]->type == STRING_TYPE)){
                        nptr->type = BOOL_TYPE;
                    } else {
                        handle_error(ERR_TYPE);
                    }                     
                    break;
                case TOK_EQ:
                    if (nptr->children[0]->type == nptr->children[1]->type){
                        nptr->type = BOOL_TYPE; // either true or false
                    } else {
                        handle_error(ERR_TYPE);
                    }
                    break;
                // Unary Operators
                case TOK_UMINUS:  // negative sign
                    if (nptr->children[0]->type == INT_TYPE){
                        nptr->type = nptr->children[0]->type;
                    } else  if (nptr->children[0]->type == STRING_TYPE){
                        nptr->type = STRING_TYPE;
                        // it can also be a string, just reverse it str.rev     
                    } else {
                        handle_error(ERR_TYPE);
                    }
                    break;
                case TOK_NOT:
                    if (nptr->children[0]->type == BOOL_TYPE){
                        nptr->type = BOOL_TYPE;
                    } else {
                        handle_error(ERR_TYPE);
                    }
                    break;
                
                default:
                    break;
            }
        case NT_LEAF:
            switch (nptr->tok){
                case TOK_ID:;
                    // get value from hash table
                    // assign value and type of nptr from what we get from the hash entry
                    entry_t *entry = get(nptr->val.sval);
                    free(nptr->val.sval);

                    if (entry != NULL){
                        if (entry->type == INT_TYPE){   
                            nptr->type = INT_TYPE;
                            nptr->val.ival = entry->val.ival;
                        } 
                        else if (entry->type == STRING_TYPE){
                            nptr->type = STRING_TYPE;
                            nptr->val.sval = malloc(strlen(entry->val.sval) + 1);
                            strcpy(nptr->val.sval, entry->val.sval);
                        }
                        else if (entry->type == BOOL_TYPE) {
                            nptr->type = BOOL_TYPE;
                            nptr->val.bval = entry->val.bval;
                        } 
                        else {
                            handle_error(ERR_TYPE);
                        }
                        nptr->type = entry->type;  
                    } else { 
                        handle_error(ERR_UNDEFINED);
                    }        
                    break;

                default:
                    break;
            }

        default:
            break;

    }

    return;
}

/* infer_root() - set the type of the root node based on the types of children
 * Parameter: A pointer to a root node, possibly NULL.
 * Return value: None.
 * Side effect: The type field of the node is updated. 
 */

static void infer_root(node_t *nptr) {
    if (nptr == NULL) return;
    // check running status
    if (terminate || ignore_input) return;

    // check for assignment
    if (nptr->type == ID_TYPE) {
        infer_type(nptr->children[1]);
    } else {
        for (int i = 0; i < 3; ++i) {
            infer_type(nptr->children[i]);
        }
        if (nptr->children[0] == NULL) {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        nptr->type = nptr->children[0]->type;
    }
    return;
}

/* eval_node() - set the value of a non-root node based on the values of children
 * Parameter: A node pointer, possibly NULL.
 * Return value: None.
 * Side effect: The val field of the node is updated.
 * (STUDENT TODO) 
 */

static void eval_node(node_t *nptr) {
    // check running status - you can ignore this.
    if (terminate || ignore_input) return;

    // Week 1 TODO: Implement a recursive post-order traversal of the AST. Remember to include a base case.
      if (nptr == NULL) {
        return;
    }
    // Traverse through the nodes
    // If it's a question mark, then only run one child
    // else evaluate all children
    if (nptr->tok == TOK_QUESTION){
        eval_node(nptr->children[0]);
        if (nptr->children[0]->val.bval){
            eval_node(nptr->children[1]);
        } else {
            eval_node(nptr->children[2]);
        }
    } else {
        // Binary or Unary Operator
        for (int index = 0; index < 2; index++) {
            eval_node(nptr->children[index]);
        }
    }
    
    switch (nptr->node_type) {
        case NT_INTERNAL:
            // Week 1 TODO: Implement evaluation for all operators on int and bool types.

            // Week 2 TODO: Extend evaluation to handle operators on string types.
            if (is_unop(nptr->tok)) {
                switch (nptr->tok) {
                    case TOK_UMINUS:
                        if (nptr->type == INT_TYPE){
                            nptr->val.ival = -(nptr->children[0]->val.ival);
                        } else if (nptr->type == STRING_TYPE){
                            // Make a deep copy of the string
                            // +1 because of the null terminator
                            nptr->val.sval = strrev(nptr->children[0]->val.sval);
                        } else {
                            handle_error(ERR_EVAL);
                        }
                        break;
                    case TOK_NOT:
                        nptr->val.bval = !(nptr->children[0]->val.bval);
                    default:
                        break;
                }
            }
            if (is_binop(nptr->tok)) {
                switch (nptr->tok) {
                    case TOK_PLUS:
                        // grab the length of one and two
                        if (nptr->type == INT_TYPE){
                            nptr->val.ival = nptr->children[0]->val.ival +  nptr->children[1]->val.ival;
                        } else if (nptr->type == STRING_TYPE){
                           // Make a deep copy of the string that's the combination of two strings
                            // +1 because of the null terminator
                            nptr->val.sval = malloc(strlen(nptr->children[0]->val.sval) + strlen(nptr->children[1]->val.sval) + 1); 
                            strcpy(nptr->val.sval, nptr->children[0]->val.sval);
                            strcat(nptr->val.sval, nptr->children[1]->val.sval);
                        } else {
                            handle_error(ERR_EVAL);
                        }
                         
                        break;
                    case TOK_BMINUS:
                        nptr->val.ival = nptr->children[0]->val.ival -  nptr->children[1]->val.ival;
                        break;
                    case TOK_TIMES:
                        if (nptr->type == INT_TYPE){
                            nptr->val.ival = nptr->children[0]->val.ival *  nptr->children[1]->val.ival;
                        } else if (nptr->type == STRING_TYPE){
                            // negative values 
                            if (nptr->children[1]->val.ival >= 0){
                                nptr->val.sval = malloc((strlen(nptr->children[0]->val.sval) * nptr->children[1]->val.ival) + 1);
                            } else if (nptr->children[1]->val.ival < 0) {
                                handle_error(ERR_EVAL);
                                break;
                            }
                             
                            // Edge case where we place the null terminator
                            if (nptr->children[1]->val.ival == 0){
                                nptr->val.sval[0] = '\0';
                                break;
                            } else {
                                strcpy(nptr->val.sval, nptr->children[0]->val.sval);
                                for (int index = 1; index < nptr->children[1]->val.ival; index++){
                                    strcat(nptr->val.sval, nptr->children[0]->val.sval);
                                }
                            }
                        } else {
                            handle_error(ERR_EVAL);
                        }
                        break;
                    case TOK_DIV:
                        if (nptr->children[1]->val.ival != 0){
                            nptr->val.ival = nptr->children[0]->val.ival /  nptr->children[1]->val.ival;
                        } else {
                            handle_error(ERR_EVAL);
                        }
                        break;
                    case TOK_MOD:
                        if (nptr->children[1]->val.ival != 0){
                            nptr->val.ival = nptr->children[0]->val.ival % nptr->children[1]->val.ival;
                        } else {
                            handle_error(ERR_EVAL);
                        }
                        break;
                    case TOK_AND:
                        nptr->val.bval = nptr->children[0]->val.bval & nptr->children[1]->val.bval;
                        break;
                    case TOK_OR:
                        nptr->val.bval = nptr->children[0]->val.bval || nptr->children[1]->val.bval;
                        break;
                    case TOK_LT:
                        if (nptr->children[0]->type == INT_TYPE){
                            nptr->val.bval = nptr->children[0]->val.ival < nptr->children[1]->val.ival;
                        } else if (nptr->children[0]->type == STRING_TYPE){                 
                            nptr->val.bval = strcmp (nptr->children[0]->val.sval, nptr->children[1]->val.sval) < 0;
                        } else {
                            handle_error(ERR_EVAL);
                        }
                         
                        break;
                    case TOK_GT:
                        if (nptr->children[0]->type == INT_TYPE){
                            nptr->val.bval = nptr->children[0]->val.ival > nptr->children[1]->val.ival;
                        } else if (nptr->children[0]->type == STRING_TYPE){                 
                            nptr->val.bval = strcmp (nptr->children[0]->val.sval, nptr->children[1]->val.sval) > 0;
                        } else {
                            handle_error(ERR_EVAL);
                        }
                        break;
                    case TOK_EQ:
                        if (nptr->children[0]->type == INT_TYPE){
                            nptr->val.bval = (nptr->children[0]->val.ival = nptr->children[1]->val.ival);
                        } else if (nptr->children[0]->type == STRING_TYPE){                 
                            nptr->val.bval = strcmp (nptr->children[0]->val.sval, nptr->children[1]->val.sval) == 0;
                        } else {
                            handle_error(ERR_EVAL);
                        }
                         
                        break;
                    default:
                        break;
                }
            }
            if (nptr->tok == TOK_QUESTION) {
                // check children[1] == int
                if (nptr->children[1]->type == INT_TYPE){
                    nptr->val.ival = nptr->children[0]->val.bval ? nptr->children[1]->val.ival : nptr->children[2]->val.ival;
                } else if (nptr->children[1]->type == BOOL_TYPE){
                    nptr->val.bval = nptr->children[0]->val.bval ? nptr->children[1]->val.bval : nptr->children[2]->val.bval;
                } else if (nptr->children[1]->type == STRING_TYPE){
                    // make a copy of the string
                    // nptr->val.sval = nptr->children[0]->val.bval ? nptr->children[1]->val.sval : nptr->children[2]->val.sval;
                    if (nptr->children[0]->val.bval){
                        nptr->val.sval = malloc(strlen(nptr->children[1]->val.sval)+ 1);
                        strcpy(nptr->val.sval, nptr->children[1]->val.sval);
                    } else {
                        nptr->val.sval = malloc(strlen(nptr->children[2]->val.sval)+ 1);
                        strcpy(nptr->val.sval, nptr->children[2]->val.sval);
                    }
                }
            }
            // For reference, the identity (do-nothing) operator has been implemented for you.
            if (nptr->tok == TOK_IDENTITY) {
                if (nptr->type == STRING_TYPE) {
                    // Week 2 TODO: You'll need to make a copy of the string.
                    nptr->val.sval = malloc(strlen(nptr->children[0]->val.sval)+ 1);
                    strcpy(nptr->val.sval, nptr->children[0]->val.sval);

                } else {
                    nptr->val.ival = nptr->children[0]->val.ival;
                }
            }
          
            break;
        case NT_LEAF:
            // // Change type and val of the nptr
            // if (nptr->tok == TOK_ID){
            //     entry_t *entry = get(nptr->val.sval);
            //     if (entry != NULL){
            //         nptr->type = entry->type;
            //         nptr->val = entry->val;
            //     }
                 

            // }
            break;
        default:
            break;
    }
    return;
}

/* eval_root() - set the value of the root node based on the values of children 
 * Parameter: A pointer to a root node, possibly NULL.
 * Return value: None.
 * Side effect: The val dield of the node is updated. 
 */

void eval_root(node_t *nptr) {
    if (nptr == NULL) return;
    // check running status
    if (terminate || ignore_input) return;

    // check for assignment
    if (nptr->type == ID_TYPE) {
        eval_node(nptr->children[1]);
        if (terminate || ignore_input) return;
        
        if (nptr->children[0] == NULL) {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        put(nptr->children[0]->val.sval, nptr->children[1]);
        return;
    }

    for (int i = 0; i < 2; ++i) {
        eval_node(nptr->children[i]);
    }
    if (terminate || ignore_input) return;
    
    if (nptr->type == STRING_TYPE) {
        (nptr->val).sval = (char *) malloc(strlen(nptr->children[0]->val.sval) + 1);
        if (! nptr->val.sval) {
            logging(LOG_FATAL, "failed to allocate string");
            return;
        }
        strcpy(nptr->val.sval, nptr->children[0]->val.sval);
    } else {
        nptr->val.ival = nptr->children[0]->val.ival;
    }
    return;
}

/* infer_and_eval() - wrapper for calling infer() and eval() 
 * Parameter: A pointer to a root node.
 * Return value: none.
 * Side effect: The type and val fields of the node are updated. 
 */

void infer_and_eval(node_t *nptr) {
    infer_root(nptr);
    eval_root(nptr);
    return;
}

/* strrev() - helper function to reverse a given string 
 * Parameter: The string to reverse.
 * Return value: The reversed string. The input string is not modified.
 * (STUDENT TODO)
 */

char *strrev(char *str) {
    // Week 2 TODO: Implement copying and reversing the string.
    char *reversed = malloc(strlen(str) + 1);
    int length = strlen(str);
    for (int index = 0; index < length; index++){
        reversed[index] = str[length - 1 - index];
    }
    reversed[length] = '\0';
    return reversed;
}

/* cleanup() - frees the space allocated to the AST
 * Parameter: The node to free.
 */
void cleanup(node_t *nptr) {
    // Week 2 TODO: Recursively free each node in the AST 
    if (nptr == NULL){
        return;
    }
    for (int index = 0; index < 3; index++) {
        cleanup(nptr->children[index]);
    }
    // if the node has a string
    if ((nptr->type == STRING_TYPE || nptr->type == ID_TYPE) && nptr->val.sval != NULL){
        
        free(nptr->val.sval);
            
    }
    free(nptr);
    return;
}


