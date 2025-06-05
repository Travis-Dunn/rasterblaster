#include "event.h"
#include "stdlib.h"
#include "assert.h"

struct EventQueue {
    int head, tail, cap;
    Event* buf;
};

int EventQueueInit(EventQueue** qq, int cap){
    assert(cap > 0 && !(cap & (cap - 1)));
    EventQueue* q = *qq;
    q = calloc(1, sizeof(*q));
    if (!q){
        /* TODO: handle this - it is a fatal error */
        return 1;
    }
    q->buf = malloc(sizeof *q->buf * cap);
    if (!q->buf){
        /* TODO: handle this - it is a fatal error */
        free(q);
        q = 0;
        return 1;
    }
    q->cap = cap;
    *qq = q;
    return q->head = q->tail = 0;
}

int EventQueueFree(EventQueue** qq){
    EventQueue* q = *qq;
    if (!q) return 1; /* TODO: still want to log this or something */
    free(q->buf);
    free(q);
    *qq = 0;
    return 0;
}

static inline int next(int i, int cap){
    return ((i + 1) & (cap - 1));
}

int EventEnqueue(EventQueue* q, Event* e){
    assert(q && q->buf);
    int n = next(q->head, q->cap);
    /* TODO: definitely want to log this */
    if (n == q->tail) return -1;
    q->buf[q->head] = *e;
    q->head = n;
    return 0;
}

int EventDequeue(EventQueue* q, Event* out){
    assert(q && q->buf);
    /* Under normal conditions, this is the only acceptable non-0 return */
    if(q->head == q->tail) return -1;
    *out = q->buf[q->tail];
    q->tail = next(q->tail, q->cap);
    return 0;
}
