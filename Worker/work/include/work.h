#ifndef __WORK_H__
#define __WORK_H__

#include "messages.h"

void wait_for_work();
void process_build_order(build_order_msg_t* message);

#endif