/* **
 * m_ircops.cpp
 *
 * Module adds the IRCOPS command that prints out a list of current online
 * opers including their class unless they have mode +H set on themselves.
 *
 * NOTES:
 * THIS COMMAND IS NOT LIMITED TO OPERS!
 *
 * I tried to keep the formatting of the output messages as close as i could
 * to kind of fit the InspIRCd look (it looks a lot like the CHECK command)
 *
 */

/* $ModDesc: /IRCOPS command that prints out a list of current online */
/* $ModAuthor: Joseph Newing (synmuffin) */
/* $ModAuthorMail: jnewing@gmail.com */
/* $ModDepends: core 1.2-1.3 */

#include "inspircd.h"

class CommandIRCOps : public Command
{

public:

    CommandIRCOps (InspIRCd* Instance) : Command(Instance,"IRCOPS", 0, 0)
	{
		this->source = "m_ircops.so";
	}

	CmdResult Handle (const std::vector<std::string> &parameters, User *user)
	{
		std::string ircops_str;
		ircops_str = "304 " + std::string(user->nick) + " :IRCOPS";

		if (user)
		{
			user->WriteServ(ircops_str + " START ");

			for (std::list<User*>::iterator i = ServerInstance->Users->all_opers.begin(); i != ServerInstance->Users->all_opers.end(); i++)
			{
				User* u = *i;

				if (!u->IsModeSet('H'))
				{
					user->WriteServ(ircops_str + " " + std::string(u->oper) + ": " + std::string(u->nick));
				}
			}

			user->WriteServ(ircops_str + " END ");
		}

		return CMD_LOCALONLY;
	}

};

class ModuleIRCOps : public Module
{

private:

	CommandIRCOps *mycommand;

public:

	ModuleIRCOps(InspIRCd* Me) : Module(Me)
	{
		mycommand = new CommandIRCOps(ServerInstance);
		ServerInstance->AddCommand(mycommand);
	}

	virtual ~ModuleIRCOps() {};

	virtual Version GetVersion()
	{
		return Version("m_ircops ver 1.0", 0, API_VERSION);
	}

};

MODULE_INIT(ModuleIRCOps)
