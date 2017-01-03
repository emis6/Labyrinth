#include <stdio.h>
#include <stdlib.h>
#include "my_heap.h"

/*Implementation of a min heap
*The elements are stored in this way in the array:
* if parent is in "i"th element of the array: right child in "i/2"th element, left child in "i/2 +1"th element of array
*/

void push (p_queue_t *h, int priority, int data) 
{
    printf("push: size: %d\n", h->size);

    if (h->len + 1 >= h->size) /*We check if we're running out of capacity we double it*/
    {
        printf("WTF\n");
        printf("size: %d\n", h->size);

        h->size = h->size ? (h->size +10) : 4;/*If it's the first time pushing an element the heap is initialized with capacity = 4*/
        
        h->nodes = my_realloc(h->nodes, (h->size -30)* sizeof (node_t),h->size * sizeof (node_t));
    }

    int i = h->len + 1;
    int j = i / 2;
    while (i > 1 && h->nodes[j].priority > priority) /*We check if the priority of the given element is less than where we want to put it it should go up*/
    {
        h->nodes[i] = h->nodes[j];
        i = j;
        j = j / 2; /*We check the parent in an upper level*/
    }
    h->nodes[i].priority = priority;
    h->nodes[i].data = data;
    h->len++;
}
 
int pop (p_queue_t *h) 
{
    int i, j, k;
    if (!h->len) 
    {
        return -1;
    }
    int data = h->nodes[1].data;
    h->nodes[1] = h->nodes[h->len];
    h->len--;
    i = 1;
    while (1) 
    {
        k = i;
        j = 2 * i;
        if (j <= h->len && h->nodes[j].priority < h->nodes[k].priority) 
        {
            k = j;
        }
        if (j + 1 <= h->len && h->nodes[j + 1].priority < h->nodes[k].priority) 
        {
            k = j + 1;
        }
        if (k == i) 
        {
            break;
        }
        h->nodes[i] = h->nodes[k];
        i = k;
    }
    h->nodes[i] = h->nodes[h->len + 1];
    return data;
}

node_t* my_realloc(node_t *nodes, int oldSize,int newSize)
{
    int i =0 ;
    node_t* ret = (node_t*) malloc(newSize *(sizeof(node_t) ));
    printf("after malloc\n");

    for(i = 0; i <oldSize; i++)
    {
        printf("in cpy for: %d \n", i);
        ret[i] = nodes[i];
    }

    return ret;
}
 
