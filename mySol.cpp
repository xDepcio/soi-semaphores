// Mamy bufor FIFO na liczby całkowite.
// * Procesy A1 generują kolejne liczby parzyste modulo 50, jeżeli w buforze jest mniej niż 10 liczb
// parzystych.
// * Procesy A2 generują kolejne liczby nieparzyste modulo 50, jeżeli liczb parzystych w buforze jest
// więcej niż nieparzystych.
// * Procesy B1 zjadają liczby parzyste pod warunkiem, że bufor zawiera co najmniej 3 liczby.
// * Procesy B2 zjadają liczby nieparzyste, pod warunkiem, że bufor zawiera co najmniej 7 liczb.
// W systemie może być dowolna liczba procesów każdego z typów. Zrealizuj wyżej wymienioną
// funkcjonalność przy pomocy semaforów.
// Zakładamy, że bufor FIFO poza standardowym put() i get() ma tylko metodę umożliwiającą
// sprawdzenie liczby na wyjściu (bez wyjmowania) oraz posiada metody zliczające elementy
// parzyste i nieparzyste. Zakładamy, że semafory mają tylko operacje P i V.

#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"

int const threadsCounts = 4;
int const bufferSize = 9;

class Buffer
{
private:
    std::vector<int> values;
    Semaphore mutex;
    bool readA, readB;

    void print(std::string name)
    {
        std::cout << "\n###" << name << " " << values.size() << "[";
        for (auto v : values)
            std::cout << v << ",";
        std::cout << "]###\n";
    }

public:
    Buffer() : mutex(1), readA(false), readB(false)
    {
    }

    void put(int value)
    {
        mutex.p();
        // Wstawienie elementu do bufora
        values.push_back(value);
        print("P");
        mutex.v();
    }

    int get()
    {
        mutex.p();
        int v = values.front();
        values.erase(values.begin());
        mutex.v();
        return v;
    }

    int readTop()
    {
        return values.front();
    }

    int countEven()
    {
        mutex.p();
        int count = 0;
        for (auto v : values)
            if (v % 2 == 0)
                ++count;
        mutex.v();
        return count;
    }

    int countOdd()
    {
        mutex.p();
        int count = 0;
        for (auto v : values)
            if (v % 2 == 1)
                ++count;
        mutex.v();
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

void* threadProd(void* arg)
{
    for (int i = 0; i < 10; ++i)
    {
        buffer.put(i);
    }

    return NULL;
}

void* threadConsA(void* arg)
{
    for (int i = 0; i < 17; ++i)
    {
        auto value = buffer.getA();
    }

    return NULL;
}

void* threadConsB(void* arg)
{
    for (int i = 0; i < 17; ++i)
    {
        auto value = buffer.getB();
    }

    return NULL;
}

int main()
{
    pthread_t tid[threadsCounts];

    pthread_create(&tid[0], NULL, threadProd, NULL);
    pthread_create(&tid[1], NULL, threadProd, NULL);
    pthread_create(&tid[2], NULL, threadProd, NULL);
    pthread_create(&tid[3], NULL, threadConsA, NULL);
    pthread_create(&tid[4], NULL, threadConsB, NULL);

    for (int i = 0; i < threadsCounts; ++i)
        pthread_join(tid[i], (void**)NULL);

    return 0;
}
