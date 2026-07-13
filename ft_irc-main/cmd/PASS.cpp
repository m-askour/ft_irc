#include"cmd.hpp"
#include"./server/server.hpp"

void server::pass(client *client, std::vector<std::string> args, std::string password)
{
    //$ERR_NEEDMOREPARAMS
    if(args.empty())
    {
        std::cout<< "461"<< ":Not enough parameters"<<std::endl;
        return;       
    }
    //the errir  462     $ERR_ALREADYREGISTRED
    if(password != args[0])
    {
        std::cout<<"462"<<":You may not reregister"<<std::endl;
        std::cout<<"the client pass: "<<args[1]<<std::endl;
        std::cout<<"the server pass: "<<password<<std::endl;
        return;       
    }
    else
        client->set_password(args[0]);
}