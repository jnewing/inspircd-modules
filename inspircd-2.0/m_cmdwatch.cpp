/**
 * m_cmdwatch.cpp
 *
 * Module adds snomasks w that allows opers with that SNOMask to be alerted when a user exacutes
 * one of the commands (specified via the InspIRCd conf.)
 *
 * Module configuration;
 * The following needs to be added to your InspIRCd config file:
 *
 * <cmdwatch wcommands="JOIN PART">
 *
 * Note: You can add / remove as many commands within that wcommands tag as you like. Each one
 * of the commands found here, when exacuted opers with SNOMask w will be spammed with output
 * like: *** CMDWATCH: Watched command: PART issued by: someuser
 *
 */

/* $ModDesc: Adds snomask w allows opers to monitor a users commands */
/* $ModAuthor: Joseph Newing (synmuffin) */
/* $ModAuthorMail: jnewing@gmail.com */
/* $ModDepends: core 2.0 */

#include <inspircd.h>

class CmdWatchModule : public Module
{

private:

         std::vector<std::string> WatchedCommands;

 public:

         CmdWatchModule()
         {
                OnRehash(NULL);
                ServerInstance->SNO->EnableSnomask('w', "CMDWATCH");

                Implementation eventlist[] = { I_OnPreCommand };
                ServerInstance->Modules->Attach(eventlist, this, 1);
         }

        ~CmdWatchModule() { }

        virtual ModResult OnPreCommand(std::string &command, std::vector<std::string>& parameters, LocalUser* user, bool validated, const std::string &original_line)
        {
                bool watched = false;

                for (std::vector<std::string>::iterator i = WatchedCommands.begin(); i != WatchedCommands.end(); i++)
                {
                        if (command.compare(*i) == 0)
                        {
                                watched = true;
                                break;
                        }
                }

                if (watched)
                        ServerInstance->SNO->WriteToSnoMask('w', "Watched command: %s issued by: %s", command.c_str(), user->nick.c_str());

                return MOD_RES_PASSTHRU;
        }

        void OnRehash(User* user)
        {
                ConfigReader Conf;
                std::string cmds = Conf.ReadValue("cmdwatch", "wcommands", 0);

                WatchedCommands.clear();

                std::stringstream dcmds(cmds);
                std::string thiscmd;

                while (dcmds >> thiscmd)
                {
                    WatchedCommands.push_back(thiscmd);
                }
        }

        Version GetVersion()
        {
                return Version("Adds snomasks w to allow opers to view when perticular commands are exacuted - epicgeeks.net - synmuffin", VF_COMMON);
        }

};

MODULE_INIT(CmdWatchModule)
