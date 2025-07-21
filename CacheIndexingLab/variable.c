/**************************************************************************
 * C S 429 EEL interpreter
 * 
 * variable.c - This file contains the skeleton of functions to be implemented
 * for EEL-2. When completed, it will contain the code to maintain a hashtable
 * for defined variables.
 * 
 * Work on it only after finishing EEL-0 and EEL-1.
 * 
 * Copyright (c) 2021. S. Chatterjee, X. Shen, T. Byrd. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "ci.h"

table_t *var_table = NULL;
static char *bool_print[] = {"false", "true"};

void init_table(void) {
    var_table = (table_t *) calloc(1, sizeof(table_t));
    if (! var_table) {
        logging(LOG_FATAL, "failed to allocate table");
        return;
    }
    var_table->entries = (entry_t **) calloc(CAPACITY, sizeof(entry_t *));
    if (! var_table->entries) {
        free(var_table);
        logging(LOG_FATAL, "failed to allocate entries");
    }
    return;
}

void delete_entry(entry_t *eptr) {
    if (! eptr) return;
    if (eptr->type == STRING_TYPE) {
        free(eptr->val.sval);
    }
    free(eptr->id);
    free(eptr);
    return;
}

void delete_entries(entry_t *eptr) {
    if (! eptr) return;
    delete_entries(eptr->next);
    delete_entry(eptr);
    return;
}

void delete_table(void) {
    if (! var_table) return;

    for (int i = 0; i < CAPACITY; ++i) {
        delete_entries(var_table->entries[i]);
    }
    free(var_table->entries);
    free(var_table);
    return;
}

/* Pre-defined hash function to index variables by their names. */
unsigned long hash_function(char *s) {
    unsigned long i = 0;
    for (int j=0; s[j]; j++)
        i += s[j];
    return i % CAPACITY;
}

/* init_entry() - provided entry constructor
 * Parameters: Variable name, pointer to a node.
 * Return value: An allocated entry. */
entry_t * init_entry(char *id, node_t *nptr) {
    if (nptr == NULL) {
        logging(LOG_FATAL, "failed to allocate entry");
        return NULL;
    }
    entry_t *eptr = (entry_t *) calloc(1, sizeof(entry_t));
    if (! eptr) {
        logging(LOG_FATAL, "failed to allocate entry");
        return NULL;
    }
    eptr->id = (char *) malloc(strlen(id) + 1);
    if (! id) {
        logging(LOG_FATAL, "failed to allocate entry id");
        free(eptr);
        return NULL;
    }
    strcpy(eptr->id, id);
    eptr->type = nptr->type;
    if (eptr->type == STRING_TYPE) {
        (eptr->val).sval = (char *) malloc(strlen(nptr->val.sval) + 1);
        if (! eptr->val.sval) {
            logging(LOG_FATAL, "failed to allocate string");
            free(eptr->id);
            free(eptr);
            return NULL;
        }
        strcpy(eptr->val.sval, nptr->val.sval);
    } else {
        eptr->val.ival = nptr->val.ival;
    }
    return eptr;
}

/* put() - insert an entry into the hashtable or update the existing entry.
 * Use a linked list to handle collisions.
 * Parameters: Variable name, pointer to a node.
 * Return value: None.
 * Side effect: The entry is inserted into the hashtable, or is updated if
 * it already exists.
 * (STUDENT TODO) 
 */

void put(char *id, node_t *nptr) {
    // Find the place to index into in the hashtable
    unsigned long index = hash_function(id);
    if (var_table->entries[index] == NULL){
        var_table->entries[index] = init_entry(id, nptr);
        return;
    }
    // Week 3 TODO: Implement adding to the hashtable.
    entry_t *entry = var_table->entries[index]->next;

    if (strcmp(var_table->entries[index]->id, id) == 0){
        delete_entry(var_table->entries[index]);
        var_table->entries[index] = init_entry(id, nptr);
        var_table->entries[index]->next = entry;
        return;
    } else {
        entry_t* tempEntry = var_table->entries[index];
        entry_t* afterEntry;
        while (tempEntry->next != NULL){
            afterEntry = tempEntry->next->next;
            if (strcmp(tempEntry->next->id, id) == 0){
                delete_entry(tempEntry->next);
                tempEntry->next = init_entry(id, nptr);
                tempEntry->next->next = afterEntry;
                return;
            }
            tempEntry = tempEntry->next;
        }
        tempEntry->next = init_entry(id, nptr);
    }
    return;

    // If we have already encountered this entry in the hashtable
    // if (entry != NULL){
    //     // Replace everything else in the entry, except the id and next
    //     // Update the entry type as the type of the 2nd child, the nptr type

    //     //go through all of the entries for a hashcode to see if it has the same id

    //         // free the string that's already there
    //         // free --> malloc --> deep copy
    //         strcpy(entry->val.sval, nptr->val.sval);      
    //     }

     
        // Otherwise, we've never encountered this variable before
        // Allocate a new entry_t struct, the value is the same as the nptr, the type is same as nptr type
        // Entry id is the string copy of id

        // entry_t *current = var_table->entries[index];
        // // go through all the elements for that hashcode and see if there is a matching id
        // //if there is a matching id, delete the old entry and put a new one in its place and return 
        // while (var_table->entries[index] != entry){
        //     if (entry != NULL){
        //         // iterate through the linked list
        //         // add entry to the end of linked list 
        //         current->next = entry;
        //         if (current == )   
        //     }
        // }
     
    // if (entry != NULL){
    //     entry = init_entry(id, nptr); 
    //     // If something is already at that index, iterate throught the existing list and add it to the end
    //     if (var_table->entries[index] == NULL){
    //         // just add it there
    //         var_table->entries[index] = entry;
    //     } else {
    //         // add it to a list of linked list
    //         // iterate through the linked list
    //         entry_t *current = var_table->entries[index];
    //         while (current->next != NULL){
    //         // go to the end of the linked list
    //         current = current->next;
    //         }
    //         // add entry to the end of linked list 
    //         current->next = entry;   
    //     }    
    // } else {
    //     handle_error(ERR_EVAL);
    // }        
}

/* get() - search for an entry in the hashtable.
 * Parameter: Variable name.
 * Return value: Pointer to the matching entry, or NULL if not found.
 * (STUDENT TODO) 
 */

entry_t* get(char* id) {
    // Week 3 TODO: Implement retrieving from the hasttable.
    // To get an index of the hashtable, we access the nptr of the first child (string) nptr->children[0]->val.sval
    int index = hash_function(id);
    // Access the array from the hash table, check if it's null
    if (var_table->entries[index] == NULL){
        return NULL;
    } else {
       // Otherwise, there is an entry here
        // Compare the element in the hashtable and just the id 
        // Compare the indentifier associated with the index (struct) and the given string from the get()
       if (strcmp(var_table->entries[index]->id, id) == 0){
           // return that element (the entry), not just the identifier itself
           return var_table->entries[index];
       } else {
           // iterate through the linked list
            entry_t *current = var_table->entries[index];
            while (current != NULL){
               // Use string comparison whenever we're comparing two strings
               if (strcmp(current->id, id) == 0){
                   return current;
               } else {
                   current = current->next;
               }
                
           }
           
       }

    }

    return NULL;
}

void print_entry(entry_t *eptr) {
    if (! eptr) return;
    switch (eptr->type) {
        case INT_TYPE:
            fprintf(outfile, "%s = %d; ", eptr->id, eptr->val.ival);
            break;
        case BOOL_TYPE:
            fprintf(outfile, "%s = %s; ", eptr->id, bool_print[eptr->val.bval]);
            break;
        case STRING_TYPE:
            fprintf(outfile, "%s = \"%s\"; ", eptr->id, eptr->val.sval);
            break;
        default:
            logging(LOG_ERROR, "unsupported entry type for printing");
            break;
    }
    print_entry(eptr->next);
    return;
}

void print_table(void) {
    if (! var_table) {
        logging(LOG_ERROR, "variable table doesn't exist");
        return;
    }
    fprintf(outfile, "\t");
    for (int i = 0; i < CAPACITY; ++i) {
        print_entry(var_table->entries[i]);
    }
    fprintf(outfile, "\n");
    return;
}
