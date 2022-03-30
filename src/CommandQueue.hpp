#ifndef COMMANDQUEUE_H_
#define COMMANDQUEUE_H_
#include "Command.hpp"
#include <list>
class CommandQueue
{
public:
  CommandQueue();
  ~CommandQueue();
  void run();
  void addCommand(Command& cmd);
private:
  //non copyable
  CommandQueue(const CommandQueue& other);
  CommandQueue& operator=(const CommandQueue& other);
  //data
  list<Command> mCommands;
  condition_variable mCondVar;  // condition variable to wait / notify
  mutex mMutex;                 // mutex to protect concurrent access to share data
};
#endif

