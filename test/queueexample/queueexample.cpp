#include <iostream>
#include <thread>
#include "gtest.h"
#include "dmatomic_queue.h"
#include "dmqueue.h"
#include "dmformat.h"

const int gNum = 100000000;
const int MaxPoolSize = 100000;

TEST(CDMAtomicQueue, CDMAtomicQueue)
{
	fmt::print("test CDMAtomicQueue {}\n", gNum);

    CDMAtomicQueue<int> q(MaxPoolSize);
	uint64_t total = 0;
    auto t = std::thread([&] {

        for (int i = 1; i < gNum;)
        {
            if (q.empty())
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(1));;
                continue;
            }
			int *a = q.front();
            q.pop();

			total += *a;
            ++i;
        }
    });

    for (int i = 1; i < gNum; i++)
    {
        q.push(i);
    }

    t.join();
	fmt::print("test CDMAtomicQueue Done total = {}\n", total);
}

TEST(CDMQueue, CDMQueue)
{
	fmt::print("test CDMQueue {}\n", gNum);

	CDMQueue q;
	q.Init(MaxPoolSize);
	uint64_t total = 0;
	auto t = std::thread([&] {
		for (int i = 1; i < gNum;)
		{
			void* p = q.PopFront();

			if (p == nullptr)
			{
				//std::this_thread::sleep_for(std::chrono::milliseconds(1));;
				continue;
			}

			int a = reinterpret_cast<int>(p);

			total += a;
			++i;
		}
		});

	for (int i = 1; i < gNum; )
	{
		if (!q.PushBack(reinterpret_cast<void*>(i)))
		{
			continue;
		}
		i++;
	}

	t.join();
	fmt::print("test CDMQueue Done total = {}\n", total);

}
