

/* CEU_C */

#define CEU_FEATURES_OS true
#define CEU_FEATURES_OS_TRUE
#define CEU_FEATURES_ASYNC true
#define CEU_FEATURES_ASYNC_TRUE
        /* CEU_FEATURES */

#include <stddef.h>     /* offsetof */
#include <stdlib.h>     /* NULL */
#include <string.h>     /* memset, strlen */
#ifdef CEU_TESTS
#include <stdio.h>
#endif

#ifdef CEU_FEATURES_LUA
#include <lua5.3/lua.h>
#include <lua5.3/lauxlib.h>
#include <lua5.3/lualib.h>
#endif

#define S8_MIN   -127
#define S8_MAX    127
#define U8_MAX    255

#define S16_MIN  -32767
#define S16_MAX   32767
#define U16_MAX   65535

#define S32_MIN  -2147483647
#define S32_MAX   2147483647
#define U32_MAX   4294967295

#define S64_MIN  -9223372036854775807
#define S64_MAX   9223372036854775807
#define U64_MAX   18446744073709551615

typedef u16 tceu_nevt;   /* TODO */
typedef u8  tceu_nstk;   /* TODO */
typedef u8 tceu_ntrl;
typedef u8 tceu_nlbl;

#define CEU_TRAILS_N 1
#ifndef CEU_STACK_N
#define CEU_STACK_N 500
#endif

#define CEU_API
CEU_API void ceu_start (int argc, char* argv[]);
CEU_API void ceu_stop  (void);
CEU_API void ceu_input (tceu_nevt id, void* params);
CEU_API int  ceu_loop  (int argc, char* argv[]);

#ifdef CEU_FEATURES_TRACE
#define CEU_TRACE_null   ((tceu_trace){NULL,NULL,0})

typedef struct tceu_trace {
    struct tceu_trace* up;
    const char* file;
    u32 line;
} tceu_trace;
#endif

struct tceu_code_mem;
struct tceu_pool_pak;

typedef struct tceu_evt {
    tceu_nevt id;
    union {
        void* mem;                   /* CEU_INPUT__PROPAGATE_CODE, CEU_EVENT__MIN */
#ifdef CEU_FEATURES_POOL
        struct tceu_pool_pak* pak;   /* CEU_INPUT__PROPAGATE_POOL */
#endif
    };
} tceu_evt;

typedef struct tceu_range {
    struct tceu_code_mem* mem;
    tceu_ntrl             trl0;
    tceu_ntrl             trlF;
} tceu_range;

typedef struct tceu_stk {
    tceu_evt   evt;
    tceu_range range;
    void*      params;
    usize      params_n;
    bool       is_alive;
    struct tceu_stk* prv;
} tceu_stk;

struct tceu_data_Exception;

typedef struct tceu_trl {
    struct {
        tceu_evt evt;
        union {
            struct {
                tceu_nlbl lbl;
                tceu_nstk level;       /* CEU_INPUT__STACKED */
            };
#ifdef CEU_FEATURES_PAUSE
            struct {
                tceu_evt  pse_evt;
                tceu_ntrl pse_skip;
                u8        pse_paused;
            };
#endif
        };
    };
} tceu_trl;

#ifdef CEU_FEATURES_EXCEPTION
typedef struct tceu_catch {
    struct tceu_catch*         up;
    struct tceu_code_mem*      mem;
    tceu_ntrl                  trl;
    struct tceu_opt_Exception* exception;
} tceu_catch;
#endif

typedef struct tceu_code_mem {
#ifdef CEU_FEATURES_POOL
    struct tceu_pool_pak* pak;
#endif
    struct tceu_code_mem* up_mem;
    u8          depth;
#ifdef CEU_FEATURES_TRACE
    tceu_trace  trace;
#endif
#ifdef CEU_FEATURES_EXCEPTION
    tceu_catch* catches;
#endif
#ifdef CEU_FEATURES_LUA
    lua_State*  lua;
#endif
    bool has_term;
    tceu_ntrl   trails_n;
    tceu_trl    _trails[0];
} tceu_code_mem;

#ifdef CEU_FEATURES_THREAD
typedef struct tceu_threads_data {
    CEU_THREADS_T id;
    u8 has_started:    1;
    u8 has_terminated: 1;
    u8 has_aborted:    1;
    u8 has_notified:   1;
    struct tceu_threads_data* nxt;
} tceu_threads_data;

typedef struct {
    tceu_code_mem*     mem;
    tceu_threads_data* thread;
} tceu_threads_param;
#endif

#ifdef CEU_FEATURES_ISR_STATIC
typedef struct tceu_isr_evt {
    tceu_nevt id;
    u8        len;
    void*     args;
} tceu_isr_evt;
#else
typedef struct tceu_evt_id_params {
    tceu_nevt id;
    void*     params;
} tceu_evt_id_params;
typedef struct tceu_isr {
    void (*fun)(tceu_code_mem*);
    tceu_code_mem*     mem;
    tceu_evt_id_params evt;
} tceu_isr;
#endif

/*****************************************************************************/

/* CEU_VECTOR_H */
#include <stdlib.h>     /* NULL */
#include <string.h>     /* memcpy */

typedef struct {
    usize max;
    usize len;
    usize ini;
    usize unit;
    u8    is_ring:    1;
    u8    is_dyn:     1;
    u8    is_freezed: 1;
    byte* buf;
} tceu_vector;

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define ceu_vector_idx(vec,idx)     ((vec)->is_ring ? (((vec)->ini + (idx)) % (vec)->max) : (idx))
#define ceu_vector_buf_get(vec,idx) (&(vec)->buf[ceu_vector_idx(vec,idx)*(vec)->unit])
#define ceu_vector_ptr(vec)         (vec)

#ifdef CEU_FEATURES_TRACE
#define ceu_vector_buf_set(vec,idx,buf,nu)      ceu_vector_buf_set_ex(vec,idx,buf,nu,CEU_TRACE(0))
#define ceu_vector_copy(dst,dst_i,src,src_i,n)  ceu_vector_copy_ex(dst,dst_i,src,src_i,n,CEU_TRACE(0))
#define ceu_vector_setmax(vec,len,freeze)       ceu_vector_setmax_ex(vec,len,freeze,CEU_TRACE(0))
#define ceu_vector_setlen_could(vec,len,grow)   ceu_vector_setlen_could_ex(vec,len,grow,CEU_TRACE(0))
#define ceu_vector_setlen(a,b,c)                ceu_vector_setlen_ex(a,b,c,CEU_TRACE(0))
#define ceu_vector_geti(a,b)                    ceu_vector_geti_ex(a,b,CEU_TRACE(0))
#else
#define ceu_vector_buf_set(vec,idx,buf,nu)      ceu_vector_buf_set_ex(vec,idx,buf,nu)
#define ceu_vector_copy(dst,dst_i,src,src_i,n)  ceu_vector_copy_ex(dst,dst_i,src,src_i,n)
#define ceu_vector_setmax(vec,len,freeze)       ceu_vector_setmax_ex(vec,len,freeze,_)
#define ceu_vector_setlen_could(vec,len,grow)   ceu_vector_setlen_could_ex(vec,len,grow)
#define ceu_vector_setlen(a,b,c)                ceu_vector_setlen_ex(a,b,c,_)
#define ceu_vector_geti(a,b)                    ceu_vector_geti_ex(a,b)
#endif

void  ceu_vector_init            (tceu_vector* vector, usize max, bool is_ring, bool is_dyn, usize unit, byte* buf);

#ifdef CEU_FEATURES_TRACE
#define ceu_vector_setmax_ex(a,b,c,d) ceu_vector_setmax_ex_(a,b,c,d)
#else
#define ceu_vector_setmax_ex(a,b,c,d) ceu_vector_setmax_ex_(a,b,c)
#endif

#ifdef CEU_FEATURES_DYNAMIC
byte* ceu_vector_setmax_ex_      (tceu_vector* vector, usize len, bool freeze
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 );
#endif

int   ceu_vector_setlen_could_ex (tceu_vector* vector, usize len, bool grow
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 );

#ifdef CEU_FEATURES_TRACE
#define ceu_vector_setlen_ex(a,b,c,d) ceu_vector_setlen_ex_(a,b,c,d)
#else
#define ceu_vector_setlen_ex(a,b,c,d) ceu_vector_setlen_ex_(a,b,c)
#endif

void  ceu_vector_setlen_ex_      (tceu_vector* vector, usize len, bool grow
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 );

byte* ceu_vector_geti_ex         (tceu_vector* vector, usize idx
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 );

void  ceu_vector_buf_set_ex      (tceu_vector* vector, usize idx, byte* buf, usize nu
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 );

void  ceu_vector_copy_ex         (tceu_vector* dst, usize dst_i, tceu_vector* src, usize src_i, usize n
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 );


/* CEU_NATIVE_PRE */


/* EVENTS_ENUM */

enum {
    /* non-emitable */
    CEU_INPUT__NONE = 0,
    CEU_INPUT__STACKED,
    CEU_INPUT__FINALIZE,
    CEU_INPUT__THROW,
    CEU_INPUT__PAUSE_BLOCK,
    CEU_INPUT__PROPAGATE_CODE,
    CEU_INPUT__PROPAGATE_POOL,

    /* emitable */
    CEU_INPUT__CLEAR,           /* 7 */
    CEU_INPUT__PAUSE,
    CEU_INPUT__RESUME,
    CEU_INPUT__CODE_TERMINATED,
CEU_INPUT__PRIM,
    CEU_INPUT__ASYNC,
    CEU_INPUT__THREAD,
    CEU_INPUT__WCLOCK,

//CEU_INPUT__MIN,
    
//CEU_INPUT__MAX,

CEU_EVENT__MIN,
    CEU_EVENT_LOCK_OK_UNLOCKED,

};

enum {
    CEU_OUTPUT__NONE = 0,
    
};

/* CEU_MAIN */
#include <stdlib.h>     /* realloc, free */
#include <stdio.h>

#ifndef ceu_callback_start
    #define ceu_callback_start(trace)
#endif
#ifndef ceu_callback_step
    #define ceu_callback_step(trace)
#endif
#ifndef ceu_callback_stop
    #define ceu_callback_stop(trace)
#endif
#ifndef ceu_callback_terminating
    #define ceu_callback_terminating(trace)
#endif
#ifndef ceu_callback_thread_terminating
    #define ceu_callback_thread_terminating(trace)
#endif
#ifndef ceu_callback_async_pending
    #define ceu_callback_async_pending(trace)
#endif
#ifndef ceu_callback_isr_enable
    #define ceu_callback_isr_enable(on,trace)
#endif
#ifndef ceu_callback_isr_attach
    #define ceu_callback_isr_attach(on,f,args,trace)
#endif
#ifndef ceu_callback_isr_emit
    #define ceu_callback_isr_emit(a,b,c)
#endif
#ifndef ceu_callback_abort
    #define ceu_callback_abort(err,trace) abort()
#endif
#ifndef ceu_callback_wclock_dt
    #define ceu_callback_wclock_dt(trace) CEU_WCLOCK_INACTIVE
#endif
#ifndef ceu_callback_wclock_min
    #define ceu_callback_wclock_min(dt,trace)
#endif
#ifndef ceu_callback_log_str
    #define ceu_callback_log_str(str,trace) printf("%s", str)
#endif
#ifndef ceu_callback_log_ptr
    #define ceu_callback_log_ptr(ptr,trace) printf("%p", ptr)
#endif
#ifndef ceu_callback_log_num
    #define ceu_callback_log_num(num,trace) printf("%d", num)
#endif
#ifndef ceu_callback_log_flush
    #define ceu_callback_log_flush(trace) fflush(stdout)
#endif

#ifdef CEU_TESTS_REALLOC
    #define ceu_callback_realloc(ptr,size,trace) ceu_main_callback_realloc(ptr,size)
    #define ceu_callback_free(ptr,trace) ceu_main_callback_realloc(ptr,0)
    void* ceu_main_callback_realloc (void* ptr, size_t size) {
        static int _ceu_tests_realloc_ = 0;
        if (size == 0) {
            _ceu_tests_realloc_--;
            free(ptr);
            return NULL;
        } else {
            if (_ceu_tests_realloc_ >= CEU_TESTS_REALLOC) {
                return NULL;
            }
            _ceu_tests_realloc_++;
        }
        return realloc(ptr, size);
    }
#else
#ifndef ceu_callback_realloc
    #define ceu_callback_realloc(ptr,size,trace) realloc(ptr,size)
#endif
#ifndef ceu_callback_free
    #define ceu_callback_free(ptr,trace) free(ptr)
#endif
#endif

int main (int argc, char* argv[]) {
    int ret = ceu_loop(argc, argv);
    return ret;
}


#if 0
#define ceu_callback_log_num(a,b)
#define ceu_callback_log_str(a,b)
#define ceu_callback_log_flush(a)
#define ceu_callback_wclock_min(a,b)
#define ceu_callback_abort(a,b)
#define ceu_callback_terminating(a)
#define ceu_callback_wclock_dt(a) CEU_WCLOCK_INACTIVE
#define ceu_callback_start(a)
#define ceu_callback_stop(a)
#define ceu_callback_step(a)
#define ceu_callback_realloc(a,b,c) NULL
#define ceu_callback_free(a,b)
#endif

//#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

#ifdef ceu_assert_ex
#define ceu_assert(a,b) ceu_assert_ex(a,b,NONE)
#else
#ifdef CEU_FEATURES_TRACE
#define ceu_assert_ex(v,msg,trace)                                  \
    if (!(v)) {                                                     \
        ceu_trace(trace, msg);                                      \
        ceu_callback_abort(0, trace);   \
    }
#define ceu_assert(v,msg) ceu_assert_ex((v),(msg), CEU_TRACE(0))
#else
#define ceu_assert_ex(v,msg,trace)                                  \
    if (!(v)) {                                                     \
        ceu_callback_abort(0, trace);   \
    }
#define ceu_assert(v,msg) ceu_assert_ex((v),(msg),NONE)
#endif
#endif

#ifndef ceu_assert_sys
#define ceu_assert_sys(v,msg)   \
    if (!(v)) {                 \
        ceu_callback_log_str(msg, CEU_TRACE_null);  \
        ceu_callback_abort(0, CEU_TRACE_null);      \
    }
#endif

#ifdef CEU_FEATURES_TRACE
static void ceu_trace (tceu_trace trace, const char* msg) {
    static bool IS_FIRST = 1;
    bool is_first = IS_FIRST;

    IS_FIRST = 0;

    if (trace.up != NULL) {
        ceu_trace(*trace.up, msg);
    }

    if (is_first) {
        IS_FIRST = 1;
        ceu_callback_log_str("\n", CEU_TRACE_null);
    }

    ceu_callback_log_str("[",        CEU_TRACE_null);
    ceu_callback_log_str(trace.file, CEU_TRACE_null);
    ceu_callback_log_str(":",        CEU_TRACE_null);
    ceu_callback_log_num(trace.line, CEU_TRACE_null);
    ceu_callback_log_str("]",        CEU_TRACE_null);
    ceu_callback_log_str(" -> ",     CEU_TRACE_null);

    if (is_first) {
        ceu_callback_log_str("runtime error: ", CEU_TRACE_null);
        ceu_callback_log_str(msg,               CEU_TRACE_null);
        ceu_callback_log_str("\n",              CEU_TRACE_null);
        ceu_callback_log_flush(CEU_TRACE_null);
    }
}
#else
#define ceu_trace(a,b)
#endif

#define CEU_ISRS_N 0

/* CEU_ISRS_DEFINES */


/* EVENTS_DEFINES */


/* CEU_DATAS_HIERS */
typedef s16 tceu_ndata;  /* TODO */
enum {
        CEU_DATA_Exception = 0,
};

tceu_ndata CEU_DATA_SUPERS_Exception [] = {
        0,
};
tceu_ndata CEU_DATA_NUMS_Exception [] = {
        CEU_DATA_Exception,
};


static int ceu_data_is (tceu_ndata* supers, tceu_ndata me, tceu_ndata cmp) {
    return (me==cmp || (me!=0 && ceu_data_is(supers,supers[me],cmp)));
}

#ifdef CEU_FEATURES_TRACE
#define ceu_data_as(a,b,c,d) ceu_data_as_(a,b,c,d)
#else
#define ceu_data_as(a,b,c,d) ceu_data_as_(a,b,c)
#endif

__attribute__((__unused__))
static void* ceu_data_as_ (tceu_ndata* supers, tceu_ndata* me, tceu_ndata cmp
#ifdef CEU_FEATURES_TRACE
                         , tceu_trace trace
#endif
                         )
{
    ceu_assert_ex(ceu_data_is(supers, *me, cmp), "invalid cast `as`", trace);
    return me;
}

#ifdef CEU_FEATURES_TRACE
#define CEU_OPTION_EVT(a,b) CEU_OPTION_EVT_(a,b)
#else
#define CEU_OPTION_EVT(a,b) CEU_OPTION_EVT_(a)
#endif

__attribute__((__unused__))
static tceu_evt* CEU_OPTION_EVT_ (tceu_evt* alias
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 )
{
    ceu_assert_ex(alias != NULL, "value is not set", trace);
    return alias;
}

/* CEU_VECTOR_C */
void ceu_vector_init (tceu_vector* vector, usize max, bool is_ring,
                      bool is_dyn, usize unit, byte* buf) {
    vector->len        = 0;
    vector->max        = max;
    vector->ini        = 0;
    vector->unit       = unit;
    vector->is_dyn     = is_dyn;
    vector->is_ring    = is_ring;
    vector->is_freezed = 0;
    vector->buf        = buf;
}

#ifdef CEU_FEATURES_DYNAMIC
byte* ceu_vector_setmax_ex_      (tceu_vector* vector, usize len, bool freeze
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 )
{
    ceu_assert_ex(vector->is_dyn, "static vector", trace);

    if (vector->max == len) {
        goto END;
    }

    if (len == 0) {
        /* free */
        if (vector->buf != NULL) {
            vector->max = 0;
            ceu_callback_free(vector->buf, trace);
            vector->buf = NULL;
        }
    } else {
        ceu_assert_ex(len > vector->max, "not implemented: shrinking vectors", trace);
        vector->buf = (byte*) ceu_callback_realloc(vector->buf, len*vector->unit, trace);

        if (vector->is_ring && vector->ini>0) {
            /*
             * [X,Y,Z,I,J,K,L,#####,A,B]       -> (grow) ->
             * [X,Y,Z,I,J,K,L,#####,A,B,-,-,-] -> (1st memcpy) ->
             * [?,?,?,I,J,K,L,#####,A,B,X,Y,Z] -> (2nd memmove) ->
             * [I,J,K,L,#####,-,-,-,A,B,X,Y,Z]
             */
            usize dif = len - vector->max;
            usize rig = vector->max - vector->ini;
            if (vector->len > rig) { // if wrap-around then need to shuffle
                usize num = vector->len - rig;
                memcpy(&vector->buf[vector->max * vector->unit], // -,-,-
                       &vector->buf[0],                          // X,Y,Z
                       (num > dif ? dif : num)  * vector->unit); // 3
                if (num > dif) {
                    memmove(&vector->buf[0],                     // X,Y,Z
                            &vector->buf[dif * vector->unit],    // I,J,K,L
                            (num - dif)      * vector->unit);    // rest
                }
            }
        }

        vector->max = len;
    }

    if (freeze) {
        vector->is_freezed = 1;
    }

END:
    return vector->buf;
}
#endif

int   ceu_vector_setlen_could_ex (tceu_vector* vector, usize len, bool grow
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 )
{
    /* must fit w/o growing */
    if (!grow) {
        if (len > vector->len) {
            return 0;
        }
    }

    /* fixed size */
#ifdef CEU_FEATURES_DYNAMIC
    if (!vector->is_dyn || vector->is_freezed)
#endif
    {
        if (len > vector->max) {
            return 0;
        }

    /* variable size */
    }
#ifdef CEU_FEATURES_DYNAMIC
    else
    {
        if (len <= vector->max) {
            /* ok */    /* len already within limits */
        } else {
            /* grow vector */
            if (ceu_vector_setmax_ex(vector,len,0,trace) == NULL) {
                if (len != 0) {
                    return 0;
                }
            }
        }
    }
#endif

    return 1;
}

void  ceu_vector_setlen_ex_      (tceu_vector* vector, usize len, bool grow
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 )
{
    /* must fit w/o growing */
    if (!grow) {
        ceu_assert_ex(len <= vector->len, "access out of bounds", trace);
    }

    /* fixed size */
#ifdef CEU_FEATURES_DYNAMIC
    if (!vector->is_dyn || vector->is_freezed)
#endif
    {
        ceu_assert_ex(len <= vector->max, "access out of bounds", trace);

    /* variable size */
    }
#ifdef CEU_FEATURES_DYNAMIC
    else
    {
        if (len <= vector->max) {
            /* ok */    /* len already within limits */
/* TODO: shrink memory */
        } else {
            /* grow vector */
            if (ceu_vector_setmax_ex(vector,len,0,trace) == NULL) {
                ceu_assert_ex(len==0, "access out of bounds", trace);
            }
        }
    }
#endif

    if (vector->is_ring && len<vector->len) {
        vector->ini = (vector->ini + (vector->len - len)) % vector->max;
    }

    vector->len = len;
}

byte* ceu_vector_geti_ex         (tceu_vector* vector, usize idx
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 )
{
    ceu_assert_ex(idx < vector->len, "access out of bounds", trace);
    return ceu_vector_buf_get(vector, idx);
}

void  ceu_vector_buf_set_ex      (tceu_vector* vector, usize idx, byte* buf, usize nu
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 )
{
    usize n = ((nu % vector->unit) == 0) ? nu/vector->unit : nu/vector->unit+1;
#if 0
    if (vector->len < idx+n) {
        char err[50];
        snprintf(err,50, "access out of bounds : length=%ld, index=%ld", vector->len, idx+n);
        ceu_assert_ex(0, err, file, line);
    }
#else
    ceu_assert_ex((vector->len >= idx+n), "access out of bounds", trace);
#endif

    usize k  = (vector->max - ceu_vector_idx(vector,idx));
    usize ku = k * vector->unit;

    if (vector->is_ring && ku<nu) {
        memcpy(ceu_vector_buf_get(vector,idx),   buf,    ku);
        memcpy(ceu_vector_buf_get(vector,idx+k), buf+ku, nu-ku);
    } else {
        memcpy(ceu_vector_buf_get(vector,idx), buf, nu);
    }
}

void  ceu_vector_copy_ex         (tceu_vector* dst, usize dst_i, tceu_vector* src, usize src_i, usize n
#ifdef CEU_FEATURES_TRACE
                                 , tceu_trace trace
#endif
                                 )
{
    usize unit = dst->unit;
    ceu_assert_ex((src->unit == dst->unit), "incompatible vectors", trace);

    ceu_assert_ex((src->len >= src_i+n), "access out of bounds", trace);
    ceu_vector_setlen_ex(dst, MAX(dst->len,dst_i+n), 1, trace);

    usize dif_src = MIN(n, (src->max - ceu_vector_idx(src,src_i)));
    usize dif_dst = MIN(n, (dst->max - ceu_vector_idx(dst,dst_i)));
    usize dif = MIN(dif_src, dif_dst);

    memcpy(ceu_vector_buf_get(dst,dst_i), ceu_vector_buf_get(src,src_i), dif*unit);

    dst_i += dif;
    src_i += dif;
    n -= dif;

    if (n == 0) {
        return;
    }

    if (dif_src > dif_dst) {
        dif = MIN(n, (src->max - ceu_vector_idx(src,src_i)));
        memcpy(ceu_vector_buf_get(dst,dst_i), ceu_vector_buf_get(src,src_i), dif*unit);
        dst_i += dif;
        src_i += dif;
        n -= dif;
    } else if (dif_dst > dif_src) {
        dif = MIN(n, (dst->max - ceu_vector_idx(dst,src_i)));
        memcpy(ceu_vector_buf_get(dst,dst_i), ceu_vector_buf_get(src,src_i), dif*unit);
        dst_i += dif;
        src_i += dif;
        n -= dif;
    }

    if (n == 0) {
        return;
    }

    memcpy(ceu_vector_buf_get(dst,dst_i), ceu_vector_buf_get(src,src_i), n);
}


#ifdef CEU_FEATURES_POOL

/* CEU_POOL_C */
#include <stdlib.h>     /* NULL */

typedef struct {
    usize   len;
    usize   free;
    usize   index;
    usize   unit;
    byte*   buf;
    byte**  queue; /* NULL on dynamic pools */
} tceu_pool;

void ceu_pool_init (tceu_pool* pool, usize len, usize unit, byte** queue, byte* buf)
{
    usize i;
    pool->len   = len;
    pool->free  = len;
    pool->index = 0;
    pool->unit  = unit;
    pool->queue = queue;
    pool->buf   = buf;
    for (i=0; i<len; i++) {
        queue[i] = &buf[i*unit];
    }
}

byte* ceu_pool_alloc (tceu_pool* pool) {
    byte* ret;

    if (pool->free == 0) {
        return NULL;
    }

    pool->free--;
    ret = pool->queue[pool->index];
    pool->queue[pool->index++] = NULL;
    if (pool->index == pool->len) {
        pool->index = 0;
    }
    return ret;
}

void ceu_pool_free (tceu_pool* pool, byte* val) {
    usize empty = pool->index + pool->free;
    if (empty >= pool->len) {
        empty -= pool->len;
    }
    pool->queue[empty] = val;
    pool->free++;
}


typedef struct tceu_code_mem_dyn {
    struct tceu_code_mem_dyn* prv;
    struct tceu_code_mem_dyn* nxt;
    u8 is_alive: 1;
    tceu_code_mem mem[0];   /* actual tceu_code_mem is in sequence */
} tceu_code_mem_dyn;

typedef struct tceu_pool_pak {
    tceu_pool         pool;
    tceu_code_mem_dyn first;
    tceu_code_mem*    up_mem;
    u8                n_traversing;
} tceu_pool_pak;

#endif

/* CEU_DATAS_MEMS */

#pragma pack(push,1)
typedef struct tceu_data_Lock {
    bool  is_locked;
} tceu_data_Lock;

typedef struct tceu_data_Exception {
    tceu_ndata _enum;
    char*  message;
} tceu_data_Exception;



#pragma pack(pop)

#ifdef CEU_FEATURES_EXCEPTION
typedef struct tceu_opt_Exception {
    bool      is_set;
    tceu_data_Exception value;
} tceu_opt_Exception;

#ifdef CEU_FEATURES_TRACE
#define CEU_OPTION_tceu_opt_Exception(a,b) CEU_OPTION_tceu_opt_Exception_(a,b)
#else
#define CEU_OPTION_tceu_opt_Exception(a,b) CEU_OPTION_tceu_opt_Exception_(a)
#endif

static tceu_opt_Exception* CEU_OPTION_tceu_opt_Exception_ (tceu_opt_Exception* opt
#ifdef CEU_FEATURES_TRACE
                                                          , tceu_trace trace
#endif
                                                          )
{
    ceu_assert_ex(opt->is_set, "value is not set", trace);
    return opt;
}
#endif

/*****************************************************************************/


typedef struct tceu_event___lpar____rpar__ {
} tceu_event___lpar____rpar__;

typedef struct tceu_code_mem_ROOT {
    tceu_code_mem _mem;
    tceu_trl      _trails[1];
    byte          _params[0];
    struct {
int  _RET;
union {
struct {
int  a;
union {
};
};
};
};
} tceu_code_mem_ROOT;



enum {
    CEU_LABEL_NONE = 0,
    CEU_LABEL_ROOT,
CEU_LABEL_Block__CLR_2,
CEU_LABEL_Block__CLR_3,
CEU_LABEL_Block__CLR_4,
CEU_LABEL_Do__OUT_5,
CEU_LABEL_Do__CLR_6,
CEU_LABEL_Block__CLR_7,

};

/*****************************************************************************/

typedef struct tceu_app {
#ifdef CEU_FEATURES_OS
    int    argc;
    char** argv;

    bool end_ok;
    int  end_val;
#endif

    /* ASYNC */
#ifdef CEU_FEATURES_ASYNC
    bool async_pending;
#endif

    /* WCLOCK */
    s32 wclk_late;
    s32 wclk_min_set;
    s32 wclk_min_cmp;

#ifdef CEU_FEATURES_THREAD
    CEU_THREADS_MUTEX_T threads_mutex;
    tceu_threads_data*  threads_head;   /* linked list of threads alive */
    tceu_threads_data** cur_;           /* TODO: HACK_6 "gc" mutable iterator */
#endif

    byte  stack[CEU_STACK_N];
    usize stack_i;

    tceu_code_mem_ROOT root;
} tceu_app;

CEU_API static tceu_app CEU_APP;

/*****************************************************************************/

__attribute__((__unused__))
static tceu_code_mem* ceu_outer (tceu_code_mem* mem, u8 n) {
    for (; mem->depth!=n; mem=mem->up_mem);
    return mem;
}

/*****************************************************************************/

#define CEU_WCLOCK_INACTIVE INT32_MAX

#ifdef CEU_FEATURES_TRACE
#define ceu_wclock(a,b,c,d) ceu_wclock_(a,b,c,d)
#else
#define ceu_wclock(a,b,c,d) ceu_wclock_(a,b,c)
#endif

__attribute__((__unused__))
static int ceu_wclock_ (s32 dt, s32* set, s32* sub
#ifdef CEU_FEATURES_TRACE
                      , tceu_trace trace
#endif
                      )
{
    s32 t;          /* expiring time of track to calculate */
    int ret = 0;    /* if track expired (only for "sub") */

    /* SET */
    if (set != NULL) {
        t = dt - CEU_APP.wclk_late;
        *set = t;

    /* SUB */
    } else {
        t = *sub;
        if ((t > CEU_APP.wclk_min_cmp) || (t > dt)) {
            *sub -= dt;    /* don't expire yet */
            t = *sub;
        } else {
            ret = 1;    /* single "true" return */
        }
    }

    /* didn't awake, but can be the smallest wclk */
    if ( (!ret) && (CEU_APP.wclk_min_set > t) ) {
        CEU_APP.wclk_min_set = t;
        ceu_callback_wclock_min(t, CEU_TRACE_null);
    }

    return ret;
}

__attribute__((__unused__))
static void ceu_params_cpy (tceu_stk* stk, void* params, usize params_n) {
    ceu_assert_sys(CEU_APP.stack_i+params_n < CEU_STACK_N, "stack overflow");
    memcpy(&CEU_APP.stack[CEU_APP.stack_i], params, params_n);
    stk->params   = &CEU_APP.stack[CEU_APP.stack_i];
    stk->params_n = params_n;
    CEU_APP.stack_i += stk->params_n;
}

/*****************************************************************************/

static void ceu_stack_clear (tceu_stk* cur, tceu_code_mem* mem) {
    if (cur == NULL) {
        return;
    }
    if (cur->range.mem == mem) {
        cur->is_alive = 0;
    }
    ceu_stack_clear(cur->prv, mem);
}

#ifdef CEU_FEATURES_POOL
static void ceu_code_mem_dyn_free (tceu_pool* pool, tceu_code_mem_dyn* cur) {
    cur->nxt->prv = cur->prv;
    cur->prv->nxt = cur->nxt;

#ifdef CEU_FEATURES_DYNAMIC
    if (pool->queue == NULL) {
        /* dynamic pool */
        ceu_callback_free(cur, CEU_TRACE_null);
    } else
#endif
    {
        /* static pool */
        ceu_pool_free(pool, (byte*)cur);
    }
}

static void ceu_code_mem_dyn_gc (tceu_pool_pak* pak) {
    if (pak->n_traversing == 0) {
        /* TODO-OPT: one element killing another is unlikely:
                     set bit in pool when this happens and only
                     traverses in this case */
        tceu_code_mem_dyn* cur = pak->first.nxt;
        while (cur != &pak->first) {
            tceu_code_mem_dyn* nxt = cur->nxt;
            if (!cur->is_alive) {
                ceu_code_mem_dyn_free(&pak->pool, cur);
            }
            cur = nxt;
        }
    }
}
#endif

/*****************************************************************************/

#ifdef CEU_FEATURES_LUA
static void ceu_lua_createargtable (lua_State* lua, char** argv, int argc, int script) {
    int i, narg;
    if (script == argc) script = 0;  /* no script name? */
    narg = argc - (script + 1);  /* number of positive indices */
    lua_createtable(lua, narg, script + 1);
    for (i = 0; i < argc; i++) {
        lua_pushstring(lua, argv[i]);
        lua_rawseti(lua, -2, i - script);
    }
    lua_setglobal(lua, "arg");
}

#endif

/*****************************************************************************/

static int ceu_lbl (tceu_nstk _ceu_level, tceu_stk* _ceu_cur, tceu_stk* _ceu_nxt, tceu_code_mem* _ceu_mem, tceu_nlbl _ceu_lbl, tceu_ntrl* _ceu_trlK);











/*****************************************************************************/

#ifdef CEU_FEATURES_EXCEPTION
static int ceu_throw_ex (tceu_catch* catches, tceu_data_Exception* exception, usize len
                  , tceu_nstk level, tceu_stk* nxt
#ifdef CEU_FEATURES_TRACE
                  , tceu_trace trace
#endif
                  )
{
    tceu_catch* cur = catches;
    while (cur != NULL) {
        if (ceu_data_is(CEU_DATA_SUPERS_Exception,exception->_enum,cur->exception->value._enum)) {
            //ceu_sys_assert(!cur->exception->is_set, "double catch");
            ceu_assert_ex(!cur->exception->is_set, "double catch", trace);
            cur->exception->is_set = 1;
            memcpy(&cur->exception->value, exception, len);

#if 0
            /* do not allow nested catches (and itself) to awake */
            cur->exception = NULL;
            while (catches != cur) {
                catches->exception = NULL;
                catches = catches->up;
            }
#endif

            //return ceu_lbl(NULL, stk, cur->mem, cur->trl, cur->mem->_trails[cur->trl].lbl);
            //return ceu_lbl(_ceu_level, _ceu_cur, _ceu_nxt, _ceu_mem, _ceu_lbl, _ceu_trlK)
            cur->mem->_trails[cur->trl].evt.id = CEU_INPUT__STACKED;
            cur->mem->_trails[cur->trl].level = level + 1;
//printf(">>> %d %d\n", cur->trl, cur->mem->_trails[cur->trl].lbl);
            tceu_evt   evt   = {CEU_INPUT__NONE, {NULL}};
            //tceu_range range = { cur->mem, cur->trl, cur->trl };
            tceu_range range = { &CEU_APP.root._mem, 0, CEU_TRAILS_N-1 };
            nxt->evt      = evt;
            nxt->range    = range;
            nxt->params_n = 0;
            return 1;
        }
        cur = cur->up;
    }
    ceu_assert_ex(0, exception->message, trace);
    return 0;
}
#ifdef CEU_FEATURES_TRACE
#define ceu_throw(a,b,c) ceu_throw_ex(a,b,c,_ceu_level,_ceu_nxt,CEU_TRACE(0))
#else
#define ceu_throw(a,b,c) ceu_throw_ex(a,b,c,_ceu_level,_ceu_nxt)
#endif
#endif

#ifdef CEU_FEATURES_THREAD
static int ceu_threads_gc (int force_join) {
    int n_alive = 0;
    CEU_APP.cur_ = &CEU_APP.threads_head;
    tceu_threads_data*  head  = *CEU_APP.cur_;
    while (head != NULL) {
        tceu_threads_data** nxt_ = &head->nxt;
        if (head->has_terminated || head->has_aborted)
        {
            if (!head->has_notified) {
                ceu_input(CEU_INPUT__THREAD, &head->id);
                head->has_notified = 1;
            }

            /* remove from list if rejoined */
            {
                int has_joined;
                if (force_join || head->has_terminated) {
                    CEU_THREADS_JOIN(head->id);
                    has_joined = 1;
                } else {
                    /* possible with "CANCEL" which prevents setting "has_terminated" */
                    has_joined = CEU_THREADS_JOIN_TRY(head->id);
                }
                if (has_joined) {
                    *CEU_APP.cur_ = head->nxt;
                    nxt_ = CEU_APP.cur_;
                    ceu_callback_free(head, CEU_TRACE_null);
                }
            }
        }
        else
        {
            n_alive++;
        }
        CEU_APP.cur_ = nxt_;
        head  = *CEU_APP.cur_;
    }
    return n_alive;
}
#endif

/*****************************************************************************/

#define CEU_GOTO(lbl) {_ceu_lbl=lbl; goto _CEU_LBL_;}

static int ceu_lbl (tceu_nstk _ceu_level, tceu_stk* _ceu_cur, tceu_stk* _ceu_nxt, tceu_code_mem* _ceu_mem, tceu_nlbl _ceu_lbl, tceu_ntrl* _ceu_trlK)
{
#define CEU_TRACE(n) ((tceu_trace){&_ceu_mem->trace,__FILE__,__LINE__+(n)})
#ifdef CEU_STACK_MAX
    {
        static void* base = NULL;
        if (base == NULL) {
            base = &_ceu_level;
        } else {
#if 0
#if 0
//Serial.begin(9600);
Serial.println((usize)base);
Serial.println((usize)&_ceu_lbl);
Serial.print(" lbl "); Serial.println(_ceu_lbl);
//Serial.flush();
    if((usize)(((byte*)base)-CEU_STACK_MAX) <= (usize)(&_ceu_level)) {
    } else {
        delay(1000);
    }
#else
printf(">>> %p / %p / %ld\n", base, &_ceu_lbl, ((u64)base)-((u64)&_ceu_lbl));
printf("%ld %ld %d\n", (usize)(base-CEU_STACK_MAX), (usize)(&_ceu_level),
            ((usize)(base-CEU_STACK_MAX) <= (usize)(&_ceu_level)));
#endif
#endif
            ceu_assert((usize)(((byte*)base)-CEU_STACK_MAX) <= (usize)(&_ceu_level), "stack overflow");
        }
    }
#endif

_CEU_LBL_:
    //printf("-=-=- %d -=-=-\n", _ceu_lbl);
    switch (_ceu_lbl) {
        case CEU_LABEL_NONE:
            break;
        
/* ROOT (n=49, ln=1) */
case CEU_LABEL_ROOT:;

/* Block (n=48, ln=1) */
{

/* Block (n=43, ln=1) */
{

/* Set_Exp (n=3, ln=1) */
((CEU_APP.root.a)) = 8;

/* Block (n=43, ln=1) */
}

/* Do (n=44, ln=1) */
ceu_assert(0, "reached end of `do`");

/* Do (n=44, ln=1) */
case CEU_LABEL_Do__OUT_5:;

/* Block (n=48, ln=1) */
}

    }
    //ceu_assert(0, "unreachable code");
    return 0;
#undef CEU_TRACE
}

#if defined(_CEU_DEBUG)
#define _CEU_DEBUG
static int xxx = 0;
#endif

static void ceu_bcast_mark (tceu_nstk level, tceu_stk* cur)
{
    tceu_ntrl trlK = cur->range.trl0;

    for (; trlK<=cur->range.trlF; trlK++)
    {
        tceu_trl* trl = &cur->range.mem->_trails[trlK];

        //printf(">>> mark [%d/%p] evt=%d\n", trlK, trl, trl->evt.id);
#ifdef CEU_TESTS
        _ceu_tests_trails_visited_++;
#endif
        switch (trl->evt.id)
        {
#ifdef CEU_FEATURES_POOL
            case CEU_INPUT__PROPAGATE_POOL: {
                tceu_code_mem_dyn* v = trl->evt.pak->first.nxt;
                while (v != &trl->evt.pak->first) {
                    tceu_range range_ = { &v->mem[0],
                                          0, (tceu_ntrl)((&v->mem[0])->trails_n-1) };
                    tceu_stk cur_ = *cur;
                    cur_.range = range_;
                    ceu_bcast_mark(level, &cur_);
                    v = v->nxt;
                }
                break;
            }
#endif

#ifdef CEU_FEATURES_PAUSE
            case CEU_INPUT__PAUSE_BLOCK: {
                u8 was_paused = trl->pse_paused;
                if ( (cur->evt.id == trl->pse_evt.id)                               &&
                     (cur->evt.id<CEU_EVENT__MIN || cur->evt.mem==trl->pse_evt.mem) &&
                     (*((u8*)cur->params) != trl->pse_paused) )
                {
                    trl->pse_paused = *((u8*)cur->params);

                    tceu_evt evt_;
                    tceu_range range_ = { cur->range.mem,
                                          (tceu_ntrl)(trlK+1), (tceu_ntrl)(trlK+trl->pse_skip) };
                    if (trl->pse_paused) {
                        evt_.id = CEU_INPUT__PAUSE;
                    } else {
                        CEU_APP.wclk_min_set = 0;   /* maybe resuming a timer, let it be the minimum set */
                        evt_.id = CEU_INPUT__RESUME;
                    }
                    tceu_stk cur_ = { evt_, range_, NULL, 0 };
                    ceu_bcast_mark(level, &cur_);
                }
                /* don't skip if pausing now */
                if (was_paused && cur->evt.id!=CEU_INPUT__CLEAR) {
                                  /* also don't skip on CLEAR (going reverse) */
                    trlK += trl->pse_skip;
                }
                break;
            }
#endif

            case CEU_INPUT__PROPAGATE_CODE: {
#if 0
                // TODO: simple optimization that could be done
                //          - do it also for POOL?
                if (occ->evt.id==CEU_INPUT__CODE_TERMINATED && occ->params==trl->evt.mem ) {
                    // dont propagate when I am terminating
                } else
#endif
                tceu_range range_ = {
                    (tceu_code_mem*)trl->evt.mem,
                    0,
                    (tceu_ntrl)(((tceu_code_mem*)trl->evt.mem)->trails_n-1)
                };
                tceu_stk cur_ = *cur;
                cur_.range = range_;
                ceu_bcast_mark(level, &cur_);
                //break;    (may awake from CODE_TERMINATED)
            }

            default: {
                if (cur->evt.id == CEU_INPUT__CLEAR) {
                    if (trl->evt.id == CEU_INPUT__FINALIZE) {
//printf("AWK %d %d\n", trlK, trl->lbl);
                        goto _CEU_AWAKE_YES_;
                    }
                } else if (cur->evt.id==CEU_INPUT__CODE_TERMINATED && trl->evt.id==CEU_INPUT__PROPAGATE_CODE) {
//printf("TERM %d %d\n", trlK, trl->lbl);
                    if (trl->evt.mem == cur->evt.mem) {
                        goto _CEU_AWAKE_YES_;
                    }
                } else if (trl->evt.id == cur->evt.id) {
#ifdef CEU_FEATURES_PAUSE
                    if (cur->evt.id==CEU_INPUT__PAUSE || cur->evt.id==CEU_INPUT__RESUME) {
                        goto _CEU_AWAKE_YES_;
                    }
#endif
                    if (trl->evt.id>CEU_EVENT__MIN || trl->evt.id==CEU_INPUT__CODE_TERMINATED) {
                        if (trl->evt.mem == cur->evt.mem) {
                            goto _CEU_AWAKE_YES_;   /* internal event matches "mem" */
                        }
                    } else {
                        if (cur->evt.id != CEU_INPUT__NONE) {
                            goto _CEU_AWAKE_YES_;       /* external event matches */
                        }
                    }
                }

                continue;

_CEU_AWAKE_YES_:
                trl->evt.id = CEU_INPUT__STACKED;
                trl->level  = level;
            }
        }
    }
}

static int ceu_bcast_exec (tceu_nstk level, tceu_stk* cur, tceu_stk* nxt)
{
    /* CLEAR: inverse execution order */
    tceu_ntrl trl0 = cur->range.trl0;
    tceu_ntrl trlF = cur->range.trlF;
    if (trl0 > trlF) {
        return 0;
    }
    if (cur->evt.id == CEU_INPUT__CLEAR) {
        tceu_ntrl tmp = trl0;
        trl0 = trlF;
        trlF = tmp;
    }

    tceu_ntrl trlK = trl0;

    //printf(">>> exec %d -> %d\n", trl0, trlF);
    while (1)
    {
        tceu_trl* trl = &cur->range.mem->_trails[trlK];

        //printf(">>> exec [%d/%p] evt=%d\n", trlK, trl, trl->evt.id);
        switch (trl->evt.id)
        {
            case CEU_INPUT__PROPAGATE_CODE: {
#if 0
                // TODO: simple optimization that could be done
                //          - do it also for POOL?
                if (occ->evt.id==CEU_INPUT__CODE_TERMINATED && occ->params==trl->evt.mem ) {
                    // dont propagate when I am terminating
                } else
#endif
                {
                    tceu_range range_ = {
                        (tceu_code_mem*)trl->evt.mem,
                        0,
                        (tceu_ntrl)(((tceu_code_mem*)trl->evt.mem)->trails_n-1)
                    };
                    tceu_stk cur_ = *cur;
                    cur_.range = range_;
                    if (ceu_bcast_exec(level, &cur_, nxt)) {
                        return 1;
                    }
                }
                break;
            }

#ifdef CEU_FEATURES_POOL
            case CEU_INPUT__PROPAGATE_POOL: {
                ceu_assert_ex(trl->evt.pak->n_traversing < 255, "bug found", CEU_TRACE_null);
                trl->evt.pak->n_traversing++;
                tceu_code_mem_dyn* v = trl->evt.pak->first.nxt;
                while (v != &trl->evt.pak->first) {
                    if (v->is_alive) {
                        tceu_range range_ = { &v->mem[0],
                                              0, (tceu_ntrl)((&v->mem[0])->trails_n-1) };
                        tceu_stk cur_ = *cur;
                        cur_.range = range_;
                        if (ceu_bcast_exec(level, &cur_, nxt)) {
                            trl->evt.pak->n_traversing--;
                            return 1;
                        }
                    }
                    v = v->nxt;
                }
                trl->evt.pak->n_traversing--;
                ceu_code_mem_dyn_gc(trl->evt.pak);
                break;
            }
#endif

            case CEU_INPUT__STACKED: {
                if (trl->evt.id==CEU_INPUT__STACKED && trl->level==level) {
                    trl->evt.id = CEU_INPUT__NONE;
//printf("STK = %d\n", trlK);
                    if (ceu_lbl(level, cur, nxt, cur->range.mem, trl->lbl, &trlK)) {
                        return 1;
                    }
//printf("<<< trlK = %d\n", trlK);
                }
                break;
            }
        }

        if (cur->evt.id == CEU_INPUT__CLEAR) {
            trl->evt.id = CEU_INPUT__NONE;
        }

        if (trlK == trlF) {
            break;
        } else if (cur->evt.id == CEU_INPUT__CLEAR) {
            trlK--; trl--;
        } else {
            trlK++; trl++;
        }
    }
    return 0;
}

static void ceu_bcast (tceu_nstk level, tceu_stk* cur)
{
    if (cur->evt.id>CEU_INPUT__PRIM && cur->evt.id<CEU_EVENT__MIN) {
        switch (cur->evt.id) {
            case CEU_INPUT__WCLOCK:
                CEU_APP.wclk_min_cmp = CEU_APP.wclk_min_set;    /* swap "cmp" to last "set" */
                CEU_APP.wclk_min_set = CEU_WCLOCK_INACTIVE;     /* new "set" resets to inactive */
                ceu_callback_wclock_min(CEU_WCLOCK_INACTIVE, CEU_TRACE_null);
                if (CEU_APP.wclk_min_cmp <= *((s32*)cur->params)) {
                    CEU_APP.wclk_late = *((s32*)cur->params) - CEU_APP.wclk_min_cmp;
                }
                break;
#ifdef CEU_FEATURES_ASYNC
            case CEU_INPUT__ASYNC:
                CEU_APP.async_pending = 0;
                break;
#endif
        }
        if (cur->evt.id != CEU_INPUT__WCLOCK) {
            CEU_APP.wclk_late = 0;
        }
    }

    //printf(">>> BCAST[%d]: %d\n", cur->evt.id, level);
    ceu_bcast_mark(level, cur);
    while (1) {
        tceu_stk nxt;
        nxt.is_alive = 1;
        nxt.prv = cur;
        int ret = ceu_bcast_exec(level, cur, &nxt);
        if (ret) {
            ceu_assert_sys(level < 255, "too many stack levels");
            ceu_bcast(level+1, &nxt);
            if (!cur->is_alive) {
                break;
            }
        } else {
            break;
        }
    }

    CEU_APP.stack_i -= cur->params_n;
    //printf("<<< BCAST: %d\n", level);
}

CEU_API void ceu_input (tceu_nevt id, void* params)
{
    s32 dt = ceu_callback_wclock_dt(CEU_TRACE_null);
    if (dt != CEU_WCLOCK_INACTIVE) {
        tceu_evt   evt   = {CEU_INPUT__WCLOCK, {NULL}};
        tceu_range range = {(tceu_code_mem*)&CEU_APP.root, 0, CEU_TRAILS_N-1};
        tceu_stk   cur   = { evt, range, &dt, 0, 1, NULL };
        ceu_bcast(1, &cur);
    }
    if (id != CEU_INPUT__NONE) {
        tceu_evt   evt   = {id, {NULL}};
        tceu_range range = {(tceu_code_mem*)&CEU_APP.root, 0, CEU_TRAILS_N-1};
        tceu_stk   cur   = { evt, range, params, 0, 1, NULL };
        ceu_bcast(1, &cur);
    }
}

CEU_API void ceu_start (int argc, char* argv[]) {
#ifdef CEU_FEATURES_OS
    CEU_APP.argc     = argc;
    CEU_APP.argv     = argv;
    CEU_APP.end_ok   = 0;
#endif

#ifdef CEU_FEATURES_ASYNC
    CEU_APP.async_pending = 0;
#endif

    CEU_APP.wclk_late = 0;
    CEU_APP.wclk_min_set = CEU_WCLOCK_INACTIVE;
    CEU_APP.wclk_min_cmp = CEU_WCLOCK_INACTIVE;

    CEU_APP.root._mem.up_mem   = NULL;
    CEU_APP.root._mem.depth    = 0;

#ifdef CEU_FEATURES_TRACE
    CEU_APP.root._mem.trace.up = NULL;
#endif
#ifdef CEU_FEATURES_EXCEPTION
    CEU_APP.root._mem.catches  = NULL;
#endif
#ifdef CEU_FEATURES_LUA
    CEU_APP.root._mem.lua      = NULL;
#endif

#ifdef CEU_FEATURES_THREAD
    pthread_mutex_init(&CEU_APP.threads_mutex, NULL);
    CEU_APP.threads_head = NULL;

    /* All code run atomically:
     * - the program is always locked as a whole
     * -    thread spawns will unlock => re-lock
     * - but program will still run to completion
     */
    CEU_THREADS_MUTEX_LOCK(&CEU_APP.threads_mutex);
#endif

    CEU_APP.stack_i = 0;

    CEU_APP.root._mem.trails_n = CEU_TRAILS_N;
    memset(&CEU_APP.root._trails, 0, CEU_TRAILS_N*sizeof(tceu_trl));
    CEU_APP.root._trails[0].evt.id = CEU_INPUT__STACKED;
    CEU_APP.root._trails[0].level  = 1;
    CEU_APP.root._trails[0].lbl    = CEU_LABEL_ROOT;

    ceu_callback_start(CEU_TRACE_null);

    tceu_evt   evt   = {CEU_INPUT__NONE, {NULL}};
    tceu_range range = {(tceu_code_mem*)&CEU_APP.root, 0, CEU_TRAILS_N-1};
    tceu_stk   cur   = { evt, range, NULL, 0, 1, NULL };
    ceu_bcast(1, &cur);
}
CEU_API void ceu_stop (void) {
#ifdef CEU_FEATURES_THREAD
    CEU_THREADS_MUTEX_UNLOCK(&CEU_APP.threads_mutex);
    ceu_assert_ex(ceu_threads_gc(1) == 0, "bug found", CEU_TRACE_null); /* wait all terminate/free */
#endif
    ceu_callback_stop(CEU_TRACE_null);
}

/*****************************************************************************/

CEU_API int ceu_loop (int argc, char* argv[])
{
    ceu_start(argc, argv);

#ifdef CEU_FEATURES_OS
    while (!CEU_APP.end_ok)
#else
    while (1)
#endif
    {
        ceu_callback_step(CEU_TRACE_null);
#ifdef CEU_FEATURES_THREAD
        if (CEU_APP.threads_head != NULL) {
            CEU_THREADS_MUTEX_UNLOCK(&CEU_APP.threads_mutex);
/* TODO: remove this!!! */
            CEU_THREADS_SLEEP(100); /* allow threads to do "atomic" and "terminate" */
            CEU_THREADS_MUTEX_LOCK(&CEU_APP.threads_mutex);
            ceu_threads_gc(0);
        }
#endif
#ifdef CEU_FEATURES_ASYNC
        ceu_input(CEU_INPUT__ASYNC, NULL);
#endif
    }

#ifdef CEU_FEATURES_OS
    ceu_stop();

#ifdef CEU_TESTS
    printf("_ceu_tests_bcasts_ = %d\n", _ceu_tests_bcasts_);
    printf("_ceu_tests_trails_visited_ = %d\n", _ceu_tests_trails_visited_);
    fflush(stdout);
#endif

    return CEU_APP.end_val;
#else
    return 0;
#endif
}
