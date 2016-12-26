#ifndef MY_HEAP
#define MY_HEAP
 
typedef struct {
    int priority; /*indicates priority of this node*/
    int data; /*The data this node contains, which in our case is index in Labyrinth*/
} node_t;
 
typedef struct {
    node_t *nodes;
    int len; /*The actual number of node of heap*/
    int size; /*The size of the nodes we have ready in hand*/
} p_queue_t;
 
void push (p_queue_t *h, int priority, int data);

 
int pop (p_queue_t *h) ;
 
#endif