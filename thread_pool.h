#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <optional>
#include <functional>

template<typename T>
class pool {
public:
    // _worker will be called on some thread for each item in _work. 
    // _reporter is called with the items remaining after every change.
    pool(std::vector<T> _work, std::function<void(T)> _worker, std::function<void(int)> _reporter)
      : work(_work), worker(_worker), reporter(_reporter) {}

    void run(int thread_count) {
        std::vector<std::thread> threads;
        for (int i = 0; i < thread_count; i++) {
            threads.emplace_back(&pool::_run, this);
        }
        for (auto& t : threads) {
            t.join();
        }
    }

    std::optional<T> request_item() {
        std::scoped_lock lock(request_mutex);
        if (work.empty()) {
            return {};
        } else {
            auto value = work.back();
            work.pop_back();
            reporter(work.size());
            return {value};
        }
    }

    void _run() {
        std::optional<T> item;
        while (item = request_item(), item.has_value()) {
            worker(item.value());
        }
    }

private:
    std::mutex request_mutex;
    std::vector<T> work;
    std::function<void(T)> worker;
    std::function<void(int)> reporter;
};
