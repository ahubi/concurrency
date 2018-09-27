#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

#define LOG(x) do { cout << hex <<"["<< this_thread::get_id() <<"] " << x << endl; } while (0)

struct thread_data{
    mutex mMutex;
    condition_variable mCondVar;
    vector<string> mDataVector;
};

void consumer(thread_data* pSharedData){
    if(pSharedData){
        
        unique_lock<mutex> cout_lck(pSharedData->mMutex);
        
        LOG("consumer thread entering execution");
        
        cout_lck.unlock();
        
        while(1){
            unique_lock<mutex> lck(pSharedData->mMutex);
            pSharedData->mCondVar.wait(lck);
            for(const auto s:pSharedData->mDataVector){
                LOG("consumer: " << s);
            }

            LOG("consumer: " << pSharedData->mDataVector.size() << " elements consumed");
            
            pSharedData->mDataVector.clear();
            lck.unlock();
            this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
} 

void producer(thread_data* pSharedData){
    if(pSharedData){
        unique_lock<mutex> cout_lck(pSharedData->mMutex);
        
        LOG("producer thread entering execution");
        
        cout_lck.unlock();
        
        while(1){
            unique_lock<mutex> lck(pSharedData->mMutex);
            
            pSharedData->mDataVector.push_back("Hello");
            pSharedData->mDataVector.push_back("World!");
            
            LOG("producer: " << pSharedData->mDataVector.size() << " elements in queue");
            
            pSharedData->mCondVar.notify_one();
            lck.unlock();
            
            this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
} 

int main(int argc, char const *argv[])
{
    thread_data* td = new thread_data;
    thread t3 (consumer, td);
    thread t1 (producer, td);
    thread t2 (consumer, td);
    
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
