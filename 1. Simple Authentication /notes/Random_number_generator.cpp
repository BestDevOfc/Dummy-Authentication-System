#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

int main(){
    // will keep on giving us the same random number due to the seed value being the same
    // int rand_val = rand();
    // std::cout << "[ Random Value ]: " << rand_val << '\n';

    int elapsed_seconds = time(nullptr); // seconds elapsed since Jan 1st 1970
    
    srand( elapsed_seconds ); // setting the seed value which is ever changing
    int rand_val = rand() % 10; // setting an upper bound

    std::cout << "[ Random Number ]: " << rand_val << '\n';

    return 0;
}
