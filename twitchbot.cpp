#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "irc.h"

using namespace std;

int main()
{
	irc::IrcCon twitch;
	
	twitch.user = "wulf2k";
	twitch.nick = "wulf2k";
	twitch.channel = "#wulf2k";
	twitch.serverAddr = "irc.chat.twitch.tv";
	
	twitch.conn();

	twitch.join(twitch.channel);
	twitch.send("PRIVMSG " + twitch.channel + " :" + twitch.nick + " initializing.");
	
	while (twitch.sockfd > -1)
	{
		twitch.get();
		while (twitch.msgs.size() > 0)
		{
			printf("<< %s\n", twitch.msgs[0].c_str());
			twitch.msgs.erase(twitch.msgs.begin());
		}
		usleep(200000);
	}

	twitch.disconn();
}