# COP 4520 Assignment 3

## Problem 1

### How to run
Use the following commands to run the program:

```
g++ -std=c++2a -pthread problem1.cpp
./a.out
```

Note: Windows Powershell users, the command is `.\a.exe`

### Approach

For this simulation, the unordered gift bag is pretty much an array of gifts that is randomly shuffled.

To satisfy the minotaur's request, each servant alternates between two tasks:
1. Take a gift out of the unordered bag and place it into the ordered list of gifts 
2. Take a gift out of the ordered list of gifts, and write a thank you note

The concurrent linked list follows the lazy synchronization protocol. This protocol allows the servants to navigate the list without needing to acquire locks for each node they traverse. This makes it more efficient since multiple servants can navigate the linked list at the same time without worrying about the nodes being changed/removed.

The servants keep working until all the gifts have been added then removed from the linked list.

### Solution Analysis

#### Code

In the solution that was implemented for this problem. Each thread signifies a servant. The solution leverages the use of `mutex` objects provided by the c++ standard library. Whenever a servant needs to access a _critical section_ of the code, it locks the mutex then unlocks it when it is finished.

In our case, each node of the concurrent linked list was a critical section of the code. This is because more than one thread might modify a node at the same time, which could result in inaccurate results or an infinite loop (or other undefined behavior). We used the `mutex` to ensure that the threads have **mutually exclusive** access to each node.

Another safety mechanism is the lazy synchronization, in which nodes are deleted in a two-step manner. First step is to mark the node as deleted (to let other threads know that it's deleted), and the second step is to physically delete the node from the list.

#### Runtime

I tested the runtime of the program taking the average runtime over `1000` trials. Below is the runtime result of the program:

```
4 threads, 500k gifts: less than 0.086 seconds
```

## Problem 2

### How to run
Use the following commands to run the program:

```
g++ -std=c++2a -pthread problem2.cpp
./a.out
```

Note: Windows Powershell users, the command is `.\a.exe`

### Approach

In this simulation:
1. Each sensor is its own thread
2. The sensors take a reading every 100 milliseconds (because doing the simulation in real time would take forever)
3. The simulation runs for 2 hours, generating 2 hourly reports. Though, we could run this program indefinitely without any issues.

Each minute:
- We get the max and min temp readings and push those to a max and a min priority queue, respectively.
- Based on those max and min temp readings, we can see if they are the biggest temp difference in the current 10 minute interval.

Each hour:
- We print out the 5 max temp readings and the 5 min temp readings.
- We print out wich 10 minute interval that had the max temp change (along with the actual change in temp).
- We clear out the priority queues to prepare for next hour's data.

### Solution Analysis

#### Code

In the solution that was implemented for this problem. Each thread signifies a sensor. Each sensor stores its readings in its own separate index of an array. Therefore, there would be NO critical section in the code where multiple threads are trying to access the same thing.

The sensors are off while the program is doing its calculations, so there is no chance of a race condition. The sensors cannot modify anything while they are sleeping.

We simulate time passing by using a for loop where every iteration is a minute. An iteration does not finish unless we get readings from all of the sensors and then do the calculations for those readings

#### Runtime

Program runtime is irrelevant since it is supposed to be running for the duration of the mission. However, since the sensors read the temperature each minute, the calculations that happen between the readings MUST take less than a minute.

Based on the methods and data structures used, it should not take more than a few milliseconds to run the calculations between the calculations. We can, in fact, do the calculations every 100 milliseconds and still be fine according to this simulation.
