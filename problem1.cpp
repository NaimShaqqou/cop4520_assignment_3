#include "LazyList.h"

#define N 4
#define NumGifts 200

atomic<int> addPos;
atomic<int> remPos;

atomic <int> counter;

int main()
{

    srand((unsigned) time(NULL));

    // randomly ordrered array that contains the gifts
    vector<int> arr(NumGifts);
    for (int i = 0; i < NumGifts; i++)
    {
        arr[i] = i + 1;
    }

    // shuffle the array
    for (int i = NumGifts - 1; i >= 0; i--)
    {
        int j = rand() % (i + 1);
        swap(arr[i], arr[j]);
    }

    // instantiate the ordered linkedlist
    LazyList linkedList = LazyList();

    auto servant = [&](int start)
    {
        // op = 0 => add
        // op = 1 => remove
        int op = 0;

        while (remPos < NumGifts)
        {
            // write thank you note and remove
            if (op)
            {
                // can't remove gifts we haven't added yet
                if (remPos >= addPos) continue;

                int tag = arr[remPos++];
                linkedList.remove(tag);

                // add gift to linkedlist in next iteration
                op ^= 1;
            }
            // add
            else
            {
                // we added all the gifts to the linked list
                if (addPos >= NumGifts) continue;

                int tag = arr[addPos++];
                linkedList.add(tag);
                counter++;

                // remove gift from linked list in next iteration
                op ^= 1;
            }
        }
    };

    vector<thread> threads;

    // the first guest is the one that can decide
    // whether everyone has seen the cupcake
    for (int i = 0; i < N; i++)
        threads.emplace_back(servant, i);

    for(int i = 5; i <= NumGifts; i += 5) {
        while(counter < i);
        cout << ((i * 100) / NumGifts) << "%" << endl;
    }

    for (int i = 0; i < N; i++)
        threads[i].join();

    return 0;
}