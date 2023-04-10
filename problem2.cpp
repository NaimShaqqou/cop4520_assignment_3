#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <map>
#include <queue>
#include <iomanip>

#define N 8
#define MIN_TEMP -70
#define MAX_TEMP 100

using namespace std;
using namespace std::chrono;

vector<mt19937> sensors;
vector<uniform_int_distribution<int>> dists;

atomic<bool> beginSimulation{false};
int curTemps[N];
bool tempReadComplete[N];

// there are 6 ten minute intervals in an hour
int maxDifferenceIn10MinuteIntervals[6];

// to get the 5 max & 5 min values
priority_queue<int> maxTemps;
priority_queue<int, vector<int>, greater<int>> minTemps;

/*
 * Assignment requirements
 * - read temperature every 1 minute
 * - 5 lowest temperatures
 * - 5 highest temperatures
 * - 10 minute time interval when the largest temperature difference was observed
*/
int main()
{
    srand((unsigned)time(NULL));

    // create rng's for the different sensor (using mt19937 & uniform_int_distribution to make unique rng for each sensor)
    // reference: https://gist.github.com/jorgbrown/252e6070d905e358ab1ee9be62f5f07e
    random_device rd;
    for (int s = 0; s < N; s++)
    {
        mt19937 mt(rd());
        uniform_int_distribution<int> unii(MIN_TEMP, MAX_TEMP);

        sensors.push_back(mt);
        dists.push_back(unii);
    }

    auto simulateSensor = [&](int index)
    {
        while(!beginSimulation) continue;
        
        while (beginSimulation)
        {
            // take temp every minute

            // wait 1 minute
            this_thread::sleep_for(milliseconds(100));

            // take temp and store it
            curTemps[index] = dists[index](sensors[index]);
            tempReadComplete[index] = true;
        }
    };

    vector<thread> threads;
    for (int i = 0; i < N; i++)
        threads.emplace_back(simulateSensor, i);

    beginSimulation = true;

    // we will run the simulation for 2 hours for simplicity's sake
    for (int m = 1; m <= 120; m++)
    {
        // we wait till all the sensors have read the temps for the minute
        int cnt;
        do 
        {
            cnt = 0;
            for (int s = 0; s < N; s++)
            {
                if (tempReadComplete[s] == true)
                    cnt++;
            }
        } while (cnt < N);

        // find the minimum and maximum temp for this minute
        int curMin = numeric_limits<int>::max();
        int curMax = numeric_limits<int>::min();
        for (int s = 0; s < N; s++)
        {
            curMin = min(curTemps[s], curMin);
            curMax = max(curTemps[s], curMax);
        }

        // insert those in the priority queues for the max/min for the hour.
        // they will be sorted automatically
        maxTemps.push(curMax);
        minTemps.push(curMin);

        // compare those min/maxes to the min/maxes of this 10 minute interval
        int curInterval = (m / 10) % 6;
        maxDifferenceIn10MinuteIntervals[curInterval] = max(curMax - curMin, maxDifferenceIn10MinuteIntervals[curInterval]);

        // time to print out the hourly report
        if (m % 60 == 0)
        {
            cout << "======== Hourly Report ========\n";

            cout << "Max 5 Temps" << setw(19) << "Min 5 Temps" << endl;
            for (int i = 0; i < 5; i++)
            {

                cout << setw(6) << maxTemps.top() << setw(19) << minTemps.top() << endl;

                maxTemps.pop();
                minTemps.pop();
            }

            // get the max/min priority queues ready 
            // for the next hour
            while (!maxTemps.empty())
                maxTemps.pop();

            while (!minTemps.empty())
                minTemps.pop();

            int maxInterval = 0;
            int maxDifference = 0;
            for (int i = 0; i < 6; i++)
            {
                if (maxDifferenceIn10MinuteIntervals[i] > maxDifference)
                {
                    maxDifference = maxDifferenceIn10MinuteIntervals[i];
                    maxInterval = i;
                }
            }
            cout << endl << "10 Minute Interval of Largest Temperature Difference" << setw(19) << "Difference" << endl;
            cout << setw(16) << maxInterval * 10 << " mins - " << (maxInterval + 1) * 10 << " mins" << setw(36) << maxDifference << endl << endl;
        }

        // get the sensors ready to get another reading
        for (int s = 0; s < N; s++)
        {
            tempReadComplete[s] = false;
        }
    }

    // end the simulation
    beginSimulation = false;

    for (int i = 0; i < N; i++)
        threads[i].join();

    return 0;
}