#include "CommandQueue.hpp"
#include <thread>
CommandQueue::CommandQueue()
{
}

CommandQueue::~CommandQueue()
{
}
void CommandQueue::addCommand(Command& cmd)
{
  unique_lock<mutex> lck(mMutex);  // mutex is locked here
  mCommands.push_back(cmd);
  mCondVar.notify_all();
  lck.unlock();
}
void CommandQueue::run()
{
  cout << "-->CommandQueue::run"<< endl;
  while (1) {
    if (!mCommands.empty()) {
      try {
        promise<CommandResult> commandResult;
        thread th1(&Command::execute, &mCommands.front(), ref(commandResult));
        /* code */
        future<CommandResult> fut = commandResult.get_future();
        CommandResult res = fut.get();
        cout << "Command: " << res.command << " finished with: " << res.result
             << endl;
        mCommands.pop_front();
        th1.join();
      } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
      }
    }else //wait for more commands
    {
      cout << "Wait for commands" << endl;
      unique_lock<mutex> lck(mMutex);  // mutex is locked here
      mCondVar.wait(lck);              // mutex will be unlocked while waiting
      lck.unlock();                    // unlock thread here
    }
  }
  cout << "<--CommandQueue::run"<< endl;
}