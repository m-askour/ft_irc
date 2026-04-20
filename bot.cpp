#include "bot.hpp"
bot::bot(): input(""), response("")
{

}
bot::bot(const bot &other): input(other.input), response(other.response)
{

}
bot &bot::operator=(const bot &other)
{
    if (this != &other)
    {
        this->input = other.input;
        this->response = other.response;
    }
    return *this;
}
bot::~bot()
{}

void bot::GetUserInput()
{
    std::string input;
    while (true)
    {
        std::getline(std::cin, input);//this to read the 
        //when there is input it weell be processing first 
        std::cout <<  processinput(input) << std::endl;        
    }
}
std::string  bot::processinput(std::string &input)
{

        history.push_back(input);
        //check what in thhis input
        std::string response;
        if (keywordMatching(input, response))
            return response;
        return failbackResponse();
   
    //generate the response 
}
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) {
    if (b == 0) {
        std::cout << "Error: Division by zero!" << std::endl;
        return 0;
    }
    return a / b;
}
std::string bot::getClientName(int socket_fd)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    getpeername(socket_fd, (struct sockaddr*)&addr, &addr_len);
    return std::string(inet_ntoa(addr.sin_addr)); // returns IP like "192.168.1.1"
}
std::string calculate(std::string input)
{
    int result = 0;
    int num1 = 0;
    int num2 = 0;
    for (int i = 0; i < input.size(); i++)
    {
        num1 = ft_atoi(input[1]);
        num2 = ft_atoi(input[3]);
        char c = input[2];

    }
        switch (c)
        {
        case '+': result = add(num1, num2);
            /* code */
            break;
        case '-': result = subtract(num1, num2);
            break;
        case '*': result = multiply(num1, num2);
            break;
        case '/': result = divide(num1, num2);
            break;
        default: result = 0;
            break;
        }
    
    char c = result + '0';
    std::string s(1,c);
    return (s);
    
}
bool bot::keywordMatching(std::string &input, std::string &response)
{
    // jockes keword
        /*1- chekc for the quastions*/
    if (input.find('?') != std::string::npos)
    {
        response = "Oh, good question… 🤔mmmmm… I don’t know the answer";
        return true;
    }
    // hello and hi and hey
    else if (input.find("HI")|| input.find("Hi") || input.find("hi") || input.find("Hello")|| input.find("HELLO") || input.find("hello") || input.find("hey") || )
    {
        response = "Hi";
        return true;
    }    
    //help (what's chatbot can do)
    else if (input.find("healp"))
    {
        response = "Hi I'm chat bot i can i can unser ur quastion or talck smal conversation";
        return true;
    }
    //what's my name
    else if (input.find("What's my name"))
    {
        response = "ur name is " + getClientName(user);
        return true;
    }    
    //calcul simple
    else if (input.find("+") != std::string::npos || input.find("-") != std::string::npos || input.find("/") != std::string::npos || input.find("*") != std::string::npos)
    {
        response = calculate(input);
        return true;
    } 
    else
    {
        response = "I’m not \
        Google or ChatGPT…  \
        I actually think before answering 😏";
        return true;
    }
    return false;
}
std::string bot::failbackResponse()
{
    //this is the output responce what the boot say and whatv the bot do
    return "I don't understand what do u main ";
}