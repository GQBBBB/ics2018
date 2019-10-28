#include "common.h"

_Context* do_syscall(_Context* c);
_Context* schedule(_Context *prev);

static _Context* do_event(_Event e, _Context* c) {
  Log("EVENT_ID=0x%x", e.event);
  switch (e.event) {
    case _EVENT_YIELD: Log("receive _EVENT_YIELD event");
					   return schedule(c); 
					   break;
	case _EVENT_SYSCALL: Log("receive _EVENT_SYSCALL event"); 
						 do_syscall(c); 
						 break;
	case _EVENT_IRQ_TIMER: Log("receive _EVENT_IRQ_TIMER event");
						   _yield();
						   break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
