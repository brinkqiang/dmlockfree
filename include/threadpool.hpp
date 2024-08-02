#ifndef THREAD_LIBRARY_H
#define THREAD_LIBRARY_H

#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>
#include "dmqueue.h"

template<size_t N, size_t Q>
class CDMQueueThreadPool {
public:
	std::vector<std::thread> threads;
	std::vector<CDMQueue> queues;
	std::atomic<bool> running{ true };
	size_t max_queue = Q;

	void ThreadFunction(size_t id) {
		while (running) {
			void* task = queues[id].PopFront();
			if (nullptr == task)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			ProcessTask(task, id);
		}
	}
public:
	virtual void OnProcessTask(void* task, size_t threadId) {
	}

	void ProcessTask(void* task, size_t threadId) {

		if (threadId < N - 1 && !this->shouldBalanceLoad(threadId + 1)) {
			this->queues[threadId + 1].PushBack(task);
			return;
		}

		OnProcessTask(task, threadId);
	}

	virtual bool shouldBalanceLoad(size_t threadId) {
		int QueueSize = queues[threadId].GetUsedSize();

		return QueueSize > max_queue / 2;
	}

public:
	CDMQueueThreadPool() {
		queues.resize(N);
		for (auto& queue : queues) {
			queue.Init(max_queue);
		}

		for (size_t i = 0; i < N; ++i) {
			threads.emplace_back(&CDMQueueThreadPool::ThreadFunction, this, i);
		}
	}

	~CDMQueueThreadPool() {
		running = false;
		for (auto& thread : threads) {
			thread.join();
		}
	}

	bool pushTask(void* task) {
		return queues[0].PushBack(task);
	}
};

#endif // THREAD_LIBRARY_H