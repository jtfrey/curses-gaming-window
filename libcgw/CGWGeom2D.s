//
// CGWGeom.s
//
// Apple Silicon (ARM64) assembly language subroutines
// and functions.
//

.global _CGWPolarPointThetaClampWithThresholdsAsm
.align 4

// float <- S0
_CGWPolarPointThetaClampWithThresholdsAsm:
// (float theta -> S0,
//  float theta_min_x -> S1
//  float theta_min_y -> S2)
                    //
                    // S3:  2π
                    // S4:  scratch, π, π/2
                    // S5:  scratch
                    //         
                    ldr     S3, F0000_M_2PI
                    fabs    S4, S0                      // S4 = fabs(A0 = theta)
                    fdiv    S5, S4, S3                  // S5 = n_2xpi = fabs(theta) / 2π
                    frintz  S5, S5                      // S5 = n_2xpi = floor(n_2xpi)
                    fcmp    S0, #0.0                    // theta < 0?
                    b.ge    F0000_no_inc_n_2xpi
                    fmov    S6, #1.0                    // S6 = 0
                    fadd    S5, S5, S6                  // S5 = n_2xpi = n_2xpi + 1
                    b       F0000_calc_theta_prime      // no need to check for n_2xpi > 0
F0000_no_inc_n_2xpi:      
                    fcmp    S5, #0.0
                    b.eq    F0000_n_2xpi_is_zero
                    fneg    S5, S5                      // we will be adding 2π(-n_2xpi)
F0000_calc_theta_prime:   
                    fmadd   S0, S5, S3, S0              // S0 = theta = theta_prime = n_2xpi * ±2π + theta

F0000_n_2xpi_is_zero:     
                    ldr     S4, F0000_M_PI
                    fcmp    S0, S4
                    b.lt    F0000_skip_ge_pi
                    fsub    S0, S0, S3

F0000_skip_ge_pi:
                    fsub    S5, S0, S4                  // S4 still has π, so S5 = theta - π
                    fabs    S5, S5
                    ldr     S6, F0000_ZEROISH
                    fcmp    S5, S6                      // |theta - π| < 1e-6?
                    b.ge    F0000_skip_force_pi
                    fmov    S0, S4
                    fneg    S0, S0                      // theta = -π
                    
F0000_skip_force_pi:
                    fabs    S5, S0                      // |theta|
                    fcmp    S5, S1                      // |theta| < theta_min_x?
                    b.ge    F0000_skip_zero
                    fmov    S0, #0.0                    // return 0.0
                    ret
                    
F0000_skip_zero:
                    fsub    S6, S4, S1                  // π - theta_min_x
                    fcmp    S5, S6                      // |theta| > π - theta_min_x?
                    b.le    F0000_skip_neg_pi
                    ldr     S0, F0000_M_NEG_PI          // return -π
                    ret

F0000_skip_neg_pi:
                    ldr     S4, F0000_M_PI_2            // S4 = π/2
                    fsub    S5, S0, S4
                    fabs    S5, S5                      // |theta - π/2|
                    fcmp    S5, S2                      // |theta - π/2| < theta_min_y
                    b.ge    F0000_skip_pi_2
                    fmov    S0, S4                      // return π/2
                    ret
                    
F0000_skip_pi_2:
                    fadd    S5, S0, S4
                    fabs    S5, S5                      // |theta + π/2|
                    fcmp    S5, S2                      // |theta + π/2| < theta_min_y
                    b.ge    F0000_skip_neg_pi_2
                    fmov    S0, S4
                    fneg    S0, S0
                    
F0000_skip_neg_pi_2:
                    ret

F0000_ZEROISH:           .single 1e-6
F0000_M_PI:              .single 3.14159265358979323846264338327950288
F0000_M_NEG_PI:          .single -3.14159265358979323846264338327950288
F0000_M_2PI:             .single 6.28318530717958647692528676655900577
F0000_M_PI_2:            .single 1.57079632679489661923132169163975144





.global _CGWXFormMatrix2DDotPoint2DAsm
.align 4

    .equ    FLOAT_VEC_1_0, 0x000000003f800000

// CGWPoint2D* <- X0
_CGWXFormMatrix2DDotPoint2DAsm:
// (CGWPoint2D* out_p -> X0
//  CGWXFormMatrix2D* X -> X1,
//  CGWPoint2D* p -> X2)
    ldp     Q0, Q1, [X1]            // load the two rows of X into V0 and V1
    ldr     D2, [X2]                // load p.x, p.y into the lowest two slots
                                    // of V2.4S
    mov     X4, #FLOAT_VEC_1_0      // load the binary form of <1.0f, 0.0f> into X4
    mov     V2.D[1], X4             // move <1.0f, 0.0f> into the upper half of V2.4S
    fmul    V3.4S, V0.4S, V2.4S     // V3 = elements of V0 . V2 (first row of X . p)
    faddp   V3.4S, V3.4S, V3.4S     // V3[0] = V3[0] + V3[1]; V3[1] = V3[2] + V3[3]
    faddp   V3.4S, V3.4S, V3.4S     // V3[0] = V3[0] + V3[1]
                                    // p.x = V3[0]
    fmul    V4.4S, V1.4S, V2.4S     // V4 = elements of V1 . V2 (second row of X . p)
    faddp   V4.4S, V4.4S, V4.4S     // V4[0] = V4[0] + V4[1]; V4[1] = V4[2] + V4[3]
    faddp   V4.4S, V4.4S, V4.4S     // V4[0] = V4[0] + V4[1]
                                    // p.y = V4[0]
    
    stp     S3, S4, [X0]            // out_p = <S3, S4>
    
    // We haven't modified X0, so it still points to out_p; we can just return!
    ret


.global _CGWXFormMatrix2DScaledSumAsm
.align 4

// CGWXFormMatrix2D* <- X0
_CGWXFormMatrix2DScaledSumAsm:
// (CGWXFormMatrix2D* out_M -> X0,
//  float A -> S1,
//  CGWXFormMatrix2D* X -> X1,
//  CGWXFormMatrix2D* Y -> X2)
    ldp     Q2, Q3, [X2]            // load the two rows of Y into V2 and V3
    fmov    W3, S1                  // save a copy of S1 in W3 (we've loaded all of Y)
    cbnz    W3, 1f                  // if A != 0, skip ahead...
    stp     Q2, Q3, [X0]            // ...this is just a copy of Y into out_M
    ret
    
1:  ldp     Q0, Q1, [X1]            // load the two rows of X into V0 and V1
    ldr     W4, #0x3f8              // binary form of +1.0f
    lsl     W4, W4, #20
    cmp     W3, W4                  // does A == 1?
    b.ne    2f
    fadd    V0.4s, V2.4s, V0.4s     // since A == 1, it's an unscaled sum
    fadd    V1.4s, V3.4s, V1.4s
    stp     Q0, Q1, [X0]
    ret
  
2:  orr     W4, W4, #0x80000000     // toggle negative on our 1.0f register
    cmp     W3, W4                  // does A == -1?
    b.ne    3f
    fsub    V0.4s, V2.4s, V0.4s     // since A == -1, it's an unscaled difference
    fsub    V1.4s, V3.4s, V1.4s
    stp     Q0, Q1, [X0]
    ret

3:  dup     V4.4S, W3               // fill V4 with 4 copies of W3 (= scalar A)
    fmla    V0.4s, V2.4s, V4.4s     // V0 = A * X_1 + Y_1
    fmla    V1.4s, V3.4s, V4.4s     // V0 = A * X_2 + Y_2
    stp     Q0, Q1, [X0]
    ret
