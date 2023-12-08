#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"

#define THREAD_SLEEP_TIME 0.5
#define MAX_THREADS_COUNT 100

class Buffer
{
private:
    std::vector<int> values;

    void print(std::string name)
    {
        std::cout << "### " << name << " " << values.size() << "[";
        for (auto v : values)
            std::cout << v << ",";
        std::cout << "] ###\n";
    }

public:
    Buffer(std::vector<int> initialVals = {}) : values(initialVals)
    {
    }

    void put(int value, std::string debugMsg = "")
    {
        values.push_back(value);
        print("PUT (" + debugMsg + ") ->");
    }

    int get(std::string debugMsg = "")
    {
        int v = values.front();
        values.erase(values.begin());
        print("GET (" + debugMsg + ") ->");
        return v;
    }

    int readFirst()
    {
        return values.front();
    }

    int countEven()
    {
        int count = 0;
        for (auto v : values)
            if (v % 2 == 0)
                ++count;
        return count;
    }

    int countOdd()
    {
        int count = 0;
        for (auto v : values)
            if (v % 2 == 1)
                ++count;
        return count;
    }
};

Buffer buffer;
Semaphore mutex(1);
Semaphore prodEvenSem(0);
Semaphore prodOddSem(0);
Semaphore consEvenSem(0);
Semaphore consOddSem(0);

unsigned int numOfProdEvenWaiting=0, numOfProdOddWaiting=0, numOfConsEvenWaiting=0, numOfConsOddWaiting=0;


bool canProdEven()
{
    return buffer.countEven() < 10;
}

bool canProdOdd()
{
    return buffer.countEven() > buffer.countOdd();
}

bool canConsEven()
{
    int totalCount = buffer.countEven() + buffer.countOdd();
    return totalCount >= 3 && buffer.readFirst() % 2 == 0;
}

bool canConsOdd()
{
    int totalCount = buffer.countEven() + buffer.countOdd();
    return totalCount >= 7 && buffer.readFirst() % 2 == 1;
}

void* threadProdA1(void* arg)
{
    int last = 0;
    while (true)
    {
        mutex.p();
        if(!canProdEven())
        {
            numOfProdEvenWaiting++;
            mutex.v();
            prodEvenSem.p();
            numOfProdEvenWaiting--;
        }

        buffer.put(last, "A1 " + std::to_string(last));
        last = (last + 2) % 50;

        if (numOfProdOddWaiting > 0 && canProdOdd())
            prodOddSem.v();
        else if (numOfConsEvenWaiting > 0 && canConsEven())
            consEvenSem.v();
        else if (numOfConsOddWaiting > 0 && canConsOdd())
            consOddSem.v();
        else
            mutex.v();

        sleep(THREAD_SLEEP_TIME);
    }

    return NULL;
}

void* threadProdA2(void* arg)
{
    int last = 1;
    while (true)
    {
        mutex.p();
        if(!canProdOdd())
        {
            numOfProdOddWaiting++;
            mutex.v();
            prodOddSem.p();
            numOfProdOddWaiting--;
        }

        buffer.put(last, "A2 " + std::to_string(last));
        last = (last + 2) % 50;

        if (numOfProdEvenWaiting > 0 && canProdEven())
            prodEvenSem.v();
        else if (numOfConsEvenWaiting > 0 && canConsEven())
            consEvenSem.v();
        else if (numOfConsOddWaiting > 0 && canConsOdd())
            consOddSem.v();
        else
            mutex.v();

        sleep(THREAD_SLEEP_TIME);
    }

    return NULL;
}

void* threadConsB1(void* arg)
{
    while (true)
    {
        mutex.p();
        if(!canConsEven())
        {
            numOfConsEvenWaiting++;
            mutex.v();
            consEvenSem.p();
            numOfConsEvenWaiting--;
        }

        buffer.get("B1");

        if (numOfProdEvenWaiting > 0 && canProdEven())
            prodEvenSem.v();
        else if (numOfProdOddWaiting > 0 && canProdOdd())
            prodOddSem.v();
        else if (numOfConsOddWaiting > 0 && canConsOdd())
            consOddSem.v();
        else
            mutex.v();

        sleep(THREAD_SLEEP_TIME);
    }

    return NULL;
}

void* threadConsB2(void* arg)
{
    while (true)
    {
        mutex.p();
        if(!canConsOdd())
        {
            numOfConsOddWaiting++;
            mutex.v();
            consOddSem.p();
            numOfConsOddWaiting--;
        }

        buffer.get("B2");

        if (numOfProdEvenWaiting > 0 && canProdEven())
            prodEvenSem.v();
        else if (numOfProdOddWaiting > 0 && canProdOdd())
            prodOddSem.v();
        else if (numOfConsEvenWaiting > 0 && canConsEven())
            consEvenSem.v();
        else
            mutex.v();

        sleep(THREAD_SLEEP_TIME);
    }

    return NULL;
}


int main()
{
    pthread_t tid[MAX_THREADS_COUNT];

    pthread_create(&tid[0], NULL, threadProdA1, NULL);
    pthread_create(&tid[1], NULL, threadProdA2, NULL);
    pthread_create(&tid[2], NULL, threadConsB1, NULL);
    pthread_create(&tid[3], NULL, threadConsB2, NULL);

    pthread_join(tid[0], (void**)NULL);
    pthread_join(tid[1], (void**)NULL);
    pthread_join(tid[2], (void**)NULL);
    pthread_join(tid[3], (void**)NULL);

    return 0;
}
