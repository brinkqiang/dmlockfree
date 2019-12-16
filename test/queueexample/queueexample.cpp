#include <iostream>
#include <dmqueue.h>
#include <thread>

int main(int argc, char *argv[]) {
  (void)argc, (void)argv;
  CDMQueue<int> q(2);
  auto t = std::thread([&] {
    while (!q.front())
      ;
    std::cout << *q.front() << std::endl;
    q.pop();
  });
  q.push(1);
  t.join();

  return 0;
}
