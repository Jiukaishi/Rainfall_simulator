#ifndef myThreadPool_H
#define myThreadPool_H
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    // bool isEmpty() const{
    //     std::unique_lock<std::mutex> lock(queueMutex);
    //     return tasks.empty();
    // }

    ThreadPool(int numThreads) : stop(false), completedTasks(0), totalTasks(numThreads){
        for (size_t i = 0; i < numThreads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });

                        if (stop && tasks.empty()) {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();

                    // to replace future
                    {
                        std::unique_lock<std::mutex> lock(completionMutex);
                        completedTasks++;
                    }
                    completionCondition.notify_one();
                }
            });
        }
    }

    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            // don't allow enqueueing after stopping the pool
            if (stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }

        condition.notify_all();

        for (std::thread &worker : workers) {
            worker.join();
        }
    }
     // 等待所有任务完成
    void waitAll() {
        std::unique_lock<std::mutex> lock(completionMutex);
        completionCondition.wait(lock, [this] { return completedTasks == totalTasks; });
        completedTasks = 0;
    }

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    std::mutex queueMutex;
    std::condition_variable condition;
    std::mutex completionMutex;
    std::condition_variable completionCondition;
    int totalTasks;
    int completedTasks;
    bool stop;
};

// 示例任务函数
// void exampleTask(int i) {
//     std::cout << "Task " << i << " executed in thread " << std::this_thread::get_id() << std::endl;
// }

// int main() {
//     // 创建线程池，其中有3个线程
//     ThreadPool pool(3);

//     // 将10个任务加入线程池
//     for (int i = 0; i < 10; ++i) {
//         pool.enqueue([i] { exampleTask(i); });
//     }

//     // 等待所有任务完成
//     std::this_thread::sleep_for(std::chrono::seconds(2));

//     return 0;
// }
#endif