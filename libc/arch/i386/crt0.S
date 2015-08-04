.section .text

.global _start
.type _start, @function
_start:
	# TODO: Locate argc, argv, envp according to System V ABI (i386 supplement)
	#       Figure 3-31: Initial Process Stack (3-28, Page 54).

	# Set up end of the stack frame linked list.
	xorl %ebp, %ebp
	pushl %ebp # rip=0
	pushl %ebp # rbp=0
	movl %esp, %ebp

	# Initialize the standard library.
	push $0 # TODO: argc
	push $0 # TODO: argv
	push $0 # TODO: envp
	call __init_libc # void(int, char**, char**)
	addl $12, %esp

	# Run the global constructors.
#if !defined(__HAS_NO_CRT_INIT)
	call _init # void(void)
#endif

	# Run main
	push $0 # TODO: argc
	push $0 # TODO: argv
	push $0 # TODO: envp
	call main # int(int, char**, char**)
	addl $12, %esp

	# Terminate the process with main's exit code.
	push %eax
	call exit
.size _start, .-_start
