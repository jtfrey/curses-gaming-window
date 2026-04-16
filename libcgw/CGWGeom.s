//
// CGWGeom.s
//
// Apple Silicon (ARM64) assembly language subroutines
// and functions.
//

.global _CGWPointPThetaClampWithThresholdsAsm
.align 4

// double <- D0
_CGWPointPThetaClampWithThresholdsAsm:
// (double theta -> D0,
//  double theta_min_x -> D1
//  double theta_min_y -> D2)
                    //
                    // D3:  2π
                    // D4:  scratch, π, π/2
                    // D5:  scratch
                    //         
                    ldr     D3, F0000_M_2PI
                    fabs    D4, D0                      // D4 = fabs(A0 = theta)
                    fdiv    D5, D4, D3                  // D5 = n_2xpi = fabs(theta) / 2π
                    frintz  D5, D5                      // D5 = n_2xpi = floor(n_2xpi)
                    fcmp    D0, #0.0                    // theta < 0?
                    b.ge    F0000_no_inc_n_2xpi
                    fmov    D6, #1.0                    // D6 = 0
                    fadd    D5, D5, D6                  // D5 = n_2xpi = n_2xpi + 1
                    b       F0000_calc_theta_prime      // no need to check for n_2xpi > 0
F0000_no_inc_n_2xpi:      
                    fcmp    D5, #0.0
                    b.eq    F0000_n_2xpi_is_zero
                    fneg    D5, D5                      // we will be adding 2π(-n_2xpi)
F0000_calc_theta_prime:   
                    fmadd   D0, D5, D3, D0              // D0 = theta = theta_prime = n_2xpi * ±2π + theta

F0000_n_2xpi_is_zero:     
                    ldr     D4, F0000_M_PI
                    fcmp    D0, D4
                    b.lt    F0000_skip_ge_pi
                    fsub    D0, D0, D3

F0000_skip_ge_pi:
                    fsub    D5, D0, D4                  // D4 still has π, so D5 = theta - π
                    fabs    D5, D5
                    ldr     D6, F0000_ZEROISH
                    fcmp    D5, D6                      // |theta - π| < 1e-6?
                    b.ge    F0000_skip_force_pi
                    fmov    D0, D4
                    fneg    D0, D0                      // theta = -π
                    
F0000_skip_force_pi:
                    fabs    D5, D0                      // |theta|
                    fcmp    D5, D1                      // |theta| < theta_min_x?
                    b.ge    F0000_skip_zero
                    fmov    D0, #0.0                    // return 0.0
                    ret
                    
F0000_skip_zero:
                    fsub    D6, D4, D1                  // π - theta_min_x
                    fcmp    D5, D6                      // |theta| > π - theta_min_x?
                    b.le    F0000_skip_neg_pi
                    ldr     D0, F0000_M_NEG_PI          // return -π
                    ret

F0000_skip_neg_pi:
                    ldr     D4, F0000_M_PI_2            // D4 = π/2
                    fsub    D5, D0, D4
                    fabs    D5, D5                      // |theta - π/2|
                    fcmp    D5, D2                      // |theta - π/2| < theta_min_y
                    b.ge    F0000_skip_pi_2
                    fmov    D0, D4                      // return π/2
                    ret
                    
F0000_skip_pi_2:
                    fadd    D5, D0, D4
                    fabs    D5, D5                      // |theta + π/2|
                    fcmp    D5, D2                      // |theta + π/2| < theta_min_y
                    b.ge    F0000_skip_neg_pi_2
                    fmov    D0, D4
                    fneg    D0, D0
                    
F0000_skip_neg_pi_2:
                    ret

F0000_ZEROISH:           .double 1e-6
F0000_M_PI:              .double 3.14159265358979323846264338327950288
F0000_M_NEG_PI:          .double -3.14159265358979323846264338327950288
F0000_M_2PI:             .double 6.28318530717958647692528676655900577
F0000_M_PI_2:            .double 1.57079632679489661923132169163975144
