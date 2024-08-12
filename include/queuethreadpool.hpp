
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef __DMQUEUE_THREAD_POOL_H_INCLUDE__
#define __DMQUEUE_THREAD_POOL_H_INCLUDE__

#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <iostream>
#include "dmqueue.h"

template<size_t N, size_t Q>
class CDMQueueThreadPool {
protected:
	std::vector<std::thread> threads;
	std::vector<CDMQueue> queues;
	std::atomic<bool> running{ true };
	size_t max_queue = Q;

	void ThreadFunction(size_t id) {
		auto& q = queues[id];
		while (running) {
			void* task = q.PopFront();
			if (nullptr == task)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			ProcessTask(task, id);
		}
	}

private:
	bool shouldBalanceLoad(size_t threadId) {
		int QueueSize = queues[threadId].GetUsedSize();

		return QueueSize > max_queue / N;
	}

	void ProcessTask(void* task, size_t threadId) {

		if (threadId < N - 1 && !this->shouldBalanceLoad(threadId + 1)) {
			this->queues[threadId + 1].PushBack(task);
			return;
		}

		OnProcessTask(task, threadId);
	}

public:
	virtual void OnProcessTask(void* task, size_t threadId)
	{
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

	bool PushTask(void* task) {
		return queues[0].PushBack(task);
	}
};

#endif // __DMQUEUE_THREAD_POOL_H_INCLUDE__
