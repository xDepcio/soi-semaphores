#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"

int const threadsCounts = 3;
// int const bufferSize = 9;

class Buffer
{
private:
    std::vector<int> values;
    // Semaphore mutex;
    // bool readA, readB;

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
        // mutex.p();
        // Wstawienie elementu do bufora
        values.push_back(value);
        print("P");
        // mutex.v();
    }

    int get()
    {
        // mutex.p();
        int v = values.front();
        values.erase(values.begin());
        // mutex.v();
        print("G");
        return v;
    }

    int readFirst()
    {
        return values.front();
    }

    int countEven()
    {
        // mutex.p();
        int count = 0;
        for (auto v : values)
            if (v % 2 == 0)
                ++count;
        // mutex.v();
        return count;
    }

    int countOdd()
    {
        // mutex.p();
        int count = 0;
        for (auto v : values)
            if (v % 2 == 1)
                ++count;
        // mutex.v();
        return count;
    }

    // int getA()
    // {
    //     mutex.p();
    //     // Odczytanie elementu z bufora (bez usuwania)
    //     int v = values.front();
    //     readA = true;
    //     print("A read");
    //     if (readB)
    //     {
    //         // usuniecie elementu z bufora
    //         values.erase(values.begin());
    //         print("A remove");
    //         readA = readB = false;
    //     }
    //     mutex.v();
    //     return v;
    // }

    // int getB()
    // {
    //     mutex.p();
    //     // Odczytanie elementu z bufora (bez usuwania)
    //     int v = values.front();
    //     readB = true;
    //     print("B read");
    //     if (readA)
    //     {
    //         // usuniecie elementu z bufora
    //         values.erase(values.begin());
    //         print("B remove");
    //         readA = readB = false;
    //     }
    //     mutex.v();
    //     return v;
    // }
};

Buffer buffer;
Semaphore mutex(1);
Semaphore prodEvenSem(10);
Semaphore prodOddSem(0);
Semaphore consEvenSem(-2);
Semaphore consOddSem(-6);

// void* threadProd(void* arg)
// {
//     for (int i = 0; i < 10; ++i)
//     {
//         buffer.put(i);
//     }

//     return NULL;
// }

void* threadProdA1(void* arg)
{
    // puts modulo 50 even nums
    int last = 0;
    while (true)
    {
        mutex.p();
        if (buffer.countEven() < 10)
        {
            buffer.put(last);
            last = (last + 2) % 50;

            consEvenSem.v();
            consOddSem.v();
            prodEvenSem.p();
            prodOddSem.v();
        }
        mutex.v();
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
        if (buffer.countEven() > buffer.countOdd())
        {
            buffer.put(last);
            last = (last + 2) % 50;

            consEvenSem.v();
            consOddSem.v();
            prodOddSem.p();
        }
        mutex.v();
    }

    return NULL;
}

void* threadConsB1(void* arg)
{
    // eats even nums
    while (true)
    {
        mutex.p();
        if (buffer.countEven() >= 3)
        {
            if(buffer.readFirst() % 2 == 0)
            {
                buffer.get();

                prodEvenSem.v();
                prodOddSem.p();
                consEvenSem.p();
                consOddSem.p();
            }
        }
        mutex.v();
    }

    return NULL;
}

void* threadConsB2(void* arg)
{
    // eats odd nums
    while (true)
    {
        mutex.p();
        if (buffer.countEven() >= 7)
        {
            if(buffer.readFirst() % 2 == 1)
            {
                buffer.get();

                prodEvenSem.v();
                prodOddSem.v();
                consEvenSem.p();
                consOddSem.p();
            }
        }
        mutex.v();
    }

    return NULL;
}

// void* threadConsA(void* arg)
// {
//     for (int i = 0; i < 17; ++i)
//     {
//         auto value = buffer.getA();
//     }

//     return NULL;
// }

// void* threadConsB(void* arg)
// {
//     for (int i = 0; i < 17; ++i)
//     {
//         auto value = buffer.getB();
//     }

//     return NULL;
// }

int main()
{
    pthread_t tid[threadsCounts];

    pthread_create(&tid[0], NULL, threadProdA1, NULL);
    pthread_create(&tid[1], NULL, threadProdA2, NULL);
    // pthread_create(&tid[2], NULL, threadProd, NULL);
    pthread_create(&tid[2], NULL, threadConsB1, NULL);
    pthread_create(&tid[3], NULL, threadConsB2, NULL);

    for (int i = 0; i < threadsCounts; ++i)
        pthread_join(tid[i], (void**)NULL);

    return 0;
}
