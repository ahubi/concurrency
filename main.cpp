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
    mutex mMutex;                       //mutex to protect concurrent access to share data
    condition_variable mCondVar;        //condition variable to wait / notify
    vector<string> mDataVector;         //data which is shared between threads
};
class Consumer{
public:
    Consumer(thread_data* p) : mData(p){};
    inline void operator()(){
        if(mData){
            //this lock is needed to sync output to cout
            unique_lock<mutex> cout_lck(mData->mMutex); //mutex is locked here
            
            LOG("Consumer thread entering execution");
            
            cout_lck.unlock();                          //unlock the mutex
            
            while(1){
                unique_lock<mutex> lck(mData->mMutex); //mutex is locked here
                mData->mCondVar.wait(lck);             //mutex will be unlocked while waiting   
                if(mData->mDataVector.size()>0){       //and locked again after thread is leaving waiting
                    LOG("Consumer: " << mData->mDataVector.back());
                    mData->mDataVector.pop_back();
                }
                lck.unlock();                               //unlock thread here
                this_thread::sleep_for(chrono::seconds(3)); //sleep just to simulate run time of a thread
            }
        }
    };
private:
    thread_data* mData;
};

void consumer(thread_data* pSharedData){
    if(pSharedData){
        //this lock is needed to sync output to cout
        unique_lock<mutex> cout_lck(pSharedData->mMutex); //mutex is locked here
        
        LOG("consumer thread entering execution");
        
        cout_lck.unlock();                              //unlock the mutex
        
         while(1){
            unique_lock<mutex> lck(pSharedData->mMutex);    //mutex is locked here
            pSharedData->mCondVar.wait(lck);                //mutex will be unlocked while waiting
            if(pSharedData->mDataVector.size()>0){          //and locked again after thread is leaving waiting
                LOG("consumer: " << pSharedData->mDataVector.back());
                pSharedData->mDataVector.pop_back();
            }
            lck.unlock();                                   //unlock thread here
            this_thread::sleep_for(chrono::seconds(1));     //sleep just to simulate run time of a thread
        }
    }
} 

void producer(thread_data* pSharedData){
    if(pSharedData){
        //this lock is needed to sync output to cout
        unique_lock<mutex> cout_lck(pSharedData->mMutex);
        
        LOG("producer thread entering execution");
        
        cout_lck.unlock();
        
        while(1){
            unique_lock<mutex> lck(pSharedData->mMutex); //lock to be able to write to data
            
            pSharedData->mDataVector.push_back("Hello");
            pSharedData->mDataVector.push_back("World!");
            
            LOG("producer: " << pSharedData->mDataVector.size() << " elements in queue");
            
            pSharedData->mCondVar.notify_all(); //notify all waiting threads
            lck.unlock();                       //unlock the mutex to allow other threads to be able to access the data
            
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
    t4.join();
    return 0;
}
