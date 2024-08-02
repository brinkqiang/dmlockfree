#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <random>
#include "gtest.h"
#include "dmqueue.h"
#include "threadpool.hpp"
#include "dmformat.h"

// ģ������Ľṹ
struct Task {
	int id;
	int complexity;

	static Task* Create(int id, int complexity)
	{
		return new Task{ id, complexity };
	}

	void Release()
	{
		delete this;
	}
};

// ȫ�ּ�����������׷������ɵ�����
std::atomic<int> completedTasks(0);

template<size_t N, size_t Q>
class TestThreadLibrary : public CDMQueueThreadPool<N, Q> {
public:
	virtual void OnProcessTask(void* taskPtr, size_t threadId) override {
		Task* task = static_cast<Task*>(taskPtr);


		fmt::print("Thread {}  processed task:  {}\n", threadId, completedTasks);

		// ģ�������ʱ
		//std::this_thread::sleep_for(std::chrono::milliseconds(task->complexity));

		task->Release();

		// ��������������
		completedTasks++;
	}
};


TEST(CDMQueue, threadpool)
{
	const int NUM_THREADS = 10;
	const int NUM_TASKS = 100000;
	const int QUEUE_SIZE = 50;

	TestThreadLibrary<NUM_THREADS, QUEUE_SIZE> lib;

	// ��ʼ�������������
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> complexityDis(1, 1);

	// ���ɲ���������
	for (int i = 0; i < NUM_TASKS;) {
		Task* task = Task::Create( i, complexityDis(gen));
		if (!lib.pushTask(task)) {
			delete task;
			continue;
		}

		++i;
	}

	// �ȴ������������
	while (completedTasks < NUM_TASKS) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cout << "All tasks completed. Total tasks processed: " << completedTasks << std::endl;
}