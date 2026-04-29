/**
 *
 * YPL — Yertle Programming Language
 *
 * Commands are added to a text file -- by convention one per line -- and have
 * a form that has a mandatory word followed by a fixed number of arguments.
 *
 * reset                                the turtle is moved back to the origin, the
 *                                      color is reset, and the stylus is enabled
 *
 * pause                                wait for a single keypress before proceeding;
 *                                      this is often used at the end of a program
 *                                      because otherwise the simulator exists
 *                                      immediately
 *
 * clear                                clear the screen
 *
 * color++                              select the next color in the palette
 * color--                              select the previous color in the palette
 * color <idx>                          select the color at the given index <idx>;
 *                                      when <idx> exceeds the number of colors it
 *                                      wraps around
 *
 * drawing-delay <dt>                   wait the given number of seconds <dt> between
 *                                      drawing each pixel; defaults to 0.05
 *
 * stylus <1|0>                         enable (1) or disable (0) the stylus
 *
 * moveto <x> <y>                       alter the turtle's position to be (<x>, <y>)
 *                                      where the coordinate system runs from [0,1] in
 *                                      both axes
 * move <d>                             move the turtle the distance <d>; if the stylus
 *                                      is enabled pixels will be drawn
 *
 * turnto-<deg|rad> <a>                 rotate the turtle to angle <a>, where the choice
 *                                      or <deg|rad> in the command dictates which unit
 *                                      is associated with <a>
 * turn-<deg|rad> <a>                   rotate the turtle by <a> relative to it's current
 *                                      orientation, where the choice or <deg|rad> in the
 *                                      command dictates which unit is associated with <a>
 *
 * curve-<dev|rad> <r> <phi>            given the turtle's current position and direction,
 *                                      rotate the direction by 90° and move <r> away from
 *                                      the position to establish the center of a circle;
 *                                      then move the turtle through an arc of <a> on that
 *                                      circle
 * contracting-curve-<dev|rad> \        given the turtle's current position and direction,
 *     <r> <phi> <contrct>              rotate the direction by 90° and move <r> away from
 *                                      the position to establish the center of a circle;
 *                                      then move the turtle through an arc of <a> on that
 *                                      circle, contracting the circle's radius by a
 *                                      constant <contrct> for every radian of rotation
 * prop-contracting-curve-<dev|rad> \   given the turtle's current position and direction,
 *     <r> <phi> <contrct>              rotate the direction by 90° and move <r> away from
 *                                      the position to establish the center of a circle;
 *                                      then move the turtle through an arc of <a> on that
 *                                      circle, contracting the circle's radius by a
 *                                      constant <contrct> for every radian of rotation
 *
 */

#include "CGWTurtle2D.h"
#include "CGWCurses.h"

int
turtleDraw(
    CGWTurtleEventPtr   the_event,
    const void          *context
)
{
    CGWTurtleEventDrawPixel *EVENT = (CGWTurtleEventDrawPixel*)the_event;
    int                     *color = (int*)context;
    mvaddch(EVENT->position.y, EVENT->position.x, ' ' | COLOR_PAIR(*color));
    refresh();
    return kCGWTurtleEventSetReturnCodeOk;
}

//

int
main(
    int         argc,
    const char* argv[]
)
{
    CGWSizeI2D          display_size;
    CGWPoint2D          pt;
    CGWTurtleRef        Yertle = NULL;
    short               colors_orig[30][3];
    short               color_max = 1, n_color = 0, pair_max = 3, n_pair = 0;
    int                 stylus_color = 3;
    int                 argi = 1;
    int                 rc = 0;
    char                err_msg[1024];
    
    
#define SAVE_COLOR(I)  color_content(I, &colors_orig[I][0], &colors_orig[I][1], &colors_orig[I][2])
    
    
    if ( argc < 2 ) exit(0);
    
    CGWCursesInit(CGWSizeI2DMake(0, 0), &display_size);
    
    // Correction, we do want sychronous input available to us:
    nodelay(stdscr, FALSE);
    
    // Colors 0 and 1 are black and white, respectively:
    SAVE_COLOR(color_max), init_color(color_max++, 0, 0, 0);
    SAVE_COLOR(color_max), init_color(color_max++, 1000, 1000, 1000);
    
    // Additional colors starting at 3:
    //
    //   3 = med gray
    //   4 = bright red
    //   5 = bright green
    //   6 = bright blue
    //   7 = bright yellow
    //   8 = bright purple
    //   9 = bright teal
    //  10 = med red
    //  11 = med green
    //  12 = med blue
    //  13 = med yellow
    //  14 = med purple
    //  15 = med teal
    SAVE_COLOR(color_max), init_color(color_max++, 500, 500, 500), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 1000, 0, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 1000, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 0, 1000), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 1000, 1000, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 1000, 0, 1000), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 1000, 1000), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 500, 0, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 500, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 0, 500), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 500, 500, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 500, 0, 500), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 500, 500), n_color++;
    
    // Pair 1:  black on black — our background color
    init_pair(1, 1, 1);
    // Pair 2:  white on black — for text
    init_pair(2, 2, 1);
    
    // Additional pairs with the colors defined above
    // as the color in both fore- and background
    // for a solid block:
    //
    //   3 = med gray
    //   4 = bright red
    //   5 = bright green
    //   6 = bright blue
    //   7 = white
    //   8 = bright yellow
    //   9 = bright purple
    //  10 = bright teal
    //  11 = med red
    //  12 = med green
    //  13 = med blue
    //  14 = med yellow
    //  15 = med purple
    //  16 = med teal
    init_pair(pair_max++, 3, 3), n_pair++;
    init_pair(pair_max++, 4, 4), n_pair++;
    init_pair(pair_max++, 5, 5), n_pair++;
    init_pair(pair_max++, 6, 6), n_pair++;
    init_pair(pair_max++, 2, 2), n_pair++;
    init_pair(pair_max++, 7, 7), n_pair++;
    init_pair(pair_max++, 8, 8), n_pair++;
    init_pair(pair_max++, 9, 9), n_pair++;
    init_pair(pair_max++, 10, 10), n_pair++;
    init_pair(pair_max++, 11, 11), n_pair++;
    init_pair(pair_max++, 12, 12), n_pair++;
    init_pair(pair_max++, 13, 13), n_pair++;
    init_pair(pair_max++, 14, 14), n_pair++;
    init_pair(pair_max++, 15, 15), n_pair++;
    
    //
    // Create our turtle display to fill the curses screen.
    // Periodic boundary conditions are applied to all edges of the
    // screen.
    //
    Yertle = CGWTurtleCreate(NULL,
                             CGWRectI2DMake(0, 0, display_size.w - 1, display_size.h - 1),
                             kCGWTurtleOptionsBoundsPeriodic);

    // Turn the stylus on to start:
    CGWTurtleStateSetStylusIsOn(Yertle, true);
    
    // To effect drawing to the curses screen we need to observe pixel events generated
    // by the turtle; as context we provide a pointer to the selected color pair index:
    CGWTurtleSetEventObserver(Yertle, kCGWTurtleEventDrawPixel, turtleDraw, &stylus_color);
    
    while ( (rc == 0) && (argi < argc) ) {
        FILE        *program_fptr = fopen(argv[argi++], "r");
        
        if ( program_fptr ) {
            while ( (rc == 0) && ! feof(program_fptr) ) {
                char        command[32];
                
                if ( fscanf(program_fptr, "%30s", command) == 1 ) {
                    if ( (strcmp(command, "#") == 0) || (strcmp(command, "//") == 0) ) {
                        // Comment line, read until a newline is encountered:
                        while ( ! feof(program_fptr) ) {
                            char        c = fgetc(program_fptr);
                            if ( (c == '\n') || (c == '\r') || (c == '\0') ) break;
                        }
                    }
                    else if ( strcasecmp(command, "reset") == 0 ) {
                        CGWTurtleStateReset(Yertle);
                        CGWTurtleStateSetStylusIsOn(Yertle, true);
                        stylus_color = 3;
                    }
                    else if ( strcasecmp(command, "pause") == 0 ) {
                        chtype      saved_chars[30], *char_ptr = saved_chars;
                        int         x = 1;
                        
                        while ( char_ptr < (saved_chars + sizeof(saved_chars)) ) *char_ptr++ = mvinch(1, x++);
                        attron(COLOR_PAIR(2));
                        mvprintw(1, 1, "Press any key to continue...");
                        attroff(COLOR_PAIR(2));
                        getch();
                        
                        // Restore the screen:
                        x = 1, char_ptr = saved_chars;
                        while ( char_ptr < (saved_chars + sizeof(saved_chars)) ) mvaddch(1, x++, *char_ptr++);
                    }
                    else if ( strcasecmp(command, "clear") == 0 ) {
                        clear();
                    }
                    else if ( strcasecmp(command, "color++") == 0 ) {
                        stylus_color = 3 + ((stylus_color - 3 + 1) % n_pair);
                    }
                    else if ( strcasecmp(command, "color--") == 0 ) {
                        stylus_color = 3 + ((stylus_color - 3 - 1) % n_pair);
                    }
                    else if ( strcasecmp(command, "color") == 0 ) {
                        int         c;
                        
                        if ( fscanf(program_fptr, "%d", &c) == 1 ) {
                            if ( c >= 0 ) stylus_color = 3 + (c % n_pair);
                        }
                    }
                    else if ( strcasecmp(command, "drawing-delay") == 0 ) {
                        double      dt;
                        
                        if ( fscanf(program_fptr, "%lg", &dt) == 1 ) {
                            if ( dt < 0.0 ) dt = 0.0;
                            CGWTurtleStateSetDrawingDelay(Yertle, dt);
                        }
                    }
                    else if ( strcasecmp(command, "stylus") == 0 ) {
                        int         b;
                        
                        if ( fscanf(program_fptr, "%d", &b) == 1 ) {
                            CGWTurtleStateSetStylusIsOn(Yertle, b ? true : false);
                        }
                    }
                    else if ( strcasecmp(command, "moveto") == 0 ) {
                        float       x, y;
                        
                        if ( fscanf(program_fptr, "%g %g", &x, &y) == 2 ) {
                            CGWTurtleStateSetPosition(Yertle, CGWPoint2DMake(x, y));
                        }
                    }
                    else if ( strcasecmp(command, "move") == 0 ) {
                        float       d;
                        
                        if ( fscanf(program_fptr, "%g", &d) == 1 ) {
                            CGWTurtleActionMove(Yertle, d);
                        }
                    }
                    else if ( strncasecmp(command, "turnto-" ,7) == 0 ) {
                        float       a;
                        
                        if ( fscanf(program_fptr, "%g", &a) == 1 ) {
                            if ( strcasecmp(command + 7, "deg") == 0 ) {
                                a = CGWDegreesToRadians(a);
                            }
                            else if ( strcasecmp(command + 7, "rad") ) {
                                break;
                            }
                            CGWTurtleStateSetAngle(Yertle, a);
                        }
                    }
                    else if ( strncasecmp(command, "turn-" ,5) == 0 ) {
                        float       a;
                        
                        if ( fscanf(program_fptr, "%g", &a) == 1 ) {
                            if ( strcasecmp(command + 5, "deg") == 0 ) {
                                a = CGWDegreesToRadians(a);
                            }
                            else if ( strcasecmp(command + 5, "rad") ) {
                                break;
                            }
                            CGWTurtleStateTurn(Yertle, a);
                        }
                    }
                    else if ( strncasecmp(command, "curve-", 6) == 0 ) { 
                        float       r, phi;
                        
                        if ( fscanf(program_fptr, "%g %g", &r, &phi) == 2 ) {
                            CGWPoint2D          o, d_v;
                            CGWXFormMatrix2D    d_rot;
                            
                            if ( strcasecmp(command + 6, "deg") == 0 ) {
                                phi = CGWDegreesToRadians(phi);
                            }
                            else if ( strcasecmp(command + 6, "rad") ) {
                                break;
                            }
                            // Create a rotation matrix for the direction vector:
                            CGWXFormMatrix2DSetIdentity(&d_rot);
                            CGWXFormMatrix2DSetRotation(&d_rot, (phi < 0.0f) ? -M_PI_2 : M_PI_2);
                            
                            // Current position and orientation:
                            o = CGWTurtleStateGetPosition(Yertle);
                            d_v = CGWTurtleStateGetOrientationVector(Yertle);
                            
                            // Scale the orientation to the radius:
                            d_v.x *= r, d_v.y *= r;
                            
                            // Apply the rotation:
                            CGWXFormMatrix2DDotPoint2D(&d_v, &d_rot, &d_v);
                            
                            // Translate the turtle's current position:
                            o = CGWPoint2DSum(o, d_v);
                            
                            CGWTurtleActionCurve(Yertle, o, phi);
                        }
                    }
                    else if ( strncasecmp(command, "contracting-curve-", 18) == 0 ) { 
                        float       r, phi, per_radian;
                        
                        if ( fscanf(program_fptr, "%g %g %g", &r, &phi, &per_radian) == 3 ) {
                            CGWPoint2D          o, d_v;
                            CGWXFormMatrix2D    d_rot;
                            
                            if ( strcasecmp(command + 18, "deg") == 0 ) {
                                phi = CGWDegreesToRadians(phi);
                            }
                            else if ( strcasecmp(command + 18, "rad") ) {
                                break;
                            }
                            // Create a rotation matrix for the direction vector:
                            CGWXFormMatrix2DSetIdentity(&d_rot);
                            CGWXFormMatrix2DSetRotation(&d_rot, (phi < 0.0f) ? -M_PI_2 : M_PI_2);
                            
                            // Current position and orientation:
                            o = CGWTurtleStateGetPosition(Yertle);
                            d_v = CGWTurtleStateGetOrientationVector(Yertle);
                            
                            // Scale the orientation to the radius:
                            d_v.x *= r, d_v.y *= r;
                            
                            // Apply the rotation:
                            CGWXFormMatrix2DDotPoint2D(&d_v, &d_rot, &d_v);
                            
                            // Translate the turtle's current position:
                            o = CGWPoint2DSum(o, d_v);
                            
                            CGWTurtleActionCurveAndContract(Yertle, o, phi, per_radian, false);
                        }
                    }
                    else if ( strncasecmp(command, "prop-contracting-curve-", 23) == 0 ) { 
                        float       r, phi, per_radian;
                        
                        if ( fscanf(program_fptr, "%g %g %g", &r, &phi, &per_radian) == 3 ) {
                            CGWPoint2D          o, d_v;
                            CGWXFormMatrix2D    d_rot;
                            
                            if ( strcasecmp(command + 23, "deg") == 0 ) {
                                phi = CGWDegreesToRadians(phi);
                            }
                            else if ( strcasecmp(command + 23, "rad") ) {
                                break;
                            }
                            // Create a rotation matrix for the direction vector:
                            CGWXFormMatrix2DSetIdentity(&d_rot);
                            CGWXFormMatrix2DSetRotation(&d_rot, (phi < 0.0f) ? -M_PI_2 : M_PI_2);
                            
                            // Current position and scaled orientation vector:
                            o = CGWTurtleStateGetPosition(Yertle);
                            d_v = CGWPoint2DScale(CGWTurtleStateGetOrientationVector(Yertle), r);
                            
                            // Apply the rotation:
                            CGWXFormMatrix2DDotPoint2D(&d_v, &d_rot, &d_v);
                            
                            // Translate the turtle's current position:
                            o = CGWPoint2DSum(o, d_v);
                            
                            CGWTurtleActionCurveAndContract(Yertle, o, phi, per_radian, true);
                        }
                    }
                    else {
                        snprintf(err_msg, sizeof(err_msg), "ERROR:  unknown command: %s\n", command);
                        rc = 1;
                    }
                }
            }
            fclose(program_fptr);
        } else {
            snprintf(err_msg, sizeof(err_msg), "ERROR:  unable to open program file\n");
            rc = 1;
        }
    }
terminate:
    // Restore original color palette:
    clear();
    while ( color_max-- ) {
        init_color(color_max, colors_orig[color_max][0], colors_orig[color_max][1], colors_orig[color_max][2]);
    }
    CGWCursesShutdown();
    CGWTurtleSummarize(Yertle);
    CGWTurtleDestroy(Yertle);
    if ( rc ) {
        fprintf(stderr, "%s", err_msg);
    }
    return rc;
}

