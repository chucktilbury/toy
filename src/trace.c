
#include "trace.h"

// Every module defines its own trace macros depending on its needs. This
// allows them all to operate together.
int trace_depth = 0;
