#include <iostream>
#include <vector>
#include <string>

class Buffer
{
private:
    std::vector<int> values;

    void print(std::string withMsg)
    {
        std::cout << "### " << withMsg << " " << values.size() << "[";
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
