#ifndef COMMAND_H_
#define COMMAND_H_
#include <string>
#include <future>
#include <iostream>

using namespace std;
struct CommandResult{
  string command;
  int result;
};

class ICommand{
  public:
  virtual void execute(promise<CommandResult>& commandResult) = 0;
};

class Command
:ICommand
{
private:
  /* data */
  string mCommand;
  //promise<CommandResult> &mCommandResult;
public:
  Command(string command);
  ~Command();
  virtual void execute(promise<CommandResult>& commandResult);
};
#endif