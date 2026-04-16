/**
 * @file CGWGeom.c
 * @brief Curses Gaming Window, Geometrical support
 * 
 * Source code accompanying CGWGeom.h.
 *
 */
 
#include "CGWGeom.h"

const CGWPoint CGWPointOrigin = { .x = 0, .y = 0 };
const CGWPointReal CGWPointRealOrigin = { .x = 0.0f, .y = 0.0f };
const CGWPointP CGWPointPOrigin = { .r = 0, .theta = 0.0f };

//

float CGWPointPThetaMinX = 1e-8;
float CGWPointPThetaMinY = 1e-8;

void
CGWPointPSetThetaMin(
    CGWSize     cartesian_dims
)
{
    CGWPointPGetThetaMin(cartesian_dims, &CGWPointPThetaMinX, &CGWPointPThetaMinY);
}

//

void
CGWPointPGetThetaMin(
    CGWSize     cartesian_dims,
    float       *theta_min_x,
    float       *theta_min_y
)
{
    double      inv_half_w = 2.0 / (double)cartesian_dims.w;
    double      inv_half_h = 2.0 / (double)cartesian_dims.h;
    
    // Since dimensions are always positive, the acos/asin
    // will both be positive -- as desired:
    *theta_min_x = asinf(inv_half_h);
    *theta_min_y = asinf(inv_half_w);
}

//

#if defined(__APPLE__) && defined(__aarch64__)

#else

double
CGWPointPThetaClampWithThresholds(
    double          theta,
    double          theta_min_x,
    double          theta_min_y
)
{
    int             n_2xpi = floor(fabs(theta) / M_2xPI) + (theta < 0 ? 1 : 0);
    double          theta_prime = theta, abs_theta;
    
    if ( n_2xpi ) {
        theta_prime = theta + ((theta < 0) ? n_2xpi * M_2xPI : -n_2xpi * M_2xPI);
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

CGWPointP
CGWPointPMakeWithThresholds(
    unsigned int    r,
    float           theta,
    float           theta_min_x,
    float           theta_min_y
)
{
    CGWPointP   the_point = { .r = r };
    the_point.theta = CGWPointPThetaClampWithThresholds(theta, theta_min_x, theta_min_y);
    return the_point;
}

//

CGWPoint
CGWPointPToCartesian(
    CGWPointP               p,
    CGWCoordConvRoundMode   rm
)
{
    double      x = p.r * cos(p.theta),
                y = p.r * sin(p.theta);
    int         xi, yi;
    
    switch ( rm ) {
        case kCGWCoordConvRoundModeTrunc:
            xi = (int)x, yi = (int)y;
            break;
        case kCGWCoordConvRoundModeRound:
            xi = (int)round(x), yi = (int)round(y);
            break;
        case kCGWCoordConvRoundModeCeil:
            xi = (int)ceil(x), yi = (int)ceil(y);
            break;
        case kCGWCoordConvRoundModeFloor:
            xi = (int)floor(x), yi = (int)floor(y);
            break;
    }
    return CGWPointMake(xi, yi);
}

//

CGWPointP
CGWPointToPolar(
    CGWPoint                c,
    CGWCoordConvRoundMode   rm
)
{
    CGWPointP   p;
    
    if ( c.x == 0 ) {
        //  y-axis aligned:
        if ( c.y < 0 ) p.r = -c.y, p.theta = -M_PI_2;
        else if ( c.y > 0) p.r = c.y, p.theta = M_PI_2;
        else p.r = 0, p.theta = 0.0f;
    }
    else if ( c.y == 0 ) {
        //  x-axis aligned:
        if ( c.x < 0 ) p.r = -c.x, p.theta = -M_PI;
        else if ( c.x > 0) p.r = c.x, p.theta = M_PI;
        else p.r = 0, p.theta = 0.0f;
    }
    else {
        double          d = sqrt(c.x * c.x + c.y * c.y);
        unsigned int    di;
        
        switch ( rm ) {
            case kCGWCoordConvRoundModeTrunc:
                di = (int)d;
                break;
            case kCGWCoordConvRoundModeRound:
                di = (int)round(d);
                break;
            case kCGWCoordConvRoundModeCeil:
                di = (int)ceil(d);
                break;
            case kCGWCoordConvRoundModeFloor:
                di = (int)floor(d);
                break;
        }
        p.r = di, p.theta = atan2(c.y, c.x);
    }
    return p;
}

//

CGWPointP
CGWPointRealToPolar(
    CGWPointReal    c
)
{
    CGWPointP   p;
    
    if ( c.x == 0 ) {
        //  y-axis aligned:
        if ( c.y < 0 ) p.r = -c.y, p.theta = -M_PI_2;
        else if ( c.y > 0) p.r = c.y, p.theta = M_PI_2;
        else p.r = 0, p.theta = 0.0f;
    }
    else if ( c.y == 0 ) {
        //  x-axis aligned:
        if ( c.x < 0 ) p.r = -c.x, p.theta = -M_PI;
        else if ( c.x > 0) p.r = c.x, p.theta = M_PI;
        else p.r = 0, p.theta = 0.0f;
    }
    else {
        p.r = sqrt(c.x * c.x + c.y * c.y), p.theta = atan2(c.y, c.x);
    }
    return p;
}

//

void
CGWPointFPStandardize(
    CGWPointFP      *pfp
)
{
    int             w_neg, f_neg;
    
    if ( (pfp->whole.x != 0) && (pfp->fx != CGWFixedPrecZero) ) {
        w_neg = (pfp->whole.x < 0), f_neg = CGWFixedPrecIsNeg(pfp->fx);    
        if ( w_neg != f_neg) {
            if ( w_neg ) {
                /* whole is negative, frac is positive:  add 1 to whole, subtract 1 from frac */
                pfp->whole.x++, pfp->fx = CGWFixedPrecAdd(pfp->fx, CGWFixedPrecNegOne);
            }
            else {
                /* frac is negative, whole is positive:  subtract 1 from whole, add 1 to frac */
                pfp->whole.x--, pfp->fx = CGWFixedPrecAdd(pfp->fx, CGWFixedPrecOne);
            }
        }
    }
    if ( (pfp->whole.y != 0) && (pfp->fy != CGWFixedPrecZero) ) {
        w_neg = (pfp->whole.y < 0), f_neg = CGWFixedPrecIsNeg(pfp->fy);    
        if ( w_neg != f_neg) {
            if ( w_neg ) {
                /* whole is negative, frac is positive:  add 1 to whole, subtract 1 from frac */
                pfp->whole.y++, pfp->fy = CGWFixedPrecAdd(pfp->fy, CGWFixedPrecNegOne);
            }
            else {
                /* frac is negative, whole is positive:  subtract 1 from whole, add 1 to frac */
                pfp->whole.y--, pfp->fy = CGWFixedPrecAdd(pfp->fy, CGWFixedPrecOne);
            }
        }
    }
}

//

bool
CGWRectOverlapsRect(
    CGWRect     r1,
    CGWRect     r2
)
{
    int         x1, x2, y1, y2;
    
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
CGWRectOverlapsCircle(
    CGWRect     r,
    CGWCircle   c
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
