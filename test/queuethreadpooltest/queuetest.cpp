#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <random>
#include "gtest.h"
#include "dmqueue.h"
#include "queuethreadpool.hpp"
#include "dmformat.h"

// 模拟任务的结构
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

// 全局计数器，用于追踪已完成的任务

template<size_t N, size_t Q>
class TestDMQueueThreadPool : public CDMQueueThreadPool<N, Q> {
public:
	virtual void OnProcessTask(void* taskPtr, size_t threadId) override {
		Task* task = static_cast<Task*>(taskPtr);


		fmt::print("Thread {}  processed task:  {}\n", threadId, completedTasks);

		// 模拟任务耗时
		std::this_thread::sleep_for(std::chrono::milliseconds(task->complexity));

		task->Release();

		// 增加完成任务计数
		completedTasks++;
	}

public:
	int GetCompletedTasks(){ return completedTasks; }
private:
	// 计数器，用于追踪已完成的任务
	std::atomic<int> completedTasks = 0;
};


TEST(CDMQueue, threadpool)
{
	const int NUM_THREADS = 10;
	const int NUM_TASKS = 10000;
	const int QUEUE_SIZE = 100;

	TestDMQueueThreadPool<NUM_THREADS, QUEUE_SIZE> pool;

	// 初始化随机数生成器
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> complexityDis(1, 1);

	// 生成并推送任务
	for (int i = 0; i < NUM_TASKS;) {
		Task* task = Task::Create( i, complexityDis(gen));
		if (!pool.PushTask(task)) {
			delete task;
			continue;
		}

		++i;
	}

	// 等待所有任务完成
	while (pool.GetCompletedTasks() < NUM_TASKS) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cout << "All tasks completed. Total tasks processed: " << pool.GetCompletedTasks() << std::endl;
}