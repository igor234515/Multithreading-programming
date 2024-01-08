#include "listener.h"

Threadpool mainThreadpool(3);


int main()
{
    Listener main_thread(3);
    main_thread.attachThreadpool(&mainThreadpool);
    main_thread.listen();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
} 