/**
 * m_iscmd.cpp
 *
 * Simple module that was used to support a irc script, it adds the /iscmd <command> command to the server allowing
 * users to check and see if <command> is indeed a valid command on this server. This module will return "VALID" or "INVALID" as
 * a notice to the user who issued the /iscmd command. (Again output was intended for scripting purposes)
 *
 * Usage: /iscmd foo   -> INVALID
 *        /iscmd part  -> VALID
 *
 */

/* $ModDesc: /ISCMD command, testing the existance of a command */
/* $ModAuthor: Joseph Newing (synmuffin) */
/* $ModAuthorMail: jnewing@gmail.com */
/* $ModDepends: core 2.0 */

#include <inspircd.h>

class IsCmdCommand : public Command
{

public:

    IsCmdCommand(Module* Creator) : Command(Creator, "ISCMD", 1, 1)
    {
        this->syntax = "<command>";
    }

    CmdResult Handle(const std::vector<std::string>& parameters, User *user)
    {
        std::string check_command = parameters[0];
        std::transform(check_command.begin(), check_command.end(), check_command.begin(), ::toupper);

        if (ServerInstance->IsValidModuleCommand(check_command, parameters.size(), user))
            user->WriteServ("NOTICE %s :VALID", user->nick.c_str());
        else
            user->WriteServ("NOTICE %s :INVALID", user->nick.c_str());

        return CMD_SUCCESS;
    }

};

class IsCmdModule : public Module
{

private:

    IsCmdCommand cmd;

public:

    IsCmdModule() : cmd(this)
    {
		ServerInstance->Modules->AddService(cmd);
    }

    ~IsCmdModule() { }

    Version GetVersion()
    {
        return Version("/iscmd - epicgeeks.net (synmuffin)", VF_COMMON);
    }

};

MODULE_INIT(IsCmdModule)
