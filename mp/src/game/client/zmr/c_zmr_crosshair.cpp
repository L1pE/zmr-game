#include "cbase.h"
#include "gamestringpool.h"
#include <filesystem.h>

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Controls.h>


#include "c_zmr_player.h"
#include "c_zmr_crosshair.h"



#define CROSSHAIRFILE_DEFAULT       "resource/zmcrosshairs_default.txt"
#define CROSSHAIRFILE               "resource/zmcrosshairs.txt"



ConVar zm_cl_zmcrosshair( "zm_cl_zmcrosshair", "0", FCVAR_ARCHIVE, "Do we display the crosshair while being the ZM?" );


CZMCrosshairSystem g_ZMCrosshairs;


CZMCrosshairSystem::CZMCrosshairSystem() : CAutoGameSystem( "CrosshairSystem" )
{
}

bool CZMCrosshairSystem::Init()
{
    LoadFiles();
    return true;
}

CZMBaseCrosshair* CZMCrosshairSystem::CreateCrosshairFromData( KeyValues* kv )
{
    if ( !kv )
        return nullptr;


    const char* type = kv->GetString( "type" );

    if ( Q_stricmp( type, "material" ) == 0 )
    {
        return new CZMMaterialCrosshair;
    }
    else if ( Q_stricmp( type, "dynamiccrosshair" ) == 0 )
    {
        return new CZMAccuracyCrosshair;
    }
    /*else if ( Q_stricmp( type, "pistol" ) == 0 )
    {
        return new CZMPistolCrosshair;
    }*/
    else if ( Q_stricmp( type, "dotonly" ) == 0 )
    {
        return new CZMDotCrosshair;
    }
    else if ( Q_stricmp( type, "none" ) == 0 )
    {
        return new CZMEmptyCrosshair;
    }

    return nullptr;
}

void CZMCrosshairSystem::LoadFiles()
{
    KeyValues* kv;

    // Try to load player's customized crosshairs first.
    kv = new KeyValues( "Crosshairs" );
    kv->LoadFromFile( filesystem, CROSSHAIRFILE );

    ReadCrosshairs( kv );

    kv->deleteThis();



    // Load defaults afterwards.
    kv = new KeyValues( "Crosshairs" );
    kv->LoadFromFile( filesystem, CROSSHAIRFILE_DEFAULT );

    ReadCrosshairs( kv );

    kv->deleteThis();
}

void CZMCrosshairSystem::WriteCrosshairsToFile() const
{
    KeyValues* kv = new KeyValues( "Crosshairs" );


    FOR_EACH_VEC( m_vCrosshairs, i )
    {
        KeyValues* mykv = kv->FindKey( m_vCrosshairs[i]->GetName(), true );

        m_vCrosshairs[i]->WriteValues( mykv );
    }


    if ( !kv->SaveToFile( filesystem, CROSSHAIRFILE ) )
    {
        Warning( "Couldn't save crosshairs to disk!\n" );
    }

    kv->deleteThis();
}

void CZMCrosshairSystem::ReadCrosshairs( KeyValues* kv )
{
    if ( !kv )
        return;


    KeyValues* crosshair = kv->GetFirstTrueSubKey();
    if ( !crosshair )
        return;


    do
    {
        if ( AddCrosshair( crosshair ) == -1 )
        {
            Warning( "Couldn't init crosshair of type: '%s'!\n", crosshair->GetString( "type", "N/A" ) );
        }
    }
    while( (crosshair = crosshair->GetNextTrueSubKey()) != nullptr );
}

int CZMCrosshairSystem::AddCrosshair( KeyValues* kv )
{
    int index = FindCrosshairByName( kv->GetName() );
    if ( index != -1 )
        return index;


    CZMBaseCrosshair* pCross = CreateCrosshairFromData( kv );

    if ( pCross )
    {
        pCross->LoadValues( kv );
        return m_vCrosshairs.AddToTail( pCross );
    }

    return -1;
}

CZMBaseCrosshair* CZMCrosshairSystem::GetCrosshairByName( const char* name ) const
{
    int index = FindCrosshairByName( name );

    return ( index != -1 ) ? m_vCrosshairs[index] : nullptr;
}

int CZMCrosshairSystem::FindCrosshairByName( const char* name ) const
{
    FOR_EACH_VEC( m_vCrosshairs, i )
    {
        const char* crsname = STRING( m_vCrosshairs[i]->GetName() );
        if ( Q_stricmp( name, crsname ) == 0 )
            return i;
    }

    return -1;
}

using namespace vgui;


//
CZMBaseCrosshair::CZMBaseCrosshair()
{
    m_szName = nullptr;
    m_szMenuName = nullptr;

    m_flOverrideCenterX = m_flOverrideCenterY = -1.0f;
}

CZMBaseCrosshair::~CZMBaseCrosshair()
{
    delete[] m_szName;
    m_szName = nullptr;
    delete[] m_szMenuName;
    m_szMenuName = nullptr;
}

void CZMBaseCrosshair::LoadValues( KeyValues* kv )
{
    int len;

    // AllocPooledString causes invalid strings so we have to do this ourselves.
    const char* name = kv->GetName();
    len = strlen( name ) + 1;
    m_szName = new char[len];
    Q_strncpy( m_szName, name, len );
    

    const char* menuname = kv->GetString( "name" );
    len = strlen( menuname ) + 1;
    m_szMenuName = new char[len];
    Q_strncpy( m_szMenuName, menuname, len );


    m_bDisplayInMenu = kv->GetBool( "displayinmenu", true );

    m_flOutlineSize = kv->GetFloat( "outline" );
    m_flOffsetFromCenter = kv->GetFloat( "offsetfromcenter" );
    m_flDotSize = kv->GetFloat( "dot" );


    // By default the GetColor will load 0,0,0,0 which is not good for us.
    if ( kv->FindKey( "color", false ) )
    {
        m_Color = kv->GetColor( "color" );
    }
    else
    {
        m_Color = Color( 255, 255, 255, 255 );
    }
    

    if ( kv->FindKey( "outlinecolor", false ) )
    {
        m_OutlineColor = kv->GetColor( "outlinecolor" );
    }
    else
    {
        m_OutlineColor = Color( 0, 0, 0, 255 );
    }
}

void CZMBaseCrosshair::WriteValues( KeyValues* kv ) const
{
    kv->SetBool( "displayinmenu", DisplayInMenu() );
    kv->SetString( "name", GetMenuName() );

    kv->SetFloat( "outline", GetOutlineWidth() );
    kv->SetFloat( "offsetfromcenter", GetOffsetFromCenter() );
    kv->SetFloat( "dot", GetDotSize() );


    char temp[64];
    Color clr;
    
    clr = GetMainColor();
    Q_snprintf( temp, sizeof( temp ), "%i %i %i %i", (int)clr[0], (int)clr[1], (int)clr[2], (int)clr[3] );
    kv->SetString( "color", temp );

    clr = GetOutlineColor();
    Q_snprintf( temp, sizeof( temp ), "%i %i %i %i", (int)clr[0], (int)clr[1], (int)clr[2], (int)clr[3] );
    kv->SetString( "outlinecolor", temp );
}

const Color& CZMBaseCrosshair::GetMainColor() const
{
    return m_Color;
}

const Color& CZMBaseCrosshair::GetOutlineColor() const
{
    return m_OutlineColor;
}

void CZMBaseCrosshair::GetDrawPosition( float& flPosX, float& flPosY ) const
{
    // Something else wants to draw somewhere else.
    if ( m_flOverrideCenterX >= 0.0f && m_flOverrideCenterY >= 0.0f )
    {
        flPosX = m_flOverrideCenterX;
        flPosY = m_flOverrideCenterY;
        return;
    }

    int vx, vy, vw, vh;
    surface()->GetFullscreenViewport( vx, vy, vw, vh );

    flPosX = vw / 2;
    flPosY = vh / 2;
}

void CZMBaseCrosshair::DrawDot()
{
    float dot = GetDotSize();
    if ( dot <= 0.0f )
        return;


    const Color clr = GetMainColor();
    const Color outlineclr = GetOutlineColor();

    float cx, cy;
    GetDrawPosition( cx, cy );


    dot /= 2.0f;

    float outw = GetOutlineWidth();

    /*
    surface()->DrawSetColor( outlineclr );
    surface()->DrawFilledRect(
        cx - dot - outw,
        cy - dot - outw,
        cx + dot + outw,
        cy + dot + outw );

    surface()->DrawSetColor( clr );
    surface()->DrawFilledRect(
        cx - dot,
        cy - dot,
        cx + dot,
        cy + dot );
    */

    if ( outw > 0.0f )
    {
        surface()->DrawSetColor( outlineclr );
        DrawMaterial(
            "zmr_crosshairs/dot",
            cx - dot - outw,
            cy - dot - outw,
            cx + dot + outw,
            cy + dot + outw );
    }

    surface()->DrawSetColor( clr );
    DrawMaterial(
        "zmr_crosshairs/dot",
        cx - dot,
        cy - dot,
        cx + dot,
        cy + dot );
}
//


//
void CZMEmptyCrosshair::WriteValues( KeyValues* kv ) const
{
    CZMBaseCrosshair::WriteValues( kv );

    kv->SetString( "type", "none" );
}

void CZMEmptyCrosshair::Draw()
{

}
//


//
void CZMDotCrosshair::WriteValues( KeyValues* kv ) const
{
    CZMBaseCrosshair::WriteValues( kv );

    kv->SetString( "type", "dotonly" );
}

void CZMDotCrosshair::Draw()
{
    DrawDot();
}
//


//
CZMBaseDynamicCrosshair::CZMBaseDynamicCrosshair()
{
    m_flOverrideDynamicScale = -1.0f;
}

void CZMBaseDynamicCrosshair::LoadValues( KeyValues* kv )
{
    CZMBaseCrosshair::LoadValues( kv );

    m_flDynamicMove = kv->GetFloat( "dynamicmove" );
}

void CZMBaseDynamicCrosshair::WriteValues( KeyValues* kv ) const
{
    CZMBaseCrosshair::WriteValues( kv );

    kv->SetFloat( "dynamicmove", m_flDynamicMove );
}
//


//
void CZMPistolCrosshair::LoadValues( KeyValues* kv )
{
    CZMBaseCrosshair::LoadValues( kv );
}

void CZMPistolCrosshair::Draw()
{
    /*
    const Color clr = GetCrosshairColor();
    const Color outlineclr = GetCrosshairColor2();


    float cx, cy;
    GetDrawPosition( cx, cy );


    
    
    float outlinewidth = GetOutlineWidth();
    float width = 2;
    float length = 2;

    float hw = width / 2.0f;

    float offset = GetOffsetFromCenter();



    IntRect rect[4];

    // Left
    rect[0].x0 = cx - offset - length;
    rect[0].y0 = cy - hw;
    rect[0].x1 = cx - offset;
    rect[0].y1 = cy + hw;

    // Right
    rect[1].x0 = cx + offset;
    rect[1].y0 = cy - hw;
    rect[1].x1 = cx + offset + length;
    rect[1].y1 = cy + hw;

    // Top
    rect[2].x0 = cx - hw;
    rect[2].y0 = cy - offset - length;
    rect[2].x1 = cx + hw;
    rect[2].y1 = cy - offset;

    // Bottom
    rect[3].x0 = cx - hw;
    rect[3].y0 = cy + offset;
    rect[3].x1 = cx + hw;
    rect[3].y1 = cy + offset + length;
    if ( outlinewidth > 0 )
    {
        float outw = GetOutlineWidth();

        IntRect rectout[4];
        COMPILE_TIME_ASSERT( ARRAYSIZE( rect ) <= ARRAYSIZE( rectout ) );

        for ( int i = 0; i < ARRAYSIZE( rect ); i++ )
        {
            rectout[i].x0 = rect[i].x0 - outw;
            rectout[i].y0 = rect[i].y0 - outw;
            rectout[i].x1 = rect[i].x1 + outw;
            rectout[i].y1 = rect[i].y1 + outw;
        }
        
        surface()->DrawSetColor( outlineclr );
        surface()->DrawFilledRectArray( rectout, ARRAYSIZE( rectout ) );
    }


    surface()->DrawSetColor( clr );
    surface()->DrawFilledRectArray( rect, ARRAYSIZE( rect ) );
    */

    DrawDot();
}
//


//
CZMMaterialCrosshair::CZMMaterialCrosshair()
{
    m_nTexMat0Id = 0;
    *m_szTexture = NULL;
}

void CZMMaterialCrosshair::LoadValues( KeyValues* kv )
{
    CZMBaseCrosshair::LoadValues( kv );


    const char* mat = kv->GetString( "material0" );

    Q_strncpy( m_szTexture, mat, sizeof( m_szTexture ) );
}

void CZMMaterialCrosshair::WriteValues( KeyValues* kv ) const
{
    CZMBaseCrosshair::WriteValues( kv );


    kv->SetString( "type", "material" );
    kv->SetString( "material0", m_szTexture );
}

void CZMMaterialCrosshair::Draw()
{
    const Color clr = GetMainColor();

    float cx, cy;
    GetDrawPosition( cx, cy );

    float offset = GetOffsetFromCenter();

    bool bHasTexture = *m_szTexture != NULL;

    if ( bHasTexture )
    {
        if ( m_nTexMat0Id == 0 )
        {
            m_nTexMat0Id = surface()->CreateNewTextureID();
            surface()->DrawSetTextureFile( m_nTexMat0Id, m_szTexture, true, false );
        }

        if ( offset > 0 )
        {
            surface()->DrawSetColor( clr );
            surface()->DrawSetTexture( m_nTexMat0Id );
            surface()->DrawTexturedRect(
                cx - offset,
                cy - offset,
                cx + offset,
                cy + offset );
        }
    }




    /*
    if ( m_hFont == NULL )
    {
        m_hFont = scheme()->GetIScheme( scheme()->GetScheme( "ClientScheme" ) )->GetFont( "ZMCrosshairSmall", false );

        if ( !m_hFont )
        {
            //Warning( "Couldn't find crosshair font!\n" );
            return;
        }
    }


    int h = surface()->GetFontTall( m_hFont );

    surface()->DrawSetTextColor( circleclr );
    surface()->DrawSetTextPos( cx - h / 2, cy - h / 2 );
    surface()->DrawSetTextFont( m_hFont );
    surface()->DrawUnicodeChar( m_wChar );
    */


    /*
    float offset = GetOffsetFromCenter();

    //int segs = 48;
    int segs = 32;

    surface()->DrawSetColor( circleclr );
    surface()->DrawOutlinedCircle( cx, cy, offset, segs );
    */

    DrawDot();
}
//


//
void CZMAccuracyCrosshair::LoadValues( KeyValues* kv )
{
    CZMBaseDynamicCrosshair::LoadValues( kv );

    m_flWidth = kv->GetFloat( "width", 2.0f );
    m_flLength = kv->GetFloat( "length", 8.0f );
}

void CZMAccuracyCrosshair::WriteValues( KeyValues* kv ) const
{
    CZMBaseDynamicCrosshair::WriteValues( kv );

    kv->SetString( "type", "dynamiccrosshair" );
    kv->SetFloat( "width", m_flWidth );
    kv->SetFloat( "length", m_flLength );
}

float CZMAccuracyCrosshair::GetWidth() const
{
    return m_flWidth;
}

float CZMAccuracyCrosshair::GetLength() const
{
    return m_flLength;
}

float CZMAccuracyCrosshair::GetDynamicMoveScale() const
{
    if ( m_flOverrideDynamicScale >= 0.0f )
        return m_flOverrideDynamicScale;

    C_ZMPlayer* pPlayer = C_ZMPlayer::GetLocalPlayer();
    if ( pPlayer )
    {
        /*
        if ( !pPlayer->IsAlive() )
        {
            C_ZMPlayer* pObserved = ( pPlayer->GetObserverMode() == OBS_MODE_IN_EYE ) ? ToZMPlayer( pPlayer->GetObserverTarget() ) : nullptr;
            if ( pObserved )
            {
                return 1.0f - pObserved->GetAccuracyRatio();
            }
        }
        */


        return 1.0f - pPlayer->GetAccuracyRatio();
    }

    return 0.0f;
}

void CZMAccuracyCrosshair::Draw()
{
    const Color clr = GetMainColor();
    const Color outlineclr = GetOutlineColor();


    float cx, cy;
    GetDrawPosition( cx, cy );


    
    
    float outlinewidth = GetOutlineWidth();
    float width = GetWidth();
    float length = GetLength();

    float hw = width / 2.0f;

    float offset = GetOffsetFromCenter();
    
    float offsetdyn = offset + GetMaxDynamicMove() * GetDynamicMoveScale();
    
    
    IntRect rect[2];

    // Left
    rect[0].x0 = cx - offsetdyn - length;
    rect[0].y0 = cy - hw;
    rect[0].x1 = cx - offsetdyn;
    rect[0].y1 = cy + hw;

    // Right
    rect[1].x0 = cx + offsetdyn;
    rect[1].y0 = cy - hw;
    rect[1].x1 = cx + offsetdyn + length;
    rect[1].y1 = cy + hw;

    // Top
    //rect[2].x0 = cx - hw;
    //rect[2].y0 = cy - offset - length;
    //rect[2].x1 = cx + hw;
    //rect[2].y1 = cy - offset;

    // Bottom
    //rect[3].x0 = cx - hw;
    //rect[3].y0 = cy + offset;
    //rect[3].x1 = cx + hw;
    //rect[3].y1 = cy + offset + length;
    if ( outlinewidth > 0 )
    {
        float outw = GetOutlineWidth();

        IntRect rectout[2];
        COMPILE_TIME_ASSERT( ARRAYSIZE( rect ) <= ARRAYSIZE( rectout ) );

        for ( int i = 0; i < ARRAYSIZE( rect ); i++ )
        {
            rectout[i].x0 = rect[i].x0 - outw;
            rectout[i].y0 = rect[i].y0 - outw;
            rectout[i].x1 = rect[i].x1 + outw;
            rectout[i].y1 = rect[i].y1 + outw;
        }
        
        surface()->DrawSetColor( outlineclr );
        surface()->DrawFilledRectArray( rectout, ARRAYSIZE( rectout ) );
    }


    surface()->DrawSetColor( clr );
    surface()->DrawFilledRectArray( rect, ARRAYSIZE( rect ) );


    DrawDot();
}

CZMCrosshairPartMat::CZMCrosshairPartMat()
{
    m_nTexId = 0;
}

void CZMCrosshairPartMat::DrawMaterial( const char* pszMat, int x0, int y0, int x1, int y1 )
{
    if ( m_nTexId == 0 )
    {
        m_nTexId = surface()->CreateNewTextureID();
        surface()->DrawSetTextureFile( m_nTexId, pszMat, true, false );
    }

    surface()->DrawSetTexture( m_nTexId );
    surface()->DrawTexturedRect( x0, y0, x1, y1 );
}
