#include "cbase.h"

#ifndef CLIENT_DLL
#include "items.h"
#endif


#include "zmr/zmr_shareddefs.h"
#include "zmr_base.h"


#include "zmr/zmr_player_shared.h"


#ifdef CLIENT_DLL
#define CZMWeaponMac10 C_ZMWeaponMac10
#endif

class CZMWeaponMac10 : public CZMBaseWeapon
{
public:
	DECLARE_CLASS( CZMWeaponMac10, CZMBaseWeapon );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

    CZMWeaponMac10();
    
    virtual void AddViewKick() OVERRIDE
    {
#define	EASY_DAMPEN			0.5f
#define	MAX_VERTICAL_KICK	2.0f	// Degrees
#define	SLIDE_LIMIT			1.0f	// Seconds
	
	    DoMachineGunKick( EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
    }
};

IMPLEMENT_NETWORKCLASS_ALIASED( ZMWeaponMac10, DT_ZM_WeaponMac10 )

BEGIN_NETWORK_TABLE( CZMWeaponMac10, DT_ZM_WeaponMac10 )
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CZMWeaponMac10 )
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_zm_mac10, CZMWeaponMac10 );
PRECACHE_WEAPON_REGISTER( weapon_zm_mac10 );

acttable_t	CZMWeaponMac10::m_acttable[] = 
{
    /*
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_PISTOL,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_PISTOL,						false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_PISTOL,				false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_PISTOL,				false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_PISTOL,			false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_PISTOL,					false },
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_PISTOL,					false },
    */
    { ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_PISTOL,              false },
    { ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_PISTOL,	    false },
    { ACT_MP_RUN,					    ACT_HL2MP_RUN_PISTOL,			    false },
    { ACT_MP_CROUCHWALK,			    ACT_HL2MP_WALK_CROUCH_PISTOL,	    false },
    { ACT_MP_ATTACK_STAND_PRIMARYFIRE,  ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,false },
    { ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,false },
    { ACT_MP_RELOAD_STAND,			    ACT_HL2MP_GESTURE_RELOAD_PISTOL,    false },
    { ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_PISTOL,    false },
    { ACT_MP_JUMP,					    ACT_HL2MP_JUMP_PISTOL,              false },
};
IMPLEMENT_ACTTABLE( CZMWeaponMac10 );

CZMWeaponMac10::CZMWeaponMac10()
{
    m_bFiresUnderwater = false;

    SetSlotFlag( ZMWEAPONSLOT_LARGE );
    SetConfigSlot( ZMWeaponConfig::ZMCONFIGSLOT_MAC10 );
}
