	.arch armv8-a
	.text
	.align	2
	.p2align 3,,7
start:
    // This test is just to make sure you are able to properly halt the emulator.
    // Make sure it works before attempting anything else.
	ret
	.size	start, .-start
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
