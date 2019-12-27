#ifndef _interruptHelper_h
#define _interruptHelper_h

/* Macro to enable all interrupts. */
#ifndef EnableInterrupts
#define EnableInterrupts asm ("CPSIE  i")
#endif


/* Macro to disable all interrupts. */
#ifndef DisableInterrupts
#define DisableInterrupts asm ("CPSID  i")
#endif

#endif
