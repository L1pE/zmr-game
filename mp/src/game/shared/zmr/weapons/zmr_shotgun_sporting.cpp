#include "cbase.h"

#ifndef CLIENT_DLL
#include "items.h"
#else
#include "prediction.h"
#endif

#include "zmr_basepump.h"


#ifdef CLIENT_DLL
#define CZMWeaponShotgunSporting C_ZMWeaponShotgunSporting
#endif

class CZMWeaponShotgunSporting : public CZMBaseWeapon
{
public:
    DECLARE_CLASS( CZMWeaponShotgunSporting, CZMBaseWeapon );
    DECLARE_NETWORKCLASS();
    DECLARE_PREDICTABLE();
    DECLARE_ACTTABLE();

    CZMWeaponShotgunSporting();

#ifdef CLIENT_DLL
    virtual CZMBaseCrosshair* GetWeaponCrosshair() const OVERRIDE { return ZMGetCrosshair( "ShotgunSporting" ); }
#endif


#ifndef CLIENT_DLL
    const char* GetDropAmmoName() const OVERRIDE { return "item_box_buckshot"; }
    int GetDropAmmoAmount() const OVERRIDE { return SIZE_AMMO_BUCKSHOT; }
#endif

    bool IsFiringBothBarrels() const { return GetActivity() == ACT_VM_SECONDARYATTACK; }


    virtual void PrimaryAttack() OVERRIDE;
    virtual void SecondaryAttack() OVERRIDE;
    virtual bool Reload() OVERRIDE;


    void ShootBarrels( bool bWantBoth );


    virtual const Vector& GetBulletSpread( void ) OVERRIDE
    {
        static Vector cone = VECTOR_CONE_6DEGREES;
        static Vector cone2 = VECTOR_CONE_15DEGREES;

        return IsFiringBothBarrels() ? cone2 : cone;
    }
    
    virtual void AddViewKick( void ) OVERRIDE
    {
        CZMPlayer* pPlayer = ToZMPlayer( GetOwner() );

        if ( !pPlayer ) return;


        QAngle viewPunch;

        if ( IsFiringBothBarrels() )
        {
            viewPunch.x = SharedRandomFloat( "sportingpax", -16.0f, -8.0f );
            viewPunch.y = SharedRandomFloat( "sportingpay", -4.0f, 4.0f );
        }
        else
        {
            viewPunch.x = SharedRandomFloat( "sportingpax", -8.0f, -6.0f );
            viewPunch.y = SharedRandomFloat( "sportingpay", -4.0f, 4.0f );
        }

        viewPunch.z = 0.0f;

        pPlayer->ViewPunch( viewPunch );
    }
    

    virtual int GetBulletsPerShot() const OVERRIDE { return 7; }
    virtual float GetFireRate( void ) OVERRIDE { return 0.45f; }
};

IMPLEMENT_NETWORKCLASS_ALIASED( ZMWeaponShotgunSporting, DT_ZM_WeaponShotgunSporting )

BEGIN_NETWORK_TABLE( CZMWeaponShotgunSporting, DT_ZM_WeaponShotgunSporting )
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CZMWeaponShotgunSporting )
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_zm_shotgun_sporting, CZMWeaponShotgunSporting );

PRECACHE_WEAPON_REGISTER( weapon_zm_shotgun_sporting );

acttable_t CZMWeaponShotgunSporting::m_acttable[] = 
{
    { ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SHOTGUN,                 false },
    { ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SHOTGUN,          false },
    { ACT_MP_RUN,					    ACT_HL2MP_RUN_SHOTGUN,                  false },
    { ACT_MP_CROUCHWALK,			    ACT_HL2MP_WALK_CROUCH_SHOTGUN,          false },
    { ACT_MP_ATTACK_STAND_PRIMARYFIRE,  ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN, false },
    { ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SHOTGUN, false },
    { ACT_MP_RELOAD_STAND,			    ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,       false },
    { ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SHOTGUN,       false },
    { ACT_MP_JUMP,					    ACT_HL2MP_JUMP_SHOTGUN,                 false },
};
IMPLEMENT_ACTTABLE( CZMWeaponShotgunSporting );

CZMWeaponShotgunSporting::CZMWeaponShotgunSporting()
{
    m_fMinRange1 = m_fMinRange2 = 0.0f;
    m_fMaxRange1 = 800.0f;
    m_fMaxRange2 = 500.0f;


    m_bFiresUnderwater = false;

    SetSlotFlag( ZMWEAPONSLOT_LARGE );
}

void CZMWeaponShotgunSporting::PrimaryAttack()
{
    ShootBarrels( false );
}

void CZMWeaponShotgunSporting::SecondaryAttack( void )
{
    ShootBarrels( true );
}

void CZMWeaponShotgunSporting::ShootBarrels( bool bWantBoth )
{
    if ( !CanAct() ) return;


    if ( m_flNextPrimaryAttack > gpGlobals->curtime )
        return;


    // If my clip is empty (and I use clips) start reload
    if ( UsesClipsForAmmo1() && !m_iClip1 ) 
    {
        Reload();
        return;
    }


    bool bShootSingle = m_iClip1 < 2 || !bWantBoth;


    float delay = GetFireRate();

    m_flNextSecondaryAttack = gpGlobals->curtime + delay;
    m_flNextPrimaryAttack = m_flNextSecondaryAttack;
    

    if ( bShootSingle )
    {
        Shoot( -1, -1, -1, m_fMaxRange1 );
    }
    else
    {
        Shoot( m_iPrimaryAmmoType, GetBulletsPerShot() * 2, 2, m_fMaxRange2, true, true );
    }
}

bool CZMWeaponShotgunSporting::Reload()
{
    bool bReloadSingle = m_iClip1 >= 1;

    return DefaultReload( GetMaxClip1(), GetMaxClip2(), bReloadSingle ? ACT_VM_RELOAD_START : ACT_VM_RELOAD );
}