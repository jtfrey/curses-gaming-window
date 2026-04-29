/**
 * @file CGWControls.c
 * @brief Curses Gaming Window, Keyboard controls
 * 
 * Keyboard control event handler implementation.
 *
 */

#include "CGWControls.h"
#include "CGWBitvector.h"
#include "CGWCurses.h"

//

#ifdef CGW_OS_MACOSX
#   include <ApplicationServices/ApplicationServices.h>
#   include <Carbon/Carbon.h>
#   include <pthread.h>
#endif

//

#ifndef CGW_CONTROLS_DEFAULT_BINDINGS_COUNT
#   define CGW_CONTROLS_DEFAULT_BINDINGS_COUNT 32
#endif

typedef struct CGWControls {
    unsigned int        n_controls;
    unsigned int        n_bindings;
    unsigned int        n_bindings_used;
    bool                has_lock;
    CGWControlBinding   *control_bindings;
    //
    CGWControlBinding   builtin_control_bindings[CGW_CONTROLS_DEFAULT_BINDINGS_COUNT];
    //
    struct {
        pthread_mutex_t     lock;
#ifdef CGW_OS_MACOSX
        pthread_t           monitor_thread;
        bool                is_monitor_launched;
        CFMachPortRef       event_tap;
        CFRunLoopRef        runloop;
        CFRunLoopSourceRef  runloop_source;
#endif
    } os;
    CGWBitvector        control_bits[];
} CGWControls;

//

bool
__CGWControlsSetNBindings(
    CGWControls         *the_controls,
    unsigned int        n_bindings
)
{
    CGWControlBinding   *new_control_bindings;
    unsigned int        n_bindings_rounded = n_bindings + (CGW_CONTROLS_DEFAULT_BINDINGS_COUNT - (n_bindings % CGW_CONTROLS_DEFAULT_BINDINGS_COUNT));
    bool                should_copy_from_builtin;
    
    if ( the_controls->n_bindings == CGW_CONTROLS_DEFAULT_BINDINGS_COUNT ) {
        new_control_bindings = (CGWControlBinding*)calloc(n_bindings_rounded, sizeof(CGWControlBinding));
        should_copy_from_builtin = true;
    } else {
        new_control_bindings = (CGWControlBinding*)realloc(the_controls->control_bindings, n_bindings_rounded * sizeof(CGWControlBinding));
        should_copy_from_builtin = false;
    }
    if ( ! new_control_bindings ) return false;
    the_controls->control_bindings = new_control_bindings;
    if ( should_copy_from_builtin ) {
        memcpy(new_control_bindings, the_controls->builtin_control_bindings, CGW_CONTROLS_DEFAULT_BINDINGS_COUNT * sizeof(CGWControlBinding));
    }
    the_controls->n_bindings = n_bindings_rounded;
    return true;
}

//

int
__CGWControlsBindingIndexForKeyId(
    CGWControls     *the_controls,
    int             key_id
)
{
    /* Do a binary search on the bindings list: */
    int             low_idx = 0, high_idx = the_controls->n_bindings_used - 1;
    
    while ( low_idx <= high_idx ) {
        int         mid_idx = low_idx + (high_idx - low_idx) / 2;
        
        if ( the_controls->control_bindings[mid_idx].key_id == key_id ) return mid_idx;
        if ( the_controls->control_bindings[mid_idx].key_id < key_id )
            low_idx = mid_idx + 1;
        else
            high_idx = mid_idx - 1;
    }
    return -1;
}
    

//

#ifdef CGW_OS_MACOSX

static int
__CGWControlCursesKeyToCGKeyCode(
    int     curses_key
)
{
    static int          _alpha[26] = {
                            kVK_ANSI_A, kVK_ANSI_B, kVK_ANSI_C, kVK_ANSI_D, kVK_ANSI_E,
                            kVK_ANSI_F, kVK_ANSI_G, kVK_ANSI_H, kVK_ANSI_I, kVK_ANSI_J,
                            kVK_ANSI_K, kVK_ANSI_L, kVK_ANSI_M, kVK_ANSI_N, kVK_ANSI_O,
                            kVK_ANSI_P, kVK_ANSI_Q, kVK_ANSI_R, kVK_ANSI_S, kVK_ANSI_T,
                            kVK_ANSI_U, kVK_ANSI_V, kVK_ANSI_W, kVK_ANSI_X, kVK_ANSI_Y,
                            kVK_ANSI_Z };
    static int          _numeric[10] = {
                            kVK_ANSI_0, kVK_ANSI_1, kVK_ANSI_2, kVK_ANSI_3, kVK_ANSI_4,
                            kVK_ANSI_5, kVK_ANSI_6, kVK_ANSI_7, kVK_ANSI_8, kVK_ANSI_9 };
    static int          _fn[20] = {
                            kVK_F1,  kVK_F2,  kVK_F3,  kVK_F4,  kVK_F5,  kVK_F6,  kVK_F7,
                            kVK_F8,  kVK_F9,  kVK_F10, kVK_F11, kVK_F12, kVK_F13, kVK_F14,
                            kVK_F15, kVK_F16, kVK_F17, kVK_F18, kVK_F19, kVK_F20 };
                            
    if ( curses_key >= 48 && curses_key <= 57 ) return _numeric[curses_key - 48];
    if ( curses_key >= 65 && curses_key <= 90 ) return _alpha[curses_key - 65];
    if ( curses_key >= 97 && curses_key <= 122 ) return _alpha[curses_key - 97];
    if ( curses_key >= KEY_F0 && curses_key <= KEY_F(20) ) return _fn[curses_key - KEY_F0];
    switch ( curses_key ) {
        case AUX_KEY_ESC:       return kVK_Escape;
        case ' ':           	return kVK_Space;
        case '=':           	return kVK_ANSI_Equal;
        case '-':           	return kVK_ANSI_Minus;
        case ']':           	return kVK_ANSI_RightBracket;
        case '[':           	return kVK_ANSI_LeftBracket;
        case '\'':          	return kVK_ANSI_Quote;
        case '/':           	return kVK_ANSI_Slash;
        case '\\':          	return kVK_ANSI_Backslash;
        case ';':           	return kVK_ANSI_Semicolon;
        case ',':           	return kVK_ANSI_Comma;
        case '.':           	return kVK_ANSI_Period;
        case '`':           	return kVK_ANSI_Grave;
        case '\n':          	return kVK_Return;
        case KEY_UP:        	return kVK_UpArrow;
        case KEY_DOWN:      	return kVK_DownArrow;
        case KEY_LEFT:      	return kVK_LeftArrow;
        case KEY_RIGHT:     	return kVK_RightArrow;
        case KEY_HOME:      	return kVK_Home;
        case KEY_BACKSPACE: 	return kVK_Delete;
        case KEY_DC:        	return kVK_ForwardDelete;
        case KEY_PPAGE:         return kVK_PageUp;
        case KEY_NPAGE:         return kVK_PageDown;
        case AUX_KEY_LSHIFT:    return kVK_Shift;
        case AUX_KEY_RSHIFT:    return kVK_RightShift;
        case AUX_KEY_LCONTROL:  return kVK_Control;
        case AUX_KEY_RCONTROL:  return kVK_RightControl;
        case AUX_KEY_LOPTION:   return kVK_Option;
        case AUX_KEY_ROPTION:   return kVK_RightOption;
        default:                break;
    }
    return -1;
}

CGEventRef
__CGWControlsKeyEventCallback(
    CGEventTapProxy     proxy,
    CGEventType         type, 
    CGEventRef          event,
    void                *refcon
)
{
    CGWControls         *the_controls = (CGWControls*)refcon;
    CGKeyCode           keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    
    if ( pthread_mutex_lock(&the_controls->os.lock) == 0 ) {
        int         def_idx = __CGWControlsBindingIndexForKeyId(the_controls, keyCode);
        
        if ( def_idx >= 0 ) {
            int         ctrl_id = the_controls->control_bindings[def_idx].ctrl_id;
            
            if ( type == kCGEventFlagsChanged ) {
                CGWBitvectorToggleBitIndex(
                        the_controls->control_bits[ctrl_id / CGW_BITVECTOR_BITS],
                        ctrl_id % CGW_BITVECTOR_BITS
                    );
            } else {
                CGWBitvectorSetBitIndex(
                        the_controls->control_bits[ctrl_id / CGW_BITVECTOR_BITS],
                        ctrl_id % CGW_BITVECTOR_BITS,
                        (type == kCGEventKeyDown)
                    );
            }
        }
        pthread_mutex_unlock(&the_controls->os.lock);
    }
    return event;
}

void*
__CGWControlsRunloopThread(
    void        *context
)
{
    CGWControls *the_controls = (CGWControls*)context;
    
    pthread_mutex_lock(&the_controls->os.lock);
    the_controls->os.runloop = CFRunLoopGetCurrent();
    the_controls->os.event_tap = CGEventTapCreate(
                                        kCGSessionEventTap,
                                        kCGHeadInsertEventTap,
                                        kCGEventTapOptionListenOnly,                // Passive listener
                                        CGEventMaskBit(kCGEventKeyDown) |           // Detect key down...
                                            CGEventMaskBit(kCGEventKeyUp) |         // ...detect key up ...
                                            CGEventMaskBit(kCGEventFlagsChanged),   // ...detect modifiers
                                        __CGWControlsKeyEventCallback,
                                        the_controls
                                    );
    if ( the_controls->os.event_tap ) {
        the_controls->os.runloop_source = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, the_controls->os.event_tap, 0);
        if ( the_controls->os.runloop_source ) {
            CFRunLoopAddSource(the_controls->os.runloop, the_controls->os.runloop_source, kCFRunLoopCommonModes);
            CGEventTapEnable(the_controls->os.event_tap, true);
            pthread_mutex_unlock(&the_controls->os.lock);
            CFRunLoopRun();
            pthread_mutex_lock(&the_controls->os.lock);
            CGEventTapEnable(the_controls->os.event_tap, false);
        }
        CFRunLoopSourceInvalidate(the_controls->os.runloop_source);
        CFRelease(the_controls->os.runloop_source);
        CFRelease(the_controls->os.event_tap);
    }
    pthread_mutex_unlock(&the_controls->os.lock);
    return NULL;
}

#endif

//

CGWControlsRef
CGWControlsCreate(
    unsigned int    n_controls
)
{
    unsigned int    n_controls_rounded = n_controls + (CGW_BITVECTOR_BITS - (n_controls % CGW_BITVECTOR_BITS));
    size_t          n_bytes = sizeof(CGWControls) + n_controls_rounded * sizeof(CGWBitvector);
    CGWControls     *new_controls = (CGWControls*)calloc(1, n_bytes);
    
    if ( new_controls ) {
        new_controls->n_controls = n_controls_rounded;
        new_controls->n_bindings = CGW_CONTROLS_DEFAULT_BINDINGS_COUNT;
        new_controls->control_bindings = &new_controls->builtin_control_bindings[0];
        pthread_mutex_init(&new_controls->os.lock, NULL);
#ifdef CGW_OS_MACOSX

       pthread_create(&new_controls->os.monitor_thread, NULL, __CGWControlsRunloopThread, new_controls);
       new_controls->os.is_monitor_launched = true;
#endif
    }
    return (CGWControlsRef)new_controls;
}

//

CGWControlsRef
CGWControlsCreateWithBindings(
    CGWControlBinding   *bindings
)
{
    CGWControlsRef  new_controls = NULL;
    
    if ( bindings ) {
        CGWControlBinding   *bindings_copy = bindings;
        int                 ctrl_id_max = INT_MIN;
        int                 n_bindings = 0;
        
        while ( bindings->ctrl_id >= 0 ) {
            if ( bindings->ctrl_id > ctrl_id_max ) ctrl_id_max = bindings->ctrl_id;
            bindings++, n_bindings++;
        }
        new_controls = CGWControlsCreate(ctrl_id_max + 1);
        if ( new_controls ) {
            while ( bindings_copy->ctrl_id >= 0 ) {
                CGWControlsRegister(new_controls, bindings_copy->key_id, bindings_copy->ctrl_id);
                bindings_copy++;
            }
        }
    }
    return new_controls;
}

//

void
CGWControlsDestroy(
    CGWControlsRef  the_controls
)
{
    pthread_mutex_destroy(&the_controls->os.lock);
    if ( the_controls->n_bindings > CGW_CONTROLS_DEFAULT_BINDINGS_COUNT ) {
        if ( the_controls->control_bindings ) free((void*)the_controls->control_bindings);
    }
#ifdef CGW_OS_MACOSX
    if ( the_controls->os.is_monitor_launched ) {
        CFRunLoopStop(the_controls->os.runloop);
        pthread_join(the_controls->os.monitor_thread, NULL);
    }
#endif
    free((void*)the_controls);
}

//

void
CGWControlsSummarize(
    CGWControlsRef  the_controls
)
{
    unsigned int    i;
    
    printf("CGWControls@%p (n_controls=%d | n_bindings=%d | n_bindings_used = %d) {\n",
        the_controls, the_controls->n_controls, the_controls->n_bindings, the_controls->n_bindings_used);
    for ( i = 0; i < the_controls->n_bindings_used; i++ ) {
        printf("    %4d: { key_id = 0x%08X, ctrl_id = %d }\n", i, the_controls->control_bindings[i].key_id, 
                                the_controls->control_bindings[i].ctrl_id);
    }
    printf("}\n");
}

//

bool
CGWControlsRegister(
    CGWControlsRef  the_controls,
    int             key_id,
    CGWControlId    ctrl_id
)
{
    int             def_idx = 0, insert_idx = -1;
    
    if ( ! the_controls->has_lock ) pthread_mutex_lock(&the_controls->os.lock);
    
#ifdef CGW_OS_MACOSX

    /* Map to a Mac virtual keycode: */
    key_id = __CGWControlCursesKeyToCGKeyCode(key_id);
    if ( key_id < 0 ) return false;
    
#endif

    while ( def_idx < the_controls->n_bindings_used ) {
        if ( the_controls->control_bindings[def_idx].key_id == key_id ) {
            return (the_controls->control_bindings[def_idx].ctrl_id == ctrl_id);
        }
        else if ( the_controls->control_bindings[def_idx].key_id > key_id ) {
            insert_idx = def_idx;
            break;
        }
        def_idx++;
    }
    if ( def_idx == the_controls->n_bindings ) {
        if ( ! __CGWControlsSetNBindings(the_controls, the_controls->n_bindings + 1) ) {
            return false;
        }
    }
    if ( insert_idx != -1 ) {
        /* Insertion in the middle (or start) of the list, move
           entries to make room: */
        unsigned int        i = the_controls->n_bindings_used;
        
        while ( i > insert_idx ) {
            the_controls->control_bindings[i] = the_controls->control_bindings[i - 1];
            i--;
        }
    } else {
        /* Append to the list: */
        insert_idx = def_idx;
    }
    
    /* Set the new binding: */
    the_controls->control_bindings[insert_idx].key_id = key_id;
    the_controls->control_bindings[insert_idx].ctrl_id = ctrl_id;
    the_controls->n_bindings_used++;

    if ( ! the_controls->has_lock ) pthread_mutex_unlock(&the_controls->os.lock);
    return true;
}

//

bool
CGWControlsRegisterArrayOfBindings(
    CGWControlsRef      the_controls,
    CGWControlBinding   *bindings
)
{
    bool            rc = true, took_lock = false;
    
    if ( ! the_controls->has_lock ) {
        pthread_mutex_lock(&the_controls->os.lock);
        the_controls->has_lock = took_lock = true;
    }
    
    while ( rc && (bindings->ctrl_id >= 0) ) {
        rc = CGWControlsRegister(the_controls, bindings->key_id, bindings->ctrl_id);
        bindings++;
    }
    
    if ( took_lock ) {
        the_controls->has_lock = false;
        pthread_mutex_unlock(&the_controls->os.lock);
    }
    
    return rc;
}

//

void
CGWControlsReset(
    CGWControlsRef  the_controls
)
{
    unsigned int    i = the_controls->n_controls / CGW_BITVECTOR_BITS;

    if ( ! the_controls->has_lock ) pthread_mutex_lock(&the_controls->os.lock);
    while ( i-- ) the_controls->control_bits[i] = 0;
    if ( ! the_controls->has_lock ) pthread_mutex_unlock(&the_controls->os.lock);
}

//

void
CGWControlsReadLock(
    CGWControlsRef the_controls
)
{
    if ( ! the_controls->has_lock ) {
        pthread_mutex_lock(&the_controls->os.lock);
        the_controls->has_lock = true;
    }
}

//

void
CGWControlsReadUnlock(
    CGWControlsRef the_controls
)
{
    if ( the_controls->has_lock ) {
        the_controls->has_lock = false;
        pthread_mutex_unlock(&the_controls->os.lock);
    }
}

//

bool
CGWControlsRead(
    CGWControlsRef  the_controls,
    CGWControlId    ctrl_id
)
{
    bool            rc = false;
    
    if ( ctrl_id < the_controls->n_controls ) {
        if ( ! the_controls->has_lock ) pthread_mutex_lock(&the_controls->os.lock);
        rc = CGWBitvectorGetBitIndex(the_controls->control_bits[ctrl_id / CGW_BITVECTOR_BITS], ctrl_id % CGW_BITVECTOR_BITS);
        if ( ! the_controls->has_lock ) pthread_mutex_unlock(&the_controls->os.lock);
    }
    return rc;
}

//

typedef struct {
    CGWControlsStates   base;
    CGWControlsRef      the_controls;
    bool                is_read;
    CGWControlState     *last_states;
    unsigned int        state_idx;
    CGWControlState     states[];
} CGWControlsStatesPrivate;

//

static CGWControlsStatesPrivate*
CGWControlsStatesPrivateCreate(
    CGWControlsRef      the_controls
)
{
    size_t                      n_bytes = sizeof(CGWControlsStatesPrivate) + 
                                            the_controls->n_controls * sizeof(CGWControlState);
    CGWControlsStatesPrivate    *new_states = (CGWControlsStatesPrivate*)calloc(1, n_bytes);
    
    if ( new_states ) {
        new_states->the_controls = the_controls;
        new_states->base.n_states = the_controls->n_controls;
    }
    return new_states;
}

//

static void
CGWControlsStatesPrivateRead(
    CGWControlsStatesPrivate    *the_states
)
{
    unsigned int        ctrl_id = 0, bv_idx = 0, n_bit = 0;
    CGWBitvector        bv;
    
    CGWControlsReadLock(the_states->the_controls);
    if ( ! the_states->is_read ) {
        /* Prepare to read for the first time: */
        the_states->base.current_states = &the_states->states[0];
        the_states->state_idx = 0;
        
        while ( ctrl_id < the_states->base.n_states ) {
            if ( n_bit == 0 ) {
                n_bit = CGW_BITVECTOR_BITS;
                bv = the_states->the_controls->control_bits[bv_idx++];
            }
            the_states->base.current_states[ctrl_id++] = (bv & (CGW_BITVECTOR_TYPE)0x1);
            bv >>= 1, n_bit--;
        }
        the_states->is_read = true;
    } else {
        the_states->last_states = the_states->base.current_states;
        the_states->base.current_states = &the_states->states[the_states->state_idx++ ? the_states->base.n_states : 0];
        
        while ( ctrl_id < the_states->base.n_states ) {
            if ( n_bit == 0 ) {
                n_bit = CGW_BITVECTOR_BITS;
                bv = the_states->the_controls->control_bits[bv_idx++];
            }
            the_states->base.current_states[ctrl_id] = (bv & (CGW_BITVECTOR_TYPE)0x1) ?
                    ( (the_states->last_states[ctrl_id]) ? kCGWControlStateOn : kCGWControlStatePressed ) :
                    kCGWControlStateOff;
            bv >>= 1, n_bit--, ctrl_id++;
        }
    }
    CGWControlsReadUnlock(the_states->the_controls);
}

//

CGWControlsStatesRef
CGWControlsStatesCreate(
    CGWControlsRef      the_controls
)
{
    return (CGWControlsStatesRef)CGWControlsStatesPrivateCreate(the_controls);
}

//

void
CGWControlsStatesDestroy(
    CGWControlsStatesRef    the_states
)
{
    free((void*)the_states);
}

//

void
CGWControlsStatesRead(
    CGWControlsStatesRef    the_states
)
{
    CGWControlsStatesPrivateRead((CGWControlsStatesPrivate*)the_states);
}

//