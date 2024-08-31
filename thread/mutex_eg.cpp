#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
// 一旦一个线程获取了互斥锁，就拥有了对共享资源的独占访问权
// 而其他线程必须等待该线程释放锁之后才能获取锁，进而访问共享资源

void print_thread_id(int id) {
    std::lock_guard<std::mutex> lock(mtx); // 自动加锁和解锁
    // 通过加锁，确保每个线程在打印id时独占输出流，避免输出混乱
    std::cout << "Thread ID: " << id << std::endl;
}

int main() {
    std::thread t1(print_thread_id, 1);
    std::thread t2(print_thread_id, 2);

    t1.join();
    t2.join();

    return 0;
}
