#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <string>

using namespace std;

#define LOG(x) do {cout << hex <<"["<< this_thread::get_id() <<"] " << x << endl;} while (0)

struct thread_data{
    mutex mMutex;
    condition_variable mCondVar;
    vector<string> mDataVector;
};
class Consumer{
public:
    Consumer(thread_data* p) : mData(p){};
    inline void operator()(){
        if(mData){
            unique_lock<mutex> cout_lck(mData->mMutex);
            
            LOG("Consumer thread entering execution");
            
            cout_lck.unlock();
            
            while(1){
                unique_lock<mutex> lck(mData->mMutex);
                mData->mCondVar.wait(lck);
                if(mData->mDataVector.size()>0){
                    LOG("Consumer: " << mData->mDataVector.back());
                    mData->mDataVector.pop_back();
                }
                lck.unlock();
                this_thread::sleep_for(chrono::seconds(3));
            }
        }
    };
private:
    thread_data* mData;
};

void consumer(thread_data* pSharedData){
    if(pSharedData){
        
        unique_lock<mutex> cout_lck(pSharedData->mMutex);
        
        LOG("consumer thread entering execution");
        
        cout_lck.unlock();
        
         while(1){
            unique_lock<mutex> lck(pSharedData->mMutex);
            pSharedData->mCondVar.wait(lck);
            if(pSharedData->mDataVector.size()>0){
                LOG("consumer: " << pSharedData->mDataVector.back());
                pSharedData->mDataVector.pop_back();
            }
            lck.unlock();
            this_thread::sleep_for(chrono::seconds(1));
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
            
            pSharedData->mCondVar.notify_all();
            lck.unlock();
            
            this_thread::sleep_for(chrono::seconds(3));
        }
    }
} 

int main(int argc, char const *argv[])
{
    thread_data* td = new thread_data;
    thread t3 (consumer, td);
    thread t1 (producer, td);
    thread t2 (consumer, td);
    thread t4 (Consumer{td});
    
    t1.join();
    t2.join();
    t3.join();
    return 0;
}
