//
// CGWMath.s
//
// Apple Silicon (ARM64) assembly language subroutines
// and functions.
//

.global _CGWQSqrtFAsm
.align 4

// float <- S0
_CGWQSqrtFAsm:
// (float x -> S0, int n_iter -> W0)
                    //
                    // W1:  x_bits
                    // W2:  C
                    // S0:  y_approx
                    // S1:  0.5
                    // S2:  copy of x
                    //
                    fmov    S2, S0                      // Copy of x to S2
                                                        // (this frees-up S0 for
                                                        //  the result since we
                                                        //  need to use x below)
                    fmov    W1, S2                      // x_bits = *(int*)&S0
                    mov     W2, #0x4b89                 // C (1 + NOT bits 0-15)
                    movk    W2, #0x1fb5, LSL#16         // C (NOT bits 16-31)
                    add     W1, W2, W1, ASR#1           // x_bits = C + (x_bits >> 1)
                    fmov    S0, W1                      // y = *(float*)&W1
                                                        // (we put it in S0 since that's
                                                        //  the return value register)
                    cbz     W0, _CGWQSqrtFAsm_end       // n_iter == 0? exit now
                    fmov    S1, #0.5                    // const 0.5
_CGWQSqrtFAsm_newton_step:
                    fdiv    S3, S2, S0                  // S3 = x/y
                    fadd    S0, S0, S3                  // y = (x/y) + y
                    fmul    S0, S0, S1                  // y = 0.5 * ((x/y) + y)
                    subs    W0, W0, #1                  // n_iter--v
                    b.ne    _CGWQSqrtFAsm_newton_step   // n_iter != 0? loop again
                    
_CGWQSqrtFAsm_end:  ret
