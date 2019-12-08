#ifndef IRC_H
#define IRC_H


#include <string>
#include <vector>

namespace irc
{
	class IrcCon
	{
	public:
		std::string nick;
		std::string user;
		std::string channel;
		const char* serverAddr;
		unsigned short serverPort = 6667;
		
		std::vector<std::string> msgs;

		int sockfd = -1;
		void disconn();
		std::vector<std::string> splitter(std::string in_pattern, std::string& content);
		void conn();
		void get();
		void send(std::string str);
		void join(std::string chan); 
	};
}

#endif