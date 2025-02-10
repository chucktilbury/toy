#ifndef _TRACE_H_
#define _TRACE_H_

//#define USE_TRACE

extern int trace_depth;
#define DEPTH_INC 2

#ifdef USE_TRACE
#define TRACE(...)                                                         \
    do {                                                                   \
        printf("%*sTRACE: %s: %d: ", trace_depth, "", __func__, __LINE__); \
        printf(__VA_ARGS__);                                               \
        printf("\n");                                                      \
    } while(0)

#define ENTER                                                \
    do {                                                     \
        printf("%*sENTER: %s: ", trace_depth, "", __func__); \
        trace_depth += DEPTH_INC;                            \
        printf("\n");                                        \
    } while(0)

#define RETURN(...)                                           \
    do {                                                      \
        trace_depth -= DEPTH_INC;                             \
        printf("%*sRETURN: %s: ", trace_depth, "", __func__); \
        printf("%s\n", #__VA_ARGS__);                         \
        return __VA_ARGS__;                                   \
    } while(0)

#else
#define TRACE(...)
#define ENTER
#define RETURN(...) return __VA_ARGS__

#endif

#endif /* _TRACE_H_ */
