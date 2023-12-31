
# Expose to extern
.global set_exception_vector_table

# Set as the function type 
.type set_exception_vector_table, @function

.align 11
.global exception_vector_table
exception_vector_table:
	b	exception_handler
	.align 7
	b	__irq_handler
	.align 7
	b	exception_handler
	.align 7
	b	exception_handler
	.align 7

	b	__irq_handler
	.align 7
	b	__irq_handler
	.align 7
	b	exception_handler
	.align 7
	b	exception_handler
	.align 7
	
	b	__low_syn_handler
	.align 7
	b	__low_irq_handler
	.align 7
	b	exception_handler
	.align 7
	b	exception_handler
	.align 7

	b	exception_handler
	.align 7
	b	__low_irq_handler
	.align 7
	b	exception_handler
	.align 7
	b	exception_handler
	.align 7

set_exception_vector_table:
	ldr	x2, =exception_vector_table
	msr	vbar_el1, x2
	ret

.macro save_all
	sub	sp, sp, 16 * 18
	stp 	x0, x1, [sp ,16 * 0]
	stp 	x2, x3, [sp ,16 * 1]
	stp 	x4, x5, [sp ,16 * 2]
	stp 	x6, x7, [sp ,16 * 3]
	stp 	x8, x9, [sp ,16 * 4]
	stp 	x10, x11, [sp ,16 * 5]
	stp 	x12, x13, [sp ,16 * 6]
	stp 	x14, x15, [sp ,16 * 7]
	stp 	x16, x17, [sp ,16 * 8]
	stp 	x18, x19, [sp ,16 * 9]
	stp 	x20, x21, [sp ,16 * 10]
	stp 	x22, x23, [sp ,16 * 11]
	stp 	x24, x25, [sp ,16 * 12]
	stp 	x26, x27, [sp ,16 * 13]
	stp 	x28, x29, [sp ,16 * 14]
	str 	x30, [sp, 16 * 15]
	mrs	x28, spsr_el1
	mrs	x29, elr_el1
	stp 	x28, x29, [sp ,16 * 16]
	mrs	x28, sp_el0	// For fork
	stp	x28, x0, [sp, 16 * 17]
.endm

.macro load_all
	ldp	x28, x0, [sp, 16 * 17]
	msr	sp_el0, x28
	ldp 	x28, x29, [sp ,16 * 16]
	msr	spsr_el1, x28
	msr	elr_el1, x29
	ldp 	x0, x1, [sp ,16 * 0]
	ldp 	x2, x3, [sp ,16 * 1]
	ldp 	x4, x5, [sp ,16 * 2]
	ldp 	x6, x7, [sp ,16 * 3]
	ldp 	x8, x9, [sp ,16 * 4]
	ldp 	x10, x11, [sp ,16 * 5]
	ldp 	x12, x13, [sp ,16 * 6]
	ldp 	x14, x15, [sp ,16 * 7]
	ldp 	x16, x17, [sp ,16 * 8]
	ldp 	x18, x19, [sp ,16 * 9]
	ldp 	x20, x21, [sp ,16 * 10]
	ldp 	x22, x23, [sp ,16 * 11]
	ldp 	x24, x25, [sp ,16 * 12]
	ldp 	x26, x27, [sp ,16 * 13]
	ldp 	x28, x29, [sp ,16 * 14]
	ldr 	x30, [sp, 16 * 15]
	add 	sp, sp, 16 * 18
.endm

exception_handler:
	save_all
	bl	exception_entry
	load_all
	eret

__irq_handler:
	save_all
	bl	irq_handler
	load_all
	eret

__low_irq_handler:
	save_all
	bl	low_irq_handler
	load_all
	eret

__low_syn_handler:
	save_all
	mov	x0, sp		// Get the SP
	bl	low_syn_handler
	load_all
	eret

.global load_reg_ret
.type load_reg_ret, @function

load_reg_ret:
	load_all
	eret
