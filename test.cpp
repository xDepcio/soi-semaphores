#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"

int const threadsCounts = 4;

class Buffer
{
private:
    std::vector<int> values;

    void print(std::string name)
    {
        std::cout << "\n###" << name << " " << values.size() << "[";
        for (auto v : values)
            std::cout << v << ",";
        std::cout << "]###\n";
    }

public:
    Buffer()
    {
    }

    void put(int value)
    {
        // Wstawienie elementu do bufora
        values.push_back(value);
        print("P");
    }

    int get()
    {
        int v = values.front();
        values.erase(values.begin());
        print("G");
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
Semaphore a1(10);
Semaphore a2(0);
// Semaphore b1(-2);
// Semaphore b2(-6);
Semaphore b1(0);
Semaphore b2(0);
Semaphore topEven(0);
Semaphore topOdd(0);

void* threadProdA1(void* arg)
{
    // puts modulo 50 even nums
    int last = 0;
    while (true)
    {
        mutex.p();
        // std::cout << "A1\n";
        printf("A1, mutex %d, a1: %d, a2: %d, b1: %d, b2: %d\n", mutex.retV(), a1.retV(), a2.retV(), b1.retV(), b2.retV());
        // if (buffer.countEven() < 10)
        // {
            a1.p();
            buffer.put(last);
            last = (last + 2) % 50;

            topEven.v();
            a2.v();
            if(buffer.countEven() + buffer.countOdd() >= 3)
                b1.v();
            else
                mutex.v();
            // if(buffer.countEven() + buffer.countOdd() >= 7)
            //     b2.v();
            // a1.p();
        // }
        // mutex.v();
    }

    return NULL;
}

void* threadProdA2(void* arg)
{
    // puts modulo 50 odd nums
    int last = 1;
    while (true)
    {
        mutex.p();
        // std::cout << "A2\n";
        printf("A2, mutex %d, a1: %d, a2: %d, b1: %d, b2: %d\n", mutex.retV(), a1.retV(), a2.retV(), b1.retV(), b2.retV());
        // if (buffer.countEven() > buffer.countOdd())
        // {
            a2.p();
            buffer.put(last);
            last = (last + 2) % 50;

            // if(buffer.countEven() + buffer.countOdd() >= 3)
            //     b1.v();
            topOdd.v();
            if(buffer.countEven() + buffer.countOdd() >= 7)
                b2.v();
            else
                mutex.v();
            // a2.p();
        // }
        // mutex.v();
    }

    return NULL;
}

void* threadConsB1(void* arg)
{
    // eats even nums
    while (true)
    {
        mutex.p();
        printf("B1, mutex %d, a1: %d, a2: %d, b1: %d, b2: %d\n", mutex.retV(), a1.retV(), a2.retV(), b1.retV(), b2.retV());
        // std::cout << "B1\n";
        // if (buffer.countEven() >= 3)
        // {
            // if(buffer.readFirst() % 2 == 0)
            // {
                topEven.p();
                b1.p();
                buffer.get();

                a1.v();
                if (buffer.readFirst() % 2 == 0)
                    topEven.v();
                if (buffer.countEven() > buffer.countOdd())
                    a2.p();
                else
                    mutex.v();
                // a2.p();
                // b1.p();
                // b2.p();
            // }
        // }
        // mutex.v();
    }

    return NULL;
}

void* threadConsB2(void* arg)
{
    // eats odd nums
    while (true)
    {
        mutex.p();
        printf("B2, mutex %d, a1: %d, a2: %d, b1: %d, b2: %d\n", mutex.retV(), a1.retV(), a2.retV(), b1.retV(), b2.retV());
        // std::cout << "B2" << " mutex: " << mutex.retV() << "\n";
        // if (buffer.countEven() >= 7)
        // {
            // if(buffer.readFirst() % 2 == 1)
            // {
                topOdd.p();
                b2.p();
                buffer.get();

                a2.v();
                if (buffer.readFirst() % 2 == 1)
                    topOdd.v();
                if (buffer.countEven() < 10)
                    a1.p();
                else
                    mutex.v();
                // a1.v();
                // a2.v();
                // b1.p();
                // b2.p();
            // }
        // }
        // mutex.v();
    }

    return NULL;
}


int main()
{
    pthread_t tid[threadsCounts];

    pthread_create(&tid[0], NULL, threadProdA1, NULL);
    pthread_create(&tid[1], NULL, threadProdA2, NULL);
    pthread_create(&tid[2], NULL, threadConsB1, NULL);
    pthread_create(&tid[3], NULL, threadConsB2, NULL);

    pthread_join(tid[0], (void**)NULL);
    pthread_join(tid[1], (void**)NULL);
    pthread_join(tid[2], (void**)NULL);
    pthread_join(tid[3], (void**)NULL);

    // for (int i = 0; i < threadsCounts; ++i)
    //     pthread_join(tid[i], (void**)NULL);

    return 0;
}
