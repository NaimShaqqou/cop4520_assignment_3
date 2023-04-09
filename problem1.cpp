#include <iostream>
#include <mutex>

using namespace std;

#define N 4

struct gift
{
    int tag;
};

struct node
{
    int val;
    int tag;
    
    // this locks the node pointed to
    // by the next pointer
    mutex next_mutex;
    struct node *next;

    // to use the lazy synchronization protocol
    bool marked = false;
};

struct linked_list
{
    node *head, *tail;
    int size;
};

linked_list *ll;

bool validate(node *prev, node *cur)
{
    return !prev->marked && !cur->marked && prev->next == cur;
}

bool remove(gift item)
{
    // get key of current item
    int tag = item.tag;

    // navigate to proper node. Get pred and cur node
    node *pred = ll->head;
    node *cur = ll->head->next;
    while (cur->tag < tag)
    {
        pred = cur;
        cur = cur->next;
    }

    // lock both pred and cur nodes
    pred->next_mutex.lock();
    cur->next_mutex.lock();

    // "validate" the node to delete
    if (validate(pred, cur))
    {
        if (cur->tag == tag)
        {
            // mark the node to remove
            cur->marked = true;

            // physically remove the node
            pred->next = cur->next;
        }
        else
        {
            return false;
        }
    }

    // unlcok both pred and cur nodes
    pred->next_mutex.unlock();
    cur->next_mutex.unlock();
}

bool contains(gift item)
{
    return false;
}

int main()
{
    return 0;
}