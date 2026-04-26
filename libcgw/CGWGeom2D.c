/**
 * @file CGWGeom2D.c
 * @brief Curses Gaming Window, Geometrical support
 * 
 * Source to accompany the header.
 *
 */

#include "CGWGeom2D.h"

const CGWPoint2D CGWPoint2DOrigin = { .x = 0.0f, .y = 0.0f };
const CGWPointI2D CGWPointI2DOrigin = { .x = 0, .y = 0 };
const CGWPointFP2D CGWPointFP2DOrigin = { .x = CGWFixedPrecZero, .y = CGWFixedPrecZero };
const CGWPolarPoint CGWPolarPointOrigin = { .r = 0.0f, .theta = 0.0f };

//

static
float
__CGWRoundFInf(
    float   v
)
{
    return ( v < 0 ) ? floor(v) : ceil(v);
}

static
float
__CGWRoundFZero(
    float   v
)
{
    return ( v < 0 ) ? ceil(v) : floor(v);
}

static CGWCoordConvRoundingFn __CGWCoordConvRoundingFns[kCGWCoordConvRoundModeMax] = {
            truncf,
            roundf,
            ceilf,
            floorf,
            __CGWRoundFInf,
            __CGWRoundFZero
        };

const CGWCoordConvRoundingFn *CGWCoordConvRoundingFns = &__CGWCoordConvRoundingFns[0];

static CGWCoordConvFPRoundingFn __CGWCoordConvFPRoundingFns[kCGWCoordConvRoundModeMax] = {
            CGWFixedPrecTrunc,
            CGWFixedPrecRound,
            CGWFixedPrecCeil,
            CGWFixedPrecFloor,
            CGWFixedPrecRoundInf,
            CGWFixedPrecRoundZero
        };

const CGWCoordConvFPRoundingFn *CGWCoordConvFPRoundingFns = &__CGWCoordConvFPRoundingFns[0];

//

float CGWPolarPointThetaMinX = 1e-8;
float CGWPolarPointThetaMinY = 1e-8;

//

void
CGWPolarPointSetGlobalThetaMin(
    int         w,
    int         h
)
{
    CGWPolarPointCalculateThetaMin(w, h, &CGWPolarPointThetaMinX, &CGWPolarPointThetaMinY);
}

//

void
CGWPolarPointCalculateThetaMin(
    int         w,
    int         h,
    float       *theta_min_x,
    float       *theta_min_y
)
{
    float       inv_half_w = 2.0 / (float)w;
    float       inv_half_h = 2.0 / (float)h;
    
    // Since dimensions are always positive, the acos/asin
    // will both be positive -- as desired:
    *theta_min_x = asinf(inv_half_h);
    *theta_min_y = asinf(inv_half_w);
}

//

CGWPolarPoint
CGWPolarPointSum(
    CGWPolarPoint   p1,
    CGWPolarPoint   p2
)
{
    CGWPolarPoint   out_p;

    if ( fabs(p1.r) < 1e-6 ) {
        out_p = p2;
    }
    else if ( fabs(p2.r) < 1e-6 ) {
        out_p = p1;
    }
    else {
        double      sin_theta_1 = sin(p1.theta),
                    sin_theta_2 = sin(p2.theta),
                    cos_theta_1 = cos(p1.theta),
                    cos_theta_2 = cos(p2.theta),
                    r1r2 = p1.r * p2.r,
                    r1_r2 = p1.r / p2.r;
        
        out_p.r = sqrt(r1r2 * (r1_r2 + 1.0/r1_r2 + 2.0 * (cos_theta_1 * cos_theta_2 + sin_theta_1 * sin_theta_2)));
        out_p.theta = atan2(p1.r * cos_theta_1 + p2.r * cos_theta_2, p1.r * sin_theta_1 + p2.r * sin_theta_2);
        CGWPolarPointStandardize(&out_p, &out_p, 1e-6);
    }
    return out_p;
}

//

#ifndef HAVE_APPLE_AARCH64

CGWPolarPoint*
CGWPolarPointStandardize(
    CGWPolarPoint   *p_out,
    CGWPolarPoint   *p_in,
    float           epsilon
)
{
    float           r, theta = p_in->theta, theta_prime;
    int             n_2xpi;
    
    if ( p_in->r < 0.0f ) {
        r = -p_in->r;
        if ( theta >= 0.0f ) {
            theta -= M_PI;
        } else {
            theta += M_PI;
        }
    } else {
        r = p_in->r;
    }
    if ( fabs(r) < epsilon ) {
        r = theta = 0.0f;
    } else {
        n_2xpi = floor(fabs(theta) / M_2xPI) + (theta < 0.0f ? 1 : 0);
        if ( n_2xpi ) {
            theta_prime = theta + ((theta < 0.0f) ? n_2xpi * M_2xPI : -n_2xpi * M_2xPI);
        } else {
            theta_prime = theta;
        }
        // theta_prime is now in the range [0, 2π].  Shift to [-π, π]:
        if ( theta_prime >= M_PI ) theta_prime -= M_2xPI;
        
        if ( fabs(theta_prime) < epsilon ) {
            theta_prime = 0.0f;
        }
        else if ( fabs(theta_prime - M_PI) < epsilon ) {
            theta_prime = -M_PI;
        }
        else if ( fabs(theta_prime - M_PI_2) < epsilon ) {
            theta_prime = M_PI_2;
        }
        else if ( fabs(theta_prime + M_PI_2) < epsilon ) {
            theta_prime = -M_PI_2;
        }
    }    
    p_out->r = r, p_out->theta = theta_prime;

    return p_out;
}

//

float
CGWPolarPointThetaClampWithThresholds(
    float           theta,
    float           theta_min_x,
    float           theta_min_y
)
{
    int             n_2xpi = floor(fabs(theta) / M_2xPI) + (theta < 0 ? 1 : 0);
    double          theta_prime = theta, abs_theta;
    
    if ( n_2xpi ) {
        theta_prime = theta + ((theta < 0.0f) ? n_2xpi * M_2xPI : -n_2xpi * M_2xPI);
    }
    // theta_prime is now in the range [0, 2π].  Shift to [-π, π]:
    if ( theta_prime >= M_PI ) theta_prime -= M_2xPI;
    // force π to be -π
    if ( fabs(theta_prime - M_PI) < 1e-6 ) theta_prime = -M_PI;
    
    //
    // Special cases when theta_prime is close to {0,-π}:
    //
    abs_theta = fabs(theta_prime);
    if ( abs_theta < theta_min_x ) return 0.0f;
    if ( abs_theta > M_PI - theta_min_x ) return -M_PI;
    
    // Special cases when theta_prime is close to {±π/2}:
    if ( fabs(theta_prime - M_PI_2) < theta_min_y ) return M_PI_2;
    if ( fabs(theta_prime + M_PI_2) < theta_min_y ) return -M_PI_2;
    
    return theta_prime;
}

#endif

//

CGWPolarPoint
CGWPointI2DToPolarPoint(
    CGWPointI2D     p
)
{
    CGWPolarPoint   p_out;
    
    if ( p.x == 0 ) {
        //  y-axis aligned:
        if ( p.y < 0 ) p_out.r = -p.y, p_out.theta = -M_PI_2;
        else if ( p.y > 0) p_out.r = p.y, p_out.theta = M_PI_2;
        else p_out.r = 0, p_out.theta = 0.0f;
    }
    else if ( p.y == 0 ) {
        //  x-axis aligned:
        if ( p.x < 0 ) p_out.r = -p.x, p_out.theta = -M_PI;
        else if ( p.x > 0) p_out.r = p.x, p_out.theta = M_PI;
        else p_out.r = 0, p_out.theta = 0.0f;
    }
    else {
        p_out.r = sqrt(p.x * p.x + p.y * p.y), p_out.theta = atan2(p.y, p.x);
    }
    return p_out;
}

//

CGWXFormMatrix2D*
CGWXFormMatrix2DRotate(
    CGWXFormMatrix2D    *M,
    float               theta
)
{
    float               sin_theta = sin(theta),
                        cos_theta = cos(theta);
    float               a = CGWXFormMatrix2D_IJ(M->M, 0, 0),
                        b = CGWXFormMatrix2D_IJ(M->M, 0, 1),
                        c = CGWXFormMatrix2D_IJ(M->M, 0, 2),
                        d = CGWXFormMatrix2D_IJ(M->M, 1, 0),
                        e = CGWXFormMatrix2D_IJ(M->M, 1, 1),
                        f = CGWXFormMatrix2D_IJ(M->M, 1, 2);
                        
    CGWXFormMatrix2D_IJ(M->M, 0, 0) = cos_theta * a - sin_theta * d;
    CGWXFormMatrix2D_IJ(M->M, 0, 1) = cos_theta * b - sin_theta * e;
    CGWXFormMatrix2D_IJ(M->M, 0, 2) = cos_theta * c - sin_theta * f;
    
    CGWXFormMatrix2D_IJ(M->M, 1, 0) = sin_theta * a + cos_theta * d;
    CGWXFormMatrix2D_IJ(M->M, 1, 1) = sin_theta * b + cos_theta * e;
    CGWXFormMatrix2D_IJ(M->M, 1, 2) = sin_theta * c + cos_theta * f;
    return M;
}

//

#ifndef HAVE_APPLE_AARCH64_NEON

CGWXFormMatrix2D*
CGWXFormMatrix2DMultiply(
    CGWXFormMatrix2D    *out_M,
    CGWXFormMatrix2D    *X,
    CGWXFormMatrix2D    *Y
)
{
    if ( out_M == X || out_M == Y ) {
        CGWXFormMatrix2D    tmpM;
        CGWXFormMatrix2D_IJ(tmpM.M, 0, 0) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 0) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(tmpM.M, 0, 1) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 1) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(tmpM.M, 0, 2) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 2) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 2) + CGWXFormMatrix2D_IJ(X->M, 0, 2);
        
        CGWXFormMatrix2D_IJ(tmpM.M, 1, 0) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 0) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(tmpM.M, 1, 1) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 1) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(tmpM.M, 1, 2) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 2) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 2) + CGWXFormMatrix2D_IJ(X->M, 1, 2);
        *out_M = tmpM;
    } else {
        CGWXFormMatrix2D_IJ(out_M->M, 0, 0) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 0) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 1) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 1) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 2) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 2) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 2) + CGWXFormMatrix2D_IJ(X->M, 0, 2);
        
        CGWXFormMatrix2D_IJ(out_M->M, 1, 0) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 0) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 1, 1) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 1) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 1, 2) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 2) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 2) + CGWXFormMatrix2D_IJ(X->M, 1, 2);
    }
    return out_M;
}

//

CGWXFormMatrix2D*
CGWXFormMatrix2DScaledSum(
    CGWXFormMatrix2D    *out_M,
    float               A,
    CGWXFormMatrix2D    *X,
    CGWXFormMatrix2D    *Y
)
{
    if ( A == 0.0f ) {
        if ( out_M != Y ) *out_M = *Y;
    }
    else if ( A == 1.0f ) {
        CGWXFormMatrix2D_IJ(out_M->M, 0, 0) = CGWXFormMatrix2D_IJ(X->M, 0, 0) + CGWXFormMatrix2D_IJ(Y->M, 0, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 1) = CGWXFormMatrix2D_IJ(X->M, 0, 1) + CGWXFormMatrix2D_IJ(Y->M, 0, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 2) = CGWXFormMatrix2D_IJ(X->M, 0, 2) + CGWXFormMatrix2D_IJ(Y->M, 0, 2);
        
        CGWXFormMatrix2D_IJ(out_M->M, 1, 0) = CGWXFormMatrix2D_IJ(X->M, 1, 0) + CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 1, 1) = CGWXFormMatrix2D_IJ(X->M, 1, 1) + CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 1, 2) = CGWXFormMatrix2D_IJ(X->M, 1, 2) + CGWXFormMatrix2D_IJ(Y->M, 1, 2);
    }
    else if ( A == -1.0f ) {
        CGWXFormMatrix2D_IJ(out_M->M, 0, 0) = CGWXFormMatrix2D_IJ(Y->M, 0, 0) - CGWXFormMatrix2D_IJ(X->M, 0, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 1) = CGWXFormMatrix2D_IJ(Y->M, 0, 1) - CGWXFormMatrix2D_IJ(X->M, 0, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 2) = CGWXFormMatrix2D_IJ(Y->M, 0, 2) - CGWXFormMatrix2D_IJ(X->M, 0, 2);
        
        CGWXFormMatrix2D_IJ(out_M->M, 1, 0) = CGWXFormMatrix2D_IJ(Y->M, 1, 0) - CGWXFormMatrix2D_IJ(X->M, 1, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 1, 1) = CGWXFormMatrix2D_IJ(Y->M, 1, 1) - CGWXFormMatrix2D_IJ(X->M, 1, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 1, 2) = CGWXFormMatrix2D_IJ(Y->M, 1, 2) - CGWXFormMatrix2D_IJ(X->M, 1, 2);
    }
    else {
        CGWXFormMatrix2D_IJ(out_M->M, 0, 0) = A * CGWXFormMatrix2D_IJ(X->M, 0, 0) + CGWXFormMatrix2D_IJ(Y->M, 0, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 1) = A * CGWXFormMatrix2D_IJ(X->M, 0, 1) + CGWXFormMatrix2D_IJ(Y->M, 0, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 2) = A * CGWXFormMatrix2D_IJ(X->M, 0, 2) + CGWXFormMatrix2D_IJ(Y->M, 0, 2);
        
        CGWXFormMatrix2D_IJ(out_M->M, 1, 0) = A * CGWXFormMatrix2D_IJ(X->M, 1, 0) + CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 1, 1) = A * CGWXFormMatrix2D_IJ(X->M, 1, 1) + CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 1, 2) = A * CGWXFormMatrix2D_IJ(X->M, 1, 2) + CGWXFormMatrix2D_IJ(Y->M, 1, 2);
    }
    return out_M;
}

//

CGWPoint2D*
CGWXFormMatrix2DDotPoint2D(
    CGWPoint2D          *out_p,
    CGWXFormMatrix2D    *X,
    CGWPoint2D          *p
)
{
    if ( p == out_p ) {
        CGWPoint2D      tmp_p;
        
        // Fill-in a tmp point then copy:
        tmp_p.x = CGWXFormMatrix2D_IJ(X->M, 0, 0) * p->x + CGWXFormMatrix2D_IJ(X->M, 0, 1) * p->y + CGWXFormMatrix2D_IJ(X->M, 0, 2);
        tmp_p.y = CGWXFormMatrix2D_IJ(X->M, 1, 0) * p->x + CGWXFormMatrix2D_IJ(X->M, 1, 1) * p->y + CGWXFormMatrix2D_IJ(X->M, 1, 2);
        *out_p = tmp_p;
    } else {
        // Fill-in out_p directly:
        out_p->x = CGWXFormMatrix2D_IJ(X->M, 0, 0) * p->x + CGWXFormMatrix2D_IJ(X->M, 0, 1) * p->y + CGWXFormMatrix2D_IJ(X->M, 0, 2);
        out_p->y = CGWXFormMatrix2D_IJ(X->M, 1, 0) * p->x + CGWXFormMatrix2D_IJ(X->M, 1, 1) * p->y + CGWXFormMatrix2D_IJ(X->M, 1, 2);
    }
    return out_p;
}

//

CGWXFormMatrix2D*
CGWXFormMatrix2DInvert(
    CGWXFormMatrix2D    *A_inv,
    CGWXFormMatrix2D    *A
)
{
    //
    //          [[A_00  A_01  A_02 ]
    //      A =  [A_10  A_11  A_12 ]
    //           [0     0     1    ]]
    //
    // The determinant of this matrix is:
    //
    //      det(A) = A_00 A_11 - A_01 A_10
    //
    // The adjoint of this matrix is:
    //
    //               [[ A_11  -A_01   (A_01 A_12 - A_02 A_11) ]
    //      adj(A) =  [-A_10   A_00   (A_02 A_10 - A_00 A_12) ]
    //                [ 0      0      (A_00 A_11 - A_01 A_10) ]]
    //
    // Thus, the inverse of A is:
    //
    //                                       [[ A_11  -A_01   (A_01 A_12 - A_02 A_11) ]
    //      A^-1 = (1/A_00 A_11 - A_01 A_10)  [-A_10   A_00   (A_02 A_10 - A_00 A_12) ]
    //                                        [ 0      0      (A_00 A_11 - A_01 A_10) ]]
    //
    //      a1 = (A_00 A_11 - A_01 A_10)   (2 mul + 1 add)
    //      a2 = (A_01 A_12 - A_02 A_11)   (2 mul + 1 add)
    //      a3 = (A_02 A_10 - A_00 A_12)   (2 mul + 1 add)
    //
    //              [[ A_11/a1  -A_01/a1   a2/a1 ]     (3 div + 1 neg)
    //      A^-1 =   [-A_10/a1   A_00/a1   a3/a2 ]     (3 div + 1 neg)
    //               [ 0         0         1     ]]
    //
    // Total of 19 floating-point ops (6 mul + 6 div + 3 add + 2 neg).
    //
    float           A_00, A_01, A_02,
                    A_10, A_11, A_12;
    float           a1, a2, a3, a4;
    
    A_00 = CGWXFormMatrix2D_IJ(A->M, 0, 0), A_01 = CGWXFormMatrix2D_IJ(A->M, 0, 1), A_02 = CGWXFormMatrix2D_IJ(A->M, 0, 2),
    A_10 = CGWXFormMatrix2D_IJ(A->M, 1, 0), A_11 = CGWXFormMatrix2D_IJ(A->M, 1, 1), A_12 = CGWXFormMatrix2D_IJ(A->M, 1, 2);
    
    a1 = A_00 * A_11 - \
         A_01 * A_10;
    a2 = A_01 * A_12 - \
         A_02 * A_11;
    a3 = A_02 * A_10 - \
         A_00 * A_12;
    a4 = 1.0f / a1;
    
    CGWXFormMatrix2D_IJ(A_inv->M, 0, 0) = a4 * A_11;
    CGWXFormMatrix2D_IJ(A_inv->M, 0, 1) = -a4 * A_01;
    CGWXFormMatrix2D_IJ(A_inv->M, 0, 2) = a4 * a2;
    CGWXFormMatrix2D_IJ(A_inv->M, 1, 0) = -a4 * A_10;
    CGWXFormMatrix2D_IJ(A_inv->M, 1, 1) = a4 * A_00;
    CGWXFormMatrix2D_IJ(A_inv->M, 1, 2) = a4 * a3;
    return A_inv;
}

//

CGWXFormMatrix2D*
CGWXFormMatrix2DInvertGE(
    CGWXFormMatrix2D    *A_inv,
    CGWXFormMatrix2D    *A
)
{
    float           a, b, c, d, e, f;
    
    if ( fabs(CGWXFormMatrix2D_IJ(A->M, 0, 0)) < fabs(CGWXFormMatrix2D_IJ(A->M, 0, 1)) ) {
        // A_{1,2} is greater in magnitude than A_{1,1} so we will
        // go with a R1 <-> R2 exchange.
        //
        //   [[A_10  A_11  A_12 |  0  1  0 ]
        //    [A_00  A_01  A_02 |  1  0  0 ]
        //    [0     0     1    |  0  0  1 ]]
        //
        //   a = A_00/A_10
        //
        //   [[A_10  A_11             A_12            |  0  1  0 ]
        //    [0     (A_01 - a A_11)  (A012 - a A_12) |  1 -a  0 ]
        //    [0     0                1               |  0  0  1 ]]
        //
        //  b = (A_01 - a A_11), c = (A_02 - a A_12)
        //
        //   [[A_10  A_11             A_12            |  0  1  0 ]
        //    [0     b                0               |  1 -a -c ]
        //    [0     0                1               |  0  0  1 ]]
        //
        //   [[A_10  A_11             A_12            |  0    1    0   ]
        //    [0     1                0               |  1/b -a/b -c/b ]
        //    [0     0                1               |  0    0    1   ]]
        //
        //   [[A_10  A_11             0 |  0    1   -A_12 ]
        //    [0     1                0 |  1/b -a/b -c/b   ]
        //    [0     0                1 |  0    0    1     ]]
        //
        // d = -A_11/b, e = 1 + a A_11/b, f = c A_11/b - A_12
        //
        //   [[A_10  0  0 |  d    e    f   ]
        //    [0     1  0 |  1/b -a/b -c/b ]
        //    [0     0  1 |  0    0    1   ]]
        //
        //   [[  1  0  0 |  d/A_10  e/A_10  f/A_10 ]            (3 div)
        //    [  0  1  0 |  1/b    -a/b    -c/b    ]            (3 div + 2 neg)
        //    [  0  0  1 |  0       0       1      ]]
        //
        //  a = A_00 / A_10                                     (1 div)            
        //  b = (A_01 - a A_11)                                 (1 mul + 1 add)
        //  c = (A_02 - a A_12)                                 (1 mul + 1 add)
        //  d = -A_11 / b                                       (1 div + 1 neg)
        //  e = 1 + a A_11 / b = (b + a A_11) / b = A_01 / b    (1 div)
        //  f = c A_11 / b - A_12 = (c A_11 - b A_12) / b = (A_11 A_02 - A_12 A_01) / b
        //                                                      (2 mul + 1 div + 1 add)
        //
        // That's a total of 20 floating-point operations (4 mul + 10 div + 3 add + 3 neg).
        // But there's the initial conditional for selecting for row swap (2 fabs + 1 add)
        // that puts it at 23 operations.
        //
        a = CGWXFormMatrix2D_IJ(A->M, 0, 0) / CGWXFormMatrix2D_IJ(A->M, 1, 0);
        b = 1.0 / (CGWXFormMatrix2D_IJ(A->M, 0, 1) - a * CGWXFormMatrix2D_IJ(A->M, 1, 1));
        c = CGWXFormMatrix2D_IJ(A->M, 0, 2) - a * CGWXFormMatrix2D_IJ(A->M, 1, 2);
        d = -CGWXFormMatrix2D_IJ(A->M, 1, 1) * b;
        e = CGWXFormMatrix2D_IJ(A->M, 0, 1) * b;
        f = (CGWXFormMatrix2D_IJ(A->M, 1, 1) * CGWXFormMatrix2D_IJ(A->M, 0, 2) - \
             CGWXFormMatrix2D_IJ(A->M, 1, 2) * CGWXFormMatrix2D_IJ(A->M, 0, 1)) * b;
        CGWXFormMatrix2D_IJ(A_inv->M, 0, 0) = d / CGWXFormMatrix2D_IJ(A->M, 1, 0);
        CGWXFormMatrix2D_IJ(A_inv->M, 0, 1) = e / CGWXFormMatrix2D_IJ(A->M, 1, 0);
        CGWXFormMatrix2D_IJ(A_inv->M, 0, 2) = f / CGWXFormMatrix2D_IJ(A->M, 1, 0);
        CGWXFormMatrix2D_IJ(A_inv->M, 1, 0) = b;
        CGWXFormMatrix2D_IJ(A_inv->M, 1, 1) = -a * b;
        CGWXFormMatrix2D_IJ(A_inv->M, 1, 2) = -c * b;
    } else {
        // No row swaps:
        //
        //   [[A_00  A_01  A_02 |  1  0  0 ]
        //    [A_10  A_11  A_12 |  0  1  0 ]
        //    [0     0     1    |  0  0  1 ]]
        //
        //   a = A_10/A_00
        //
        //   [[A_00  A_01             A_02            |  1  0  0 ]
        //    [0     (A_11 - a A_01)  (A_12 - a A_02) | -a  1  0 ]
        //    [0     0                1               |  0  0  1 ]]
        //
        //  b = (A_11 - a A_01), c = (A_12 - a A_02)
        //
        //   [[A_00  A_01             A_02            |  1  0  0 ]
        //    [0     b                0               | -a  1 -c ]
        //    [0     0                1               |  0  0  1 ]]
        //
        //   [[A_00  A_01             A_02            |  1    0    0   ]
        //    [0     1                0               | -a/b  1/b -c/b ]
        //    [0     0                1               |  0    0    1   ]]
        //
        //   [[A_00  A_01             0 |  1    0    -A_02 ]
        //    [0     1                0 | -a/b  1/b -c/b   ]
        //    [0     0                1 |  0    0    1     ]]
        //
        // d = 1 + (a A_01)/b, e = -A_01/b, f = c A_01/b - A_02
        //
        //   [[A_00  0  0 |  d    e    f   ]
        //    [0     1  0 | -a/b  1/b -c/b ]
        //    [0     0  1 |  0    0    1   ]]
        //
        //   [[  1  0  0 |  d/A_00  e/A_00  f/A_00 ]            (3 div)
        //    [  0  1  0 | -a/b     1/b    -c/b    ]            (3 div + 2 neg)
        //    [  0  0  1 |  0       0       1      ]]
        //
        //  a = A_10 / A_00                                     (1 div)
        //  b = (A_11 - a A_01)                                 (1 mul + 1 add)
        //  c = (A_12 - a A_02)                                 (1 mul + 1 add)
        //  d = 1 + (a A_01)/b = (b + a A_01)/b = A_11 / b      (1 div)
        //  e = -A_01 / b                                       (1 div + 1 neg)
        //  f = c A_01 / b - A_02 = (c A_01 - b A_02) / b = (A_01 A_12 - A_02 A_11) / b
        //                                                      (2 mul + 1 div + 1 add)
        //
        // That's a total of 20 floating-point operations (4 mul + 10 div + 3 add + 3 neg).
        // But there's the initial conditional for selecting for row swap (2 fabs + 1 add)
        // that puts it at 23 operations.
        //
        a = CGWXFormMatrix2D_IJ(A->M, 1, 0) / CGWXFormMatrix2D_IJ(A->M, 0, 0);
        b = 1.0 / (CGWXFormMatrix2D_IJ(A->M, 1, 1) - a * CGWXFormMatrix2D_IJ(A->M, 0, 1));
        c = CGWXFormMatrix2D_IJ(A->M, 1, 2) - a * CGWXFormMatrix2D_IJ(A->M, 0, 2);
        d = CGWXFormMatrix2D_IJ(A->M, 1, 1) * b;
        e = -CGWXFormMatrix2D_IJ(A->M, 0, 1) * b;
        f = (CGWXFormMatrix2D_IJ(A->M, 0, 1) * CGWXFormMatrix2D_IJ(A->M, 1, 2) - \
             CGWXFormMatrix2D_IJ(A->M, 0, 2) * CGWXFormMatrix2D_IJ(A->M, 1, 1)) * b;
        CGWXFormMatrix2D_IJ(A_inv->M, 0, 0) = d / CGWXFormMatrix2D_IJ(A->M, 0, 0);
        CGWXFormMatrix2D_IJ(A_inv->M, 0, 1) = e / CGWXFormMatrix2D_IJ(A->M, 0, 0);
        CGWXFormMatrix2D_IJ(A_inv->M, 0, 2) = f / CGWXFormMatrix2D_IJ(A->M, 0, 0);
        CGWXFormMatrix2D_IJ(A_inv->M, 1, 0) = -a * b;
        CGWXFormMatrix2D_IJ(A_inv->M, 1, 1) = b;
        CGWXFormMatrix2D_IJ(A_inv->M, 1, 2) = -c * b;
    }
    return A_inv;
}

#endif

//

bool
CGWRectI2DOverlapsRect(
    CGWRectI2D      r1,
    CGWRectI2D      r2
)
{
    unsigned int    x1, x2, y1, y2;
    
    /* If we project both rectangles onto the x-axis,
     * do their ranges overlap?
     */
    x1 = r1.origin.x + r1.size.w; x2 = r2.origin.x + r2.size.w;
    if ( (r1.origin.x >= r2.origin.x && r1.origin.x <= x2) ||
         (x1 >= r2.origin.x && x1 <= x2) ) return true;
    
    /* If we project both rectangles onto the y-axis,
     * do their ranges overlap?
     */
    y1 = r1.origin.y + r1.size.h; y2 = r2.origin.y + r2.size.h;
    if ( (r1.origin.y >= r2.origin.y && r1.origin.y <= y2) ||
         (y1 >= r2.origin.y && y1 <= y2) ) return true;
    
    return false;
}

//

bool
CGWRectI2DOverlapsCircle(
    CGWRectI2D  r,
    CGWCircleI  c
)
{
    int64_t     dist, rem;
    int64_t     xn, yn;
    
    // Project the circle origin onto the boundary of the
    // rectangle:
    xn = r.origin.x + r.size.w;     // rect x_max
    xn = ( c.origin.x < xn ) ? c.origin.x : xn;
    xn = ( r.origin.x < xn ) ? xn : r.origin.x;
    yn = r.origin.y + r.size.h;     // rect y_max
    yn = ( c.origin.y < yn ) ? c.origin.y : yn;
    yn = ( r.origin.y < yn ) ? yn : r.origin.y;
    
    // Distance from the circle center to the projected
    // point on rectangle:
    xn = c.origin.x - xn, yn = c.origin.y - yn;
    dist = CGWSqrtInt64(xn * xn + yn * yn, &rem);
    
    return ((dist < c.radius) || ((dist == c.radius) && (rem == 0)));
}

//

bool
CGWRect2DOverlapsRect(
    CGWRect2D       r1,
    CGWRect2D       r2
)
{
    float           x1, x2, y1, y2;
    
    /* If we project both rectangles onto the x-axis,
     * do their ranges overlap?
     */
    x1 = r1.origin.x + r1.size.w; x2 = r2.origin.x + r2.size.w;
    if ( (r1.origin.x >= r2.origin.x && r1.origin.x <= x2) ||
         (x1 >= r2.origin.x && x1 <= x2) ) return true;
    
    /* If we project both rectangles onto the y-axis,
     * do their ranges overlap?
     */
    y1 = r1.origin.y + r1.size.h; y2 = r2.origin.y + r2.size.h;
    if ( (r1.origin.y >= r2.origin.y && r1.origin.y <= y2) ||
         (y1 >= r2.origin.y && y1 <= y2) ) return true;
    
    return false;
}

//

bool
CGWRect2DOverlapsCircle(
    CGWRect2D   r,
    CGWCircle   c
)
{
    float       dist;
    float       xn, yn;
    
    // Project the circle origin onto the boundary of the
    // rectangle:
    xn = r.origin.x + r.size.w;     // rect x_max
    xn = ( c.origin.x < xn ) ? c.origin.x : xn;
    xn = ( r.origin.x < xn ) ? xn : r.origin.x;
    yn = r.origin.y + r.size.h;     // rect y_max
    yn = ( c.origin.y < yn ) ? c.origin.y : yn;
    yn = ( r.origin.y < yn ) ? yn : r.origin.y;
    
    // Distance from the circle center to the projected
    // point on rectangle:
    xn = c.origin.x - xn, yn = c.origin.y - yn;
    dist = sqrt(xn * xn + yn * yn);
    
    return (dist <= c.radius);
}

