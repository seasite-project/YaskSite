#ifndef LOOP_ADAPT_POLICIES_H
#define LOOP_ADAPT_POLICIES_H

#include <loop_adapt_types.h>
#include <errno.h>


int loop_adapt_set_policy(LoopAdaptPolicy pol);
int loop_adapt_init_policy();
int loop_adapt_apply_policy(int step);


#endif /* LOOP_ADAPT_POLICIES_H */
