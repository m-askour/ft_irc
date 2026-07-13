#include"cmd.hpp"
#include"./server/server.hpp"

void server::user(client *client,std::vector<std::string> args)
{
    //the error 
    //$ERR_NEEDMOREPARAMS
    if(args.empty())
    {
        std::cout<< "461"<< ":Not enough parameters"<<std::endl;
        return;       
    }
   //$ERR_ALREADYREGISTRED
    if(client->get_username() == args[0])
    {
        std::cout<<"462"<<":You may not reregister"<<std::endl;
        return;       
    }
    client->set_username(args[0]);
}