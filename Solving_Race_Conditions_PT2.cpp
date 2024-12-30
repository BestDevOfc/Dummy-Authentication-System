#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>



class SomeClass{
    public:
        std::mutex glock;
        std::ofstream results_file;
        SomeClass(){
            results_file.open("Results.txt", std::ios::app);
        }

        int some_function( int some_argument ){
            glock.lock();
                results_file << "[ Argument being written ]: " << some_argument << '\n';
            glock.unlock();
            return 0;
        }
        int main(){
            const int MAX_TASKS = 1000;
            std::vector< std::thread > threads = {};
            for( int i = 0; i < MAX_TASKS; i ++ ){
                /*
                because we are inside of a class we need to bind the class and function together
                // std::thread task_thread( &some_function, i );
                // threads.push_back( std::move( task_thread ) );

                We could directly just do this and it would work because std::thread under the hood
                will construct the thread for us. I just did it explicitly to make it easier to 
                understand the program flow.

                // threads.emplace_back( &SomeClass::some_function, this, i );
                */
                threads.emplace_back( std::thread( &SomeClass::some_function, this, i ) );
                
            }
            
            // wait for the threads to finish.
            for( std::thread& thread : threads ){
                thread.join();
            }
            
            std::cout << "[ Returned to the main Thread! ]\n";

            results_file.close();


            return 0;
        }
};


int main(){
    SomeClass Object;
    Object.main();
    
    std::cout << "[ Test ]\n";

    return 0;
}
