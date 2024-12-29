/*
    ^^^ JThreads are not yet supported in M-chip series, we'll just
    be sticking to the .join() method.
*/


#include <iostream>
#include <vector>
#include <thread>

int some_function( int some_argument );

int main(){
    const int MAX_WORKERS = 10;
    std::vector< std::jthread > threads = {};
    
    for( int i = 0; i <= MAX_WORKERS; i++ ){
        std::jthread thread( &some_function, i );
        threads.push_back( std::move( thread ) )
    }
    
    // no need to do .join() is automatically handled
    /*
    ^^^ JThreads are not yet supported in M-chip series, we'll just
    be sticking to the .join() method.
    */

    std::cout << "[ Returned to the Main Thread ]\n";

    return 0;
}



int some_function( int some_argument ){
    std::cout << "[ Some thread with argument ]: " << some_argument << '\n';
    return 0;
}
