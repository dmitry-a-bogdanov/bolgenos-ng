#ifndef __BOLGENOS_NG__MMU_H__
#define __BOLGENOS_NG__MMU_H__ (1)

#ifdef __ASSEMBLER__


#define KERNEL_SEGMENT		(0x800)
#define KERNEL_OFFSET		(0x0000)

#define KERNEL_CODE_SEGMENT	(0x08)
#define KERNEL_DATA_SEGMENT	(0x10)

#define SEGMENT_ORDER (4)

#define linear_address(segment, offset)	\
	(((segment) << SEGMENT_ORDER) + offset)

#define relocate_forward(address, segment, offset) \
	((address) + linear_address(segment, offset))

/* Content of CR0 */
#define PROTECTED_MODE 0x00000001

/* asm data structures */
/* TODO: move to separate header */

#define bits_at(number, offset, mask) \
	(((number) >> (offset)) & mask)

#define S_EXEC		(0x8)
#define S_READ		(0x2)
#define S_WRITE		(0x2)

#define S_SYSTEM(flag)	((flag) << 4)
#define	S_DPL(flag)	((flag) << 5)
#define S_PRESENT(flag)	((flag) << 7)


#define S_DATA			(0 << 3)
#define S_DATA_ACCESSED		(1 << 0)
#define S_DATA_WRITE		(1 << 1)
#define S_DATA_EXP_DOWN		(1 << 2)

#define S_CODE			(1 << 3)
#define S_CODE_ACCESSED		(1 << 0)
#define S_CODE_READ		(1 << 1)
#define S_CODE_CONF		(1 << 2)

/* Do not set */
#define S_64BIT			(1 << 5)

#define S_CODE_32		(1 << 6)
#define S_STACK_BIG		(1 << 6)
#define S_EXPAND_DOWN		(1 << 6)

#define S_GRANULARITY		(1 << 7)


#define SEGMENT_DESCRIPTOR(base, limit, type, S, DPL, P, DB)		\
	.word bits_at(limit, 0, 0xffff);		\
	.word bits_at(base, 0, 0xffff);			\
	.byte bits_at(base, 16, 0xff);			\
	.byte (S_SYSTEM(S)|S_DPL(DPL)|S_PRESENT(P)|(type));	\
	.byte ((DB)|bits_at(limit, 16, 0xf));	\
	.byte bits_at(base, 24, 0xff)

#define SEGMENT_NULL	\
	.word 0x0, 0x0;				\
	.byte 0x0, 0x0, 0x0, 0x0

#else /* ! __ASSEMBLER__ */
#endif

#endif /* __BOLGENOS_NG__MMU_H__ */
