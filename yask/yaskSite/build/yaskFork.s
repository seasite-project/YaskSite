# mark_description "Intel(R) C++ Intel(R) 64 Compiler for applications running on Intel(R) 64, Version 16.0.3.210 Build 20160415";
# mark_description "";
# mark_description "-I/home/hpc/unrz/unrz002h/yask/yaskSite/include -DyaskSite_EXPORTS -isystem /apps/intel/mpi/5.1.3.210/intel6";
# mark_description "4/include -qopenmp -fPIC -std=c++0x -Wall -Wextra -Werror -pedantic -Wunused-variable -save-temps -fsource-a";
# mark_description "sm -w3 -wd981 -wd383 -fPIC -o CMakeFiles/yaskSite.dir/src/yaskFork.cpp.o -c";
	.file "yaskFork.cpp"
	.text
..TXTST0:
# -- Begin  YASKfinalize
	.text
# mark_begin;
       .align    16,0x90
	.globl YASKfinalize
# --- YASKfinalize(yaskSite *)
YASKfinalize:
# parameter 1: %rdi
..B1.1:                         # Preds ..B1.0

###     {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value_YASKfinalize.1:
..L2:
                                                          #199.5

###         UNUSED(opt);
###         return -1;

        movl      $-1, %eax                                     #201.16
        ret                                                     #201.16
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	YASKfinalize,@function
	.size	YASKfinalize,.-YASKfinalize
	.data
# -- End  YASKfinalize
	.text
# -- Begin  YASKstencil
	.text
# mark_begin;
       .align    16,0x90
	.globl YASKstencil
# --- YASKstencil(yaskSite *)
YASKstencil:
# parameter 1: %rdi
..B2.1:                         # Preds ..B2.0

###     {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value_YASKstencil.4:
..L5:
                                                          #205.5

###         UNUSED(opt);
###         return -1;

        movl      $-1, %eax                                     #207.16
        ret                                                     #207.16
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	YASKstencil,@function
	.size	YASKstencil,.-YASKstencil
	.data
# -- End  YASKstencil
	.text
# -- Begin  YASKgetElPtr
	.text
# mark_begin;
       .align    16,0x90
	.globl YASKgetElPtr
# --- YASKgetElPtr(yaskSite *, const char *, int, int, int, int, bool)
YASKgetElPtr:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %edx
# parameter 4: %ecx
# parameter 5: %r8d
# parameter 6: %r9d
# parameter 7: 8 + %rsp
..B3.1:                         # Preds ..B3.0

###     {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value_YASKgetElPtr.7:
..L8:
                                                          #211.5

###         UNUSED(opt);
###         UNUSED(data);
###         UNUSED(t);
###         UNUSED(x);
###         UNUSED(y);
###         UNUSED(z);
###         UNUSED(checkBounds);
###         return NULL;

        xorl      %eax, %eax                                    #219.16
        ret                                                     #219.16
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	YASKgetElPtr,@function
	.size	YASKgetElPtr,.-YASKgetElPtr
	.data
# -- End  YASKgetElPtr
	.text
# -- Begin  __sti__$E
	.text
# mark_begin;
       .align    16,0x90
# --- __sti__$E()
__sti__$E:
..B4.1:                         # Preds ..B4.0
	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value___sti__$E.10:
..L11:
                                                         #
        pushq     %rsi                                          #
	.cfi_def_cfa_offset 16

### static cache_info L1(L1_cache);

        lea       L1(%rip), %rdi                                #18.22
        lea       .L_2__STRING.0(%rip), %rsi                    #18.22
        movl      $8, %edx                                      #18.22
..___tag_value___sti__$E.13:
#       cache_info::cache_info(cache_info *, char *, int)
        call      _ZN10cache_infoC1EPci@PLT                     #18.22
..___tag_value___sti__$E.14:
                                # LOE rbx rbp r12 r13 r14 r15
..B4.2:                         # Preds ..B4.1
        lea       L2(%rip), %rdi                                #18.22
        lea       .L_2__STRING.1(%rip), %rsi                    #18.22
        movl      $8, %edx                                      #18.22
..___tag_value___sti__$E.15:
#       cache_info::cache_info(cache_info *, char *, int)
        call      _ZN10cache_infoC1EPci@PLT                     #18.22
..___tag_value___sti__$E.16:
                                # LOE rbx rbp r12 r13 r14 r15
..B4.3:                         # Preds ..B4.2
        lea       L3(%rip), %rdi                                #18.22
        lea       .L_2__STRING.2(%rip), %rsi                    #18.22
        movl      $8, %edx                                      #18.22
        addq      $8, %rsp                                      #18.22
	.cfi_def_cfa_offset 8
#       cache_info::cache_info(cache_info *, char *, int)
        jmp       _ZN10cache_infoC1EPci@PLT                     #18.22
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	__sti__$E,@function
	.size	__sti__$E,.-__sti__$E
	.data
# -- End  __sti__$E
	.text
# -- Begin  YASKinit
	.text
# mark_begin;
       .align    16,0x90
	.globl YASKinit
# --- YASKinit(yaskSite *)
YASKinit:
# parameter 1: %rdi
..B5.1:                         # Preds ..B5.0

###     {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value_YASKinit.19:
..L20:
                                                         #193.5

###         UNUSED(opt);
###         return -1;

        movl      $-1, %eax                                     #195.16
        ret                                                     #195.16
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	YASKinit,@function
	.size	YASKinit,.-YASKinit
	.data
# -- End  YASKinit
	.bss
	.align 8
L1:
	.type	L1,@object
	.size	L1,32
	.space 32	# pad
	.align 8
L2:
	.type	L2,@object
	.size	L2,32
	.space 32	# pad
	.align 8
L3:
	.type	L3,@object
	.size	L3,32
	.space 32	# pad
	.section .rodata.str1.4, "aMS",@progbits,1
	.align 4
.L_2__STRING.0:
	.long	909586995
	.long	1881160504
	.long	1635150194
	.word	25972
	.byte	0
	.type	.L_2__STRING.0,@object
	.size	.L_2__STRING.0,15
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.1:
	.long	825374258
	.long	540750900
	.long	1986622064
	.long	6648929
	.type	.L_2__STRING.1,@object
	.size	.L_2__STRING.1,16
	.align 4
.L_2__STRING.2:
	.long	825374258
	.long	808465460
	.long	1752375355
	.long	1684370017
	.byte	0
	.type	.L_2__STRING.2,@object
	.size	.L_2__STRING.2,17
	.section .ctors, "wa"
	.align 8
__init_0:
	.type	__init_0,@object
	.size	__init_0,8
	.quad	__sti__$E
	.data
# mark_proc_addr_taken __sti__$E;
	.section .note.GNU-stack, ""
	.weak DW.ref.__gxx_personality_v0
	.hidden DW.ref.__gxx_personality_v0
	.type	DW.ref.__gxx_personality_v0,@object
	.size	DW.ref.__gxx_personality_v0,8
// -- Begin DWARF2 SEGMENT .eh_frame
	.section .eh_frame,"a",@progbits
.eh_frame_seg:
	.align 1
// -- Begin DWARF2 SEGMENT .gnu.linkonce.d.DW.ref.__gxx_personality_v0
	.section .gnu.linkonce.d.DW.ref.__gxx_personality_v0,"a",@progbits
	.weak DW.ref.__gxx_personality_v0
	.align 1
.hidden DW.ref.__gxx_personality_v0
DW.ref.__gxx_personality_v0:
	.8byte __gxx_personality_v0
# End
