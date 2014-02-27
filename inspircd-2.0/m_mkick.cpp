/**
 * m_mkick.cpp
 *
 * Simple module that adds the MKICK command (mass kick) allows channel ops to /mkick <#channel> [exception1,exception2] :[reason]
 * it will kick all ops, non-ops, voices etc... the only people it will not kick is the issuer of the command and any nicks
 * specified in the comma delimited exceptions list.
 *
 */

/* $ModDesc: /MKICK command, allowing for mass kicking */
/* $ModAuthor: Joseph Newing (synmuffin) */
/* $ModAuthorMail: jnewing@gmail.com */
/* $ModDepends: core 2.0 */

#include "inspircd.h"

class CommandMKick : public Command
{

public:

	CommandMKick(Module* Creator) : Command(Creator, "MKICK", 1)
	{
		Penalty = 0;
		syntax = "<#channel> [exception1,exception2,...] :[reason]";
	}

	CmdResult Handle(const std::vector<std::string> &parameters, User *user)
	{
		bool exceptions = false;
		std::vector<std::string> except;
		std::string reason;

		Channel *channel = ServerInstance->FindChan(parameters[0]);
		if (channel)
		{
			if (channel->GetPrefixValue(user) >= OP_VALUE)
			{
				if (parameters.size() == 3)
				{
					exceptions = true;
					split(parameters[1], ',', except);
					reason.assign(parameters[2], 0, ServerInstance->Config->Limits.MaxKick);
				}

				if (parameters.size() == 2)
				{
					User* u;
					split(parameters[1], ',', except);
					u = ServerInstance->FindNick(except[0]);

					if (u)
						exceptions = true;
					else
						reason.assign(parameters[1], 0, ServerInstance->Config->Limits.MaxKick);
				}

				if (parameters.size() == 1)
				{
					reason.assign(user->nick, 0, ServerInstance->Config->Limits.MaxKick);
				}

				const UserMembList* cl = channel->GetUsers();
				for (UserMembCIter i = cl->begin(); i != cl->end(); i++)
				{
					bool mkicku = true;

					if (exceptions)
					{
						for (std::vector<std::string>::const_iterator ci = except.begin(); ci != except.end(); ci++)
						{
							if (i->first->nick.compare(*ci) == 0)
								mkicku = false;
						}
					}

					if (i->first->nick.compare(user->nick) == 0)
							mkicku = false;

					if (mkicku)
					{
						User* u;
						if (IS_LOCAL(i->first))
							u = ServerInstance->FindNickOnly(i->first->nick);
						else
							u = ServerInstance->FindNick(i->first->nick);

						channel->KickUser(user, u, reason.c_str());
					}
				}

				return CMD_SUCCESS;
			}
		}

		return CMD_FAILURE;
	}

	/**
     * split()
	 *
	 * simple function to split a string based on a delimiter
	 * filles the passes vector with the results.
	 *
	 */
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
	{
		std::stringstream ss(s);
		std::string item;

		while(std::getline(ss, item, delim))
		{
			elems.push_back(item);
		}

		return elems;
	}
};

class ModuleMKick : public Module
{

private:

	CommandMKick cmd;

public:

	ModuleMKick() : cmd(this) {}

	void init()
	{
		ServerInstance->Modules->AddService(cmd);
	}

	Version GetVersion()
	{
		return Version("Adds the /MKICK command to allows ops to mass kick channels - synmuffin: v2.0", VF_COMMON);
	}
};

MODULE_INIT(ModuleMKick)
