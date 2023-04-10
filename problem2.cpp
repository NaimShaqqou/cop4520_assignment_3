#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <map>

#define N 8
#define MIN_TEMP -70
#define MAX_TEMP 100

using namespace std;

map<mt19937, uniform_int_distribution<int>> sensors;

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

        sensors[mt] = unii;
    }

    std::map<std::string, int>::iterator it = sensors.begin();
 
  // Iterate through the sensors and print the elements
    while (it != sensors.end())
    {
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
        ++it;
    }
    return 0;
}