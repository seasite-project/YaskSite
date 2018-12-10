
'/home/hpc/unrz/unrz002h/yask/realv_grids.s'...

'/home/hpc/unrz/unrz002h/yask/stencil_calc.s'...

'/home/hpc/unrz/unrz002h/yask/stencil_main.s'...

SIMD loop ..B162.8:
        vmulsd    (%rdx,%rsi,8), %xmm0, %xmm1                   #385.51	src/realv.hpp:385  51
        vaddsd    8(%r14,%rsi,8), %xmm1, %xmm2                  #353.51	src/realv.hpp:353  51
        vaddsd    -8(%r14,%rsi,8), %xmm2, %xmm3                 #353.51	src/realv.hpp:353  51
        vaddsd    (%r15,%rsi,8), %xmm3, %xmm4                   #353.51	src/realv.hpp:353  51
        vaddsd    (%r8,%rsi,8), %xmm4, %xmm5                    #353.51	src/realv.hpp:353  51
        vmovsd    %xmm5, (%rax,%rsi,8)                          #299.20	src/realv.hpp:299  20  is_stmt 1
        incq      %rsi                                          #55.2	src/stencil_sub_block_loops.hpp:55  2
        cmpq      %rcx, %rsi                                    #55.2	src/stencil_sub_block_loops.hpp:55  2
        jb        ..B162.8      # Prob 82%                      #55.2	src/stencil_sub_block_loops.hpp:55  2  is_stmt 1
9 total instrs
Instr counts per instr type (FLOP count is a subtotal):
   5  SIMD FLOP
   4  SIMD add
   1  SIMD move
   1  SIMD mul
   1  compare
   1  jump
   1  other instr
Instr counts per operand type:
   5  64-bit SIMD non-stack load
   1  64-bit SIMD non-stack store
   3  non-SIMD reg only
Instr counts per source file:
   6  src/realv.hpp
   3  src/stencil_sub_block_loops.hpp
Num SIMD regs used: 6

'/home/hpc/unrz/unrz002h/yask/utils.s'...
