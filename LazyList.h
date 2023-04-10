#include <iostream>
#include <mutex>
#include <unordered_set>
#include <thread>
#include <vector>

using namespace std;

struct node 
{
    // the mutex is to lock the current node
    mutex curNode;
    int tag;

    // node gets marked when it's deleted
    bool marked;
    node *next;

    node(int tag, node *next = nullptr, bool marked = false): tag(tag), next(next), marked(marked) {}
};

class LazyList 
{
    private:
        node *head;

    public:
        // the add function only works when there are at least 2 nodes in the list
        // otherwise we run into null exceptions
        LazyList()
        {
            head = new node(numeric_limits<int>::min());
            head->next = new node(numeric_limits<int>::max());
        }

        ~LazyList()
        {
            if (head) delete head;
            if (head->next) delete head->next;
        }

        
        bool validate(node *prev, node *cur)
        {
            return !prev->marked && !cur->marked && prev->next == cur;
        }        

        bool add(int tag)
        {
            bool ret = false;
            while (true)
            {
                // traverse to the place where we want to add the node
                node *prev = head;
                node *cur = head->next;
                while (cur->tag < tag)
                {
                    prev = cur;
                    cur = cur->next;
                }

                prev->curNode.lock();
                cur->curNode.lock();

                if (validate(prev, cur))
                {
                    if (cur->tag == tag)
                    {
                        ret = false;
                    }
                    else
                    {
                        node *newNode = new node(tag);
                        newNode->next = cur;
                        prev->next = newNode;

                        ret = true;
                    }

                    prev->curNode.unlock();
                    cur->curNode.unlock();

                    return ret;
                }

                prev->curNode.unlock();
                cur->curNode.unlock();
            }
        }

        bool remove(int tag)
        {
            while (true)
            {
                node *prev = head;
                node *cur = head->next;

                while (cur->tag < tag)
                {
                    prev = cur;
                    cur = cur->next;
                }

                // reached the end of the list
                if (cur->tag == numeric_limits<int>::max())
                    return false;

                if (prev->curNode.try_lock() && cur->curNode.try_lock())
                {
                    if (validate(prev, cur))
                    {
                        if (cur->tag == tag)
                        {
                            // mark node as deleted
                            cur->marked = true;

                            // actually delete the node
                            prev->next = cur->next;

                            // unlcok both pred and cur nodes
                            prev->curNode.unlock();
                            cur->curNode.unlock();
                            delete cur;
                            return true;
                        }
                        else
                        {
                            // unlcok both pred and cur nodes
                            prev->curNode.unlock();
                            cur->curNode.unlock();

                            return false;
                        }
                    }
                }

                // unlcok both pred and cur nodes
                prev->curNode.unlock();
                cur->curNode.unlock();
            }
        }

        bool contains(int tag)
        {
            node *cur = head;

            while(cur->tag < tag)
            {
                cur = cur->next;
            }

            // make sure the tags match and the node is not removed
            return cur->tag == tag && !cur->marked;
        }

        void print()
        {
            node *cur = head;
            while (cur != nullptr)
            {
                cout << cur->tag << " => marked : " << cur->marked << endl;
                cur = cur->next;
            }
        }
};