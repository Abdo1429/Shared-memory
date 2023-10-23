#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

int c = 10;
int d = 100;
std::mutex mtx; // Mutex to protect shared data
std::condition_variable cv_update, cv_read; // Condition variables

int readCount = 0; // Number of processes that have read the current values
int printCount = 0; // Number of times values have been printed to the console

void updateFunction(int i) {
    std::unique_lock<std::mutex> lock(mtx);
    while (printCount < 15) {
        // Wait until at least two processes have read the current values
        cv_update.wait(lock, [] { return readCount >= 2; });
        c = c + i;
        d = d - i;
        readCount = 0; // Reset read count
        cv_read.notify_all(); // Notify reader processes
    }
}

void readFunction(int i) {
    std::unique_lock<std::mutex> lock(mtx);
    while (printCount < 15) {
        cv_read.wait(lock, [] { return readCount < 2; });
        std::cout << "Process " << i << ": c=" << c << ", d=" << d << std::endl;
        readCount++;
        printCount++;
        if (readCount >= 2) {
            cv_update.notify_all(); // Notify update processes
        }
    }
}

int main() {
    std::thread t1(updateFunction, 1);
    std::thread t2(updateFunction, 2);
    std::thread t3(readFunction, 3);
    std::thread t4(readFunction, 4);
    std::thread t5(readFunction, 5);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}
