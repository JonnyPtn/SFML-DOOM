module;
#include "p_mobj.h"
#include "p_pspr.h"
export module doom.player;

import doom.ticcmd;
import doom.definitions;

//
// Player states.
//
export enum playerstate_t
{
	// Playing or camping.
	PST_LIVE,
	// Dead on the ground, view follows killer.
	PST_DEAD,
	// Ready to restart/respawn???
	PST_REBORN

};

export struct player_t
{
	mobj_t* mo;
	playerstate_t playerstate;
	ticcmd_t cmd;

	// Determine POV,
	//  including viewpoint bobbing during movement.
	// Focal origin above r.z
	fixed_t viewz;
	// Base height above floor for viewz.
	fixed_t viewheight;
	// Bob/squat speed.
	fixed_t deltaviewheight;
	// bounded/scaled total momentum.
	fixed_t bob;

	// This is only used between levels,
	// mo->health is used during levels.
	int health;
	int armorpoints;
	// Armor type is 0-2.
	int armortype;

	// Power ups. invinc and invis are tic counters.
	int powers[NUMPOWERS];
	bool cards[NUMCARDS];
	bool backpack;

	// Frags, kills of other players.
	int frags[MAXPLAYERS];
	weapontype_t readyweapon;

	// Is wp_nochange if not changing.
	weapontype_t pendingweapon;

	bool weaponowned[NUMWEAPONS];
	int ammo[NUMAMMO];
	int maxammo[NUMAMMO];

	// True if button down last tic.
	int attackdown;
	int usedown;

	// Bit flags, for cheats and debug.
	// See cheat_t, above.
	int cheats;

	// Refired shots are less accurate.
	int refire;

	// For intermission stats.
	int killcount;
	int itemcount;
	int secretcount;

	// Hint messages.
	const char *message;

	// For screen flashing (red or bright).
	int damagecount;
	int bonuscount;

	// Who did damage (NULL for floors/ceilings).
	mobj_t *attacker;

	// So gun flashes light up areas.
	int extralight;

	// Current PLAYPAL, ???
	//  can be set to REDCOLORMAP for pain, etc.
	int fixedcolormap;

	// Player skin colorshift,
	//  0-3 for which color to draw player.
	int colormap;

	// Overlay view sprites (gun, etc).
	pspdef_t psprites[NUMPSPRITES];

	// True if secret level has been done.
	bool didsecret;

};

export inline player_t players[MAXPLAYERS];