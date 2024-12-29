// concurrency: runs on one core -> less heavy on system
// parallelism: multiple threads on numerous cores (better for mass credential spraying programs) -> more heavy on the system 
// ^^ be careful if writing to a shared file object when using this

#include <iostream>
#include <vector>
#include <future>

void some_func(int some_argument) {
    std::cout << "[ Some Func! - " << some_argument << " ]" << std::endl;
}

void start_concurrency() {
    const int max_workers = 100;
    const int total_tasks = 1000;

    std::vector<std::future<void>> futures;

    for (int i = 0; i < total_tasks; ++i) {
        futures.emplace_back(std::async(std::launch::async, some_func, i));

        // To limit the number of active threads, wait for every `maxWorkers` tasks
        if (futures.size() == max_workers) {
            for (auto &future : futures) {
                future.get();  // Ensure the current batch of tasks is completed
            }
            futures.clear();
        }
    }

    // Ensure remaining tasks are completed
    for (auto &future : futures) {
        future.get();
    }
}

int main() {
    start_concurrency();
    return 0;
}
