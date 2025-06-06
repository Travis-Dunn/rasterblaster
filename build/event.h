#ifndef EVENT_H
#define EVENT_H

typedef enum { /* just one for now, there will be more */
    EVT_LBUTTONDOWN = 0,
    EVT_KEYDOWN
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
