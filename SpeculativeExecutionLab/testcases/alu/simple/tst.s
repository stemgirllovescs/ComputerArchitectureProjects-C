	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // Simple tests for the tst instruction.
    movz x0, #0xff00
    movz x1, #0x0ff0, lsl 16
    movz x2, #0x00ff, lsl 32
    movz x3, #0xf00f, lsl 48
    nop
    tst x0, x1
    tst x1, x2
    tst x2, x3
    tst x3, x0
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
