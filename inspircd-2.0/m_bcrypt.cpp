/**
 * Adds BCrypt support for Inspircd 2.0.xx
 *
 * m_bcrypt Based on the bcrypt key derivation function designed by Niels Provos and David Mazieres, based on
 * the Blowfish cipher as presented at USENIX 1999.
 *
 * Since this module does have an external dependancy (libxcrypt) it's should be placed in modules/extra
 * Once this module is loaded you can bcrypt hash passwords using /BCRYPT <password> however I've also
 * implemented a catch so you can use the /MKPASSWD <bcrypt> <password> as well.
 *
 * The reason I had to do it this was is due to how MKPASSWD returns hexsum() where as I wanted
 * to return sum() as IMO something like this: $2a$07$as097fda7f98d7f0d7sf97df8asf709a7f909
 * looks cleaner than : EA00000000000000000000000000
 *
 * todo: perhaps I'll write my own Blowfish class and tag it on to this as well, then we could move this out of extra
 * and do away with all external dpendancies.
 */

/* $ModDesc: Adds BCrypt support for Inspircd 2.0.xx */
/* $ModAuthor: Joseph Newing (synmuffin) */
/* $ModAuthorMail: jnewing@gmail.com */
/* $ModDepends: core 2.0 */

/* $LinkerFlags: -lxcrypt */

#include "inspircd.h"
#include "hash.h"

#include <xcrypt.h>

/* BCrypt HashProvider */
class BCryptProvider : public HashProvider
{

private:

	std::string generate_salt()
	{
		char salt[32], entrophy[16];

		for (unsigned int x = 0; x < sizeof(entrophy); x++)
			entrophy[x] = ServerInstance->GenRandomInt(0xff);

		if (!crypt_gensalt_rn("$2a$", WorkFactor, entrophy, sizeof(entrophy), salt, sizeof(salt)))
			return "";

 		return salt;
	}

	std::string compute_hash(const std::string &password, const std::string &salt)
	{
		char hash[64];

		if (!crypt_rn(password.c_str(), salt.c_str(), hash, 64))
			return "";

		return hash;
	}

public:

	unsigned int WorkFactor;

	std::string sum(const std::string &data)
	{
		return compute_hash(data, generate_salt());
	}

	std::string sumIV(unsigned int *IV, const char *HexMap, const std::string &sdata)
	{
		return sum(sdata);
	}

	BCryptProvider(Module *parent) : HashProvider(parent, "hash/bcrypt", 30, 64), WorkFactor(10) {}

};

/* /BCRYPT Command */
class CommandBCrypt : public Command
{

public:

	CommandBCrypt(Module *Creator) : Command(Creator, "BCRYPT", 1)
	{
		syntax = "<password>";
		Penalty = 5;
	}

	void BCryptHash(User* user, const std::string &data)
	{
		HashProvider* hp = ServerInstance->Modules->FindDataService<HashProvider>("hash/bcrypt");
		std::string hash = hp->sum(data);

		user->WriteServ("NOTICE %s :%s", user->nick.c_str(), hash.c_str());
	}

	CmdResult Handle (const std::vector<std::string>& parameters, User *user)
	{
		BCryptHash(user, parameters[0]);

		return CMD_SUCCESS;
	}

};

/* Module */
class ModuleBCrypt : public Module
{

private:

	CommandBCrypt cmd;		// BCRYPT Command
	BCryptProvider bcrypt;	// hash/bcrypt HashProvider

public:

	ModuleBCrypt() : cmd(this), bcrypt(this)
	{
	}

	void init()
	{
		OnRehash(NULL);

		ServerInstance->Modules->AddService(cmd);
		ServerInstance->Modules->AddService(bcrypt);

		Implementation eventlist[] = { I_OnPreCommand, I_OnPassCompare };
		ServerInstance->Modules->Attach(eventlist, this, sizeof(eventlist)/sizeof(Implementation));
	}

	void OnRehash(User* user)
	{
		ConfigTag *tag = ServerInstance->Config->ConfValue("bcrypt");
		bcrypt.WorkFactor = tag->getInt("workfactor", 8);
	}

	virtual ModResult OnPreCommand(std::string &command, std::vector<std::string> &parameters, LocalUser *user, bool validated, const std::string &original_line)
	{
		if (command == "MKPASSWD" &&
			parameters[0] == "bcrypt")
		{
			user->WriteServ("NOTICE %s :bcrypt hashed password for %s is %s",
				user->nick.c_str(), parameters[1].c_str(), bcrypt.sum(parameters[1]).c_str());

			return MOD_RES_DENY;
		}

		return MOD_RES_PASSTHRU;
	}

	virtual ModResult OnPassCompare(Extensible *ex, const std::string &data, const std::string &input, const std::string &hashtype)
	{
		if (hashtype != "bcrypt")
			return MOD_RES_PASSTHRU;

		HashProvider* hp = ServerInstance->Modules->FindDataService<HashProvider>("hash/" + hashtype);

		std::string hash = hp->sumIV(0, data.c_str(), input);

		if (!hash.empty() &&
			hash == data)
			return MOD_RES_ALLOW;

		return MOD_RES_DENY;
	}

	Version GetVersion()
	{
		return Version("Implements BCrypt hashing", VF_COMMON);
	}

};

MODULE_INIT(ModuleBCrypt)
