#include "times_queue.h"

#include "more_memory.h"
#include "more_time.h"

struct node {
    struct timeval time;
    size_t bytes;
    struct node *next;
};

struct TimesQueue_T {
    struct node *head;
    struct node *tail;
    size_t curr_bytes;
    size_t byte_limit;
    struct timeval time_limit;
};

TimesQueue_T TimesQueue_new(size_t byte_limit, struct timeval *time_limit) {
    TimesQueue_T queue;
    queue = checked_malloc(sizeof(*queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->curr_bytes = 0;
    queue->byte_limit = byte_limit;
    queue->time_limit = *time_limit;
    return queue;
}

static struct node *make_node(struct timeval *currtime, size_t bytes) {
    struct node *node;
    node = checked_malloc(sizeof(*node));
    node->next = NULL;
    node->time = *currtime;
    node->bytes = bytes;
    return node;
}

// Pre-condition: head is not NULL
static void pop_head(TimesQueue_T queue) {
    queue->curr_bytes -= queue->head->bytes;
    struct node *next = queue->head->next;
    free(queue->head);
    queue->head = next;
    if (next == NULL) {
        queue->tail = NULL;
    }
}

static void push(TimesQueue_T queue, struct timeval *currtime, size_t bytes) {
    struct node *node = make_node(currtime, bytes);
    if (queue->tail == NULL) {
        queue->head = node;
    } else {
        queue->tail->next = node;
    }
    queue->tail = node;
    queue->curr_bytes += bytes;
}

int TimesQueue_push(TimesQueue_T queue, size_t bytes) {
    if (queue->time_limit.tv_sec == 0 && queue->time_limit.tv_usec == 0) {
        return 1;
    }

    struct timeval currtime = get_current_time();
    int too_old = 1;
    while (queue->head != NULL && too_old) {
        struct timeval diff = subtract_times(&currtime, &(queue->head->time));
        too_old = compare_times(&diff, &(queue->time_limit)) > 0;
        if (too_old) {
            pop_head(queue);
        }
    }
    if (queue->curr_bytes + bytes > queue->byte_limit) {
        return 0;
    } else {
        push(queue, &currtime, bytes);
        return 1;
    }
}

void TimesQueue_free(TimesQueue_T *queue_p) {
    if (queue_p == NULL || *queue_p == NULL) {
        return;
    }

    struct node *curr = (*queue_p)->head;
    while (curr != NULL) {
        struct node *next = curr->next;
        free(curr);
        curr = next;
    }

    free(*queue_p);
    *queue_p = NULL;
}