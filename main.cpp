#define BOT_NICK "Lavie"
#define BOT_DESC "An IRC bot made by zed0 and Zaer"
#define COMMAND_CHAR '-'
#define INITIAL_CHAN "#cranman"

#include <cstdlib>
#include <sstream>
#include <fstream>
#include <ctime>
#include "irc.h"
#include "http.h"
#include "stringUtils.h"
#include "plugins.h"

using std::ifstream;
using std::ios_base;

int handleAllCommands(const string& nick, const string& channel, const vector<string>& words);
int handleAllMessages(const string& nick, const string& channel, const vector<string>& words);
//Should return 1 if it did anything, oterwise 0.
int handleAllStartupOptions(const vector<string>& args);

irc ircNet;
plugins pluginList;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	vector<string> args(argv, argv + argc);
	if(args.size() <3)
	{
		cerr << "usage: " + string(argv[0]) + " hostname port [options]" << endl;
		return 1;
	}
	ircNet.setNick(BOT_NICK);
	ircNet.setDesc(BOT_DESC);
	ircNet.joinChannel(INITIAL_CHAN);
	handleAllStartupOptions(args);
	ircNet.connect(argv[1], argv[2]);
	string message;
	while(true)
	{
		pluginList.doTick();
		if(ircNet.checkMessages(message))
		{
			if(message.find("PRIVMSG",0) != string::npos)
			{
				vector<string> words = stringUtils::msgWords(message);
				if(words.at(0).at(0) == COMMAND_CHAR) //if the line begins with our command char then handle commands
				{
					words.at(0).erase(0,1); //strip the command character off the front
					handleAllCommands(stringUtils::msgNick(message), stringUtils::msgChannel(message), words);
				}
				handleAllMessages(stringUtils::msgNick(message), stringUtils::msgChannel(message), words);
			}
		}
	}
	return 0;
}

int handleAllCommands(const string& nick, const string& channel, const vector<string>& words)
{
	pluginList.handleCommand(nick, channel, words);
	return 0;
}

int handleAllMessages(const string& nick, const string& channel, const vector<string>& words)
{
	pluginList.handleMessage(nick, channel, words);
	return 0;
}

int handleAllStartupOptions(const vector<string>& args)
{
	pluginList.startupOptions(args);
	return 0;
}
