#include "Command.hpp"

Command::Command(string command)
:mCommand(command)
{
}
Command::~Command()
{}
void Command::execute(promise<CommandResult>& commandResult){
  cout <<"Command: " << mCommand << " executed" << endl;
  CommandResult result = {mCommand, 0};
  commandResult.set_value(result);
}
