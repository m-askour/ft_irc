#include "cmd.hpp"
#include "./server/server.hpp"

void server::mode(client *cl, const std::vector<std::string> &args)
{
    if (args.empty())
    {
        sendReply(cl, "461", "MODE", "Not enough parameters");
        return;
    }

    std::string channelName = args[0];
    std::map<std::string, Channel*>::iterator it = channels.find(channelName);
    if (it == channels.end())
    {
        sendReply(cl, "403", channelName, "No such channel");
        return;
    }
    Channel *channel = it->second;

    if (channel->users.find(cl) == channel->users.end())
    {
        sendReply(cl, "442", channelName, "You're not on that channel");
        return;
    }

    if (args.size() < 2)
    {
        std::string modes = "+";
        std::string params;
        if (channel->inviteOnly)
            modes += "i";
        if (channel->topicProtected)
            modes += "t";
        if (channel->keyEnabled)
        {
            modes += "k";
            params += " " + channel->password;
        }
        if (channel->limitEnabled)
        {
            modes += "l";
            std::ostringstream oss;
            oss << channel->userLimit;
            params += " " + oss.str();
        }
        sendReply(cl, "324", channelName, modes + params);
        return;
    }

    if (channel->operators.find(cl) == channel->operators.end())
    {
        sendReply(cl, "482", channelName, "You're not channel operator");
        return;
    }

    std::string modeString = args[1];
    if (modeString.empty())
    {
        sendReply(cl, "461", "MODE", "Not enough parameters");
        return;
    }
    if (modeString[0] != '+' && modeString[0] != '-')
    {
        sendReply(cl, "472", std::string(1, modeString[0]), "is unknown mode char to me");
        return;
    }
    if (modeString.size() == 1)
    {
        sendReply(cl, "461", "MODE", "Not enough parameters");
        return;
    }
    
    char sign = '+';
    char lastSign = '\0';
    size_t parameterIndex = 2;
    std::string appliedModes;
    std::string appliedParams;

    for (size_t i = 0; i < modeString.size(); i++)
    {
        char c = modeString[i];

        if (c == '+' || c == '-')
        {
            sign = c;
            continue;
        }

        switch (c)
        {
            case 'i':
                channel->inviteOnly = (sign == '+');
                if (lastSign != sign)
                {
                    appliedModes += sign;
                    lastSign = sign;
                }
                appliedModes += c;
                break;

            case 't':
                channel->topicProtected = (sign == '+');
                if (lastSign != sign)
                {
                    appliedModes += sign;
                    lastSign = sign;
                }
                appliedModes += c;
                break;

            case 'k':
                if (sign == '+')
                {
                    if (channel->keyEnabled)
                    {
                        sendReply(cl, "467", channelName, "Channel key already set");
                        return;
                    }
                    if (parameterIndex >= args.size())
                    {
                        sendReply(cl, "461", "MODE", "Not enough parameters");
                        return;
                    }
                    channel->keyEnabled = true;
                    channel->password = args[parameterIndex];
                    appliedParams += " " + args[parameterIndex];
                    parameterIndex++;
                }
                else
                {
                    channel->keyEnabled = false;
                    channel->password.clear();
                }
                if (lastSign != sign)
                {
                    appliedModes += sign;
                    lastSign = sign;
                }
                appliedModes += c;
                break;

            case 'l':
                if (sign == '+')
                {
                    if (parameterIndex >= args.size())
                    {
                        sendReply(cl, "461", "MODE", "Not enough parameters");
                        return;
                    }
                    std::istringstream iss(args[parameterIndex]);
                    size_t limit;
                    char extra;
                    if (!(iss >> limit) || (iss >> extra) || limit == 0)
                    {
                        sendReply(cl, "461", "MODE", "Invalid limit value");
                        return;
                    }
                    channel->limitEnabled = true;
                    channel->userLimit = limit;
                    appliedParams += " " + args[parameterIndex];
                    parameterIndex++;
                }
                else
                {
                    channel->limitEnabled = false;
                    channel->userLimit = 0;
                }
                if (lastSign != sign)
                {
                    appliedModes += sign;
                    lastSign = sign;
                }
                appliedModes += c;
                break;

            case 'o':
            {
                if (parameterIndex >= args.size())
                {
                    sendReply(cl, "461", "MODE", "Not enough parameters");
                    return;
                }
                std::string nickname = args[parameterIndex];
                parameterIndex++;

                client *target = find_client(nickname);
                if (!target || channel->users.find(target) == channel->users.end())
                {
                    sendReply(cl, "441", nickname, "They aren't on that channel");
                    return;
                }

                if (sign == '+')
                    channel->operators.insert(target);
                else
                    channel->operators.erase(target);

                if (lastSign != sign)
                {
                    appliedModes += sign;
                    lastSign = sign;
                }
                appliedModes += c;
                appliedParams += " " + nickname;
                break;
            }

            default:
                sendReply(cl, "472", std::string(1, c), "is unknown mode char to me");
                return;
        }
    }

    if (appliedModes.empty())
        return;

    std::string prefix = cl->get_nickname() + "!" + cl->get_username() + "@localhost";    //ahari!alaa@localhost
    std::string broadcast = ":" + prefix + " MODE " + channelName + " " + appliedModes + appliedParams; //  :ahari!alaa@localhost MODE #channel +it

    broadcastToChannel(channel, broadcast, NULL);
}