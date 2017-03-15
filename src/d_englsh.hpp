#pragma once

#include <string>

//
//	Printed strings for translation
//
const std::string s_PressKey                ("press a key.");
const std::string s_PressYorN               ("press y or n.");
const std::string s_QuitMsg                 ("are you sure you want to\nquit this great game?");
const std::string s_CantLoadNet             ("you can't do load while in a net game!\n\n" + s_PressKey);
const std::string s_CantQloadNet            ("you can't quickload during a netgame!\n\n" + s_PressKey);
const std::string s_HaventPickedQsaveSlot   ("you haven't picked a quicksave slot yet!\n\n" + s_PressKey);
const std::string s_CantSaveDead            ("you can't save if you aren't playing!\n\n" + s_PressKey);
const std::string s_QsaveConfirm            ("quicksave over your game named\n\n'%s'?\n\n" + s_PressYorN);
const std::string s_QloadConfirm            ("do you want to quickload the game named\n\n'%s'?\n\n" + s_PressYorN);

const std::string s_CantNewNet(\
    "you can't start a new game\n"\
    "while in a network game.\n\n" + s_PressKey);

const std::string s_NightmareConfirm(\
    "are you sure? this skill level\n"\
    "isn't even remotely fair.\n\n" + s_PressYorN);

const std::string s_Shareware(\
    "this is the shareware version of doom.\n\n"\
    "you need to order the entire trilogy.\n\n" + s_PressKey);

const std::string s_MessagesOff ("Messages OFF");
const std::string s_MessagesOn  ("Messages ON");
const std::string s_CantEndNet  ("you can't end a netgame!\n\n" + s_PressKey);
const std::string s_EndConfirm  ("are you sure you want to end the game?\n\n" + s_PressYorN);

const std::string s_YtoQuit("(press y to quit)");

const std::string s_DetailHigh      ("High detail");
const std::string s_DetailLow       ("Low detail");
const std::string s_GammaLevelZero	("Gamma correction OFF");
const std::string s_GammaLevelOne	("Gamma correction level 1");
const std::string s_GammaLevelTwo	("Gamma correction level 2");
const std::string s_GammaLevelThree	("Gamma correction level 3");
const std::string s_GammaLevelFour	("Gamma correction level 4");
const std::string s_EmptySlot	    ("empty slot");

//
//	P_inter.C
//
const std::string s_GotArmour	    ("Picked up the armor.");
const std::string s_GotMega	        ("Picked up the MegaArmor!");
const std::string s_GotHealthBonus	("Picked up a health bonus.");
const std::string s_GotArmourBonus	("Picked up an armor bonus.");
const std::string s_GotStimpack	    ("Picked up a stimpack.");
const std::string s_GotMedikitNeed	("Picked up a medikit that you REALLY need!");
const std::string s_GotMedikit	    ("Picked up a medikit.");
const std::string s_GotSuper	    ("Supercharge!");

const std::string s_GotBlueCard	    ("Picked up a blue keycard.");
const std::string s_GotYellowCard	("Picked up a yellow keycard.");
const std::string s_GotRedCard	    ("Picked up a red keycard.");
const std::string s_GotBlueSkull	("Picked up a blue skull key.");
const std::string s_GotYellowSkull	("Picked up a yellow skull key.");
const std::string s_GotRedSkull	    ("Picked up a red skull key.");

const std::string s_GotInvulnerability	("Invulnerability!");
const std::string s_GotBerserk	        ("Berserk!");
const std::string s_GotInvisibility	    ("Partial Invisibility");
const std::string s_GotRadiationSuit	("Radiation Shielding Suit");
const std::string s_GotMap	            ("Computer Area Map");
const std::string s_GotVisor	        ("Light Amplification Visor");
const std::string s_GotMegaSphere	    ("MegaSphere!");

const std::string s_GotClip	        ("Picked up a clip.");
const std::string s_GotClipBox	    ("Picked up a box of bullets.");
const std::string s_GotRocket	    ("Picked up a rocket.");
const std::string s_GotRocketBox	("Picked up a box of rockets.");
const std::string s_GotCell	        ("Picked up an energy cell.");
const std::string s_GotCellBox	    ("Picked up an energy cell pack.");
const std::string s_GotShells	    ("Picked up 4 shotgun shells.");
const std::string s_GotShellBox	    ("Picked up a box of shotgun shells.");
const std::string s_GotBackpack	    ("Picked up a backpack full of ammo!");

const std::string s_GotBFG9000	    ("You got the BFG9000!  Oh, yes.");
const std::string s_GotChaingun	    ("You got the chaingun!");
const std::string s_GotChainsaw	    ("A chainsaw!  Find some meat!");
const std::string s_GotLauncher	    ("You got the rocket launcher!");
const std::string s_GotPlasma	    ("You got the plasma gun!");
const std::string s_GotShotgun	    ("You got the shotgun!");
const std::string s_GotSuperShotgun	("You got the super shotgun!");

const std::string s_BlueKeyNeeded	("You need a blue key to activate this object");
const std::string s_RedKeyNeeded	("You need a red key to activate this object");
const std::string s_YellowKeyNeeded	("You need a yellow key to activate this object");
const std::string s_BlueKeyNeededDoor	("You need a blue key to open this door");
const std::string s_RedKeyNeededDoor	("You need a red key to open this door");
const std::string s_YellowKeyNeededDoor	("You need a yellow key to open this door");

const std::string s_GameSaved	    ("game saved.");

//
//	HU_stuff.C
//
const std::string s_MessageUnsent("[Message unsent]");

const std::string s_E1M1	("E1M1: Hangar");
const std::string s_E1M2	("E1M2: Nuclear Plant");
const std::string s_E1M3	("E1M3: Toxin Refinery");
const std::string s_E1M4	("E1M4: Command Control");
const std::string s_E1M5	("E1M5: Phobos Lab");
const std::string s_E1M6	("E1M6: Central Processing");
const std::string s_E1M7	("E1M7: Computer Station");
const std::string s_E1M8	("E1M8: Phobos Anomaly");
const std::string s_E1M9	("E1M9: Military Base");

const std::string s_E2M1	("E2M1: Deimos Anomaly");
const std::string s_E2M2	("E2M2: Containment Area");
const std::string s_E2M3	("E2M3: Refinery");
const std::string s_E2M4	("E2M4: Deimos Lab");
const std::string s_E2M5	("E2M5: Command Center");
const std::string s_E2M6	("E2M6: Halls of the Damned");
const std::string s_E2M7	("E2M7: Spawning Vats");
const std::string s_E2M8	("E2M8: Tower of Babel");
const std::string s_E2M9	("E2M9: Fortress of Mystery");

const std::string s_E3M1	("E3M1: Hell Keep");
const std::string s_E3M2	("E3M2: Slough of Despair");
const std::string s_E3M3	("E3M3: Pandemonium");
const std::string s_E3M4	("E3M4: House of Pain");
const std::string s_E3M5	("E3M5: Unholy Cathedral");
const std::string s_E3M6	("E3M6: Mt. Erebus");
const std::string s_E3M7	("E3M7: Limbo");
const std::string s_E3M8	("E3M8: Dis");
const std::string s_E3M9	("E3M9: Warrens");

const std::string s_E4M1	("E4M1: Hell Beneath");
const std::string s_E4M2	("E4M2: Perfect Hatred");
const std::string s_E4M3	("E4M3: Sever The Wicked");
const std::string s_E4M4	("E4M4: Unruly Evil");
const std::string s_E4M5	("E4M5: They Will Repent");
const std::string s_E4M6	("E4M6: Against Thee Wickedly");
const std::string s_E4M7	("E4M7: And Hell Followed");
const std::string s_E4M8	("E4M8: Unto The Cruel");
const std::string s_E4M9	("E4M9: Fear");

const std::string s_Level1("level 1: entryway");
const std::string s_Level2("level 2: underhalls");
const std::string s_Level3("level 3: the gantlet");
const std::string s_Level4("level 4: the focus");
const std::string s_Level5("level 5: the waste tunnels");
const std::string s_Level6("level 6: the crusher");
const std::string s_Level7("level 7: dead simple");
const std::string s_Level8("level 8: tricks and traps");
const std::string s_Level9("level 9: the pit");
const std::string s_Level10("level 10: refueling base");
const std::string s_Level11("level 11: 'o' of destruction!");
const std::string s_Level12("level 12: the factory");
const std::string s_Level13("level 13: downtown");
const std::string s_Level14("level 14: the inmost dens");
const std::string s_Level15("level 15: industrial zone");
const std::string s_Level16("level 16: suburbs");
const std::string s_Level17("level 17: tenements");
const std::string s_Level18("level 18: the courtyard");
const std::string s_Level19("level 19: the citadel");
const std::string s_Level20("level 20: gotcha!");
const std::string s_Level21("level 21: nirvana");
const std::string s_Level22("level 22: the catacombs");
const std::string s_Level23("level 23: barrels o' fun");
const std::string s_Level24("level 24: the chasm");
const std::string s_Level25("level 25: bloodfalls");
const std::string s_Level26("level 26: the abandoned mines");
const std::string s_Level27("level 27: monster condo");
const std::string s_Level28("level 28: the spirit world");
const std::string s_Level29("level 29: the living end");
const std::string s_Level30("level 30: icon of sin");
const std::string s_Level31("level 31: wolfenstein");
const std::string s_Level32("level 32: grosse");

const std::string s_PlutLevel1("level 1: congo");
const std::string s_PlutLevel2("level 2: well of souls");
const std::string s_PlutLevel3("level 3: aztec");
const std::string s_PlutLevel4("level 4: caged");
const std::string s_PlutLevel5("level 5: ghost town");
const std::string s_PlutLevel6("level 6: baron's lair");
const std::string s_PlutLevel7("level 7: caughtyard");
const std::string s_PlutLevel8("level 8: realm");
const std::string s_PlutLevel9("level 9: abattoire");
const std::string s_PlutLevel10("level 10: onslaught");
const std::string s_PlutLevel11("level 11: hunted");
const std::string s_PlutLevel12("level 12: speed");
const std::string s_PlutLevel13("level 13: the crypt");
const std::string s_PlutLevel14("level 14: genesis");
const std::string s_PlutLevel15("level 15: the twilight");
const std::string s_PlutLevel16("level 16: the omen");
const std::string s_PlutLevel17("level 17: compound");
const std::string s_PlutLevel18("level 18: neurosphere");
const std::string s_PlutLevel19("level 19: nme");
const std::string s_PlutLevel20("level 20: the death domain");
const std::string s_PlutLevel21("level 21: slayer");
const std::string s_PlutLevel22("level 22: impossible mission");
const std::string s_PlutLevel23("level 23: tombstone");
const std::string s_PlutLevel24("level 24: the final frontier");
const std::string s_PlutLevel25("level 25: the temple of darkness");
const std::string s_PlutLevel26("level 26: bunker");
const std::string s_PlutLevel27("level 27: anti-christ");
const std::string s_PlutLevel28("level 28: the sewers");
const std::string s_PlutLevel29("level 29: odyssey of noises");
const std::string s_PlutLevel30("level 30: the gateway of hell");
const std::string s_PlutLevel31("level 31: cyberden");
const std::string s_PlutLevel32("level 32: go 2 it");

const std::string s_TNTLevel1("level 1: system control");
const std::string s_TNTLevel2("level 2: human bbq");
const std::string s_TNTLevel3("level 3: power control");
const std::string s_TNTLevel4("level 4: wormhole");
const std::string s_TNTLevel5("level 5: hanger");
const std::string s_TNTLevel6("level 6: open season");
const std::string s_TNTLevel7("level 7: prison");
const std::string s_TNTLevel8("level 8: metal");
const std::string s_TNTLevel9("level 9: stronghold");
const std::string s_TNTLevel10("level 10: redemption");
const std::string s_TNTLevel11("level 11: storage facility");
const std::string s_TNTLevel12("level 12: crater");
const std::string s_TNTLevel13("level 13: nukage processing");
const std::string s_TNTLevel14("level 14: steel works");
const std::string s_TNTLevel15("level 15: dead zone");
const std::string s_TNTLevel16("level 16: deepest reaches");
const std::string s_TNTLevel17("level 17: processing area");
const std::string s_TNTLevel18("level 18: mill");
const std::string s_TNTLevel19("level 19: shipping/respawning");
const std::string s_TNTLevel20("level 20: central processing");
const std::string s_TNTLevel21("level 21: administration center");
const std::string s_TNTLevel22("level 22: habitat");
const std::string s_TNTLevel23("level 23: lunar mining project");
const std::string s_TNTLevel24("level 24: quarry");
const std::string s_TNTLevel25("level 25: baron's den");
const std::string s_TNTLevel26("level 26: ballistyx");
const std::string s_TNTLevel27("level 27: mount pain");
const std::string s_TNTLevel28("level 28: heck");
const std::string s_TNTLevel29("level 29: river styx");
const std::string s_TNTLevel30("level 30: last call");
const std::string s_TNTLevel31("level 31: pharaoh");
const std::string s_TNTLevel32("level 32: caribbean");

const std::string s_ChatMacro1("I'm ready to kick butt!");
const std::string s_ChatMacro2("I'm OK.");
const std::string s_ChatMacro3("I'm not looking too good!");
const std::string s_ChatMacro4("Help!");
const std::string s_ChatMacro5("You suck!");
const std::string s_ChatMacro6("Next time, scumbag...");
const std::string s_ChatMacro7("Come here!");
const std::string s_ChatMacro8("I'll take care of it.");
const std::string s_ChatMacro9("Yes");
const std::string s_ChatMacro0("No");

const std::string s_TalkToSelf1("You mumble to yourself");
const std::string s_TalkToSelf("Who's there?");
const std::string s_TalkToSelf3("You scare yourself");
const std::string s_TalkToSelf4("You start to rave");
const std::string s_TalkToSelf5("You've lost it...");

const std::string s_MessageSent("[Message Sent]");

// The following should NOT be changed unless it seems
// just AWFULLY necessary

const std::string s_PlayerGreen("Green: ");
const std::string s_PlayerIndigo("Indigo: ");
const std::string s_PlayerBrown("Brown: ");
const std::string s_PlayerRed("Red: ");

const std::string s_KeyGreen("g");
const std::string s_KeyIndigo("i");
const std::string s_KeyBrown("b");
const std::string s_KeyRed("r");

//
//	AM_map.C
//

const std::string s_FollowOn("Follow Mode ON");
const std::string s_FollowOff("Follow Mode OFF");

const std::string s_GridOn("Grid ON");
const std::string s_GridOff("Grid OFF");

const std::string s_MarkedSpot("Marked Spot");
const std::string s_MarksCleared("All Marks Cleared");

//
//	ST_stuff.C
//

const std::string s_MusicChange("Music Change");
const std::string s_NoMusic("IMPOSSIBLE SELECTION");
const std::string s_DegreelessOn("Degreelessness Mode On");
const std::string s_DegreelessOff("Degreelessness Mode Off");

const std::string s_VeryHappyAmmoAdded("Very Happy Ammo Added");
const std::string s_AmmoAdded("Ammo (no keys) Added");

const std::string s_NoClipOn("No Clipping Mode ON");
const std::string s_NoClipOff("No Clipping Mode OFF");

const std::string s_Behold("inVuln, Str, Inviso, Rad, Allmap, or Lite-amp");
const std::string s_BeholdX("Power-up Toggled");

const std::string s_Choppers("... doesn't suck - GM");
const std::string s_ChangeLevel("Changing Level...");

//
//	F_Finale.C
//
const std::string s_E1Text(\
    "Once you beat the big badasses and\n"\
    "clean out the moon base you're supposed\n"\
    "to win, aren't you? Aren't you? Where's\n"\
    "your fat reward and ticket home? What\n"\
    "the hell is this? It's not supposed to\n"\
    "end this way!\n"\
    "\n" \
    "It stinks like rotten meat, but looks\n"\
    "like the lost Deimos base.  Looks like\n"\
    "you're stuck on The Shores of Hell.\n"\
    "The only way out is through.\n"\
    "\n"\
    "To continue the DOOM experience, play\n"\
    "The Shores of Hell and its amazing\n"\
    "sequel, Inferno!\n");


const std::string s_E2Text(\
    "You've done it! The hideous cyber-\n"\
    "demon lord that ruled the lost Deimos\n"\
    "moon base has been slain and you\n"\
    "are triumphant! But ... where are\n"\
    "you? You clamber to the edge of the\n"\
    "moon and look down to see the awful\n"\
    "truth.\n" \
    "\n"\
    "Deimos floats above Hell itself!\n"\
    "You've never heard of anyone escaping\n"\
    "from Hell, but you'll make the bastards\n"\
    "sorry they ever heard of you! Quickly,\n"\
    "you rappel down to  the surface of\n"\
    "Hell.\n"\
    "\n" \
    "Now, it's on to the final chapter of\n"\
    "DOOM! -- Inferno.");


const std::string s_E3Text(\
    "The loathsome spiderdemon that\n"\
    "masterminded the invasion of the moon\n"\
    "bases and caused so much death has had\n"\
    "its ass kicked for all time.\n"\
    "\n"\
    "A hidden doorway opens and you enter.\n"\
    "You've proven too tough for Hell to\n"\
    "contain, and now Hell at last plays\n"\
    "fair -- for you emerge from the door\n"\
    "to see the green fields of Earth!\n"\
    "Home at last.\n" \
    "\n"\
    "You wonder what's been happening on\n"\
    "Earth while you were battling evil\n"\
    "unleashed. It's good that no Hell-\n"\
    "spawn could have come through that\n"\
    "door with you ...");


const std::string s_E4Text(\
    "the spider mastermind must have sent forth\n"\
    "its legions of hellspawn before your\n"\
    "final confrontation with that terrible\n"\
    "beast from hell.  but you stepped forward\n"\
    "and brought forth eternal damnation and\n"\
    "suffering upon the horde as a true hero\n"\
    "would in the face of something so evil.\n"\
    "\n"\
    "besides, someone was gonna pay for what\n"\
    "happened to daisy, your pet rabbit.\n"\
    "\n"\
    "but now, you see spread before you more\n"\
    "potential pain and gibbitude as a nation\n"\
    "of demons run amok among our cities.\n"\
    "\n"\
    "next stop, hell on earth!");


// after level 6, put this:

const std::string s_C1Text(\
    "YOU HAVE ENTERED DEEPLY INTO THE INFESTED\n" \
    "STARPORT. BUT SOMETHING IS WRONG. THE\n" \
    "MONSTERS HAVE BROUGHT THEIR OWN REALITY\n" \
    "WITH THEM, AND THE STARPORT'S TECHNOLOGY\n" \
    "IS BEING SUBVERTED BY THEIR PRESENCE.\n" \
    "\n"\
    "AHEAD, YOU SEE AN OUTPOST OF HELL, A\n" \
    "FORTIFIED ZONE. IF YOU CAN GET PAST IT,\n" \
    "YOU CAN PENETRATE INTO THE HAUNTED HEART\n" \
    "OF THE STARBASE AND FIND THE CONTROLLING\n" \
    "SWITCH WHICH HOLDS EARTH'S POPULATION\n" \
    "HOSTAGE.");

// After level 11, put this:

const std::string s_C2Text(\
    "YOU HAVE WON! YOUR VICTORY HAS ENABLED\n" \
    "HUMANKIND TO EVACUATE EARTH AND ESCAPE\n"\
    "THE NIGHTMARE.  NOW YOU ARE THE ONLY\n"\
    "HUMAN LEFT ON THE FACE OF THE PLANET.\n"\
    "CANNIBAL MUTATIONS, CARNIVOROUS ALIENS,\n"\
    "AND EVIL SPIRITS ARE YOUR ONLY NEIGHBORS.\n"\
    "YOU SIT BACK AND WAIT FOR DEATH, CONTENT\n"\
    "THAT YOU HAVE SAVED YOUR SPECIES.\n"\
    "\n"\
    "BUT THEN, EARTH CONTROL BEAMS DOWN A\n"\
    "MESSAGE FROM SPACE: \"SENSORS HAVE LOCATED\n"\
    "THE SOURCE OF THE ALIEN INVASION. IF YOU\n"\
    "GO THERE, YOU MAY BE ABLE TO BLOCK THEIR\n"\
    "ENTRY.  THE ALIEN BASE IS IN THE HEART OF\n"\
    "YOUR OWN HOME CITY, NOT FAR FROM THE\n"\
    "STARPORT.\" SLOWLY AND PAINFULLY YOU GET\n"\
    "UP AND RETURN TO THE FRAY.");


// After level 20, put this:

const std::string s_C3Text(\
    "YOU ARE AT THE CORRUPT HEART OF THE CITY,\n"\
    "SURROUNDED BY THE CORPSES OF YOUR ENEMIES.\n"\
    "YOU SEE NO WAY TO DESTROY THE CREATURES'\n"\
    "ENTRYWAY ON THIS SIDE, SO YOU CLENCH YOUR\n"\
    "TEETH AND PLUNGE THROUGH IT.\n"\
    "\n"\
    "THERE MUST BE A WAY TO CLOSE IT ON THE\n"\
    "OTHER SIDE. WHAT DO YOU CARE IF YOU'VE\n"\
    "GOT TO GO THROUGH HELL TO GET TO IT?");


// After level 29, put this:

const std::string s_C4Text(\
    "THE HORRENDOUS VISAGE OF THE BIGGEST\n"\
    "DEMON YOU'VE EVER SEEN CRUMBLES BEFORE\n"\
    "YOU, AFTER YOU PUMP YOUR ROCKETS INTO\n"\
    "HIS EXPOSED BRAIN. THE MONSTER SHRIVELS\n"\
    "UP AND DIES, ITS THRASHING LIMBS\n"\
    "DEVASTATING UNTOLD MILES OF HELL'S\n"\
    "SURFACE.\n"\
    "\n"\
    "YOU'VE DONE IT. THE INVASION IS OVER.\n"\
    "EARTH IS SAVED. HELL IS A WRECK. YOU\n"\
    "WONDER WHERE BAD FOLKS WILL GO WHEN THEY\n"\
    "DIE, NOW. WIPING THE SWEAT FROM YOUR\n"\
    "FOREHEAD YOU BEGIN THE LONG TREK BACK\n"\
    "HOME. REBUILDING EARTH OUGHT TO BE A\n"\
    "LOT MORE FUN THAN RUINING IT WAS.\n");



// Before level 31, put this:

const std::string s_C5Text(\
    "CONGRATULATIONS, YOU'VE FOUND THE SECRET\n"\
    "LEVEL! LOOKS LIKE IT'S BEEN BUILT BY\n"\
    "HUMANS, RATHER THAN DEMONS. YOU WONDER\n"\
    "WHO THE INMATES OF THIS CORNER OF HELL\n"\
    "WILL BE.");


// Before level 32, put this:

const std::string s_C6Text(\
    "CONGRATULATIONS, YOU'VE FOUND THE\n"\
    "SUPER SECRET LEVEL!  YOU'D BETTER\n"\
    "BLAZE THROUGH THIS ONE!\n");


// after map 06	

const std::string s_P1Text(\
    "You gloat over the steaming carcass of the\n"\
    "Guardian.  With its death, you've wrested\n"\
    "the Accelerator from the stinking claws\n"\
    "of Hell.  You relax and glance around the\n"\
    "room.  Damn!  There was supposed to be at\n"\
    "least one working prototype, but you can't\n"\
    "see it. The demons must have taken it.\n"\
    "\n"\
    "You must find the prototype, or all your\n"\
    "struggles will have been wasted. Keep\n"\
    "moving, keep fighting, keep killing.\n"\
    "Oh yes, keep living, too.");


// after map 11

const std::string s_P2Text(\
    "Even the deadly Arch-Vile labyrinth could\n"\
    "not stop you, and you've gotten to the\n"\
    "prototype Accelerator which is soon\n"\
    "efficiently and permanently deactivated.\n"\
    "\n"\
    "You're good at that kind of thing.");


// after map 20

const std::string s_P3Text(\
    "You've bashed and battered your way into\n"\
    "the heart of the devil-hive.  Time for a\n"\
    "Search-and-Destroy mission, aimed at the\n"\
    "Gatekeeper, whose foul offspring is\n"\
    "cascading to Earth.  Yeah, he's bad. But\n"\
    "you know who's worse!\n"\
    "\n"\
    "Grinning evilly, you check your gear, and\n"\
    "get ready to give the bastard a little Hell\n"\
    "of your own making!");

// after map 30

const std::string s_P4Text(\
    "The Gatekeeper's evil face is splattered\n"\
    "all over the place.  As its tattered corpse\n"\
    "collapses, an inverted Gate forms and\n"\
    "sucks down the shards of the last\n"\
    "prototype Accelerator, not to mention the\n"\
    "few remaining demons.  You're done. Hell\n"\
    "has gone back to pounding bad dead folks \n"\
    "instead of good live ones.  Remember to\n"\
    "tell your grandkids to put a rocket\n"\
    "launcher in your coffin. If you go to Hell\n"\
    "when you die, you'll need it for some\n"\
    "final cleaning-up ...");

// before map 31

const std::string s_P5Text(\
    "You've found the second-hardest level we\n"\
    "got. Hope you have a saved game a level or\n"\
    "two previous.  If not, be prepared to die\n"\
    "aplenty. For master marines only.");

// before map 32

const std::string s_P6Text(\
    "Betcha wondered just what WAS the hardest\n"\
    "level we had ready for ya?  Now you know.\n"\
    "No one gets out alive.");


const std::string s_T1Text(\
    "You've fought your way out of the infested\n"\
    "experimental labs.   It seems that UAC has\n"\
    "once again gulped it down.  With their\n"\
    "high turnover, it must be hard for poor\n"\
    "old UAC to buy corporate health insurance\n"\
    "nowadays..\n"\
    "\n"\
    "Ahead lies the military complex, now\n"\
    "swarming with diseased horrors hot to get\n"\
    "their teeth into you. With luck, the\n"\
    "complex still has some warlike ordnance\n"\
    "laying around.");


const std::string s_T2Text(\
    "You hear the grinding of heavy machinery\n"\
    "ahead.  You sure hope they're not stamping\n"\
    "out new hellspawn, but you're ready to\n"\
    "ream out a whole herd if you have to.\n"\
    "They might be planning a blood feast, but\n"\
    "you feel about as mean as two thousand\n"\
    "maniacs packed into one mad killer.\n"\
    "\n"\
    "You don't plan to go down easy.");


const std::string s_T3Text(\
    "The vista opening ahead looks real damn\n"\
    "familiar. Smells familiar, too -- like\n"\
    "fried excrement. You didn't like this\n"\
    "place before, and you sure as hell ain't\n"\
    "planning to like it now. The more you\n"\
    "brood on it, the madder you get.\n"\
    "Hefting your gun, an evil grin trickles\n"\
    "onto your face. Time to take some names.");

const std::string s_T4Text(\
    "Suddenly, all is silent, from one horizon\n"\
    "to the other. The agonizing echo of Hell\n"\
    "fades away, the nightmare sky turns to\n"\
    "blue, the heaps of monster corpses start \n"\
    "to evaporate along with the evil stench \n"\
    "that filled the air. Jeeze, maybe you've\n"\
    "done it. Have you really won?\n"\
    "\n"\
    "Something rumbles in the distance.\n"\
    "A blue light begins to glow inside the\n"\
    "ruined skull of the demon-spitter.");


const std::string s_T5Text(\
    "What now? Looks totally different. Kind\n"\
    "of like King Tut's condo. Well,\n"\
    "whatever's here can't be any worse\n"\
    "than usual. Can it?  Or maybe it's best\n"\
    "to let sleeping gods lie..");


const std::string s_T6Text(\
    "Time for a vacation. You've burst the\n"\
    "bowels of hell and by golly you're ready\n"\
    "for a break. You mutter to yourself,\n"\
    "Maybe someone else can kick Hell's ass\n"\
    "next time around. Ahead lies a quiet town,\n"\
    "with peaceful flowing water, quaint\n"\
    "buildings, and presumably no Hellspawn.\n"\
    "\n"\
    "As you step off the transport, you hear\n"\
    "the stomp of a cyberdemon's iron shoe.");



//
// Character cast strings F_FINALE.C
//
const std::string s_Zombie("ZOMBIEMAN");
const std::string s_ShotgunGuy("SHOTGUN GUY");
const std::string s_HeavyDude("HEAVY WEAPON DUDE");
const std::string s_Imp("IMP");
const std::string s_Demon("DEMON");
const std::string s_LostSoul("LOST SOUL");
const std::string s_CacoDemon("CACODEMON");
const std::string s_HellKnight("HELL KNIGHT");
const std::string s_Baron("BARON OF HELL");
const std::string s_Arach("ARACHNOTRON");
const std::string s_PainElemental("PAIN ELEMENTAL");
const std::string s_Revenant("REVENANT");
const std::string s_Mancubus("MANCUBUS");
const std::string s_ArchVile("ARCH-VILE");
const std::string s_Spider("THE SPIDER MASTERMIND");
const std::string s_CyberDemon("THE CYBERDEMON");
const std::string s_Hero("OUR HERO");