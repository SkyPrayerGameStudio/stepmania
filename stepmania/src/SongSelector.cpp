#include "stdafx.h"
/*
-----------------------------------------------------------------------------
 Class: SongSelector

 Desc: See header.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/

#include "SongSelector.h"
#include "RageLog.h"
#include "SongManager.h"
#include "GameState.h"
#include "ThemeManager.h"
#include "GameManager.h"

//
// Defines specific to SongSelector
//
const float GROUP_X				=	CENTER_X;
const float GROUP_Y				=	CENTER_Y - 160;

const float SONG_BANNER_X		=	CENTER_X;
const float SONG_BANNER_Y		=	CENTER_Y - 80;

const float SONG_BANNER_WIDTH	=	286;
const float SONG_BANNER_HEIGHT	=	92;

const float ARROWS_X[2]			=	{ SONG_BANNER_X - 200, SONG_BANNER_X + 200 };
const float ARROWS_Y[2]			=	{ SONG_BANNER_Y,       SONG_BANNER_Y };

const float SONG_TEXT_BANNER_X	=	CENTER_X;
const float SONG_TEXT_BANNER_Y	=	CENTER_Y - 10;

const float GAME_STYLE_X		=	CENTER_X;
const float GAME_STYLE_Y		=	CENTER_Y + 40;

const float STEPS_X				=	CENTER_X;
const float STEPS_Y				=	CENTER_Y + 90;

SongSelector::SongSelector()
{
	LOG->Trace( "ScreenEditMenu::ScreenEditMenu()" );

	NewNotes = false;

	m_textGroup.LoadFromFont( THEME->GetPathTo("Fonts","header1") );
	m_textGroup.SetXY( GROUP_X, GROUP_Y );
	m_textGroup.SetDiffuse( RageColor(0.7f,0.7f,0.7f,1) );
	m_textGroup.SetText( "blah" );
	this->AddChild( &m_textGroup );

	m_Banner.SetXY( SONG_BANNER_X, SONG_BANNER_Y );
	m_Banner.SetCroppedSize( SONG_BANNER_WIDTH, SONG_BANNER_HEIGHT );
	this->AddChild( &m_Banner );

	m_TextBanner.SetXY( SONG_TEXT_BANNER_X, SONG_TEXT_BANNER_Y );
	this->AddChild( &m_TextBanner );
	
	m_sprArrowLeft.Load( THEME->GetPathTo("Graphics","edit menu left") );
	m_sprArrowLeft.SetXY( ARROWS_X[0], ARROWS_Y[0] );
	m_sprArrowLeft.SetDiffuse( RageColor(1,1,1,0) );
	this->AddChild( &m_sprArrowLeft );

	m_sprArrowRight.Load( THEME->GetPathTo("Graphics","edit menu right") );
	m_sprArrowRight.SetXY( ARROWS_X[1], ARROWS_Y[1] );
	m_sprArrowRight.SetDiffuse( RageColor(1,1,1,0) );
	this->AddChild( &m_sprArrowRight );

	m_textStyle.LoadFromFont( THEME->GetPathTo("Fonts","header1") );
	m_textStyle.SetXY( GAME_STYLE_X, GAME_STYLE_Y );
	m_textStyle.SetDiffuse( RageColor(0.7f,0.7f,0.7f,1) );
	m_textStyle.SetText( "blah" );
	this->AddChild( &m_textStyle );

	m_textNotes.LoadFromFont( THEME->GetPathTo("Fonts","header1") );
	m_textNotes.SetXY( STEPS_X, STEPS_Y );
	m_textNotes.SetDiffuse( RageColor(0.7f,0.7f,0.7f,1) );
	m_textNotes.SetText( "blah" );
	this->AddChild( &m_textNotes );

	// data structures
	ChangeSelectedRow(ROW_GROUP);

	SONGMAN->GetGroupNames( m_sGroups );
	GAMEMAN->GetGameplayStylesForGame( GAMESTATE->m_CurGame, m_Styles, true );
	m_iSelectedGroup = m_iSelectedSong = m_iSelectedStyle = m_iSelectedNotes = 0;

	if( GAMESTATE->m_pCurSong )
	{
		unsigned i;

		for( i=0; i<m_sGroups.size(); i++ )
			if( GAMESTATE->m_pCurSong->m_sGroupName == m_sGroups[i] )
				m_iSelectedGroup = i;
		OnGroupChange();

		for( i=0; i<m_pSongs.size(); i++ )
			if( GAMESTATE->m_pCurSong == m_pSongs[i] )
				m_iSelectedSong = i;
		OnSongChange();

		for( i=0; i<m_Styles.size(); i++ )
		{
			if( GAMESTATE->GetCurrentStyleDef() == GAMEMAN->GetStyleDefForStyle(m_Styles[i]) )
				m_iSelectedStyle = i;
		}
		OnNotesTypeChange();

		for( i=0; i<m_pNotess.size(); i++ )
			if( GAMESTATE->m_pCurNotes[PLAYER_1] == m_pNotess[i] )
				m_iSelectedNotes = i;
		OnNotesChange();
	}
	else
		OnGroupChange();


	m_soundChangeMusic.Load( THEME->GetPathTo("Sounds","select music change music") );
}

SongSelector::~SongSelector()
{

}

void SongSelector::DrawPrimitives()
{
	ActorFrame::DrawPrimitives();
}

void SongSelector::Up()
{
	if( m_SelectedRow == 0 )	// can't go up any further
		return;	

	ChangeSelectedRow(SelectedRow(m_SelectedRow-1));
}

void SongSelector::Down()
{
	if( m_SelectedRow == NUM_ROWS-1 )	// can't go down any further
		return;	

	ChangeSelectedRow(SelectedRow(m_SelectedRow+1));
}

void SongSelector::Left()
{
	switch( m_SelectedRow )
	{
	case ROW_GROUP:
		if( m_iSelectedGroup == 0 )	// can't go left any further
			return;
		m_iSelectedGroup--;
		OnGroupChange();
		break;
	case ROW_SONG:
		if( m_iSelectedSong == 0 )	// can't go left any further
			return;
		m_iSelectedSong--;
		OnSongChange();
		break;
	case ROW_STYLE:
		if( m_iSelectedStyle == 0 )	// can't go left any further
			return;
		m_iSelectedStyle--;
		OnNotesTypeChange();
		break;
	case ROW_STEPS:
		if( m_iSelectedNotes == 0 )	// can't go left any further
			return;
		m_iSelectedNotes--;
		OnNotesChange();
		break;
	default:
		ASSERT(false);
	}
}

void SongSelector::Right()
{
	switch( m_SelectedRow )
	{
	case ROW_GROUP:
		if( m_iSelectedGroup == m_sGroups.size()-1 )	// can't go right any further
			return;
		m_iSelectedGroup++;
		OnGroupChange();
		break;
	case ROW_SONG:
		if( m_iSelectedSong == m_pSongs.size()-1 )	// can't go right any further
			return;
		m_iSelectedSong++;
		OnSongChange();
		break;
	case ROW_STYLE:
		if( m_iSelectedStyle == m_Styles.size()-1 )	// can't go right any further
			return;
		m_iSelectedStyle++;
		OnNotesTypeChange();
		break;
	case ROW_STEPS:
		if( m_iSelectedNotes == m_pNotess.size()-1 )	// can't go right any further
			return;
		m_iSelectedNotes++;
		OnNotesChange();
		break;
	default:
		ASSERT(false);
	}
}


void SongSelector::ChangeSelectedRow( SelectedRow row )
{
	m_textGroup.SetEffectNone();
	m_sprArrowLeft.SetDiffuse( RageColor(1,1,1,0) );
	m_sprArrowRight.SetDiffuse( RageColor(1,1,1,0) );
	m_textStyle.SetEffectNone();
	m_textNotes.SetEffectNone();
	m_SelectedRow = row;
	
	switch( m_SelectedRow )
	{
	case ROW_GROUP:			m_textGroup.SetEffectGlowing();			break;
	case ROW_SONG:
		m_sprArrowLeft.SetDiffuse( RageColor(1,1,1,1) );
		m_sprArrowRight.SetDiffuse( RageColor(1,1,1,1) );
		break;
	case ROW_STYLE:	m_textStyle.SetEffectGlowing(); 		break;
	case ROW_STEPS:			m_textNotes.SetEffectGlowing();			break;
	default:		ASSERT(false);
	}
}

void SongSelector::OnGroupChange()
{
	m_iSelectedGroup = clamp( m_iSelectedGroup, 0u, m_sGroups.size()-1 );

	m_textGroup.SetText( SONGMAN->ShortenGroupName(GetSelectedGroup()) );

	// reload songs
	m_pSongs.clear();
	SONGMAN->GetSongsInGroup( GetSelectedGroup(), m_pSongs );

	OnSongChange();
}

void SongSelector::OnSongChange()
{
	m_iSelectedSong = clamp( m_iSelectedSong, 0u, m_pSongs.size()-1 );

	m_Banner.LoadFromSong( GetSelectedSong() );
	m_TextBanner.LoadFromSong( GetSelectedSong() );

	OnNotesTypeChange();
}

void SongSelector::OnNotesTypeChange()
{
	m_iSelectedStyle = clamp( m_iSelectedStyle, 0u, m_Styles.size()-1 );

	m_textStyle.SetText( GAMEMAN->GetStyleDefForStyle(GetSelectedStyle())->m_szName );

	m_pNotess.clear();
	GetSelectedSong()->GetNotesThatMatch( GAMEMAN->GetStyleDefForStyle(GetSelectedStyle())->m_NotesType, m_pNotess );
	SortNotesArrayByDifficulty( m_pNotess );
	m_pNotess.push_back( NULL );		// marker for "(NEW)"
	m_iSelectedNotes = 0;

	OnNotesChange();
}

void SongSelector::OnNotesChange()
{
	m_iSelectedNotes = clamp( m_iSelectedNotes, 0u, m_pNotess.size()-1 );

	if( GetSelectedNotes() == NULL )
		m_textNotes.SetText( "(NEW)" );
	else
	{
		CString sDescription = GetSelectedNotes()->m_sDescription;
		if( sDescription == "" )
			sDescription = "[no name]";
		if( GetSelectedNotes()->m_bAutoGen )
			sDescription += " (autogen)";
		m_textNotes.SetText( sDescription );
	}
}

