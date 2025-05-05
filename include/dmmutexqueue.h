#ifndef __DMMUTEXQUEUE_H_INCLUDE__
#define __DMMUTEXQUEUE_H_INCLUDE__

#include "dmqueue.h"
#include <mutex>

class CDMMutexQueue {
public:
    CDMMutexQueue() : m_bInitialized(false) {}

    bool Init(int nSize) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_bInitialized) {
            return false; // 已初始化，拒绝重复操作
        }
        bool ret = m_queue.Init(nSize);
        if (ret) {
            m_bInitialized = true;
        }
        return ret;
    }

    bool PushBack(void* ptr) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_bInitialized) {
            return false; // 队列未初始化
        }
        return m_queue.PushBack(ptr);
    }

    void* PopFront() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_bInitialized) {
            return nullptr; // 队列未初始化
        }
        return m_queue.PopFront();
    }

    int GetUsedSize() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_bInitialized) {
            return 0; // 队列未初始化
        }
        return m_queue.GetUsedSize();
    }

private:
    CDMQueue m_queue;
    mutable std::mutex m_mutex; // mutable允许const函数修改
    bool m_bInitialized;       // 初始化标志
};

#endif // __DMMUTEXQUEUE_H_INCLUDE__