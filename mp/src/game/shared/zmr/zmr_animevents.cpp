#include "cbase.h"
#include "eventlist.h"
#include "activitylist.h"

void RegisterZMAnimEvents()
{
    // It seems you can't use AE_TYPE_SHARED
	REGISTER_SHARED_ANIMEVENT( AE_ZM_MELEEHIT, AE_TYPE_CLIENT | AE_TYPE_SERVER );
	REGISTER_SHARED_ANIMEVENT( AE_ZM_REVOLVERSHOOT, AE_TYPE_CLIENT | AE_TYPE_SERVER );
	REGISTER_SHARED_ANIMEVENT( AE_ZM_BEAMON, AE_TYPE_SERVER );
	REGISTER_SHARED_ANIMEVENT( AE_ZM_BEAMOFF, AE_TYPE_SERVER );
	REGISTER_SHARED_ANIMEVENT( AE_ZM_LIGHTERFLAME, AE_TYPE_SERVER );
	REGISTER_SHARED_ANIMEVENT( AE_ZM_CLOTHFLAME, AE_TYPE_SERVER );
}

void RegisterZMActivities()
{
    // NOTE: Has to be in the same order as ai_activity.h
    REGISTER_SHARED_ACTIVITY( ACT_RIFLE_RELOAD_START );
    REGISTER_SHARED_ACTIVITY( ACT_RIFLE_RELOAD_FINISH );
    REGISTER_SHARED_ACTIVITY( ACT_RIFLE_LEVER );
    
    REGISTER_SHARED_ACTIVITY( ACT_ZM_IDLE_SLEDGE );
    REGISTER_SHARED_ACTIVITY( ACT_ZM_RUN_SLEDGE );
    REGISTER_SHARED_ACTIVITY( ACT_ZM_IDLE_CROUCH_SLEDGE );
    REGISTER_SHARED_ACTIVITY( ACT_ZM_WALK_CROUCH_SLEDGE );
    REGISTER_SHARED_ACTIVITY( ACT_ZM_GESTURE_RANGE_ATTACK_SLEDGE );
    REGISTER_SHARED_ACTIVITY( ACT_ZM_JUMP_SLEDGE );
}