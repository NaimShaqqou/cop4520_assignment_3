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
    node *head = nullptr;
    int size = 0;
};

linked_list *ll = new linked_list;

bool validate(node *prev, node *cur)
{
    if (cur == nullptr)
        return !prev->marked && prev->next == cur;

    return !prev->marked && !cur->marked && prev->next == cur;
}

bool add(gift item)
{
    int tag = item.tag;
    cout << "gift tag = " << tag << "\n";

    while (true)
    {
        if (ll->size == 0)
        {
            // create node and add to the list
            node *newNode = new node;
            newNode->tag = tag;

            ll->head = newNode;
            ll->head->next = nullptr;
            ll->size++;

            return true;
        }

        if (ll->size == 1)
        {
            node *newNode = new node;
            newNode->tag = tag;

            if (ll->head->tag > tag)
            {
                newNode->next = ll->head;
                ll->head = newNode;
            }
            else
            {
                ll->head->next = newNode;
                newNode->next = nullptr;
            }

            ll->size++;
            return true;
        }


        node *prev = ll->head;
        node *cur = ll->head->next;
        while (cur != nullptr && cur->tag < tag)
        {
            prev = cur;
            cur = cur->next;
        }

        cout << "found place to add to:\n" << "prev: " << prev->tag << "\n";

        // lock both pred and cur nodes
        prev->next_mutex.lock();

        if (cur != nullptr)
            cur->next_mutex.lock();

        cout << "acquired locks\n";

        if (validate(prev, cur))
        {
            cout << "validated\n";

            // if the gift is already in the list
            if (cur != nullptr && cur->tag == tag)
            {
                prev->next_mutex.unlock();
                cur->next_mutex.unlock();
                return false;
            }
            // create node and add to the list
            node *newNode = new node;
            newNode->tag = tag;

            newNode->next = cur;
            prev->next = newNode;

            ll->size++;
            cout << "added node\n";

            // unlock both pred and cur nodes
            prev->next_mutex.unlock();
            if (cur != nullptr)
                    cur->next_mutex.unlock();
            
            return true;
        }

    }
}

bool remove(gift item)
{
    if (ll->size == 0) return false;

    // get key of current item
    int tag = item.tag;

    while (true)
    {
        // navigate to proper node. Get pred and cur node
        node *pred = ll->head;
        node *cur = ll->head->next;

        if (pred->tag == tag)
        {
            cur = pred;
            pred = nullptr;

            cur->marked = true;
            ll->head = ll->head->next;
            return true;
        }
        else
        {
            while (cur->tag < tag)
            {
                pred = cur;
                cur = cur->next;
            }
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

                ll->size--;

                if (ll->size == 0)
                    ll->head = nullptr;

                return true;
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
}

bool contains(gift item)
{
    int tag = item.tag;
    node *cur = ll->head;

    while(cur->tag < tag)
    {
        cur = cur->next;
    }

    // make sure the tags match and the node is not removed
    return cur->tag == tag && !cur->marked;
}

int main()
{
    for (int i = 0; i < 5; i++)
    {
        gift g;
        g.tag = i;
        add(g);
    }

    gift g;
    g.tag = 0;
    remove(g);

    node *temp = ll->head;
    for (int i = 0; i < ll->size; i++)
    {
        cout << temp->tag << "\n";
        temp = temp->next;
    }

    return 0;
}