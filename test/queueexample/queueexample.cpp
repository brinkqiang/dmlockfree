#include <iostream>
#include <dmqueue.h>
#include <thread>
#include <gtest.h>

const int gNum = 100000000;

TEST(queueexample, queueexample)
{
    CDMTQueue<int> q(100000);
    auto t = std::thread([&] {
        int i = 0;
        for (;i != gNum;)
        {
            if (q.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));;
                continue;
            }
            q.pop();
            ++i;
        }
    });

    for (int i = 0; i < gNum; i++)
    {
        q.push(i);
    }

    t.join();
    std::cout << "pass" << std::endl;
}
