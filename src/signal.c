#include <signal.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "utils.h"

volatile int running = 1;

void interruptHandler(__maybe_not_used int dummy)
{
    signal(dummy, SIG_IGN);
    running = 0;
}