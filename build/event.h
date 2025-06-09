#ifndef EVENT_H
#define EVENT_H

typedef enum { /* just one for now, there will be more */
    EVT_NONE = 0,
    EVT_LBUTTONDOWN,
    EVT_KEYDOWN,
    EVT_KEYUP,
    EVT_COUNT
} EventType;

typedef struct {
    EventType type;
    int buf[2];
} Event;

typedef struct EventQueue EventQueue;

int EventQueueInit      (EventQueue** qq, int cap);
int EventQueueFree      (EventQueue** qq);
int EventEnqueue        (EventQueue* q, Event* e);
int EventDequeue        (EventQueue* q, Event* e);
int EventQueueNotEmpty  (EventQueue* q);

#endif /* EVENT_H */
