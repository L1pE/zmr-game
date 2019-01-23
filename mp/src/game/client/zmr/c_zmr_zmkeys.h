#pragma once


#define KEYCONFIG_NAME_ZM           "keys_zm"
#define KEYCONFIG_NAME_SURVIVOR     "keys_survivor"

#define KEYCONFIG_ZM                "cfg/"KEYCONFIG_NAME_ZM".cfg"
#define KEYCONFIG_SURVIVOR          "cfg/"KEYCONFIG_NAME_SURVIVOR".cfg"

#define KEYCONFIG_ZM_DEF            "cfg/"KEYCONFIG_NAME_ZM"_default.cfg"
#define KEYCONFIG_SURVIVOR_DEF      "cfg/"KEYCONFIG_NAME_SURVIVOR"_default.cfg"



struct zmkeydata_t
{
    ButtonCode_t key;
    char cmd[128];
};

enum ZMKeyTeam_t
{
    KEYTEAM_NEUTRAL = 0, // It can be used by any team
    KEYTEAM_SURVIVOR, // Survivor/spectator
    KEYTEAM_ZM,
};


typedef CUtlVector<zmkeydata_t*> zmkeydatalist_t;

class CZMTeamKeysConfig
{
public:
    CZMTeamKeysConfig();
    ~CZMTeamKeysConfig();


    static bool IsZMCommand( const char* cmd );
    static bool IsSurvivorCommand( const char* cmd );
    static bool IsNeutralCommand( const char* cmd );

    static ZMKeyTeam_t GetCommandType( const char* cmd );


    static bool LoadConfigByTeam( ZMKeyTeam_t team, zmkeydatalist_t& list );

    static bool ParseConfig( const char* cfg, zmkeydatalist_t& list );
    static bool SaveConfig( const char* cfg, const zmkeydatalist_t& list );

    // Force survivor config execution no matter what.
    static void ExecuteTeamConfig( bool bForce = false );
    static void ExecuteTeamConfig( int iTeam );


    static zmkeydata_t* FindKeyDataFromList( const char* cmd, zmkeydatalist_t& list );
    static zmkeydata_t* FindKeyDataFromListByKey( ButtonCode_t key, zmkeydatalist_t& list );
};
