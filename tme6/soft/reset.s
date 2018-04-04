#################################################################################
#	File : reset.s
#	Author : Alain Greiner
#	Date : 25/12/2011
#################################################################################
#       This is an improved boot code for a bi-processor architecture.
#	Depending on the proc_id, each processor
#       - initializes the interrupt vector.
#       - initializes the ICU MASK registers.
#       - initializes the TIMER .
#       - initializes the Status Register.
#       - initializes the stack pointer.
#       - initializes the EPC register, and jumps to the user code.
#################################################################################
		
	.section .reset,"ax",@progbits

	.extern	seg_stack_base
	.extern	seg_data_base

	.func	reset
	.type   reset, %function

reset:
       	.set noreorder

# get the processor id
        mfc0	$27,	$15,	1		# get the proc_id
        andi	$27,	$27,	0x1		# no more than 2 processors
        bne	$27,	$0,	proc1

proc0:
        # initialises interrupt vector entries for PROC[0]

        #initializes the ICU[0] MASK register

        # initializes TIMER[0] PERIOD and RUNNING registers

        # initializes stack pointer for PROC[0]
	la	$29,	seg_stack_base
        li	$27,	0x10000			# stack size = 64K
	addu	$29,	$29,	$27    		# $29 <= seg_stack_base + 64K

        # initializes SR register for PROC[0]
       	li	$26,	0x0000FF13	
       	mtc0	$26,	$12			# SR <= 0x0000FF13

        # jump to main in user mode: main[0]
	la	$26,	seg_data_base
        lw	$26,	0($26)			# $26 <= main[0]
	mtc0	$26,	$14			# write it in EPC register
	eret

proc1:
        # initialises interrupt vector entries for PROC[1]

        #initializes the ICU[1] MASK register

        # initializes TIMER[1] PERIOD and RUNNING registers

        # initializes stack pointer for PROC[1]
	la	$29,	seg_stack_base
        li	$27,	0x10000			# stack size = 64K
	addu	$29,	$29,	$27    		# $29 <= seg_stack_base + 64K
	addu	$29,	$29,	$27    		# $29 <= seg_stack_base + 128K

        # initializes SR register for PROC[1]
       	li	$26,	0x0000FF13	
       	mtc0	$26,	$12			# SR <= 0x0000FF13

        # jump to main in user mode: main[1]
	la	$26,	seg_data_base
        lw	$26,	4($26)			# $26 <= main[1]
	mtc0	$26,	$14			# write it in EPC register
	eret

	.set reorder

	.set reorder

	.endfunc
	.size	reset, .-reset

