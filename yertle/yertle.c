/**
 *
 * YPL — Yertle Programming Language
 *
 * Commands are added to a text file -- by convention one per line -- and have
 * a form that has a mandatory word followed by a fixed number of arguments.
 */

#include "CGWTurtle2D.h"
#include "CGWCurses.h"
#include <getopt.h>
#include <errno.h>

//

const struct option yertleCliArgs[] = {
        { "help",               no_argument,        NULL,   'h' },
        { "x-bounds",           required_argument,  NULL,   'x' },
        { "y-bounds",           required_argument,  NULL,   'y' },
        { "auto-clear",         no_argument,        NULL,   'c' },
        { "auto-pause",         no_argument,        NULL,   'p' },
        { "no-default-colors",  no_argument,        NULL,   'n' },
        { NULL,                 0,                  NULL,    0  }
    };
const char *yertleCliArgsStr = "hx:y:cpn";

void
usage(
    const char  *exe
)
{
    printf(
        "usage:\n\n"
        "    %s  {options} <turtle-script> {{options} <turtle-script> ...}\n\n"
        "  options:\n\n"
        "    --help/-h                      show this help\n"
        "    --x-bounds/-x <bounds-type>    set the type of boundary for the left\n"
        "                                   and right edges (default: hardwall)\n"
        "    --y-bounds/-y <bounds-type>    set the type of boundary for the top\n"
        "                                   and bottom edges (default: hardwall)\n"
        "    --auto-clear/-c                force a screen clear between scripts\n"
        "    --auto-pause/-p                force a pause command between scripts\n"
        "    --no-default-colors/-n         do not register the default color\n"
        "                                   palette\n"
        "\n"
        "    <bounds-type> = hardwall|elastic|periodic\n\n"
        "        hardwall:      the turtle will stop (head-on collision) or slide\n"
        "                       along the wall in the direction it was headed\n"
        "        elastic:       the turtle will be deflected away from the wall at\n"
        "                       the appropriate angle of reflection and continue\n"
        "                       travelling the remaining distance\n"
        "        periodic:      the boundary acts like a portal, with the turtle\n"
        "                       transported to the opposite boundary and all relative\n"
        "                       motion proceeding unchanged\n\n"
        "    default color palette:\n\n"
        "        0=white         1=lt gray      2=med gray      3=dark gray\n"
        "        5=red           6=green        7=blue          8=yellow\n"
        "        9=purple       10=teal        11=med red      12=med green\n"
        "       13=med blue     14=med yellow  15=med purple   16=med teal\n"
        "\n",
        exe);
}

//

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

typedef struct {
    int             orig_colors_capacity, orig_colors_used, colors_max;
    short           *orig_colors;
    
    int             n_pairs, turtle_pairs_base;
} color_data_t;

int color_palette_add_pair(color_data_t *color_data, int fg, int bg);
int color_palette_add_color(color_data_t *color_data, short r, short g, short b);

static inline int color_palette_turtle_to_pair(color_data_t *color_data, int *turtle_color)
{
    int         c = *turtle_color % (color_data->n_pairs - color_data->turtle_pairs_base);
    return color_data->turtle_pairs_base + 1 + (*turtle_color = c);
}

color_data_t*
color_data_create(
    int             colors_max
)
{
    color_data_t    *new_color_data = (color_data_t*)malloc(sizeof(color_data_t));
    
    if ( new_color_data ) {
        new_color_data->orig_colors_capacity = new_color_data->orig_colors_used = 0;
        new_color_data->colors_max = (colors_max < 0) ? COLORS : ((colors_max < 3) ? 3 : colors_max);
        new_color_data->orig_colors = NULL;
        
        new_color_data->n_pairs = 0;
        new_color_data->turtle_pairs_base = 2;
        
        color_palette_add_color(new_color_data,    0,    0,    0);  /* black */
        color_palette_add_color(new_color_data, 1000, 1000, 1000);  /* white */
        
        color_palette_add_pair(new_color_data, 1, 1);               /* black on black bg */
        color_palette_add_pair(new_color_data, 2, 1);               /* white on black bg */
        color_palette_add_pair(new_color_data, 2, 2);               /* white on white bg */
    }
    return new_color_data;
}

//

void
color_data_destroy(
    color_data_t    *color_data
)
{
    while ( color_data->orig_colors_used ) {
        int     cidx = color_data->orig_colors_used--;
        int     idx = 3 * color_data->orig_colors_used;
        
        init_color(cidx,
            *(color_data->orig_colors + idx),
            *(color_data->orig_colors + idx + 1),
            *(color_data->orig_colors + idx + 2));
    }
    free((void*)color_data);
}

//

int
color_palette_add_defaults(
    color_data_t    *color_data
)
{
    int             cidx, n_pairs = 0;
    
    // Color definitions (in addition to the ones in color_data_create()):
    //
    //   3 = light gray
    //   4 = med gray
    //   5 = dark gray
    //   6 = bright red
    //   7 = bright green
    //   8 = bright blue
    //   9 = bright yellow
    //  10 = bright purple
    //  11 = bright teal
    //  12 = med red
    //  13 = med green
    //  14 = med blue
    //  15 = med yellow
    //  16 = med purple
    //  17 = med teal
    //
    // used to make the following pairs:
    //
    //   4 = light gray
    //   5 = med gray
    //   6 = dark gray
    //   7 = bright red
    //   8 = bright green
    //   9 = bright blue
    //  10 = bright yellow
    //  11 = bright purple
    //  12 = bright teal
    //  13 = med red
    //  14 = med green
    //  15 = med blue
    //  16 = med yellow
    //  17 = med purple
    //  18 = med teal
    //
    cidx = color_palette_add_color(color_data, 750, 750, 750), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 500, 500, 500), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 250, 250, 250), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 1000, 0, 0), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 0, 1000, 0), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 0, 0, 1000), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 1000, 1000, 0), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 1000, 0, 1000), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 0, 1000, 1000), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 500, 0, 0), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 0, 500, 0), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 0, 0, 500), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 500, 500, 0), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 500, 0, 500), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    cidx = color_palette_add_color(color_data, 0, 500, 500), color_palette_add_pair(color_data, cidx, cidx), n_pairs++;
    return n_pairs;
}

//

int
color_palette_add_pair(
    color_data_t    *color_data,
    int             fg,
    int             bg
)
{
    init_pair(++color_data->n_pairs, fg, bg);
    return color_data->n_pairs;
}

//

int
color_palette_add_color(
    color_data_t    *color_data,
    short           r,
    short           g,
    short           b
)
{
    int             idx;
    
    if ( color_data->orig_colors_used + 1 >= color_data->orig_colors_capacity  ) {
        int         capacity;
        short       *new_orig;
        
        if ( color_data->orig_colors_capacity >= color_data->colors_max ) {
            fprintf(stderr, "ERROR:  maximum of %d colors already registered\n", color_data->colors_max);
            return -1;
        }
        else if ( color_data->orig_colors_capacity == 0 ) {
            capacity = 20;
        } else {
            capacity = color_data->orig_colors_capacity + 8;
        }
        if ( capacity > color_data->colors_max ) capacity = color_data->colors_max;
        new_orig = realloc(color_data->orig_colors, capacity * 3 * sizeof(short));
        if ( ! new_orig ) {
            fprintf(stderr, "ERROR:  unable to allocate original color storage\n");
            return -1;
        }
        color_data->orig_colors_capacity = capacity;
        color_data->orig_colors = new_orig;
    }
    idx = 3 * color_data->orig_colors_used++;
    color_content(color_data->orig_colors_used,
            color_data->orig_colors + idx,
            color_data->orig_colors + idx + 1,
            color_data->orig_colors + idx + 2);
    init_color(color_data->orig_colors_used, r, g, b);
    return color_data->orig_colors_used;
}

//

void
tpause(void)
{
    chtype      saved_chars[30], *char_ptr = &saved_chars[0];
    int         x = 1;
    
    while ( x < 30 ) *char_ptr++ = mvinch(1, x++);
    attron(COLOR_PAIR(2));
    mvprintw(1, 1, "Press any key to continue...");
    attroff(COLOR_PAIR(2));
    getch();
    
    // Restore the screen:
    while ( x > 1 ) mvaddch(1, --x, *(--char_ptr));
}

//

int
main(
    int             argc,
    char* const*    argv
)
{
    CGWSizeI2D          display_size;
    CGWTurtleRef        Yertle = NULL;
    color_data_t        *color_data = NULL;
    int                 stylus_color, stylus_pair;
    int                 opt;
    int                 rc = 0;
    char                err_msg[1024];
    bool                should_exit = false, should_init_colors = true,
                        force_pause = false, force_clear = false;
    CGWBitvector        turtle_opts = 0;
    
    while ( ((opt = getopt_long(argc, argv, yertleCliArgsStr, yertleCliArgs, NULL)) != -1) ) {
        switch ( opt ) {
            case 'h':
                usage(argv[0]);
                exit(0);
            case 'x': {
                CGWBitvector        set_val;
                
                if ( strcasecmp(optarg, "hardwall") == 0 )
                    turtle_opts = (turtle_opts & ~kCGWTurtleOptionsXBoundsMask) | kCGWTurtleOptionsXBoundsHardWall;
                else if ( strcasecmp(optarg, "elastic") == 0 )
                    turtle_opts = (turtle_opts & ~kCGWTurtleOptionsXBoundsMask) | kCGWTurtleOptionsXBoundsElastic;
                else if ( strcasecmp(optarg, "periodic") == 0 )
                    turtle_opts = (turtle_opts & ~kCGWTurtleOptionsXBoundsMask) | kCGWTurtleOptionsXBoundsPeriodic;
                else {
                    fprintf(stderr, "ERROR:  invalid x-bounds value: %s\n", optarg);
                    exit(EINVAL);
                }
                break;
            }
            case 'y': {
                CGWBitvector        set_val;
                
                if ( strcasecmp(optarg, "hardwall") == 0 )
                    turtle_opts = (turtle_opts & ~kCGWTurtleOptionsYBoundsMask) | kCGWTurtleOptionsYBoundsHardWall;
                else if ( strcasecmp(optarg, "elastic") == 0 )
                    turtle_opts = (turtle_opts & ~kCGWTurtleOptionsYBoundsMask) | kCGWTurtleOptionsYBoundsElastic;
                else if ( strcasecmp(optarg, "periodic") == 0 )
                    turtle_opts = (turtle_opts & ~kCGWTurtleOptionsYBoundsMask) | kCGWTurtleOptionsYBoundsPeriodic;
                else {
                    fprintf(stderr, "ERROR:  invalid y-bounds value: %s\n", optarg);
                    exit(EINVAL);
                }
                break;
            }
            case 'c':
                force_clear = true;
                break;
            case 'p':
                force_pause = true;
                break;
            case 'n': {
                should_init_colors = false;
                break;
            }
        }
    }
    
    CGWCursesInit(CGWSizeI2DMake(0, 0), &display_size);
    if ( ! has_colors() ) {
        CGWCursesShutdown();
        fprintf(stderr, "ERROR:  color is not available in this terminal\n");
        exit(ENOTSUP);
    }
    else if ( ! can_change_color() ) {
        CGWCursesShutdown();
        fprintf(stderr, "ERROR:  colors cannot be altered in this terminal\n");
        exit(ENOTSUP);
    }
    else {
        color_data = color_data_create(COLORS);
        if ( should_init_colors ) color_palette_add_defaults(color_data);
        stylus_color = 0, stylus_pair = color_palette_turtle_to_pair(color_data, &stylus_color);

        // Correction, we do want sychronous input available to us:
        nodelay(stdscr, FALSE);

        //
        // Create our turtle display to fill the curses screen.
        // Periodic boundary conditions are applied to all edges of the
        // screen.
        //
        Yertle = CGWTurtleCreate(NULL,
                                 CGWRectI2DMake(0, 0, display_size.w, display_size.h),
                                 turtle_opts);
        // Turn the stylus on to start:
        CGWTurtleStateSetStylusIsOn(Yertle, true);
        
        // To effect drawing to the curses screen we need to observe pixel events generated
        // by the turtle; as context we provide a pointer to the selected color pair index:
        CGWTurtleSetEventObserver(Yertle, kCGWTurtleEventDrawPixel, turtleDraw, &stylus_pair);
    }

    while ( ! should_exit && (optind < argc) ) {
        FILE        *program_fptr = fopen(argv[optind++], "r");
        
        if ( program_fptr ) {
            while ( (rc == 0) && ! feof(program_fptr) ) {
                char        command[32];
                
                if ( fscanf(program_fptr, "%30s", command) == 1 ) {
                    if ( (command[0] == '#') || (strncmp(command, "//", 2) == 0) ) {
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
                        tpause();
                    }
                    else if ( strcasecmp(command, "clear") == 0 ) {
                        clear();
                    }
                    else if ( strcasecmp(command, "add-color-rgb") == 0 ) {
                        short           r, g, b;
                        
                        if ( fscanf(program_fptr, "%hd %hd %hd", &r, &g, &b) == 3 ) {
                            if ( (r>=0 && r<=1000) && (g>=0 && g<=1000) && (b>=0 && b<=1000) ) {
                                int         cidx = color_palette_add_color(color_data, r, g, b);
                                
                                if ( cidx > 0 ) {
                                    color_palette_add_pair(color_data, cidx, cidx);
                                }
                            }
                        }
                    }
                    else if ( strcasecmp(command, "add-color-hex") == 0 ) {
                        unsigned int    packed_hex_color;
                        
                        if ( fscanf(program_fptr, "%X", &packed_hex_color) == 1 ) {
                            int         r = (1000 * ((packed_hex_color & 0xFF0000) >> 16)) / 255,
                                        g = (1000 * ((packed_hex_color & 0x00FF00) >> 8)) / 255,
                                        b = (1000 * (packed_hex_color & 0x0000FF)) / 255;
                            int         cidx = color_palette_add_color(color_data, r, g, b);
                            
                            if ( cidx > 0 ) {
                                color_palette_add_pair(color_data, cidx, cidx);
                            }
                        }
                    }
                    else if ( strcasecmp(command, "color++") == 0 ) {
                        stylus_color++, stylus_pair = color_palette_turtle_to_pair(color_data, &stylus_color);
                    }
                    else if ( strcasecmp(command, "color--") == 0 ) {
                        stylus_color--, stylus_pair = color_palette_turtle_to_pair(color_data, &stylus_color);
                    }
                    else if ( strcasecmp(command, "color") == 0 ) {
                        int         c;
                        
                        if ( fscanf(program_fptr, "%d", &c) == 1 ) {
                            if ( c >= 0 ) {
                                stylus_color = c, stylus_pair = color_palette_turtle_to_pair(color_data, &stylus_color);
                            }
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
                            
                            // Translate the turtle's current position to get the origin of the circle:
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
                            
                            // Translate the turtle's current position to get the origin of the circle:
                            o = CGWPoint2DSum(o, d_v);
                            
                            CGWTurtleActionCurveAndContract(Yertle, o, phi, per_radian, true);
                        }
                    }
                    else {
                        snprintf(err_msg, sizeof(err_msg), "ERROR:  unknown command: %s\n", command);
                        rc = 1;
                        should_exit = EINVAL;
                    }
                }
            }
            fclose(program_fptr);
            
            if ( force_pause ) tpause();
            if ( force_clear ) clear();
        } else {
            snprintf(err_msg, sizeof(err_msg), "ERROR:  unable to open program file\n");
            rc = ENOENT;
            should_exit = true;
        }
    }
terminate:
    // Restore original color palette:
    clear();
    color_data_destroy(color_data);
    CGWCursesShutdown();
    CGWTurtleSummarize(Yertle);
    CGWTurtleDestroy(Yertle);
    if ( rc ) {
        fprintf(stderr, "%s", err_msg);
    }
    return rc;
}

