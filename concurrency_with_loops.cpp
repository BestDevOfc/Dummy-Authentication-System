#include <iostream>
#include <vector>
#include <thread>

void some_function(int some_argument){
    std::cout << "[ Thread with the following argument ]: " << some_argument << '\n';
}

int main(){
    
    std::vector<std::thread> threads;
    // create 10 threads:
    for( int i = 0; i <= 10; i++ ){
        std::thread a_thread(&some_function, i);
        /*
        threads can only be moved around, they cannot be copied because that would require 
        copying the underlying system thread which isn't supported in C++.
        we can also do ...push_back( std::thread(...) ) because it's moved directly into the push_back function
        threads.push_back( std::move(a_thread) );
        */
       threads.push_back( std::move( a_thread ) );
    }

    // don't want our program to terminate execution until all of the threads have finished, time to join them 
    // into our main thread
    for( int i = 0; i <= 10; i++ ){
        threads[i].join();
    }

    std::cout << "\n[ Returned back the Main Thread! ]\n";
    return 0;
}
