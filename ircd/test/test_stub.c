/* test_stub.c - support stubs for test programs */

#include "client.h"
#include "ircd_log.h"
#include "s_debug.h"
#include <stdarg.h>
#include <stdio.h>

struct Client me;
int log_inassert;

void
log_write(enum LogSys subsys, enum LogLevel severity, unsigned int flags,
          const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void
debug(int level, const char *form, ...)
{
    va_list args;

    va_start(args, form);
    vfprintf(stdout, form, args);
    va_end(args);
}

int
exit_client(struct Client *cptr, struct Client *bcptr, struct Client *sptr,
            const char *comment)
{
    Debug((DEBUG_LIST, "exit_client(%p, %p, %p, \"%s\")\n", cptr, bcptr, sptr, comment));
    return 0;
}
