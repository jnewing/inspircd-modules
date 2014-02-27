/**
 * m_inject.cpp
 *
 * Really basic cut & dry module that allows opers to inject a command as any user on the network.
 * please make it noted that this is not my module it was originally written by Chernov-Phoenix Alexey this is simply my re-working to
 * make it function on 2.x
 *
 * The reason I re-wrote it was we upgraded to InspIRCd 2.x here at work and we used to use this module and wanted it to live again!
 *
 * Usage: /INJECT <user> <command>
 * Example: /inject synmuffin PRIVMSG #somechannel :foo bar (this would make the user synmuffin send the message "foo bar" to the channel #somechannel
 *
 * Don't be stupid with this module... nuf said I'm not your mum.
 *
 */

/* $ModDesc: Inject a command as any user on the network */
/* $ModAuthor: Joseph Newing (synmuffin) */
/* $ModAuthorMail: jnewing@gmail.com */
/* $ModDepends: core 2.0 */

#include "inspircd.h"

class CommandInject : public Command
{

public:

	CommandInject(Module* Creator) : Command(Creator, "INJECT", 2)
	{
		flags_needed = 'o';
		Penalty = 3;
		syntax = "<nick> <cmd>";
	}

	CmdResult Handle(const std::vector<std::string> &parameters, User* user)
	{
		User* u;
		std::string command;

		u = ServerInstance->FindNick(parameters[0]);

		if (u)
		{
			for (int x = 1; x < (int)parameters.size(); x++)
			{
				if (x > 1)
					command.append(" ");

				command.append(parameters[x]);
			}

			if (IS_LOCAL(u)) // thanks miniCruzer
			{
                ServerInstance->SNO->WriteToSnoMask('a', "%s INJECTed the command: '%s' as user: %s", user->nick.c_str(), command.c_str(), u->nick.c_str());
            }
			else
            {
				ServerInstance->SNO->WriteToSnoMask('A', "%s INJECTed the command: '%s' as user: %s", user->nick.c_str(), command.c_str(), u->nick.c_str());
            }

			ServerInstance->Parser->ProcessBuffer(command, (LocalUser*)u);

			return CMD_SUCCESS;
		}

		return CMD_FAILURE;
	}

};

class ModuleInject : public Module
{

private:

	CommandInject cmd;

public:

	ModuleInject() : cmd(this) {}

	void init()
	{
		ServerInstance->Modules->AddService(cmd);
	}

	Version GetVersion()
	{
		return Version("Adds the /INJECT command for opers allowing them to inject commands to a server as any user on the network - synmuffin: v1.0", VF_COMMON);
	}
};

MODULE_INIT(ModuleInject)
