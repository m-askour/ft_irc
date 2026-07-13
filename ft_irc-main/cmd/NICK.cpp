#include"cmd.hpp"
#include"./server/server.hpp"

void server::nick(client *client,std::vector<std::string> args)
{
    //all the error      431 $ERR_NONICKNAMEGIVEN             $ERR_ERRONEUSNICKNAME
    if(args.empty())
    {
        std::cout<< "431"<< ":No nickname given"<<std::endl;
        return;       
    }
   //      433 $ERR_NICKNAMEINUSE               
    if(args[0] == client->get_nickname())
    {
        std::cout<< "433"<<args[0]<< ":Nickname is already in use"<<std::endl;
        return;       
    }
//    // 436 $ERR_NICKCOLLISION i don't know how to do this 
//     if(client->is_kelled_client())
//     {
//         std::cout<<"436"<<args[0]<<":Nickname collision KILL"<<std::endl;
//         return;
//     }
    client->set_nickname(args[0]);
}