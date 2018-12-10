# mark_description "Intel(R) C++ Intel(R) 64 Compiler for applications running on Intel(R) 64, Version 16.0.3.210 Build 20160415";
# mark_description "";
# mark_description "-I/home/hpc/unrz/unrz002h/yask/yaskSite/include -DyaskSite_EXPORTS -isystem /apps/intel/mpi/5.1.3.210/intel6";
# mark_description "4/include -qopenmp -fPIC -std=c++0x -Wall -Wextra -Werror -pedantic -Wunused-variable -save-temps -fsource-a";
# mark_description "sm -w3 -wd981 -wd383 -fPIC -o CMakeFiles/yaskSite.dir/src/MPI_Manager.cpp.o -c";
	.file "MPI_Manager.cpp"
	.text
..TXTST0:
# -- Begin  _ZN11MPI_ManagerC2EPiPPPci
	.text
# mark_begin;
       .align    16,0x90
	.globl _ZN11MPI_ManagerC2EPiPPPci
# --- MPI_Manager::MPI_Manager(MPI_Manager *const, int *, char ***, int)
_ZN11MPI_ManagerC2EPiPPPci:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
# parameter 4: %ecx
..B1.1:                         # Preds ..B1.0

### MPI_Manager::MPI_Manager(int* argc_, char*** argv_, int printRank_):argc(argc_), argv(argv_), nRanks(1), myRank(0), printRank(printRank_)

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__ZN11MPI_ManagerC2EPiPPPci.1:
..L2:
                                                          #14.14
#       MPI_Manager::MPI_Manager(MPI_Manager *, int *, char ***, int)
        jmp       _ZN11MPI_ManagerC1EPiPPPci@PLT                #14.14
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_ZN11MPI_ManagerC2EPiPPPci,@function
	.size	_ZN11MPI_ManagerC2EPiPPPci,.-_ZN11MPI_ManagerC2EPiPPPci
	.data
# -- End  _ZN11MPI_ManagerC2EPiPPPci
	.text
# -- Begin  _ZN11MPI_ManagerC1EPiPPPci
	.text
# mark_begin;
       .align    16,0x90
	.globl _ZN11MPI_ManagerC1EPiPPPci
# --- MPI_Manager::MPI_Manager(MPI_Manager *, int *, char ***, int)
_ZN11MPI_ManagerC1EPiPPPci:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %rdx
# parameter 4: %ecx
..B2.1:                         # Preds ..B2.0

### {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.4:
..L5:
                                                          #15.1
        pushq     %r14                                          #15.1
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
        subq      $16, %rsp                                     #15.1
	.cfi_def_cfa_offset 32
        movq      %rdi, %r14                                    #15.1

### #ifdef yaskSite_HAVE_MPI
###     int provided = 0;
###     MPI_Init_thread(argc, argv, MPI_THREAD_SERIALIZED, &provided);

        movq      %rsi, %rdi                                    #18.5
        xorl      %eax, %eax                                    #14.14
        movl      %eax, (%rsp)                                  #17.18
        movq      %rsi, (%r14)                                  #14.74
        movq      %rdx, %rsi                                    #18.5
        movq      %rdx, 8(%r14)                                 #14.87
        movl      $2, %edx                                      #18.5
        movl      %ecx, 24(%r14)                                #14.127
        lea       (%rsp), %rcx                                  #18.5
        movl      $1, 16(%r14)                                  #14.14
        movl      %eax, 20(%r14)                                #14.14
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.9:
#       MPI_Init_thread(int *, char ***, int, int *)
        call      MPI_Init_thread@PLT                           #18.5
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.10:
                                # LOE rbx rbp r12 r13 r14 r15
..B2.2:                         # Preds ..B2.1

### 
###     if(provided < MPI_THREAD_SERIALIZED)

        cmpl      $2, (%rsp)                                    #20.19
        jge       ..B2.12       # Prob 0%                       #20.19
                                # LOE rbx rbp r12 r13 r14 r15
..B2.3:                         # Preds ..B2.2

###     {
###         ERROR_PRINT("MPI error: MPI_THREAD_SERIALIZED not provided.\n");

        movl      $1140850688, %edi                             #22.9
        lea       4(%rsp), %rsi                                 #22.9
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.11:
#       MPI_Comm_rank(MPI_Comm, int *)
        call      MPI_Comm_rank@PLT                             #22.9
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.12:
                                # LOE eax
..B2.4:                         # Preds ..B2.3
        testl     %eax, %eax                                    #22.9
        je        ..B2.6        # Prob 50%                      #22.9
                                # LOE
..B2.5:                         # Preds ..B2.4
        movl      $-1, 4(%rsp)                                  #22.9
        movq      stderr@GOTPCREL(%rip), %r12                   #22.9
        jmp       ..B2.9        # Prob 100%                     #22.9
                                # LOE r12
..B2.6:                         # Preds ..B2.4
        movl      4(%rsp), %ebp                                 #22.9
        testl     %ebp, %ebp                                    #22.9
        jne       ..B2.21       # Prob 50%                      #22.9
                                # LOE ebp
..B2.7:                         # Preds ..B2.6
        lea       .L_2__STRING.1(%rip), %rbx                    #22.9
        movl      $47, %eax                                     #22.9
        movq      %rbx, %rdx                                    #22.9
        movq      %rdx, %rcx                                    #22.9
        movd      %eax, %xmm1                                   #22.9
        andq      $-16, %rdx                                    #22.9
        punpcklbw %xmm1, %xmm1                                  #22.9
        andl      $15, %ecx                                     #22.9
        punpcklbw %xmm1, %xmm1                                  #22.9
        pshufd    $0, %xmm1, %xmm1                              #22.9
        pxor      %xmm0, %xmm0                                  #22.9
        movdqa    (%rdx), %xmm2                                 #22.9
        pcmpeqb   %xmm2, %xmm0                                  #22.9
        pmovmskb  %xmm0, %r10d                                  #22.9
        pcmpeqb   %xmm1, %xmm2                                  #22.9
        shrl      %cl, %r10d                                    #22.9
        lea       -1(%r10), %rax                                #22.9
        xorl      %eax, %r10d                                   #22.9
        pmovmskb  %xmm2, %eax                                   #22.9
        shrl      %cl, %eax                                     #22.9
        andl      %r10d, %eax                                   #22.9
        je        ..L13         # Prob 60%                      #22.9
        bsr       %rax, %rax                                    #22.9
        addq      %rcx, %rax                                    #22.9
        addq      %rdx, %rax                                    #22.9
..L13:                                                          #
        orl       %r10d, %r10d                                  #22.9
        jg        ..L15         # Prob 60%                      #22.9
..L14:                                                          #
        call      __intel_sse2_strrchr@PLT                      #22.9
..L15:                                                          #
                                # LOE rax rbx ebp
..B2.20:                        # Preds ..B2.7
        movq      stderr@GOTPCREL(%rip), %r12                   #22.9
        testq     %rax, %rax                                    #22.9
        lea       .L_2__STRING.0(%rip), %rsi                    #22.9
        lea       1(%rax), %r8                                  #22.9
        cmove     %rbx, %r8                                     #22.9
        movl      %ebp, %edx                                    #22.9
        lea       __$U0(%rip), %rcx                             #22.9
        movl      $22, %r9d                                     #22.9
        xorl      %eax, %eax                                    #22.9
        movq      (%r12), %rdi                                  #22.9
#       fprintf(FILE *, const char *, ...)
        call      fprintf@PLT                                   #22.9
                                # LOE r12
..B2.8:                         # Preds ..B2.20
        movq      (%r12), %rdi                                  #22.9
#       fflush(FILE *)
        call      fflush@PLT                                    #22.9
                                # LOE r12
..B2.9:                         # Preds ..B2.5 ..B2.8 ..B2.21
        movl      $10, %edi                                     #22.9
        movq      (%r12), %rsi                                  #22.9
        call      fputc@PLT                                     #22.9
                                # LOE r12
..B2.10:                        # Preds ..B2.9
        movq      (%r12), %rdi                                  #22.9
#       fflush(FILE *)
        call      fflush@PLT                                    #22.9
                                # LOE
..B2.11:                        # Preds ..B2.10

###         exit(-1);

        movl      $-1, %edi                                     #23.9
#       exit(int)
        call      exit@PLT                                      #23.9
                                # LOE
..B2.12:                        # Preds ..B2.2                  # Infreq

###     }
### 
###     comm = MPI_COMM_WORLD;
###     MPI_Comm_rank(comm, &myRank);

        movl      $1140850688, %edi                             #27.5
        lea       20(%r14), %rsi                                #27.5
        movl      $1140850688, 8(%rsi)                          #26.5
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.16:
#       MPI_Comm_rank(MPI_Comm, int *)
        call      MPI_Comm_rank@PLT                             #27.5
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.17:
                                # LOE rbx rbp r12 r13 r14 r15
..B2.13:                        # Preds ..B2.12                 # Infreq

###     MPI_Comm_size(comm,  &nRanks);

        movl      28(%r14), %edi                                #28.5
        lea       16(%r14), %rsi                                #28.5
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.18:
#       MPI_Comm_size(MPI_Comm, int *)
        call      MPI_Comm_size@PLT                             #28.5
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.19:
                                # LOE rbx rbp r12 r13 r14 r15
..B2.14:                        # Preds ..B2.13                 # Infreq

### #else
###     comm = 0;
### #endif
### 
###     //
### #ifdef yaskSite_HAVE_OpenMP
###     //just triggering openmp
###     omp_get_num_procs();

#       omp_get_num_procs()
        call      omp_get_num_procs@PLT                         #36.5
                                # LOE rbx rbp r12 r13 r14 r15
..B2.15:                        # Preds ..B2.14                 # Infreq

### #endif
### 
###     global_barrier();

        movq      %r14, %rdi                                    #39.5
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.20:
#       MPI_Manager::global_barrier(MPI_Manager *)
        call      _ZN11MPI_Manager14global_barrierEv@PLT        #39.5
..___tag_value__ZN11MPI_ManagerC1EPiPPPci.21:
                                # LOE rbx rbp r12 r13 r15
..B2.16:                        # Preds ..B2.15                 # Infreq

### 
### #ifdef ENABLE_LOOPADAPT
###     int cpu[1] = {2};
###     loop_adapt_pin(cpu[0]);
###     loop_adapt_set_policy(LOOP_ADAPT_POL_DUMP);
###     loop_adapt_set_analyze_iters(1);
###     loop_adapt_set_adapt_iters_freq(1);
###     loop_adapt_set_adapt_iters_max_steps(500);
###     loop_adapt_init(1, (int*)cpu);
### #endif
### }

        addq      $16, %rsp                                     #50.1
	.cfi_def_cfa_offset 16
	.cfi_restore 14
        popq      %r14                                          #50.1
	.cfi_def_cfa_offset 8
        ret                                                     #50.1
	.cfi_def_cfa_offset 32
	.cfi_offset 14, -16
                                # LOE
..B2.21:                        # Preds ..B2.6                  # Infreq
        movq      stderr@GOTPCREL(%rip), %r12                   #22.9
        jmp       ..B2.9        # Prob 100%                     #22.9
        .align    16,0x90
	.cfi_endproc
                                # LOE r12
# mark_end;
	.type	_ZN11MPI_ManagerC1EPiPPPci,@function
	.size	_ZN11MPI_ManagerC1EPiPPPci,.-_ZN11MPI_ManagerC1EPiPPPci
	.data
# -- End  _ZN11MPI_ManagerC1EPiPPPci
	.text
# -- Begin  _ZN11MPI_Manager14global_barrierEv
	.text
# mark_begin;
       .align    16,0x90
	.globl _ZN11MPI_Manager14global_barrierEv
# --- MPI_Manager::global_barrier(MPI_Manager *)
_ZN11MPI_Manager14global_barrierEv:
# parameter 1: %rdi
..B3.1:                         # Preds ..B3.0

### {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__ZN11MPI_Manager14global_barrierEv.28:
..L29:
                                                         #65.1

### #ifdef yaskSite_HAVE_MPI
###     MPI_Barrier(comm);

        movl      28(%rdi), %edi                                #67.5
#       MPI_Barrier(MPI_Comm)
        jmp       MPI_Barrier@PLT                               #67.5
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_ZN11MPI_Manager14global_barrierEv,@function
	.size	_ZN11MPI_Manager14global_barrierEv,.-_ZN11MPI_Manager14global_barrierEv
	.data
# -- End  _ZN11MPI_Manager14global_barrierEv
	.text
# -- Begin  _ZN11MPI_ManagerD2Ev
	.text
# mark_begin;
       .align    16,0x90
	.globl _ZN11MPI_ManagerD2Ev
# --- MPI_Manager::~MPI_Manager(MPI_Manager *const)
_ZN11MPI_ManagerD2Ev:
# parameter 1: %rdi
..B4.1:                         # Preds ..B4.0

### MPI_Manager::~MPI_Manager()

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__ZN11MPI_ManagerD2Ev.31:
..L32:
                                                         #52.14
#       MPI_Manager::~MPI_Manager(MPI_Manager *)
        jmp       _ZN11MPI_ManagerD1Ev@PLT                      #52.14
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_ZN11MPI_ManagerD2Ev,@function
	.size	_ZN11MPI_ManagerD2Ev,.-_ZN11MPI_ManagerD2Ev
	.data
# -- End  _ZN11MPI_ManagerD2Ev
	.text
# -- Begin  _ZN11MPI_ManagerD1Ev
	.text
# mark_begin;
       .align    16,0x90
	.globl _ZN11MPI_ManagerD1Ev
# --- MPI_Manager::~MPI_Manager(MPI_Manager *)
_ZN11MPI_ManagerD1Ev:
# parameter 1: %rdi
..B5.1:                         # Preds ..B5.0

### {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b, _ZN11MPI_ManagerD1Ev$$LSDA
..___tag_value__ZN11MPI_ManagerD1Ev.35:
..L36:
                                                         #53.1
        pushq     %rsi                                          #53.1
	.cfi_def_cfa_offset 16

### #ifdef yaskSite_HAVE_MPI
###     global_barrier();

..___tag_value__ZN11MPI_ManagerD1Ev.38:
#       MPI_Manager::global_barrier(MPI_Manager *)
        call      _ZN11MPI_Manager14global_barrierEv@PLT        #55.5
..___tag_value__ZN11MPI_ManagerD1Ev.39:
                                # LOE rbx rbp r12 r13 r14 r15
..B5.2:                         # Preds ..B5.1

###     MPI_Finalize();

..___tag_value__ZN11MPI_ManagerD1Ev.40:
#       MPI_Finalize()
        call      MPI_Finalize@PLT                              #56.5
..___tag_value__ZN11MPI_ManagerD1Ev.41:
                                # LOE rbx rbp r12 r13 r14 r15
..B5.4:                         # Preds ..B5.2

### #endif
### 
### #ifdef ENABLE_LOOPADAPT
###     loop_adapt_close();
### #endif
### }

        popq      %rcx                                          #62.1
	.cfi_def_cfa_offset 8
        ret                                                     #62.1
	.cfi_def_cfa_offset 16
                                # LOE
..___tag_value__ZN11MPI_ManagerD1Ev.34:
..B5.5:                         # Preds ..B5.2 ..B5.1
        movq      %rax, (%rsp)                                  #53.1
                                # LOE rbx rbp r12 r13 r14 r15
..B5.6:                         # Preds ..B5.5
        xorl      %eax, %eax                                    #53.1
        movq      (%rsp), %rdi                                  #53.1
..___tag_value__ZN11MPI_ManagerD1Ev.44:
        call      __cxa_call_unexpected@PLT                     #53.1
..___tag_value__ZN11MPI_ManagerD1Ev.45:
                                # LOE rbx rbp r12 r13 r14 r15
..B5.7:                         # Preds ..B5.6
        movq      (%rsp), %rdi                                  #53.1
..___tag_value__ZN11MPI_ManagerD1Ev.46:
        call      _Unwind_Resume@PLT                            #53.1
..___tag_value__ZN11MPI_ManagerD1Ev.47:
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_ZN11MPI_ManagerD1Ev,@function
	.size	_ZN11MPI_ManagerD1Ev,.-_ZN11MPI_ManagerD1Ev
	.section .gcc_except_table, "a"
	.align 4
_ZN11MPI_ManagerD1Ev$$LSDA:
	.byte	255
	.byte	155
	.uleb128	..___tag_value__ZN11MPI_ManagerD1Ev.52 - ..___tag_value__ZN11MPI_ManagerD1Ev.51
..___tag_value__ZN11MPI_ManagerD1Ev.51:
	.byte	1
	.uleb128	..___tag_value__ZN11MPI_ManagerD1Ev.50 - ..___tag_value__ZN11MPI_ManagerD1Ev.49
..___tag_value__ZN11MPI_ManagerD1Ev.49:
	.uleb128	..___tag_value__ZN11MPI_ManagerD1Ev.38 - ..___tag_value__ZN11MPI_ManagerD1Ev.35
	.uleb128	..___tag_value__ZN11MPI_ManagerD1Ev.41 - ..___tag_value__ZN11MPI_ManagerD1Ev.38
	.uleb128	..___tag_value__ZN11MPI_ManagerD1Ev.34 - ..___tag_value__ZN11MPI_ManagerD1Ev.35
	.byte	1
	.uleb128	..___tag_value__ZN11MPI_ManagerD1Ev.44 - ..___tag_value__ZN11MPI_ManagerD1Ev.35
	.uleb128	..___tag_value__ZN11MPI_ManagerD1Ev.47 - ..___tag_value__ZN11MPI_ManagerD1Ev.44
	.byte	0
	.byte	0
..___tag_value__ZN11MPI_ManagerD1Ev.50:
	.byte	127
	.byte	0
	.long	0
..___tag_value__ZN11MPI_ManagerD1Ev.52:
	.byte	0
	.data
# -- End  _ZN11MPI_ManagerD1Ev
	.section .rodata, "a"
	.align 1
__$U0:
	.long	1598640205
	.long	1634623821
	.long	7497063
	.type	__$U0,@object
	.size	__$U0,12
	.section .rodata.str1.4, "aMS",@progbits,1
	.align 4
.L_2__STRING.1:
	.long	1836017711
	.long	1885876069
	.long	1853173603
	.long	1966045810
	.long	813331054
	.long	795357744
	.long	1802723705
	.long	1935767855
	.long	1953059691
	.long	1920151397
	.long	1347235683
	.long	1632460617
	.long	1701273966
	.long	1885548146
	.word	112
	.type	.L_2__STRING.1,@object
	.size	.L_2__STRING.1,58
	.section .rodata.str1.32, "aMS",@progbits,1
	.align 32
.L_2__STRING.0:
	.long	825449243
	.long	1635343213
	.long	1767074675
	.long	542991732
	.long	1680164176
	.long	1381123360
	.long	1629508175
	.long	1931812980
	.long	1008740648
	.long	624587557
	.long	540687972
	.long	541675597
	.long	1869771365
	.long	1293957746
	.long	1415530832
	.long	1095062088
	.long	1163091780
	.long	1279347026
	.long	1145395785
	.long	1953459744
	.long	1869770784
	.long	1701079414
	.long	453652068
	.long	7155803
	.type	.L_2__STRING.0,@object
	.size	.L_2__STRING.0,96
	.data
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
