// https://www.youtube.com/watch?v=tGfG3u0CB7s&list=PLvv0ScY6vfd_ocTP2ZLicgqKnvq50OCXM&index=2&ab_channel=MikeShah


#include <iostream>
#include <thread>

void some_function(int some_argument){
    std::cout << "[ Some_function(" << some_argument << ") ]\n";
}

int main(){
    std::thread some_thread(&some_function, 69);
    

    std::cout << "[ In the main thread ! ]\n";
    
    // forces the thread to join the main thread, that way execution will continue
    // until the thread terminates, this is important because if the "return 0" happens before our
    // thread is finished it'll lead to unexpected behavior such as Core Dump Aborts.
    some_thread.join();
    

    return 0;
}
