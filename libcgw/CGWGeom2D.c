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

#ifndef HAVE_APPLE_AARCH64

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
        
        CGWXFormMatrix2D_IJ(tmpM.M, 0, 0) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 0) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(tmpM.M, 0, 1) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 1) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(tmpM.M, 0, 2) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 2) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 2) + CGWXFormMatrix2D_IJ(X->M, 1, 2);
        *out_M = tmpM;
    } else {
        CGWXFormMatrix2D_IJ(out_M->M, 0, 0) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 0) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 1) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 1) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 2) = CGWXFormMatrix2D_IJ(X->M, 0, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 2) + CGWXFormMatrix2D_IJ(X->M, 0, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 2) + CGWXFormMatrix2D_IJ(X->M, 0, 2);
        
        CGWXFormMatrix2D_IJ(out_M->M, 0, 0) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 0) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 0);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 1) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 1) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 1);
        CGWXFormMatrix2D_IJ(out_M->M, 0, 2) = CGWXFormMatrix2D_IJ(X->M, 1, 0) * CGWXFormMatrix2D_IJ(Y->M, 0, 2) + CGWXFormMatrix2D_IJ(X->M, 1, 1) * CGWXFormMatrix2D_IJ(Y->M, 1, 2) + CGWXFormMatrix2D_IJ(X->M, 1, 2);
    }
    return out_M;
}

//

#ifndef HAVE_APPLE_AARCH64_NEON

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

#endif