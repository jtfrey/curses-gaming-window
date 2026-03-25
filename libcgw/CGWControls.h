/**
 * @file CGWControls.h
 * @brief Curses Gaming Window, Keyboard controls
 * 
 * A header file that binds keyboard keys to controls
 * are harvested.
 *
 */

#ifndef __CGWCONTROLS_H__
#define __CGWCONTROLS_H__

#include "CGWConfig.h"

/**
 * A control identifier
 * Each distinct control to which the system responds is
 * given an integer identifier.  This is typically most-easily
 * accomplished by creating an enum with the first control having
 * id 0 and the final member of the enum being a constant
 * indicating the number of controls.  E.g.
 *
 *     enum {
 *         kControlIdStart = 0,
 *         kControlIdSelect,
 *         kControlIdA,
 *         kControlIdB,
 *         kControlIdUp,
 *         kControlIdDown,
 *         kControlIdLeft,
 *         kControlIdRight,
 *         kControlIdMax
 *     };
 *
 * Negative ids should NOT be used.
 */
typedef int CGWControlId;

/**
 * A control binding
 * Associates an ASCII (or curses) key with a control id.
 */
typedef struct {
    CGWControlId        ctrl_id;    /*!< the control id to associate with the key */
    int                 key_id;     /*!< the key id, either as an ASCII character or curses KEY
                                         constant */
} CGWControlBinding;

/**
 * CGWControlBinding initializer
 * Macro that generates a struct initializer setting the key and ctrl
 * id fields (\p K and \p C, respectively).
 *
 * For example:
 *
 *     enum { kControlIdUp = 0, kControlIdDown, kControlIdMax };
 *
 *     CGWControlBinding    game_controls[] = {
 *             CGWControlBindingInit(KEY_UP, kControlIdUp),
 *             CGWControlBindingInit(KEY_DOWN, kControlIdDown),
 *             CGWControlBindingEndOfList()
 *         };
 *
 * @param K         the key id
 * @param C         the control id
 */
#define CGWControlBindingInit(K, C)  { .key_id = (K), .ctrl_id = (C) }

/**
 * CGWControlBinding array sentinel
 * Macro that generates a struct initializer for a \ref CGWControlBinding
 * that ends an array of such structures.
 *
 * For example:
 *
 *     enum { kControlIdUp = 0, kControlIdDown, kControlIdMax };
 *
 *     CGWControlBinding    game_controls[] = {
 *             CGWControlBindingInit(KEY_UP, kControlIdUp),
 *             CGWControlBindingInit(KEY_DOWN, kControlIdDown),
 *             CGWControlBindingEndOfList()
 *         };
 */
#define CGWControlBindingEndOfList() { .key_id = -1, .ctrl_id = -1 }

/**
 * Type of a reference to a CGWControls object
 * Since CGWControls objects are always dynamically-allocated, a
 * reference (pointer) is used when working with one.
 */
typedef struct CGWControls * CGWControlsRef;

/**
 * Create an empty controls object
 * Creates a new \ref CGWControls object with a \ref CGWControlId
 * range from [0, n_controls).
 *
 * The object has no key bindings.
 *
 * Dependent on the underlying operating system, a separate thread
 * may be created in order to monitor for keyboard events.
 *
 * @param n_controls    the upper-bound to the control id range
 * @return              NULL if any error occurred, a \ref CGWControlsRef
 *                      if successful
 */
CGWControlsRef CGWControlsCreate(unsigned int n_controls);

/**
 * Create a controls object with the given bindings
 * The array of \ref CGWControlBinding data structures pointed to
 * by \p bindings is used to initialize and populate a new
 * \ref CGWControls object.
 *
 * See \ref CGWControlBindingInit() and \ref CGWControlBindingEndOfList()
 * for information on the structure of the bindings array.
 *
 * Dependent on the underlying operating system, a separate thread
 * may be created in order to monitor for keyboard events.
 *
 * @param bindings      the array of key bindings
 * @return              NULL if any error occurred, a \ref CGWControlsRef
 *                      if successful
 */
CGWControlsRef CGWControlsCreateWithBindings(CGWControlBinding *bindings);

/**
 * Shutdown keyboard event monitoring
 * Terminate keyboard event monitoring associated with \p the_controls.
 * Deallocate and invalidate the object.
 * @param the_controls      the controls object to be shutdown
 */
void CGWControlsDestroy(CGWControlsRef the_controls);

/**
 * Summarize control bindings to stdout
 * Writes a summary of \p the_controls to stdout.  Primarily useful
 * for debugging.
 * @param the_controls      the \ref CGWControls to display
 */
void CGWControlsSummarize(CGWControlsRef the_controls);

/**
 * Register a key binding
 * Bind the key indicated by the ASCII or curses key code \p key_id
 * to the given control id in \p the_controls.
 * @param the_controls      the \ref CGWControls to alter
 * @param key_id            the ASCII or curses key code
 * @param ctrl_id           the control to bind the key to
 * @return                  returns true if the key was successfully
 *                          bound, false otherwise
 */
bool CGWControlsRegister(CGWControlsRef the_controls, int key_id, CGWControlId ctrl_id);

/**
 * Register an array of key bindings en masse
 * The array of \ref CGWControlBinding data structures pointed to
 * by \p bindings are registered with \p the_controls.
 *
 * See \ref CGWControlBindingInit() and \ref CGWControlBindingEndOfList()
 * for information on the structure of the bindings array.
 * @param the_controls      the \ref CGWControls to alter
 * @param bindings          the array of key bindings
 * @return                  true if at least one binding was made, false
 *                          if none were
 */
bool CGWControlsRegisterArrayOfBindings(CGWControlsRef the_controls, CGWControlBinding *bindings); 

/**
 * Reset the control states
 * All control states are reset to zero.
 * @param the_controls      the \ref CGWControls to alter
 */
void CGWControlsReset(CGWControlsRef the_controls);

/**
 * Lock-out control state changes
 * Should be called prior to making multiple queries via
 * \ref CGWControlsRead() to prevent any changes' being
 * made while the state is being examined.
 * @param the_controls      the \ref CGWControls to lock
 */
void CGWControlsReadLock(CGWControlsRef the_controls);

/**
 * Release the lock on control state changes
 * Should be called after a call to \ref CGWControlsReadLock()
 * and subsequent queries via \ref CGWControlsRead().
 * @param the_controls      the \ref CGWControls to unlock
 */
void CGWControlsReadUnlock(CGWControlsRef the_controls);

/**
 * Read control state
 * Returns the state of the control with id \p ctrl_id.
 *
 * If \ref CGWControlsReadLock() was not called, the lock is
 * taken and release by this function itself to ensure consistent
 * state.
 * @param the_controls      the \ref CGWControls to unlock
 * @return                  the value of the control; false is also
 *                          returned if an error occurs
 */
bool CGWControlsRead(CGWControlsRef the_controls, CGWControlId ctrl_id);

/**
 * Tri-state for cumulative key events
 * A state machine for key events.
 */
typedef enum {
    kCGWControlStateOff = 0,        /*!< the control is not pressed */
    kCGWControlStatePressed = 1,    /*!< the control has been pressed */
    kCGWControlStateOn = 2          /*!< the control continued to be pressed */
} CGWControlState;

/**
 * Cumulative controls states
 * Rather than read instantaneous states using \ref CGWControlsRead(),
 * a \ref CGWControlsState captures the state of every control as a
 * three-valued state machine.  E.g. the caller can react to only the
 * initial keypress that moves the control state out of being off.
 *
 * Private fields are layered on top of what is presented here in the
 * implementation of \ref CGWControlsStates, including the storage for
 * two state buffers that are swapped between current and last each
 * time new state is read for the controls.
 */
typedef struct {
    unsigned int        n_states;           /*!< the number of states in the array at
                                                 \p current_states */
    CGWControlState     *current_states;    /*!< the array of states last read */
} CGWControlsStates;

/**
 * Type of a reference to a CGWControlsStates object
 * Since CGWControlsStates objects are always dynamically-allocated, a
 * reference (pointer) is used when working with one.
 */
typedef CGWControlsStates * CGWControlsStatesRef;

/**
 * Create a controls states object
 * Create a \ref CGWControlsStates object capable of reading control
 * state from \p the_controls.  Note that only the pointer to \p
 * the_controls is used by the \ref CGWControlsStates, so that object
 * must be destroyed only AFTER any CGWControlsStates object(s) have
 * been destroyed.
 * @param the_controls      the \ref CGWControls object
 * @return                  a control states object prepared to read
 *                          state from \p the_controls, or NULL if
 *                          any error occurred
 */
CGWControlsStatesRef CGWControlsStatesCreate(CGWControlsRef the_controls);

/**
 * Destroy a controls states object
 * Deallocate and invalidate \p the_states.
 * @param the_states        the \ref CGWControlsStates object
 */
void CGWControlsStatesDestroy(CGWControlsStatesRef the_states);

/**
 * Read and update controls states
 * Reads control values from the \ref CGWControls associated with
 * \p the_states and updates the \ref CGWControlsStates::current_states
 * array accordingly.
 * @param the_states        the \ref CGWControlsStates object
 */
void CGWControlsStatesRead(CGWControlsStatesRef the_states);

/**
 * Get the state of a control
 * Macro that returns the state of control id \p CID in the
 * \ref CGWControlsStates object \p CS.  No bounds-checking is
 * performed, so the caller should ensure that \p CID is within
 * the appropriate range -- zero (0) through
 * \ref CGWControlsStates::n_states.
 * @param CS                the \ref CGWControlsStates object
 * @param CID               the \ref CGWControlId to retrieve
 * @return                  the \ref CGWControlState associated with
 *                          the control
 */
#define CGWControlsStatesGet(CS, CID) ((CS)->current_states[(CID)])


#endif /* __CGWCONTROLS_H__ */