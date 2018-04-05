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
	.extern	seg_icu_base
	.extern	seg_timer_base

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
	la	$26,	_interrupt_vector	# $26 <= &_interrupt_vector[0]
	addi	$26,	$26,	0x8		# $26 <= &_interrupt_vector[2]
	la	$27,	_isr_timer
	sw	$27,	($26)			# _itrpt_vector[2] = isr_timer

	la	$26,	_interrupt_vector	# $26 <= &_interrupt_vector[0]
	addi	$26,	$26,	0xc		# $26 <= &_interrupt_vector[3]
	la	$27,	_isr_tty_get
	sw	$27,	($26)			# _itrpt_vector[3] = isr_tty_get


        #initializes the ICU[0] MASK register
	la	$26,	seg_icu_base
	addi	$26,	$26,	0x00		# access output channel 0
	addi	$26,	$26,	0x08		# access ICU_MASK_SET
	li	$27,	0x4			# enable channel 2 (TIMER 0)
	sw	$27,	($26)
	li	$27,	0x8			# enable channel 3 (TTY 0)
	sw	$27,	($26)

        # initializes TIMER[0] PERIOD and RUNNING registers
	la	$26,	seg_timer_base
	addi	$26,	$26,	0x00		# access timer 0
	addi	$26,	$26,	0x8		# access TIMER_PERIOD
	li	$27,	50000			# set period
	sw	$27,	($26)

	la	$26,	seg_timer_base
	addi	$26,	$26,	0x00		# access timer 0
	addi	$26,	$26,	0x4		# access TIMER_RUNNING
	li	$27,	0x1			# set timer running
	#sw	$27,	($26)

	# initializes stack pointer for PROC[0]
	la	$29,	seg_stack_base
        li	$27,	50000			# stack size = 64K
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
	la	$26,	_interrupt_vector	# $26 <= &_interrupt_vector[0]
	addi	$26,	$26,	0x10		# $26 <= &_interrupt_vector[4]
	la	$27,	_isr_timer
	sw	$27,	($26)			# _itrpt_vector[4] = isr_timer

	la	$26,	_interrupt_vector	# $26 <= &_interrupt_vector[0]
	addi	$26,	$26,	0x14		# $26 <= &_interrupt_vector[5]
	la	$27,	_isr_tty_get
	sw	$27,	($26)			# _itrpt_vector[5] = isr_tty_get

        #initializes the ICU[1] MASK register
	la	$26,	seg_icu_base
	addi	$26,	$26,	0x20		# access output channel 1
	addi	$26,	$26,	0x08		# access ICU_MASK_SET
	li	$27,	0x10			# enable channel 4 (TIMER 1)
	sw	$27,	($26)
	li	$27,	0x20			# enable channel 5 (TTY 1)
	sw	$27,	($26)

        # initializes TIMER[1] PERIOD and RUNNING registers
	la	$26,	seg_timer_base
	addi	$26,	$26,	0x10		# access timer 1
	addi	$26,	$26,	0x8		# access TIMER_PERIOD
	li	$27,	100000			# set period
	sw	$27,	($26)

	la	$26,	seg_timer_base
	addi	$26,	$26,	0x10		# access timer 1
	addi	$26,	$26,	0x4		# access TIMER_RUNNING
	li	$27,	0x1			# set timer running
	#sw	$27,	($26)

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

