#ifndef __ASM_DIRECTIVES__
#define __ASM_DIRECTIVES__

//until we figure how to do weak_alias in assembly this is a temporary solution
#define __setcontext setcontext
#define __getcontext getcontext
#define __swapcontext swapcontext
//#define __startcontext startcontext
#define weak_alias(...) /**/


#define SYS_ify(syscall_name)	(__NR_##syscall_name)

#ifndef C_SYMBOL_NAME
# define C_SYMBOL_NAME(name) name
#endif

#define ASM_SIZE_DIRECTIVE(name) .size name,.-name;

/* Define a macro we can use to construct the asm name for a C symbol.  */
# define C_LABEL(name)  name##:

#ifdef __ASSEMBLER__
# define cfi_startproc			.cfi_startproc
# define cfi_endproc			.cfi_endproc
# define cfi_def_cfa(reg, off)		.cfi_def_cfa reg, off
# define cfi_def_cfa_register(reg)	.cfi_def_cfa_register reg
# define cfi_def_cfa_offset(off)	.cfi_def_cfa_offset off
# define cfi_adjust_cfa_offset(off)	.cfi_adjust_cfa_offset off
# define cfi_offset(reg, off)		.cfi_offset reg, off
# define cfi_rel_offset(reg, off)	.cfi_rel_offset reg, off
# define cfi_register(r1, r2)		.cfi_register r1, r2
# define cfi_return_column(reg)	.cfi_return_column reg
# define cfi_restore(reg)		.cfi_restore reg
# define cfi_same_value(reg)		.cfi_same_value reg
# define cfi_undefined(reg)		.cfi_undefined reg
# define cfi_remember_state		.cfi_remember_state
# define cfi_restore_state		.cfi_restore_state
# define cfi_window_save		.cfi_window_save
# define cfi_personality(enc, exp)	.cfi_personality enc, exp
# define cfi_lsda(enc, exp)		.cfi_lsda enc, exp

#endif


#ifdef __aarch64__ //////////////////////////////////////////////////////////

#ifdef __LP64__
# define AARCH64_R(NAME)	R_AARCH64_ ## NAME
# define PTR_REG(n)		x##n
# define PTR_LOG_SIZE		3
# define DELOUSE(n)
#else
# define AARCH64_R(NAME)	R_AARCH64_P32_ ## NAME
# define PTR_REG(n)		w##n
# define PTR_LOG_SIZE		2
# define DELOUSE(n)		mov     w##n, w##n
#endif

#  define JUMPTARGET(sym)	sym
#  define HIDDEN_JUMPTARGET(name) JUMPTARGET(name)

//FIXME:
#  define SYSCALL_ERROR_HANDLER                                 \
.Lsyscall_error:                                                \
	b	__syscall_error;

# undef	PSEUDO_END
# define PSEUDO_END(name)						      \
  SYSCALL_ERROR_HANDLER							      \
  END (name)

#undef	END
#define END(name)						\
  cfi_endproc;							\
  ASM_SIZE_DIRECTIVE(name)

# define CALL_MCOUNT            /* Do nothing.  */

/* Define an entry point visible from C.  */
#define ENTRY(name)						\
  .globl C_SYMBOL_NAME(name);					\
  .type C_SYMBOL_NAME(name),%function;				\
  .align 4;							\
  C_LABEL(name)							\
  cfi_startproc;						\
  CALL_MCOUNT

#elif __x86_64__ ///////////////////////////////////////////////////////////////

/* ELF uses byte-counts for .align, most others use log2 of count of bytes.  */
#define ALIGNARG(log2) 1<<log2

/* Local label name for asm code. */
#ifndef L
/* ELF-like local names start with `.L'.  */
# define L(name)	.L##name
#endif

#define ENTRY(name)						\
  .globl C_SYMBOL_NAME(name);						      \
  .type C_SYMBOL_NAME(name),@function;					      \
  .align ALIGNARG(4);							      \
  C_LABEL(name)								      \
  cfi_startproc;							      \
  CALL_MCOUNT
//.text
//        .global _start
//.__getcontext:

//#undef	PSEUDO_END
#define	PSEUDO_END(name)						      \
  END (name)

#undef	END
#define END(name)							      \
  cfi_endproc;								      \
  ASM_SIZE_DIRECTIVE(name)


/* If compiled for profiling, call `mcount' at the start of each function.  */
#ifdef	PROF
/* The mcount code relies on a normal frame pointer being on the stack
   to locate our caller, so push one just for its benefit.  */
#define CALL_MCOUNT                                                          \
  pushq %rbp;                                                                \
  cfi_adjust_cfa_offset(8);                                                  \
  movq %rsp, %rbp;                                                           \
  cfi_def_cfa_register(%rbp);                                                \
  call JUMPTARGET(mcount);                                                   \
  popq %rbp;                                                                 \
  cfi_def_cfa(rsp,8);
#else
#define CALL_MCOUNT		/* Do nothing.  */
#endif

#else //////////////////////////////////////////////////////////////////////////

#error "Unknown architecture"

#endif



#endif //__ASM_DIRECTIVES__