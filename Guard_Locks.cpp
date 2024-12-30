#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>

/*

Only improvement we need to make now is thread guard locks.
in python we can do try and excepts, so even if a write or operation fails our 
lock will be released. However in C++ it's different, if an exception happens in between the lock,
it just remains locked.

*/

class SomeClass{
    public:
        std::mutex gLock;
        std::ofstream results_file;
        SomeClass(){
            results_file.open("Results.txt", std::ios::out);
        }

        int some_function( int some_argument ){
            // this will automatically .lock() and .unlock() when it's out of scope of the function (when function is terminated)
            // for us.
            std::lock_guard< std::mutex > guardLock( gLock );
            try{
                results_file << "[ Argument being written ]: " << some_argument << '\n';
                throw "ArtificalException";
            }
            catch( ... ){
                // how to print the actual exception like in python?
                std::cerr << "[ An exception occured when writing to the file for argument ]: " << some_argument << '\n';
            }
            
            // resource still unlocks and our codebase is now a little bit safer.
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
