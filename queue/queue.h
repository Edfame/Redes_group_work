#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define QUEUESIZE 10
#define COLUMNSIZE 256
#define GARBAGE ""
#define DELIMITER ";"
#define END "\0"

typedef struct queue
{
	char queued[QUEUESIZE][COLUMNSIZE];
	short head;
	short tail;
	short size;
}queue;

struct queue *new_queue();
void queue_destroy(queue *queue);
bool queue_insert(queue *queue, char *val);
bool queue_remove(queue *queue);
char *queue_get_tail(queue *queue);
bool queue_is_full(queue *queue);

void queue_to_array(queue *queue, char *to_return);
