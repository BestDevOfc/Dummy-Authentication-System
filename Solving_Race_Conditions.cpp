// to compile: clang++ --std=c++17 -o main main.cpp -stdlib=libc++ -lpthread

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>


int some_function();

// the global mutex lock prevents a Data Race Condition from happening.
int global_actions = 0;
std::mutex global_lock;

int main(){
    const int MAX_TASKS = 100;
    std::vector< std::thread > threads = {};


    for( int i = 0; i < MAX_TASKS; i++ ){
        std::thread a_thread( &some_function );
        threads.push_back( std::move( a_thread ) );
    }

    for( std::thread& thread : threads ){
        thread.join();
    }

    std::cout << "[ The Value of global_actions is ]: " << global_actions << '\n';

    return 0;
}

int some_function(){
    // think of it as threading.Lock().acquire() and ...release() in python's concurrent.futures library
    global_lock.lock();
        global_actions += 1;
    global_lock.unlock();
    return 0;
}
