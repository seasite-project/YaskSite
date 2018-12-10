        movl      $111, %ebx # INSERTED BY KERNCRAFT IACA MARKER UTILITY
        .byte     100        # INSERTED BY KERNCRAFT IACA MARKER UTILITY
        .byte     103        # INSERTED BY KERNCRAFT IACA MARKER UTILITY
        .byte     144        # INSERTED BY KERNCRAFT IACA MARKER UTILITY
        vmulsd    (%rdx,%rsi,8), %xmm0, %xmm1                   #385.51	src/realv.hpp:385  51
        vaddsd    8(%r14,%rsi,8), %xmm1, %xmm2                  #353.51	src/realv.hpp:353  51
        vaddsd    -8(%r14,%rsi,8), %xmm2, %xmm3                 #353.51	src/realv.hpp:353  51
        vaddsd    (%r15,%rsi,8), %xmm3, %xmm4                   #353.51	src/realv.hpp:353  51
        vaddsd    (%r8,%rsi,8), %xmm4, %xmm5                    #353.51	src/realv.hpp:353  51
        vmovsd    %xmm5, (%rax,%rsi,8)                          #299.20	src/realv.hpp:299  20  is_stmt 1
        incq      %rsi                                          #55.2	src/stencil_sub_block_loops.hpp:55  2
        cmpq      %rcx, %rsi                                    #55.2	src/stencil_sub_block_loops.hpp:55  2
        jb        ..B162.8      # Prob 82%                      #55.2	src/stencil_sub_block_loops.hpp:55  2  is_stmt 1
        movl      $222, %ebx # INSERTED BY KERNCRAFT IACA MARKER UTILITY
        .byte     100        # INSERTED BY KERNCRAFT IACA MARKER UTILITY
        .byte     103        # INSERTED BY KERNCRAFT IACA MARKER UTILITY
        .byte     144        # INSERTED BY KERNCRAFT IACA MARKER UTILITY

