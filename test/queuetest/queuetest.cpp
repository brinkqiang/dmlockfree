
#include <cassert>
#include <chrono>
#include <iostream>
#include <set>
#include <thread>
#include <gtest.h>
#include "dmatomic_queue.h"

// TestType tracks correct usage of constructors and destructors
struct TestType {
  static std::set<const TestType *> constructed;
  TestType() noexcept {
    assert(constructed.count(this) == 0);
    constructed.insert(this);
  };
  TestType(const TestType &other) noexcept {
    assert(constructed.count(this) == 0);
    assert(constructed.count(&other) == 1);
    constructed.insert(this);
  };
  TestType(TestType &&other) noexcept {
    assert(constructed.count(this) == 0);
    assert(constructed.count(&other) == 1);
    constructed.insert(this);
  };
  TestType &operator=(const TestType &other) noexcept {
    assert(constructed.count(this) == 1);
    assert(constructed.count(&other) == 1);
    return *this;
  };
  TestType &operator=(TestType &&other) noexcept {
    assert(constructed.count(this) == 1);
    assert(constructed.count(&other) == 1);
    return *this;
  }
  ~TestType() noexcept {
    assert(constructed.count(this) == 1);
    constructed.erase(this);
  };
};

std::set<const TestType *> TestType::constructed;

TEST(queuetest, queuetest) {
  // Functionality test
  {
    CDMAtomicQueue<TestType> q(11);
    assert(q.front() == nullptr);
    assert(q.size() == 0);
    assert(q.empty() == true);
    assert(q.capacity() == 11);
    for (int i = 0; i < 10; i++) {
      q.emplace();
    }
    assert(q.front() != nullptr);
    assert(q.size() == 10);
    assert(q.empty() == false);
    assert(TestType::constructed.size() == 10);
    assert(q.try_emplace() == false);
    q.pop();
    assert(q.size() == 9);
    assert(TestType::constructed.size() == 9);
    q.pop();
    assert(q.try_emplace() == true);
    assert(TestType::constructed.size() == 9);
  }
  assert(TestType::constructed.size() == 0);

  // Copyable only type
  {
    struct Test {
      Test() {}
      Test(const Test &) {}
      Test(Test &&) = delete;
    };
    CDMAtomicQueue<Test> q(16);
    // lvalue
    Test v;
    q.emplace(v);
    q.try_emplace(v);
    q.push(v);
    q.try_push(v);
    static_assert(noexcept(q.emplace(v)) == false, "");
    static_assert(noexcept(q.try_emplace(v)) == false, "");
    static_assert(noexcept(q.push(v)) == false, "");
    static_assert(noexcept(q.try_push(v)) == false, "");
    // xvalue
    q.push(Test());
    q.try_push(Test());
    static_assert(noexcept(q.push(Test())) == false, "");
    static_assert(noexcept(q.try_push(Test())) == false, "");
  }

  // Copyable only type (noexcept)
  {
    struct Test {
      Test() noexcept {}
      Test(const Test &) noexcept {}
      Test(Test &&) = delete;
    };
    CDMAtomicQueue<Test> q(16);
    // lvalue
    Test v;
    q.emplace(v);
    q.try_emplace(v);
    q.push(v);
    q.try_push(v);
    static_assert(noexcept(q.emplace(v)) == true, "");
    static_assert(noexcept(q.try_emplace(v)) == true, "");
    static_assert(noexcept(q.push(v)) == true, "");
    static_assert(noexcept(q.try_push(v)) == true, "");
    // xvalue
    q.push(Test());
    q.try_push(Test());
    static_assert(noexcept(q.push(Test())) == true, "");
    static_assert(noexcept(q.try_push(Test())) == true, "");
  }

  // Movable only type
  {
    CDMAtomicQueue<std::unique_ptr<int>> q(16);
    // lvalue
    // auto v = std::unique_ptr<int>(new int(1));
    // q.emplace(v);
    // q.try_emplace(v);
    // q.push(v);
    // q.try_push(v);
    // xvalue
    q.emplace(std::unique_ptr<int>(new int(1)));
    q.try_emplace(std::unique_ptr<int>(new int(1)));
    q.push(std::unique_ptr<int>(new int(1)));
    q.try_push(std::unique_ptr<int>(new int(1)));
    auto v = std::unique_ptr<int>(new int(1));
    static_assert(noexcept(q.emplace(std::move(v))) == true, "");
    static_assert(noexcept(q.try_emplace(std::move(v))) == true, "");
    static_assert(noexcept(q.push(std::move(v))) == true, "");
    static_assert(noexcept(q.try_push(std::move(v))) == true, "");
  }

  // Test we throw when capacity < 2
  {
    bool throws = false;
    try {
      CDMAtomicQueue<int> q(0);
    } catch (...) {
      throws = true;
    }
    assert(throws);
  }

  // Fuzz and performance test
  {
    const size_t iter = 100000;
    CDMAtomicQueue<size_t> q(iter / 1000 + 1);
    std::atomic<bool> flag(false);
    std::thread producer([&] {
      while (!flag)
        ;
      for (size_t i = 0; i < iter; ++i) {
        q.emplace(i);
      }
    });

    size_t sum = 0;
    auto start = std::chrono::system_clock::now();
    flag = true;
    for (size_t i = 0; i < iter; ++i) {
      while (!q.front())
        ;
      sum += *q.front();
      q.pop();
    }
    auto end = std::chrono::system_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    assert(q.front() == nullptr);
    assert(sum == iter * (iter - 1) / 2);

    producer.join();

    std::cout << duration.count() / iter << " ns/iter" << std::endl;
  }
}
