# mark_description "Intel(R) C++ Intel(R) 64 Compiler for applications running on Intel(R) 64, Version 16.0.3.210 Build 20160415";
# mark_description "";
# mark_description "-I/home/hpc/unrz/unrz002h/yask/yaskSite/include -DyaskSite_EXPORTS -isystem /apps/intel/mpi/5.1.3.210/intel6";
# mark_description "4/include -qopenmp -fPIC -std=c++0x -Wall -Wextra -Werror -pedantic -Wunused-variable -save-temps -fsource-a";
# mark_description "sm -w3 -wd981 -wd383 -fPIC -o CMakeFiles/yaskSite.dir/src/util.cpp.o -c";
	.file "util.cpp"
	.text
..TXTST0:
# -- Begin  _Z14systemCallUtilPcS_
	.text
# mark_begin;
       .align    16,0x90
	.globl _Z14systemCallUtilPcS_
# --- systemCallUtil(char *, char *)
_Z14systemCallUtilPcS_:
# parameter 1: %rdi
# parameter 2: %rsi
..B1.1:                         # Preds ..B1.0

### {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__Z14systemCallUtilPcS_.1:
..L2:
                                                          #9.1

### #ifdef LOG_SYS_CALL
###     if(sysLogFileName)
###     {
###         FILE *sysCallLog;
###         sysCallLog = fopen(sysLogFileName, "a");
###         if(sysCallLog==NULL)
###         {
###             ERROR_PRINT("Could not open sys log file %s", sysLogFileName);
###         }
###         fprintf(sysCallLog, "%s\n", cmd);
###         fclose(sysCallLog);
###     }
### #else
###     UNUSED(sysLogFileName);
### #endif
### 
###     system(cmd);

#       system(const char *)
        jmp       system@PLT                                    #26.5
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_Z14systemCallUtilPcS_,@function
	.size	_Z14systemCallUtilPcS_,.-_Z14systemCallUtilPcS_
	.data
# -- End  _Z14systemCallUtilPcS_
	.text
# -- Begin  _Z5cachePc
	.text
# mark_begin;
       .align    16,0x90
	.globl _Z5cachePc
# --- cache(char *)
_Z5cachePc:
# parameter 1: %rsi
..B2.1:                         # Preds ..B2.0

### {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__Z5cachePc.4:
..L5:
                                                          #43.1
        movq      %rsi, %r8                                     #43.1
        movq      %rdi, %rcx                                    #43.1

###     if(strcmp(str,"L1")==0)

        movq      %r8, %rdi                                     #44.8
        lea       .L_2__STRING.1(%rip), %rsi                    #44.8
..L6:                                                           #44.8
        movb      (%rdi), %dl                                   #44.8
        cmpb      (%rsi), %dl                                   #44.8
        jne       ..L8          # Prob 50%                      #44.8
        testb     %dl, %dl                                      #44.8
        je        ..L7          # Prob 50%                      #44.8
        movb      1(%rdi), %dl                                  #44.8
        cmpb      1(%rsi), %dl                                  #44.8
        jne       ..L8          # Prob 50%                      #44.8
        addq      $2, %rdi                                      #44.8
        addq      $2, %rsi                                      #44.8
        testb     %dl, %dl                                      #44.8
        jne       ..L6          # Prob 50%                      #44.8
..L7:                                                           #
        xorl      %eax, %eax                                    #44.8
        jmp       ..L9          # Prob 100%                     #44.8
..L8:                                                           #
        sbbl      %eax, %eax                                    #44.8
        orl       $1, %eax                                      #44.8
..L9:                                                           #
                                # LOE rcx rbx rbp r8 r12 r13 r14 r15 eax
..B2.8:                         # Preds ..B2.1
        testl     %eax, %eax                                    #44.26
        je        ..B2.5        # Prob 28%                      #44.26
                                # LOE rcx rbx rbp r8 r12 r13 r14 r15
..B2.2:                         # Preds ..B2.8

###     {
###         return L1;
###     }
###     else if(strcmp(str,"L2")==0)

        movq      %r8, %rdi                                     #48.13
        lea       .L_2__STRING.0(%rip), %rsi                    #48.13
..L10:                                                          #48.13
        movb      (%rdi), %dl                                   #48.13
        cmpb      (%rsi), %dl                                   #48.13
        jne       ..L12         # Prob 50%                      #48.13
        testb     %dl, %dl                                      #48.13
        je        ..L11         # Prob 50%                      #48.13
        movb      1(%rdi), %dl                                  #48.13
        cmpb      1(%rsi), %dl                                  #48.13
        jne       ..L12         # Prob 50%                      #48.13
        addq      $2, %rdi                                      #48.13
        addq      $2, %rsi                                      #48.13
        testb     %dl, %dl                                      #48.13
        jne       ..L10         # Prob 50%                      #48.13
..L11:                                                          #
        xorl      %eax, %eax                                    #48.13
        jmp       ..L13         # Prob 100%                     #48.13
..L12:                                                          #
        sbbl      %eax, %eax                                    #48.13
        orl       $1, %eax                                      #48.13
..L13:                                                          #
                                # LOE rcx rbx rbp r12 r13 r14 r15 eax
..B2.9:                         # Preds ..B2.2
        testl     %eax, %eax                                    #48.31
        jne       ..B2.4        # Prob 50%                      #48.31
                                # LOE rcx rbx rbp r12 r13 r14 r15
..B2.3:                         # Preds ..B2.9

###     {
###         return L2;

        movq      L2(%rip), %rdx                                #50.16
        movq      %rcx, %rax                                    #50.16
        movq      8+L2(%rip), %rsi                              #50.16
        movq      16+L2(%rip), %rdi                             #50.16
        movq      24+L2(%rip), %r8                              #50.16
        movq      %rdx, (%rcx)                                  #50.16
        movq      %rsi, 8(%rcx)                                 #50.16
        movq      %rdi, 16(%rcx)                                #50.16
        movq      %r8, 24(%rcx)                                 #50.16
        ret                                                     #50.16
                                # LOE
..B2.4:                         # Preds ..B2.9

###     }
###     else
###     {
###         return L3;

        movq      L3(%rip), %rdx                                #54.16
        movq      %rcx, %rax                                    #54.16
        movq      8+L3(%rip), %rsi                              #54.16
        movq      16+L3(%rip), %rdi                             #54.16
        movq      24+L3(%rip), %r8                              #54.16
        movq      %rdx, (%rcx)                                  #54.16
        movq      %rsi, 8(%rcx)                                 #54.16
        movq      %rdi, 16(%rcx)                                #54.16
        movq      %r8, 24(%rcx)                                 #54.16
        ret                                                     #54.16
                                # LOE
..B2.5:                         # Preds ..B2.8
        movq      L1(%rip), %rdx                                #46.16
        movq      %rcx, %rax                                    #46.16
        movq      8+L1(%rip), %rsi                              #46.16
        movq      16+L1(%rip), %rdi                             #46.16
        movq      24+L1(%rip), %r8                              #46.16
        movq      %rdx, (%rcx)                                  #46.16
        movq      %rsi, 8(%rcx)                                 #46.16
        movq      %rdi, 16(%rcx)                                #46.16
        movq      %r8, 24(%rcx)                                 #46.16
        ret                                                     #46.16
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_Z5cachePc,@function
	.size	_Z5cachePc,.-_Z5cachePc
	.data
# -- End  _Z5cachePc
	.text
# -- Begin  _Z12removeSpacesPc
	.text
# mark_begin;
       .align    16,0x90
	.globl _Z12removeSpacesPc
# --- removeSpaces(char *)
_Z12removeSpacesPc:
# parameter 1: %rdi
..B3.1:                         # Preds ..B3.0

### {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__Z12removeSpacesPc.15:
..L16:
                                                         #59.1
        pushq     %r13                                          #59.1
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
        pushq     %r14                                          #59.1
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
        pushq     %r15                                          #59.1
	.cfi_def_cfa_offset 32
	.cfi_offset 15, -32

###     char* str_cpy = strdup(str);

        movq      %rdi, %rcx                                    #60.21
        call      __intel_sse2_strdup@PLT                       #60.21
                                # LOE rax rbx rbp r12
..B3.11:                        # Preds ..B3.1
        movq      %rax, %r15                                    #60.21
                                # LOE rbx rbp r12 r15
..B3.2:                         # Preds ..B3.11
        lea       1(%r15), %r14                                 #60.21

###     char* i = str_cpy;
###     while(*i != 0)

        movb      (%r15), %r13b                                 #62.12
        testb     %r13b, %r13b                                  #62.17
        je        ..B3.8        # Prob 10%                      #62.17
                                # LOE rbx rbp r12 r14 r15 r13b
..B3.3:                         # Preds ..B3.2

###     {
###        if(isspace(*i))

#       __ctype_b_loc()
        call      __ctype_b_loc@PLT                             #64.11
                                # LOE rax rbx rbp r12 r14 r15 r13b
..B3.12:                        # Preds ..B3.3
        movq      (%rax), %rdx                                  #64.11
                                # LOE rdx rbx rbp r12 r14 r15 r13b
..B3.4:                         # Preds ..B3.6 ..B3.12
        movsbq    %r13b, %rcx                                   #64.11
        movzwl    (%rdx,%rcx,2), %esi                           #64.11
        testl     $8192, %esi                                   #64.11
        je        ..B3.6        # Prob 50%                      #64.11
                                # LOE rdx rbx rbp r12 r14 r15
..B3.5:                         # Preds ..B3.4

###        {
###            *i =  '_';

        movb      $95, -1(%r14)                                 #66.13
                                # LOE rdx rbx rbp r12 r14 r15
..B3.6:                         # Preds ..B3.4 ..B3.5
        movb      (%r14), %r13b                                 #62.12

###        }
###        i++;

        incq      %r14                                          #68.8
        testb     %r13b, %r13b                                  #62.17
        jne       ..B3.4        # Prob 82%                      #62.17
                                # LOE rdx rbx rbp r12 r14 r15 r13b
..B3.8:                         # Preds ..B3.6 ..B3.2

###     }
### 
###     return str_cpy;

        movq      %r15, %rax                                    #71.12
	.cfi_restore 15
        popq      %r15                                          #71.12
	.cfi_def_cfa_offset 24
	.cfi_restore 14
        popq      %r14                                          #71.12
	.cfi_def_cfa_offset 16
	.cfi_restore 13
        popq      %r13                                          #71.12
	.cfi_def_cfa_offset 8
        ret                                                     #71.12
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_Z12removeSpacesPc,@function
	.size	_Z12removeSpacesPc,.-_Z12removeSpacesPc
	.data
# -- End  _Z12removeSpacesPc
	.text
# -- Begin  _ZN10cache_infoC2EPci
	.text
# mark_begin;
       .align    16,0x90
	.globl _ZN10cache_infoC2EPci
# --- cache_info::cache_info(cache_info *const, char *, int)
_ZN10cache_infoC2EPci:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %edx
..B4.1:                         # Preds ..B4.0

### cache_info::cache_info(char* cache_str, int bytePerWord)

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__ZN10cache_infoC2EPci.30:
..L31:
                                                         #29.13
#       cache_info::cache_info(cache_info *, char *, int)
        jmp       _ZN10cache_infoC1EPci@PLT                     #29.13
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_ZN10cache_infoC2EPci,@function
	.size	_ZN10cache_infoC2EPci,.-_ZN10cache_infoC2EPci
	.data
# -- End  _ZN10cache_infoC2EPci
	.text
# -- Begin  __sti__$E
	.text
# mark_begin;
       .align    16,0x90
# --- __sti__$E()
__sti__$E:
..B5.1:                         # Preds ..B5.0
	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value___sti__$E.33:
..L34:
                                                         #
        pushq     %rsi                                          #
	.cfi_def_cfa_offset 16

### static cache_info L1(L1_cache);

        lea       L1(%rip), %rdi                                #18.22
        lea       .L_2__STRING.4(%rip), %rsi                    #18.22
        movl      $8, %edx                                      #18.22
..___tag_value___sti__$E.36:
#       cache_info::cache_info(cache_info *, char *, int)
        call      _ZN10cache_infoC1EPci@PLT                     #18.22
..___tag_value___sti__$E.37:
                                # LOE rbx rbp r12 r13 r14 r15
..B5.2:                         # Preds ..B5.1
        lea       L2(%rip), %rdi                                #18.22
        lea       .L_2__STRING.5(%rip), %rsi                    #18.22
        movl      $8, %edx                                      #18.22
..___tag_value___sti__$E.38:
#       cache_info::cache_info(cache_info *, char *, int)
        call      _ZN10cache_infoC1EPci@PLT                     #18.22
..___tag_value___sti__$E.39:
                                # LOE rbx rbp r12 r13 r14 r15
..B5.3:                         # Preds ..B5.2
        lea       L3(%rip), %rdi                                #18.22
        lea       .L_2__STRING.6(%rip), %rsi                    #18.22
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
# -- Begin  _ZN10cache_infoC1EPci
	.text
# mark_begin;
       .align    16,0x90
	.globl _ZN10cache_infoC1EPci
# --- cache_info::cache_info(cache_info *, char *, int)
_ZN10cache_infoC1EPci:
# parameter 1: %rdi
# parameter 2: %rsi
# parameter 3: %edx
..B6.1:                         # Preds ..B6.0

### {

	.cfi_startproc
	.cfi_personality 0x93,DW.ref.__gxx_personality_v0
..___tag_value__ZN10cache_infoC1EPci.42:
..L43:
                                                         #30.1
        pushq     %r12                                          #30.1
	.cfi_def_cfa_offset 16
	.cfi_offset 12, -16
        pushq     %r13                                          #30.1
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
        pushq     %r14                                          #30.1
	.cfi_def_cfa_offset 32
	.cfi_offset 14, -32
        pushq     %r15                                          #30.1
	.cfi_def_cfa_offset 40
	.cfi_offset 15, -40
        pushq     %rbx                                          #30.1
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
        subq      $16, %rsp                                     #30.1
	.cfi_def_cfa_offset 64

###     //writable string
###     char* cache_str_w = strdup(cache_str);

        movq      %rsi, %rcx                                    #32.25
        movl      %edx, %r13d                                   #30.1
        movq      %rdi, %r12                                    #30.1
        call      __intel_sse2_strdup@PLT                       #32.25
                                # LOE rax rbp r12 r13d
..B6.12:                        # Preds ..B6.1
        movq      %rax, %rbx                                    #32.25
                                # LOE rbx rbp r12 r13d
..B6.2:                         # Preds ..B6.12

###     char* byte_str = strtok(cache_str_w,";");

        movq      %rbx, %rcx                                    #33.22
        lea       .L_2__STRING.2(%rip), %rdx                    #33.22
        call      __intel_sse2_strtok@PLT                       #33.22
                                # LOE rax rbx rbp r12 r13d
..B6.13:                        # Preds ..B6.2
        movq      %rax, %r14                                    #33.22
                                # LOE rbx rbp r12 r14 r13d
..B6.3:                         # Preds ..B6.13

###     char* shared_str = strtok(NULL,";");

        xorl      %ecx, %ecx                                    #34.24
        lea       .L_2__STRING.2(%rip), %rdx                    #34.24
        call      __intel_sse2_strtok@PLT                       #34.24
                                # LOE rax rbx rbp r12 r14 r13d
..B6.14:                        # Preds ..B6.3
        movq      %rax, %r15                                    #34.24
                                # LOE rbx rbp r12 r14 r15 r13d
..B6.4:                         # Preds ..B6.14

###     bytes=atof(byte_str);

        movq      %r14, %rdi                                    #35.11
        xorl      %esi, %esi                                    #35.11
#       strtod(const char *, char **)
        call      strtod@PLT                                    #35.11
                                # LOE rbx rbp r12 r15 r13d xmm0
..B6.15:                        # Preds ..B6.4
        movaps    %xmm0, %xmm1                                  #35.11
                                # LOE rbx rbp r12 r15 r13d xmm1
..B6.5:                         # Preds ..B6.15

###     shared=(strstr(shared_str, "shared")!=NULL)?true:false;

        movq      %r15, %rdi                                    #36.13
        lea       .L_2__STRING.3(%rip), %rsi                    #36.13
        movsd     %xmm1, (%r12)                                 #35.5
        movsd     %xmm1, (%rsp)                                 #36.13
        call      strstr@PLT                                    #36.13
                                # LOE rax rbx rbp r12 r13d
..B6.16:                        # Preds ..B6.5
        movsd     (%rsp), %xmm1                                 #
        testq     %rax, %rax                                    #36.43
        je        ..B6.7        # Prob 12%                      #36.43
                                # LOE rbx rbp r12 r13d xmm1 ymm1 zmm1
..B6.6:                         # Preds ..B6.16
        movb      $1, 24(%r12)                                  #36.5
        jmp       ..B6.8        # Prob 100%                     #36.5
                                # LOE rbx rbp r12 r13d xmm1 ymm1 zmm1
..B6.7:                         # Preds ..B6.16
        movb      $0, 24(%r12)                                  #36.5
                                # LOE rbx rbp r12 r13d xmm1 ymm1 zmm1
..B6.8:                         # Preds ..B6.6 ..B6.7

###     name=cache_str_w;
###     words=bytes/bytePerWord;

        pxor      %xmm0, %xmm0                                  #38.17

###     free(cache_str_w);

        movq      %rbx, %rdi                                    #39.5
        cvtsi2sd  %r13d, %xmm0                                  #38.17
        divsd     %xmm0, %xmm1                                  #38.17
        movq      %rbx, 16(%r12)                                #37.5
        movsd     %xmm1, 8(%r12)                                #38.5
        addq      $16, %rsp                                     #39.5
	.cfi_def_cfa_offset 48
	.cfi_restore 3
        popq      %rbx                                          #39.5
	.cfi_def_cfa_offset 40
	.cfi_restore 15
        popq      %r15                                          #39.5
	.cfi_def_cfa_offset 32
	.cfi_restore 14
        popq      %r14                                          #39.5
	.cfi_def_cfa_offset 24
	.cfi_restore 13
        popq      %r13                                          #39.5
	.cfi_def_cfa_offset 16
	.cfi_restore 12
        popq      %r12                                          #39.5
	.cfi_def_cfa_offset 8
#       free(void *)
        jmp       free@PLT                                      #39.5
        .align    16,0x90
	.cfi_endproc
                                # LOE
# mark_end;
	.type	_ZN10cache_infoC1EPci,@function
	.size	_ZN10cache_infoC1EPci,.-_ZN10cache_infoC1EPci
	.data
# -- End  _ZN10cache_infoC1EPci
	.bss
	.align 16
L2:
	.type	L2,@object
	.size	L2,32
	.space 32	# pad
	.align 16
L3:
	.type	L3,@object
	.size	L3,32
	.space 32	# pad
	.align 16
L1:
	.type	L1,@object
	.size	L1,32
	.space 32	# pad
	.section .rodata.str1.4, "aMS",@progbits,1
	.align 4
.L_2__STRING.1:
	.word	12620
	.byte	0
	.type	.L_2__STRING.1,@object
	.size	.L_2__STRING.1,3
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.0:
	.word	12876
	.byte	0
	.type	.L_2__STRING.0,@object
	.size	.L_2__STRING.0,3
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.4:
	.long	909586995
	.long	1881160504
	.long	1635150194
	.word	25972
	.byte	0
	.type	.L_2__STRING.4,@object
	.size	.L_2__STRING.4,15
	.space 1, 0x00 	# pad
	.align 4
.L_2__STRING.5:
	.long	825374258
	.long	540750900
	.long	1986622064
	.long	6648929
	.type	.L_2__STRING.5,@object
	.size	.L_2__STRING.5,16
	.align 4
.L_2__STRING.6:
	.long	825374258
	.long	808465460
	.long	1752375355
	.long	1684370017
	.byte	0
	.type	.L_2__STRING.6,@object
	.size	.L_2__STRING.6,17
	.space 3, 0x00 	# pad
	.align 4
.L_2__STRING.2:
	.word	59
	.type	.L_2__STRING.2,@object
	.size	.L_2__STRING.2,2
	.space 2, 0x00 	# pad
	.align 4
.L_2__STRING.3:
	.long	1918986355
	.word	25701
	.byte	0
	.type	.L_2__STRING.3,@object
	.size	.L_2__STRING.3,7
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
