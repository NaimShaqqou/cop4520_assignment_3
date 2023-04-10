#include "LazyList.h"

#define N 4
#define NumGifts 500000

atomic<int> addPos{0};
atomic<int> remPos{0};

atomic<int> counter;

int main()
{

    srand((unsigned)time(NULL));

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
            if (op == 1)
            {
                // can't remove gifts we haven't added yet
                if (remPos == addPos)
                    continue;

                int tag = arr[remPos++];

                linkedList.remove(tag);

                // add gift to linkedlist in next iteration
                op ^= 1;
                continue;
            }

            // add
            if (op == 0)
            {
                // we added all the gifts to the linked list
                if (addPos == NumGifts)
                    continue;

                int tag = arr[addPos++];

                // remove gift from linked list in next iteration
                op ^= 1;
                continue;
            }
        }
    };

    vector<thread> threads;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++)
        threads.emplace_back(servant, i);

    for (int i = 0; i < N; i++)
        threads[i].join();

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    cout << "All " << NumGifts << " thank you notes have been sent out.\n";
    cout << "Runtime: " << duration.count() / 1e+3 << " seconds" << endl;

    return 0;
}