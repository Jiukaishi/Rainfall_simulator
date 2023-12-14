#ifndef BARRIER_H
#define BARRIER_H
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
class MyBarrier {
public:
  explicit MyBarrier(int num_threads)
      : count(num_threads), notified(false) {}

  void Wait() {
    std::unique_lock<std::mutex> lock(mutex);
    if (--count == 0) {
      // 最后一个到达的线程负责唤醒所有等待的线程
      notified = true;
      cv.notify_all();
    } else {
      // 其他线程等待唤醒
      while (!notified) //避免虚假唤醒
        cv.wait(lock);
    }
  }

private:
  int count;
  std::mutex mutex;
  bool notified; //通知信号
  std::condition_variable cv;
};
#endif