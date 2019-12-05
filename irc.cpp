#include <algorithm>
#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std;

class IrcCon {
    public:
	string nick = "Wulf2kbot";
	string user = "Wulf2kbot";
	string channel = "#wulf2k";
	const char* serverAddr = "irc.chat.twitch.tv";
	unsigned short serverPort = 6667;
	
	vector<string> cmds;

	int sockfd = -1;

	void disconn()
	{
		printf("Closing socket.\n");
		close(sockfd);
	}

	vector<string> splitter(string in_pattern, string& content)
	{
		vector<string> split_content;

		regex pattern(in_pattern);
		copy( sregex_token_iterator(content.begin(), content.end(), pattern, -1),
			sregex_token_iterator(),back_inserter(split_content));  
		return split_content;
	}

	void conn()
	{
		int portno, n;
		struct sockaddr_in serv_addr;
		struct hostent *server;

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
		{
			printf("Error opening socket.\n");
			return;
		}
		printf("Looking up hostname: %s\n", serverAddr);

		server = gethostbyname(serverAddr);
		if (server == NULL)
		{
			printf("Host not found.\n");
			return;
		}

		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_port = htons(serverPort);
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr,
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);

		char str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(serv_addr.sin_addr), str, INET_ADDRSTRLEN);

		printf("IP: %s\n", str);
		printf("Port: %u\n", ntohs(serv_addr.sin_port));

		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

		printf("Attempting connection.\n");
		int ret = connect(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
		if (ret < 0)
		{
			printf("Connection failed: %d\n", ret);
			return;
		}

		ifstream file("oAuth.txt");
		string oauth;
		getline(file, oauth);
		
		send("PASS " + oauth);
		send("NICK " + nick);
		send("USER " + user);
	};


	void get()
	{
		int n;
		char buffer[1024];
		bzero(buffer, 1024);

		n = read(sockfd, buffer, 1024);
		if (n < 0)
		{
			return;
		};
		
		string msg(buffer);
		vector<string> lines = splitter(R"(\r\n)", msg);
		for (string line: lines)
		{
				if (line.find("PING") == 0)
				{
					printf("<< %s\n", line.c_str());
					send("PONG");
				} else {
					cmds.insert(cmds.end(), line);
				}
		}	
	};
	void send(string str)
	{
		if (!str.find("PASS") == 0)
		{
			printf(">> %s\n", str.c_str());
		}
		
		str += "\r\n";
		int n;
		n = write(sockfd, str.c_str(), str.length());
		if (n < 0)
		{
			printf("Error writing to socket.\n");
		};
	};
	void join(string chan)
	{
		send("JOIN " + chan);
	}
};

int main()
{
	IrcCon irc;
	irc.conn();

	irc.join(irc.channel);
	irc.send("PRIVMSG " + irc.channel + " :" + irc.nick + " initializing.");
	
	while (irc.sockfd > -1)
	{
		irc.get();
		while (irc.cmds.size() > 0)
		{
			printf("<< %s\n", irc.cmds[0].c_str());
			irc.cmds.erase(irc.cmds.begin());
		}
		usleep(200000);
	}

	irc.disconn();
}
