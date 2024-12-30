#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <atomic>

/*

Only improvement we need to make now is thread guard locks.
in python we can do try and excepts, so even if a write or operation fails our 
lock will be released. However in C++ it's different, if an exception happens in between the lock,
it just remains locked.

*/

class SomeClass{
    public:
        // mainly used for simple data types in hard-ware
        std::atomic< int > shared_value = 0;
        
        SomeClass(){
            
        }

        int some_function( int some_argument ){
            // this will automatically .lock() and .unlock() when it's out of scope of the function (when function is terminated)
            // for us.
            
            // this operation is supported by the atomic class.
            // this is not supported by atomic operators so it will actually still lead to a data race:
            // shared_value = shared_value + 1;
            
            // locking and unlocking is done automatically on this resource.
            shared_value += 1;
            
            return 0;
        }
        int main(){
            const int MAX_TASKS = 1000;
            std::vector< std::thread > threads = {};
            for( int i = 0; i < MAX_TASKS; i ++ ){
                threads.emplace_back( std::thread( &SomeClass::some_function, this, i ) );
                
            }
            
            // wait for the threads to finish.
            for( std::thread& thread : threads ){
                thread.join();
            }
            

            return 0;
        }
};


int main(){
    SomeClass Object;
    Object.main();
    
    std::cout << "[ Returned to the main Thread! ]\n";
    std::cout << "[ Value of the shared variable ]: " << Object.shared_value << '\n';
    return 0;
}
