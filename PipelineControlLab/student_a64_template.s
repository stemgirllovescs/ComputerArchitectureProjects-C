  // This is the general format of an assembly-language program file.
    // Written by: Katherine Liang xl5878
    .arch armv8-a
    .text
    // Code for all functions go here.

    // ***** WEEK 1 deliverables *****
    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global popcntRD
    .type   popcntRD, %function
popcntRD:
    // (STUDENT TODO) Code for popcntRD goes here.
    // Input parameter n is passed in x0.
    // Output value is returned in x0.
    movz x9, #0x5555, lsl #48
    movk x9, #0x5555, lsl #32
    movk x9, #0x5555, lsl #16
    movk x9, #0x5555, lsl #0

    ands x10, x0, x9
    lsr x0, x0, 1
    ands x0, x0, x9
    adds x0, x10, x0

    movz x9, #0x3333, lsl #48
    movk x9, #0x3333, lsl #32
    movk x9, #0x3333, lsl #16
    movk x9, #0x3333, lsl #0

    ands x10, x0, x9
    lsr x0, x0, 2
    ands x0, x0, x9
    adds x0, x10, x0

    movz x9, #0x0F0F, lsl #48
    movk x9, #0x0F0F, lsl #32
    movk x9, #0x0F0F, lsl #16
    movk x9, #0x0F0F, lsl #0

    ands x10, x0, x9
    lsr x0, x0, 4
    ands x0, x0, x9
    adds x0, x10, x0

    movz x9, #0x00FF, lsl #48
    movk x9, #0x00FF, lsl #32
    movk x9, #0x00FF, lsl #16
    movk x9, #0x00FF, lsl #0

    ands x10, x0, x9
    lsr x0, x0, 8
    ands x0, x0, x9
    adds x0, x10, x0

    movz x9, #0x0000, lsl #48
    movk x9, #0xFFFF, lsl #32
    movk x9, #0x0000, lsl #16
    movk x9, #0xFFFF, lsl #0

    ands x10, x0, x9
    lsr x0, x0, 16
    ands x0, x0, x9
    adds x0, x10, x0

    movz x9, #0x0000, lsl #48
    movk x9, #0x0000, lsl #32
    movk x9, #0xFFFF, lsl #16
    movk x9, #0xFFFF, lsl #0

    ands x10, x0, x9
    lsr x0, x0, 32
    ands x0, x0, x9
    adds x0, x10, x0

    movz x12, #0x0000, lsl #48
    movk x12, #0x0000, lsl #32
    movk x12, #0x0000, lsl #16
    movk x12, #0x007F, lsl #0

    ands x0, x0, x12
    
    ret
    .size   popcntRD, .-popcntRD
    // ... and ends with the .size above this line.

    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global BCD_to_long
    .type   BCD_to_long, %function
BCD_to_long:
    // (STUDENT TODO) Code for BCD_to_long goes here.
    // Input parameter buf is passed in x0.
    // Output value is returned in x0.
    subs x8, x0, #0xA
    b.lt .L2
    subs x7, x0, #0xF
    b.gt .L2
    movz x6, #0
    subs x0, x6, #1
    b .L1

    .L2:
    movz x1, #0
    movz x2, #0
    movz x3, #0

    subs x0, x0, #0

    b.gt .L3

    b .L6

    .L3:
    ands x2, x0, #0x0F
    movz x4, #0

    subs x5, x4, x3
    b.lt .L4
    b .L5

    .L5:
    adds x1, x1, x2
    lsr x0, x0, #4
    adds x3, x3, #1

    subs x0, x0, #0

    b.gt .L3
    b .L6

    .L4:
    movz x8, #0
    movz x7, #0
    lsl x8, x2, #3
    lsl x7, x2, #1
    adds x2, x8, x7
    adds x4, x4, #1

    subs x5, x4, x3

    b.lt .L4
    b .L5

    .L6:
    movz x0, #0
    adds x0, x1, #0
    b .L1

    .L1:
    ret
    .size   BCD_to_long, .-BCD_to_long
    // ... and ends with the .size above this line.

    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global unicode_to_UTF8
    .type   unicode_to_UTF8, %function
unicode_to_UTF8:
    // (STUDENT TODO) Code for unicode_to_UTF8 goes here.
    // Input parameter a is passed in x0; input parameter utf8 is passed in x1.
    // There are no output values.
    ldur x9, [x1]
    movz x9, #0x0
    stur x9, [x1]

    movz x10, #0x7F
    subs x13, x0, x10
    b.le .T1

    movz x10, #0x07FF
    subs x13, x0, x10
    b.le .T2

    movz x10, #0xFFFF
    subs x13, x0, x10
    b.le .T3

    movz x10, #0x0010, lsl #16
    movk x13, #0xFFFF
    adds x10, x10, x13
    subs x13, x0, x10
    b.le .T4

    b .L888

    .T1:
    ldur x9, [x1]
    movz x9, #0x0
    adds x9, x0, #0x0
    stur x9, [x1]
    b .L888

    .T2:
    b .L888

    .T3:
    movz x15, #0x00E0, lsl #16
    movz x14, #0x8080
    adds x15, x15, x14
    ands x11, x0, #0x003F
    ands x12, x0, #0x0FC0
    ands x13, x0, #0xF000
    lsl x12, x12, #2
    lsl x13, x13, #4
    adds x15, x15, x11
    adds x15, x15, x12
    adds x15, x15, x13
    movz x9, #0x0
    ands x9, x15, #0xFF
    lsl x9, x9, #8
    lsr x15, x15, #8
    ands x11, x15, #0xFF
    adds x9, x9, x11
    lsl x9, x9, #8
    lsr x15, x15, #8
    ands x11, x15, #0xFF
    adds x9, x9, x11
    stur x9, [x1]
    b .L888

    .T4:
    movz x12, #0xF080, lsl #16
    movz x14, #0x8080
    adds x12, x12, x14
    ands x11, x0, #0x003F
    ands x10, x0, #0x0FC0
    movz x14, 0x0003, lsl #16
    adds x13, x14, #0xF000
    ands x13, x0, x13
    movz x15, #0x001C, lsl #16
    ands x14, x0, x15
    lsl x10, x10, #2
    lsl x13, x13, #4
    lsl x14, x14, #6
    adds x12, x12, x11
    adds x12, x12, x10
    adds x12, x12, x13
    adds x12, x12, x14
    movz x9, #0x0
    ands x9, x12, #0xFF
    lsl x9, x9, #8
    lsr x12, x12, #8
    ands x11, x12, #0xFF
    adds x9, x9, x11
    lsl x9, x9, #8
    lsr x12, x12, #8
    ands x11, x12, #0xFF
    adds x9, x9, x11
    lsl x9, x9, #8
    lsr x12, x12, #8
    ands x11, x12, #0xFF
    adds x9, x9, x11
    stur x9, [x1]
    b .L888

    .L888:
    ret
    .size   unicode_to_UTF8, .-unicode_to_UTF8
    // ... and ends with the .size above this line.

    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global matrix_transpose
    .type   matrix_transpose, %function
matrix_transpose:
    // (STUDENT TODO) Code for matrix_transpose goes here.
    // Input parameter ptr_in is passed in x0; input parameter prt_out is passed in x1;
    // input parameter dimension is passed in x2
    // Output value is returned in x0.
    cmp x0, #0
    b.eq .err
    cmp x1, #0
    b.eq .err
    cmp x2, #0
    b.eq .err
    
    movz x3, #0 // inner counter
    movz x4, #0 // outer_counter
    // x15 = free_register
    movz x15, #0
    movz x6, #0 
    adds x6, x6, x0 // pointer in
    movz x7, #0
    //adds x7, x7, x1 // pointer out 
    movz x8, #0
    lsl x8, x2, #3 // x8 = x2 <<3
    movz x9, #0 // x9 = "multi-8 offset"


    .outer_loop:
        movz x7, #0
        adds x7, x7, x1
        adds x7, x7, x9
        movz x3, #0 

    .inner_loop:
        ldur x15, [x6]
        stur x15, [x7]
        adds x6, x6, #8
        adds x7, x7, x8
        // increment inner counter
        adds x3, x3, #1
        cmp x3, x2
        b.lt .inner_loop // b.condition automatically checks the condition before this line

        adds x9, x9, #8
    // increment outer counter
        adds x4, x4, #1
        cmp x4, x2
        b.lt .outer_loop

    b .ends

    .err:
    movz x0, #0
    subs x0, x0, #1

.ends:
    ret



    .size   matrix_transpose, .-matrix_transpose
    // ... and ends with the .size above this line.
    // Make sure we don't have a null pointer exception
 
    
    






    // ***** WEEK 2 deliverables *****

    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global ustrncmp
    .type   ustrncmp, %function
ustrncmp:
    // (STUDENT TODO) Code for ustrncmp goes here.
    // Input parameter str1 is passed in x0; input parameter str2 is passed in x1;
    //  input parameter num is passed in x2
    // Output value is returned in x0.

    // x0 input for str1
    // x1 input for str2
    // x2 num

    // loop counter
    movz x8, #0 

    // holds 8 byte string of x0
    movz x3, #0 
    // holds 8 byte string of x1
    movz x4, #0  

    // current char of str 1
    movz x5, #0  
    // current char of str 2
    movz x6, #0  

    .loop_traversal:
        cmp x8, x2  //compares loop control to the #num of chars asked to compare (x2)
        b.eq .end_loop

        b .getbyte //calls helper method to get byte
        .cont: //continue point for .traverse
        adds x8, x8, #1 //increments loop control

        //check for equals if equal run loop again
        cmp x5, x6
        b.eq .loop_traversal
        
        //check null terminator (#0) checks both strings
        cmp x5, #0
        b.eq .earlyTerm 
        cmp x6, #0
        b.eq .earlyTerm

        //checks for not equals different return values based on < or >
        cmp x5, x6
        b.lt .lessThan
        cmp x5, x6
        b.gt .greatThan

    .getbyte:
        //loads 8 byte string of chars
        ldur x3, [x0]
        ldur x4, [x1]
        //applys byte mask to get rightmost byte (one char)
        ands x5, x3, #0xFF
        ands x6, x4, #0xFF
        //increments mem address of str1 and str2 by 1
        adds x0, x0, #1
        adds x1, x1, #1
        b .cont

    .earlyTerm:
    //sets return value to 100 as per assignment guidelines
    movz x0, #100
    b .end5

    .lessThan:
    //sets return to -1
    movz x0, #0
    subs x0, x0, #1
    b .end5

    .greatThan:
    //sets return to 1
    movz x0, #1
    b .end5

    .end_loop:
    //sets return to 2
    movz x0, #2
    .end5:
    ret
    ret
    .size   ustrncmp, .-ustrncmp
    // ... and ends with the .size above this line.

    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global gcd_rec
    .type   gcd_rec, %function
gcd_rec:
    // (STUDENT TODO) Code for gcd_rec goes here.
    // Input parameter a is passed in x0; input parameter b is passed in x1.
    // Output value is returned in x0.
    
    movz x2, #0
    movz x3, #0

    adds x2, x2, x0
    adds x3,x3, x1
    movz x20, #0 // stack pointer
    add x20, x30, #0

    movz x10, #0    //temp variables to hold calculations (x10, x11)
    movz x11, #0
    cmp x2, #0 //make sure neither number is 0
    b.eq .zero
    cmp x3, #0
    b.eq .zero

    .GCD:
    subs x10, x2, x3 //subtracts x0 - x1
    cmp x10, #0 // If 0 it must be the GCD so return that value
    b.eq .hasGcd
    cmp x10, #0 // If negative it must be moved around *euclids algorithm*
    b.lt .changeNumber
    movz x2, #0 // sets x2 to the solution of x0 - x1
    adds x2, x2, x10
    bl .GCD //idk recursion
    add x30, x20, #0

    .hasGcd:
    movz x0, #0 //sets x0 to the value of x6 and returns
    adds x0, x0, x3
    b .endGCD

    .changeNumber: //looks at euclids i think its X - Y = -(Z) -> y - z = a
    movz x11, #0
    adds x11, x11, x2
    movz x2, #0
    adds x2, x2, x3
    movz x3, #0
    adds x3, x3, x11
    subs x10, x2, x3 // subtract again using gcd formula to check for 0, if zero it must be 1
    cmp x10, #0
    b.lt .noGCD
    b .GCD

    .noGCD:
    movz x0, #1 //returns 1
    b .endGCD

    .zero:
    movz x0, #0 //returns -1
    subs x0, x0, #1
    b .endGCD

    .endGCD:
    ret
    .size   gcd_rec, .-gcd_rec
    // ... and ends with the .size above this line.

    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global tree_traversal
    .type   tree_traversal, %function
tree_traversal:
    // (STUDENT TODO) Code for tree_traversal goes here.
    // Input parameter root is passed in x0; input parameter bit_string is passed in x1;
    //  input parameter bit_string_length is passed in x2
    // Output value is returned in x0.
    //preconditions
    cmp x0, #0
    b.eq .err1
    cmp x2, #0
    b.eq .err1

    movz x10, #0 //loop counter
    movz x11, #0 //tree traversal direction
    movz x12, #0 //traversal string
    movz x13, #0 //answer
    adds x12, x12, x1 //sets mem address value to x12
    
    .treeTraversal:
    cmp x10, x2 //loop stop
    b.eq .retVal

    cmp x0, #0  //check if it's null node  
    b.eq .err1

    ands x11, x12, #1 //gets right most byte of traversal string
    cmp x11, #0 // if 0 must get left node and if 1 get right node
    b.eq .getLeftNode

    .getRightNode:
    ldur x0, [x0, #8] //gets right node  
    adds x10, x10, #1 //increments counter
    lsr x12, x12, #1 //shifts traversal string to the right
    b .treeTraversal

    .getLeftNode:
    ldur x0, [x0, #0] //gets left node 
    adds x10, x10, #1 //increments counter
    lsr x12, x12, #1  //shifts traversal string to the right
    b .treeTraversal

    .retVal:
    ldur x13, [x0, #16] //gets value of node  
    movz x0, #0 //zeres x0
    adds x0, x0, x13 //sets ret value to x0
    b .stop

    .err1:
    movz x0, #0 //returns -1
    subs x0, x0, #1
    b .stop

    .stop:
    ret
    .size   tree_traversal, .-tree_traversal
    // ... and ends with the .size above this line.

    // Every function starts from the .align below this line ...
    .align  2
    .p2align 3,,7
    .global pattern_table_update
    .type   pattern_table_update, %function
pattern_table_update:
    // (STUDENT TODO) Code for pattern_table_update goes here.
    // Input parameter pattern_table is passed in x0; input parameter history is passed in x1;
    //  input parameter incoming_state is passed in x2; 
    //  input parameter num_incoming_state_changes is passed in x3
    // Output value is returned in x0.
    cmp x0, #0 //checks if pattern table or state changes is 0
    b.eq .err2
    cmp x3, #0
    b.eq .err2

    movz x7, #0 //loop counter

    .patternTableUpdate:
    cmp x7, x3    //checks if state changes has been reached
    b.eq .complete_table_update
    adds x7, x7, #1 //increments counters

    b .shifts_history //shifts history
    .update:

    cmp x8, #0 //checks if it increases or decreases method
    b.eq .pattern_decre
    b .pattern_incre

    //shifts history and updates pattern table and incoming state
    .shifts_history:
    ands x8, x2, #1 //shifts right and masks to 1 bytes the traversal string
    lsr x2, x2, #1

    lsl x1, x1, #1      //shifts left and masks byte for 5 bytes history string
    ands x1, x1, #0x1F  
    adds x1, x1, x8

    adds x10, x0, x1 //getting pattern, preserving the leftmost 7 bytes and then changes the rightmost byte char (+1, or -1)
    ldur x11, [x10]
    movz x15, #0
    adds x15, x11, #0
    ands x15, x15, #0xFFFFFFFFFFFFFF00
    ands x11, x11, #0xFF
    b .update

    //new pattern value
    .setNewPattern: //uses stur to add in new pattern value
    adds x11, x11, x15
    stur x11, [x10]
    b .patternTableUpdate

    //decreses or increases pattern value
    .pattern_decre: //-1
    subs x11, x11, #1
    cmp x11, #0
    b.ge .setNewPattern
    adds x11, x11, #1
    b .setNewPattern

    .pattern_incre:
    adds x11, x11, #1 //+1
    cmp x11, #4
    b.le .setNewPattern
    subs x11, x11, #1
    b .setNewPattern

    //returns -1  
    .err2:
    movz x0, #0
    subs x0, x0, #1
    b .end_now

    //returns 0 
    .complete_table_update:
    movz x0, #0
    b .end_now

    .end_now:
    ret
    .size   pattern_table_update, .-pattern_table_update
    // ... and ends with the .size above this line.

    .section    .rodata
    .align  4
    // (STUDENT TODO) Any read-only global variables go here.

    .data
    // (STUDENT TODO) Any modifiable global variables go here.

    .align  3
