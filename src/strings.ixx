// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	Globally defined strings.
//
//-----------------------------------------------------------------------------
module;
#include <SFML/Window/Keyboard.hpp>
#include <format>
export module strings;

// Misc. other strings.
export const auto SAVEGAMENAME = "doomsav";

// Not done in french?

// QuitDOOM messages
export constexpr auto NUM_QUITMESSAGES = 22;

#ifdef FRENCH
//
// D_Main.C
//
export const auto D_DEVSTR = "MODE DEVELOPPEMENT ON.\n";
export const auto D_CDROM = "VERSION CD-ROM: DEFAULT.CFG DANS C:\\DOOMDATA\n";

//
//	M_Menu.C
//
export const auto PRESSKEY = "APPUYEZ SUR UNE TOUCHE.";
export const auto PRESSYN = "APPUYEZ SUR Y OU N";
export const auto QUITMSG = "VOUS VOULEZ VRAIMENT\nQUITTER CE SUPER JEU?";
export const auto LOADNET = std::format("VOUS NE POUVEZ PAS CHARGER\nUN JEU EN RESEAU!\n\n{}",PRESSKEY);
export const auto QLOADNET = std::format("CHARGEMENT RAPIDE INTERDIT EN RESEAU!\n\n{}",PRESSKEY);
export const auto QSAVESPOT = std::format("VOUS N'AVEZ PAS CHOISI UN EMPLACEMENT!\n\n{}",PRESSKEY);
#define SAVEDEAD                                                               \
  "VOUS NE POUVEZ PAS SAUVER SI VOUS NE JOUEZ "                                \
  "PAS!\n\n" PRESSKEY
export const auto QSPROMPT = std::format("SAUVEGARDE RAPIDE DANS LE FICHIER \n\n'%s'?\n\n{}",PRESSYN);
#define QLPROMPT                                                               \
  "VOULEZ-VOUS CHARGER LA SAUVEGARDE"                                          \
  "\n\n'%s'?\n\n" PRESSYN
export const auto NEWGAME = std::format("VOUS NE POUVEZ PAS LANCER\nUN NOUVEAU JEU SUR RESEAU.\n\n{}",PRESSKEY);
export const auto NIGHTMARE = std::format("VOUS CONFIRMEZ? CE NIVEAU EST\nVRAIMENT IMPITOYABLE!n{}", PRESSYN);
#define SWSTRING                                                               \
  "CECI EST UNE VERSION SHAREWARE DE DOOM.\n\n"                                \
  "VOUS DEVRIEZ COMMANDER LA TRILOGIE COMPLETE.\n\n" PRESSKEY
export const auto MSGOFF = "MESSAGES OFF";
export const auto MSGON = "MESSAGES ON";
#define NETEND                                                                 \
  "VOUS NE POUVEZ PAS METTRE FIN A UN JEU SUR "                                \
  "RESEAU!\n\n" PRESSKEY
export const auto ENDGAME = std::format("VOUS VOULEZ VRAIMENT METTRE FIN AU JEU?\n\n{}",PRESSYN);

export const auto DOSY = "(APPUYEZ SUR Y POUR REVENIR AU OS.)";

export const auto DETAILHI = "GRAPHISMES MAXIMUM ";
export const auto DETAILLO = "GRAPHISMES MINIMUM ";
export const auto GAMMALVL0 = "CORRECTION GAMMA OFF";
export const auto GAMMALVL1 = "CORRECTION GAMMA NIVEAU 1";
export const auto GAMMALVL2 = "CORRECTION GAMMA NIVEAU 2";
export const auto GAMMALVL3 = "CORRECTION GAMMA NIVEAU 3";
export const auto GAMMALVL4 = "CORRECTION GAMMA NIVEAU 4";
export const auto EMPTYSTRING = "EMPLACEMENT VIDE";

//
//	P_inter.C
//
export const auto GOTARMOR = "ARMURE RECUPEREE.";
export const auto GOTMEGA = "MEGA-ARMURE RECUPEREE!";
export const auto GOTHTHBONUS = "BONUS DE SANTE RECUPERE.";
export const auto GOTARMBONUS = "BONUS D'ARMURE RECUPERE.";
export const auto GOTSTIM = "STIMPACK RECUPERE.";
export const auto GOTMEDINEED = "MEDIKIT RECUPERE. VOUS EN AVEZ VRAIMENT BESOIN!";
export const auto GOTMEDIKIT = "MEDIKIT RECUPERE.";
export const auto GOTSUPER = "SUPERCHARGE!";

export const auto GOTBLUECARD = "CARTE MAGNETIQUE BLEUE RECUPEREE.";
export const auto GOTYELWCARD = "CARTE MAGNETIQUE JAUNE RECUPEREE.";
export const auto GOTREDCARD = "CARTE MAGNETIQUE ROUGE RECUPEREE.";
export const auto GOTBLUESKUL = "CLEF CRANE BLEUE RECUPEREE.";
export const auto GOTYELWSKUL = "CLEF CRANE JAUNE RECUPEREE.";
export const auto GOTREDSKULL = "CLEF CRANE ROUGE RECUPEREE.";

export const auto GOTINVUL = "INVULNERABILITE!";
export const auto GOTBERSERK = "BERSERK!";
export const auto GOTINVIS = "INVISIBILITE PARTIELLE ";
export const auto GOTSUIT = "COMBINAISON ANTI-RADIATIONS ";
export const auto GOTMAP = "CARTE INFORMATIQUE ";
export const auto GOTVISOR = "VISEUR A AMPLIFICATION DE LUMIERE ";
export const auto GOTMSPHERE = "MEGASPHERE!";

export const auto GOTCLIP = "CHARGEUR RECUPERE.";
export const auto GOTCLIPBOX = "BOITE DE BALLES RECUPEREE.";
export const auto GOTROCKET = "ROQUETTE RECUPEREE.";
export const auto GOTROCKBOX = "CAISSE DE ROQUETTES RECUPEREE.";
export const auto GOTCELL = "CELLULE D'ENERGIE RECUPEREE.";
export const auto GOTCELLBOX = "PACK DE CELLULES D'ENERGIE RECUPERE.";
export const auto GOTSHELLS = "4 CARTOUCHES RECUPEREES.";
export const auto GOTSHELLBOX = "BOITE DE CARTOUCHES RECUPEREE.";
export const auto GOTBACKPACK = "SAC PLEIN DE MUNITIONS RECUPERE!";

export const auto GOTBFG9000 = "VOUS AVEZ UN BFG9000!  OH, OUI!";
export const auto GOTCHAINGUN = "VOUS AVEZ LA MITRAILLEUSE!";
export const auto GOTCHAINSAW = "UNE TRONCONNEUSE!";
export const auto GOTLAUNCHER = "VOUS AVEZ UN LANCE-ROQUETTES!";
export const auto GOTPLASMA = "VOUS AVEZ UN FUSIL A PLASMA!";
export const auto GOTSHOTGUN = "VOUS AVEZ UN FUSIL!";
export const auto GOTSHOTGUN2 = "VOUS AVEZ UN SUPER FUSIL!";

//
// P_Doors.C
//
const auto PD_BLUEO = "IL VOUS FAUT UNE CLEF BLEUE";
export const auto PD_REDO = "IL VOUS FAUT UNE CLEF ROUGE";
export const auto PD_YELLOWO = "IL VOUS FAUT UNE CLEF JAUNE";
#define PD_BLUEK PD_BLUEO
#define PD_REDK PD_REDO
#define PD_YELLOWK PD_YELLOWO

//
//	G_game.C
//
export const auto GGSAVED = "JEU SAUVEGARDE.";

//
//	HU_stuff.C
//
export const auto HUSTR_MSGU = "[MESSAGE NON ENVOYE]";

export const auto HUSTR_E1M1 = "E1M1: HANGAR";
export const auto HUSTR_E1M2 = "E1M2: USINE NUCLEAIRE ";
export const auto HUSTR_E1M3 = "E1M3: RAFFINERIE DE TOXINES ";
export const auto HUSTR_E1M4 = "E1M4: CENTRE DE CONTROLE ";
export const auto HUSTR_E1M5 = "E1M5: LABORATOIRE PHOBOS ";
export const auto HUSTR_E1M6 = "E1M6: TRAITEMENT CENTRAL ";
export const auto HUSTR_E1M7 = "E1M7: CENTRE INFORMATIQUE ";
export const auto HUSTR_E1M8 = "E1M8: ANOMALIE PHOBOS ";
export const auto HUSTR_E1M9 = "E1M9: BASE MILITAIRE ";

export const auto HUSTR_E2M1 = "E2M1: ANOMALIE DEIMOS ";
export const auto HUSTR_E2M2 = "E2M2: ZONE DE CONFINEMENT ";
export const auto HUSTR_E2M3 = "E2M3: RAFFINERIE";
export const auto HUSTR_E2M4 = "E2M4: LABORATOIRE DEIMOS ";
export const auto HUSTR_E2M5 = "E2M5: CENTRE DE CONTROLE ";
export const auto HUSTR_E2M6 = "E2M6: HALLS DES DAMNES ";
export const auto HUSTR_E2M7 = "E2M7: CUVES DE REPRODUCTION ";
export const auto HUSTR_E2M8 = "E2M8: TOUR DE BABEL ";
export const auto HUSTR_E2M9 = "E2M9: FORTERESSE DU MYSTERE ";

export const auto HUSTR_E3M1 = "E3M1: DONJON DE L'ENFER ";
export const auto HUSTR_E3M2 = "E3M2: BOURBIER DU DESESPOIR ";
export const auto HUSTR_E3M3 = "E3M3: PANDEMONIUM";
export const auto HUSTR_E3M4 = "E3M4: MAISON DE LA DOULEUR ";
export const auto HUSTR_E3M5 = "E3M5: CATHEDRALE PROFANE ";
export const auto HUSTR_E3M6 = "E3M6: MONT EREBUS";
export const auto HUSTR_E3M7 = "E3M7: LIMBES";
export const auto HUSTR_E3M8 = "E3M8: DIS";
export const auto HUSTR_E3M9 = "E3M9: CLAPIERS";

export const auto HUSTR_1 = "NIVEAU 1: ENTREE ";
export const auto HUSTR_2 = "NIVEAU 2: HALLS SOUTERRAINS ";
export const auto HUSTR_3 = "NIVEAU 3: LE FEU NOURRI ";
export const auto HUSTR_4 = "NIVEAU 4: LE FOYER ";
export const auto HUSTR_5 = "NIVEAU 5: LES EGOUTS ";
export const auto HUSTR_6 = "NIVEAU 6: LE BROYEUR ";
export const auto HUSTR_7 = "NIVEAU 7: L'HERBE DE LA MORT";
export const auto HUSTR_8 = "NIVEAU 8: RUSES ET PIEGES ";
export const auto HUSTR_9 = "NIVEAU 9: LE PUITS ";
export const auto HUSTR_10 = "NIVEAU 10: BASE DE RAVITAILLEMENT ";
export const auto HUSTR_11 = "NIVEAU 11: LE CERCLE DE LA MORT!";

export const auto HUSTR_12 = "NIVEAU 12: L'USINE ";
export const auto HUSTR_13 = "NIVEAU 13: LE CENTRE VILLE";
export const auto HUSTR_14 = "NIVEAU 14: LES ANTRES PROFONDES ";
export const auto HUSTR_15 = "NIVEAU 15: LA ZONE INDUSTRIELLE ";
export const auto HUSTR_16 = "NIVEAU 16: LA BANLIEUE";
export const auto HUSTR_17 = "NIVEAU 17: LES IMMEUBLES";
export const auto HUSTR_18 = "NIVEAU 18: LA COUR ";
export const auto HUSTR_19 = "NIVEAU 19: LA CITADELLE ";
export const auto HUSTR_20 = "NIVEAU 20: JE T'AI EU!";

export const auto HUSTR_21 = "NIVEAU 21: LE NIRVANA";
export const auto HUSTR_22 = "NIVEAU 22: LES CATACOMBES ";
export const auto HUSTR_23 = "NIVEAU 23: LA GRANDE FETE ";
export const auto HUSTR_24 = "NIVEAU 24: LE GOUFFRE ";
export const auto HUSTR_25 = "NIVEAU 25: LES CHUTES DE SANG";
export const auto HUSTR_26 = "NIVEAU 26: LES MINES ABANDONNEES ";
export const auto HUSTR_27 = "NIVEAU 27: CHEZ LES MONSTRES ";
export const auto HUSTR_28 = "NIVEAU 28: LE MONDE DE L'ESPRIT ";
export const auto HUSTR_29 = "NIVEAU 29: LA LIMITE ";
export const auto HUSTR_30 = "NIVEAU 30: L'ICONE DU PECHE ";

export const auto HUSTR_31 = "NIVEAU 31: WOLFENSTEIN";
export const auto HUSTR_32 = "NIVEAU 32: LE MASSACRE";

export const auto HUSTR_CHATMACRO1 = "JE SUIS PRET A LEUR EN FAIRE BAVER!";
export const auto HUSTR_CHATMACRO2 = "JE VAIS BIEN.";
export const auto HUSTR_CHATMACRO3 = "JE N'AI PAS L'AIR EN FORME!";
export const auto HUSTR_CHATMACRO4 = "AU SECOURS!";
export const auto HUSTR_CHATMACRO5 = "TU CRAINS!";
export const auto HUSTR_CHATMACRO6 = "LA PROCHAINE FOIS, MINABLE...";
export const auto HUSTR_CHATMACRO7 = "VIENS ICI!";
export const auto HUSTR_CHATMACRO8 = "JE VAIS M'EN OCCUPER.";
export const auto HUSTR_CHATMACRO9 = "OUI";
export const auto HUSTR_CHATMACRO0 = "NON";

export const auto HUSTR_TALKTOSELF1 = "VOUS PARLEZ TOUT SEUL ";
export const auto HUSTR_TALKTOSELF2 = "QUI EST LA?";
export const auto HUSTR_TALKTOSELF3 = "VOUS VOUS FAITES PEUR ";
export const auto HUSTR_TALKTOSELF4 = "VOUS COMMENCEZ A DELIRER ";
export const auto HUSTR_TALKTOSELF5 = "VOUS ETES LARGUE...";

export const auto HUSTR_MESSAGESENT = "[MESSAGE ENVOYE]";

// The following should NOT be changed unless it seems
// just AWFULLY necessary

export const auto HUSTR_PLRGREEN = "VERT: ";
export const auto HUSTR_PLRINDIGO = "INDIGO: ";
export const auto HUSTR_PLRBROWN = "BRUN: ";
export const auto HUSTR_PLRRED = "ROUGE: ";

#define HUSTR_KEYGREEN 'g' // french key should be "V"
#define HUSTR_KEYINDIGO 'i'
#define HUSTR_KEYBROWN 'b'
#define HUSTR_KEYRED 'r'

//
//	AM_map.C
//

export const auto AMSTR_FOLLOWON = "MODE POURSUITE ON";
export const auto AMSTR_FOLLOWOFF = "MODE POURSUITE OFF";

export const auto AMSTR_GRIDON = "GRILLE ON";
export const auto AMSTR_GRIDOFF = "GRILLE OFF";

export const auto AMSTR_MARKEDSPOT = "REPERE MARQUE ";
export const auto AMSTR_MARKSCLEARED = "REPERES EFFACES ";

//
//	ST_stuff.C
//

export const auto STSTR_MUS = "CHANGEMENT DE MUSIQUE ";
export const auto STSTR_NOMUS = "IMPOSSIBLE SELECTION";
export const auto STSTR_DQDON = "INVULNERABILITE ON ";
export const auto STSTR_DQDOFF = "INVULNERABILITE OFF";

export const auto STSTR_KFAADDED = "ARMEMENT MAXIMUM! ";
export const auto STSTR_FAADDED = "ARMES (SAUF CLEFS) AJOUTEES";

export const auto STSTR_NCON = "BARRIERES ON";
export const auto STSTR_NCOFF = "BARRIERES OFF";

export const auto STSTR_BEHOLD = " inVuln, Str, Inviso, Rad, Allmap, or Lite-amp";
export const auto STSTR_BEHOLDX = "AMELIORATION ACTIVEE";

export const auto STSTR_CHOPPERS = "... DOESN'T SUCK - GM";
export const auto STSTR_CLEV = "CHANGEMENT DE NIVEAU...";

//
//	F_Finale.C
//
export auto E1TEXT =                                                                  \
  "APRES AVOIR VAINCU LES GROS MECHANTS\n"                                     \
  "ET NETTOYE LA BASE LUNAIRE, VOUS AVEZ\n"                                    \
  "GAGNE, NON? PAS VRAI? OU EST DONC VOTRE\n"                                  \
  " RECOMPENSE ET VOTRE BILLET DE\n"                                           \
  "RETOUR? QU'EST-QUE CA VEUT DIRE?CE"                                         \
  "N'EST PAS LA FIN ESPEREE!\n"                                                \
  "\n"                                                                         \
  "CA SENT LA VIANDE PUTREFIEE, MAIS\n"                                        \
  "ON DIRAIT LA BASE DEIMOS. VOUS ETES\n"                                      \
  "APPAREMMENT BLOQUE AUX PORTES DE L'ENFER.\n"                                \
  "LA SEULE ISSUE EST DE L'AUTRE COTE.\n"                                      \
  "\n"                                                                         \
  "POUR VIVRE LA SUITE DE DOOM, JOUEZ\n"                                       \
  "A 'AUX PORTES DE L'ENFER' ET A\n"                                           \
  "L'EPISODE SUIVANT, 'L'ENFER'!\n"

export auto E2TEXT =                                                                 \
  "VOUS AVEZ REUSSI. L'INFAME DEMON\n"                                         \
  "QUI CONTROLAIT LA BASE LUNAIRE DE\n"                                        \
  "DEIMOS EST MORT, ET VOUS AVEZ\n"                                            \
  "TRIOMPHE! MAIS... OU ETES-VOUS?\n"                                          \
  "VOUS GRIMPEZ JUSQU'AU BORD DE LA\n"                                         \
  "LUNE ET VOUS DECOUVREZ L'ATROCE\n"                                          \
  "VERITE.\n"                                                                  \
  "\n"                                                                         \
  "DEIMOS EST AU-DESSUS DE L'ENFER!\n"                                         \
  "VOUS SAVEZ QUE PERSONNE NE S'EN\n"                                          \
  "EST JAMAIS ECHAPPE, MAIS CES FUMIERS\n"                                     \
  "VONT REGRETTER DE VOUS AVOIR CONNU!\n"                                      \
  "VOUS REDESCENDEZ RAPIDEMENT VERS\n"                                         \
  "LA SURFACE DE L'ENFER.\n"                                                   \
  "\n"                                                                         \
  "VOICI MAINTENANT LE CHAPITRE FINAL DE\n"                                    \
  "DOOM! -- L'ENFER."

export auto E3TEXT =                                                                 \
  "LE DEMON ARACHNEEN ET REPUGNANT\n"                                          \
  "QUI A DIRIGE L'INVASION DES BASES\n"                                        \
  "LUNAIRES ET SEME LA MORT VIENT DE SE\n"                                     \
  "FAIRE PULVERISER UNE FOIS POUR TOUTES.\n"                                   \
  "\n"                                                                         \
  "UNE PORTE SECRETE S'OUVRE. VOUS ENTREZ.\n"                                  \
  "VOUS AVEZ PROUVE QUE VOUS POUVIEZ\n"                                        \
  "RESISTER AUX HORREURS DE L'ENFER.\n"                                        \
  "IL SAIT ETRE BEAU JOUEUR, ET LORSQUE\n"                                     \
  "VOUS SORTEZ, VOUS REVOYEZ LES VERTES\n"                                     \
  "PRAIRIES DE LA TERRE, VOTRE PLANETE.\n"                                     \
  "\n"                                                                         \
  "VOUS VOUS DEMANDEZ CE QUI S'EST PASSE\n"                                    \
  "SUR TERRE PENDANT QUE VOUS AVEZ\n"                                          \
  "COMBATTU LE DEMON. HEUREUSEMENT,\n"                                         \
  "AUCUN GERME DU MAL N'A FRANCHI\n"                                           \
  "CETTE PORTE AVEC VOUS..."

// after level 6, put this:

export auto C1TEXT =                                                                  \
  "VOUS ETES AU PLUS PROFOND DE L'ASTROPORT\n"                                 \
  "INFESTE DE MONSTRES, MAIS QUELQUE CHOSE\n"                                  \
  "NE VA PAS. ILS ONT APPORTE LEUR PROPRE\n"                                   \
  "REALITE, ET LA TECHNOLOGIE DE L'ASTROPORT\n"                                \
  "EST AFFECTEE PAR LEUR PRESENCE.\n"                                          \
  "\n"                                                                         \
  "DEVANT VOUS, VOUS VOYEZ UN POSTE AVANCE\n"                                  \
  "DE L'ENFER, UNE ZONE FORTIFIEE. SI VOUS\n"                                  \
  "POUVEZ PASSER, VOUS POURREZ PENETRER AU\n"                                  \
  "COEUR DE LA BASE HANTEE ET TROUVER \n"                                      \
  "L'INTERRUPTEUR DE CONTROLE QUI GARDE LA \n"                                 \
  "POPULATION DE LA TERRE EN OTAGE."

// After level 11, put this:

export auto C2TEXT =                                                                  \
  "VOUS AVEZ GAGNE! VOTRE VICTOIRE A PERMIS\n"                                 \
  "A L'HUMANITE D'EVACUER LA TERRE ET \n"                                      \
  "D'ECHAPPER AU CAUCHEMAR. VOUS ETES \n"                                      \
  "MAINTENANT LE DERNIER HUMAIN A LA SURFACE \n"                               \
  "DE LA PLANETE. VOUS ETES ENTOURE DE \n"                                     \
  "MUTANTS CANNIBALES, D'EXTRATERRESTRES \n"                                   \
  "CARNIVORES ET D'ESPRITS DU MAL. VOUS \n"                                    \
  "ATTENDEZ CALMEMENT LA MORT, HEUREUX \n"                                     \
  "D'AVOIR PU SAUVER VOTRE RACE.\n"                                            \
  "MAIS UN MESSAGE VOUS PARVIENT SOUDAIN\n"                                    \
  "DE L'ESPACE: \"NOS CAPTEURS ONT LOCALISE\n"                                 \
  "LA SOURCE DE L'INVASION EXTRATERRESTRE.\n"                                  \
  "SI VOUS Y ALLEZ, VOUS POURREZ PEUT-ETRE\n"                                  \
  "LES ARRETER. LEUR BASE EST SITUEE AU COEUR\n"                               \
  "DE VOTRE VILLE NATALE, PRES DE L'ASTROPORT.\n"                              \
  "VOUS VOUS RELEVEZ LENTEMENT ET PENIBLEMENT\n"                               \
  "ET VOUS REPARTEZ POUR LE FRONT."

// After level 20, put this:

export auto C3TEXT =                                                                 \
  "VOUS ETES AU COEUR DE LA CITE CORROMPUE,\n"                                 \
  "ENTOURE PAR LES CADAVRES DE VOS ENNEMIS.\n"                                 \
  "VOUS NE VOYEZ PAS COMMENT DETRUIRE LA PORTE\n"                              \
  "DES CREATURES DE CE COTE. VOUS SERREZ\n"                                    \
  "LES DENTS ET PLONGEZ DANS L'OUVERTURE.\n"                                   \
  "\n"                                                                         \
  "IL DOIT Y AVOIR UN MOYEN DE LA FERMER\n"                                    \
  "DE L'AUTRE COTE. VOUS ACCEPTEZ DE\n"                                        \
  "TRAVERSER L'ENFER POUR LE FAIRE?"

// After level 29, put this:

 export auto C4TEXT =                                                                 \
  "LE VISAGE HORRIBLE D'UN DEMON D'UNE\n"                                      \
  "TAILLE INCROYABLE S'EFFONDRE DEVANT\n"                                      \
  "VOUS LORSQUE VOUS TIREZ UNE SALVE DE\n"                                     \
  "ROQUETTES DANS SON CERVEAU. LE MONSTRE\n"                                   \
  "SE RATATINE, SES MEMBRES DECHIQUETES\n"                                     \
  "SE REPANDANT SUR DES CENTAINES DE\n"                                        \
  "KILOMETRES A LA SURFACE DE L'ENFER.\n"                                      \
  "\n"                                                                         \
  "VOUS AVEZ REUSSI. L'INVASION N'AURA.\n"                                     \
  "PAS LIEU. LA TERRE EST SAUVEE. L'ENFER\n"                                   \
  "EST ANEANTI. EN VOUS DEMANDANT OU IRONT\n"                                  \
  "MAINTENANT LES DAMNES, VOUS ESSUYEZ\n"                                      \
  "VOTRE FRONT COUVERT DE SUEUR ET REPARTEZ\n"                                 \
  "VERS LA TERRE. SA RECONSTRUCTION SERA\n"                                    \
  "BEAUCOUP PLUS DROLE QUE SA DESTRUCTION.\n"

// Before level 31, put this:

#define C5TEXT                                                                 \
  "FELICITATIONS! VOUS AVEZ TROUVE LE\n"                                       \
  "NIVEAU SECRET! IL SEMBLE AVOIR ETE\n"                                       \
  "CONSTRUIT PAR LES HUMAINS. VOUS VOUS\n"                                     \
  "DEMANDEZ QUELS PEUVENT ETRE LES\n"                                          \
  "HABITANTS DE CE COIN PERDU DE L'ENFER."

// Before level 32, put this:

#define C6TEXT                                                                 \
  "FELICITATIONS! VOUS AVEZ DECOUVERT\n"                                       \
  "LE NIVEAU SUPER SECRET! VOUS FERIEZ\n"                                      \
  "MIEUX DE FONCER DANS CELUI-LA!\n"

//
// Character cast strings F_FINALE.C
//
export const auto CC_ZOMBIE = "ZOMBIE";
export const auto CC_SHOTGUN = "TYPE AU FUSIL";
export const auto CC_HEAVY = "MEC SUPER-ARME";
export const auto CC_IMP = "DIABLOTIN";
export const auto CC_DEMON = "DEMON";
export const auto CC_LOST = "AME PERDUE";
export const auto CC_CACO = "CACODEMON";
export const auto CC_HELL = "CHEVALIER DE L'ENFER";
export const auto CC_BARON = "BARON DE L'ENFER";
export const auto CC_ARACH = "ARACHNOTRON";
export const auto CC_PAIN = "ELEMENTAIRE DE LA DOULEUR";
export const auto CC_REVEN = "REVENANT";
export const auto CC_MANCU = "MANCUBUS";
export const auto CC_ARCH = "ARCHI-INFAME";
export const auto CC_SPIDER = "L'ARAIGNEE CERVEAU";
export const auto CC_CYBER = "LE CYBERDEMON";
export const auto CC_HERO = "NOTRE HEROS";

//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------

#else

//
//	Printed strings for translation
//

//
// D_Main.C
//
export const auto D_DEVSTR = "Development mode ON.\n";
export const auto D_CDROM = "CD-ROM Version: default.cfg from c:\\doomdata\n";

//
//	M_Menu.C
//
export const auto PRESSKEY = "press a key.";
export const auto PRESSYN = "press y or n.";
export const auto QUITMSG = "are you sure you want to\nquit this great game?";
export const auto LOADNET = std::format("you can't do load while in a net game!\n\n{}",PRESSKEY);
export const auto QLOADNET = std::format("you can't quickload during a netgame!\n\n{}",PRESSKEY);
export const auto QSAVESPOT = std::format("you haven't picked a quicksave slot yet!\n\n{}",PRESSKEY);
export const auto SAVEDEAD = std::format("you can't save if you aren't playing!\n\n{}",PRESSKEY);
export const auto QSPROMPT = std::format("quicksave over your game named\n\n'%s'?\n\n{}",PRESSYN);
export const auto QLPROMPT = std::format("do you want to quickload the game named\n\n'%s'?\n\n{}",PRESSYN);

export const auto NEWGAME = std::format("you can't start a new game\nwhile in a network game.\n\n{}", PRESSKEY);

export const auto NIGHTMARE = std::format("are you sure? this skill level\nisn't even remotely fair.\n\n", PRESSYN);

export const auto SWSTRING = std::format("this is the shareware version of doom.\n\nyou need to order the entire trilogy.\n\n", PRESSKEY);

export const auto MSGOFF = "Messages OFF";
export const auto MSGON = "Messages ON";
export const auto NETEND = std::format("you can't end a netgame!\n\n{}",PRESSKEY);
export const auto ENDGAME = std::format("are you sure you want to end the game?\n\n{}",PRESSYN);

export const auto DOSY = "(press y to quit)";

export const auto DETAILHI = "High detail";
export const auto DETAILLO = "Low detail";
export const auto GAMMALVL0 = "Gamma correction OFF";
export const auto GAMMALVL1 = "Gamma correction level 1";
export const auto GAMMALVL2 = "Gamma correction level 2";
export const auto GAMMALVL3 = "Gamma correction level 3";
export const auto GAMMALVL4 = "Gamma correction level 4";
export const auto EMPTYSTRING = "empty slot";

//
//	P_inter.C
//
export const auto GOTARMOR = "Picked up the armor.";
export const auto GOTMEGA = "Picked up the MegaArmor!";
export const auto GOTHTHBONUS = "Picked up a health bonus.";
export const auto GOTARMBONUS = "Picked up an armor bonus.";
export const auto GOTSTIM = "Picked up a stimpack.";
export const auto GOTMEDINEED = "Picked up a medikit that you REALLY need!";
export const auto GOTMEDIKIT = "Picked up a medikit.";
export const auto GOTSUPER = "Supercharge!";

export const auto GOTBLUECARD = "Picked up a blue keycard.";
export const auto GOTYELWCARD = "Picked up a yellow keycard.";
export const auto GOTREDCARD = "Picked up a red keycard.";
export const auto GOTBLUESKUL = "Picked up a blue skull key.";
export const auto GOTYELWSKUL = "Picked up a yellow skull key.";
export const auto GOTREDSKULL = "Picked up a red skull key.";

export const auto GOTINVUL = "Invulnerability!";
export const auto GOTBERSERK = "Berserk!";
export const auto GOTINVIS = "Partial Invisibility";
export const auto GOTSUIT = "Radiation Shielding Suit";
export const auto GOTMAP = "Computer Area Map";
export const auto GOTVISOR = "Light Amplification Visor";
export const auto GOTMSPHERE = "MegaSphere!";

export const auto GOTCLIP = "Picked up a clip.";
export const auto GOTCLIPBOX = "Picked up a box of bullets.";
export const auto GOTROCKET = "Picked up a rocket.";
export const auto GOTROCKBOX = "Picked up a box of rockets.";
export const auto GOTCELL = "Picked up an energy cell.";
export const auto GOTCELLBOX = "Picked up an energy cell pack.";
export const auto GOTSHELLS = "Picked up 4 shotgun shells.";
export const auto GOTSHELLBOX = "Picked up a box of shotgun shells.";
export const auto GOTBACKPACK = "Picked up a backpack full of ammo!";

export const auto GOTBFG9000 = "You got the BFG9000!  Oh, yes.";
export const auto GOTCHAINGUN = "You got the chaingun!";
export const auto GOTCHAINSAW = "A chainsaw!  Find some meat!";
export const auto GOTLAUNCHER = "You got the rocket launcher!";
export const auto GOTPLASMA = "You got the plasma gun!";
export const auto GOTSHOTGUN = "You got the shotgun!";
export const auto GOTSHOTGUN2 = "You got the super shotgun!";

//
// P_Doors.C
//
export const auto PD_BLUEO = "You need a blue key to activate this object";
export const auto PD_REDO = "You need a red key to activate this object";
export const auto PD_YELLOWO = "You need a yellow key to activate this object";
export const auto PD_BLUEK = "You need a blue key to open this door";
export const auto PD_REDK = "You need a red key to open this door";
export const auto PD_YELLOWK = "You need a yellow key to open this door";

//
//	G_game.C
//
export const auto GGSAVED = "game saved.";

//
//	HU_stuff.C
//
export const auto HUSTR_MSGU = "[Message unsent]";

export const auto HUSTR_E1M1 = "E1M1: Hangar";
export const auto HUSTR_E1M2 = "E1M2: Nuclear Plant";
export const auto HUSTR_E1M3 = "E1M3: Toxin Refinery";
export const auto HUSTR_E1M4 = "E1M4: Command Control";
export const auto HUSTR_E1M5 = "E1M5: Phobos Lab";
export const auto HUSTR_E1M6 = "E1M6: Central Processing";
export const auto HUSTR_E1M7 = "E1M7: Computer Station";
export const auto HUSTR_E1M8 = "E1M8: Phobos Anomaly";
export const auto HUSTR_E1M9 = "E1M9: Military Base";

export const auto HUSTR_E2M1 = "E2M1: Deimos Anomaly";
export const auto HUSTR_E2M2 = "E2M2: Containment Area";
export const auto HUSTR_E2M3 = "E2M3: Refinery";
export const auto HUSTR_E2M4 = "E2M4: Deimos Lab";
export const auto HUSTR_E2M5 = "E2M5: Command Center";
export const auto HUSTR_E2M6 = "E2M6: Halls of the Damned";
export const auto HUSTR_E2M7 = "E2M7: Spawning Vats";
export const auto HUSTR_E2M8 = "E2M8: Tower of Babel";
export const auto HUSTR_E2M9 = "E2M9: Fortress of Mystery";

export const auto HUSTR_E3M1 = "E3M1: Hell Keep";
export const auto HUSTR_E3M2 = "E3M2: Slough of Despair";
export const auto HUSTR_E3M3 = "E3M3: Pandemonium";
export const auto HUSTR_E3M4 = "E3M4: House of Pain";
export const auto HUSTR_E3M5 = "E3M5: Unholy Cathedral";
export const auto HUSTR_E3M6 = "E3M6: Mt. Erebus";
export const auto HUSTR_E3M7 = "E3M7: Limbo";
export const auto HUSTR_E3M8 = "E3M8: Dis";
export const auto HUSTR_E3M9 = "E3M9: Warrens";

export const auto HUSTR_E4M1 = "E4M1: Hell Beneath";
export const auto HUSTR_E4M2 = "E4M2: Perfect Hatred";
export const auto HUSTR_E4M3 = "E4M3: Sever The Wicked";
export const auto HUSTR_E4M4 = "E4M4: Unruly Evil";
export const auto HUSTR_E4M5 = "E4M5: They Will Repent";
export const auto HUSTR_E4M6 = "E4M6: Against Thee Wickedly";
export const auto HUSTR_E4M7 = "E4M7: And Hell Followed";
export const auto HUSTR_E4M8 = "E4M8: Unto The Cruel";
export const auto HUSTR_E4M9 = "E4M9: Fear";

export const auto HUSTR_1 = "level 1: entryway";
export const auto HUSTR_2 = "level 2: underhalls";
export const auto HUSTR_3 = "level 3: the gantlet";
export const auto HUSTR_4 = "level 4: the focus";
export const auto HUSTR_5 = "level 5: the waste tunnels";
export const auto HUSTR_6 = "level 6: the crusher";
export const auto HUSTR_7 = "level 7: dead simple";
export const auto HUSTR_8 = "level 8: tricks and traps";
export const auto HUSTR_9 = "level 9: the pit";
export const auto HUSTR_10 = "level 10: refueling base";
export const auto HUSTR_11 = "level 11: 'o' of destruction!";

export const auto HUSTR_12 = "level 12: the factory";
export const auto HUSTR_13 = "level 13: downtown";
export const auto HUSTR_14 = "level 14: the inmost dens";
export const auto HUSTR_15 = "level 15: industrial zone";
export const auto HUSTR_16 = "level 16: suburbs";
export const auto HUSTR_17 = "level 17: tenements";
export const auto HUSTR_18 = "level 18: the courtyard";
export const auto HUSTR_19 = "level 19: the citadel";
export const auto HUSTR_20 = "level 20: gotcha!";

export const auto HUSTR_21 = "level 21: nirvana";
export const auto HUSTR_22 = "level 22: the catacombs";
export const auto HUSTR_23 = "level 23: barrels o' fun";
export const auto HUSTR_24 = "level 24: the chasm";
export const auto HUSTR_25 = "level 25: bloodfalls";
export const auto HUSTR_26 = "level 26: the abandoned mines";
export const auto HUSTR_27 = "level 27: monster condo";
export const auto HUSTR_28 = "level 28: the spirit world";
export const auto HUSTR_29 = "level 29: the living end";
export const auto HUSTR_30 = "level 30: icon of sin";

export const auto HUSTR_31 = "level 31: wolfenstein";
export const auto HUSTR_32 = "level 32: grosse";

export const auto PHUSTR_1 = "level 1: congo";
export const auto PHUSTR_2 = "level 2: well of souls";
export const auto PHUSTR_3 = "level 3: aztec";
export const auto PHUSTR_4 = "level 4: caged";
export const auto PHUSTR_5 = "level 5: ghost town";
export const auto PHUSTR_6 = "level 6: baron's lair";
export const auto PHUSTR_7 = "level 7: caughtyard";
export const auto PHUSTR_8 = "level 8: realm";
export const auto PHUSTR_9 = "level 9: abattoire";
export const auto PHUSTR_10 = "level 10: onslaught";
export const auto PHUSTR_11 = "level 11: hunted";

export const auto PHUSTR_12 = "level 12: speed";
export const auto PHUSTR_13 = "level 13: the crypt";
export const auto PHUSTR_14 = "level 14: genesis";
export const auto PHUSTR_15 = "level 15: the twilight";
export const auto PHUSTR_16 = "level 16: the omen";
export const auto PHUSTR_17 = "level 17: compound";
export const auto PHUSTR_18 = "level 18: neurosphere";
export const auto PHUSTR_19 = "level 19: nme";
export const auto PHUSTR_20 = "level 20: the death domain";

export const auto PHUSTR_21 = "level 21: slayer";
export const auto PHUSTR_22 = "level 22: impossible mission";
export const auto PHUSTR_23 = "level 23: tombstone";
export const auto PHUSTR_24 = "level 24: the final frontier";
export const auto PHUSTR_25 = "level 25: the temple of darkness";
export const auto PHUSTR_26 = "level 26: bunker";
export const auto PHUSTR_27 = "level 27: anti-christ";
export const auto PHUSTR_28 = "level 28: the sewers";
export const auto PHUSTR_29 = "level 29: odyssey of noises";
export const auto PHUSTR_30 = "level 30: the gateway of hell";

export const auto PHUSTR_31 = "level 31: cyberden";
export const auto PHUSTR_32 = "level 32: go 2 it";

export const auto THUSTR_1 = "level 1: system control";
export const auto THUSTR_2 = "level 2: human bbq";
export const auto THUSTR_3 = "level 3: power control";
export const auto THUSTR_4 = "level 4: wormhole";
export const auto THUSTR_5 = "level 5: hanger";
export const auto THUSTR_6 = "level 6: open season";
export const auto THUSTR_7 = "level 7: prison";
export const auto THUSTR_8 = "level 8: metal";
export const auto THUSTR_9 = "level 9: stronghold";
export const auto THUSTR_10 = "level 10: redemption";
export const auto THUSTR_11 = "level 11: storage facility";

export const auto THUSTR_12 = "level 12: crater";
export const auto THUSTR_13 = "level 13: nukage processing";
export const auto THUSTR_14 = "level 14: steel works";
export const auto THUSTR_15 = "level 15: dead zone";
export const auto THUSTR_16 = "level 16: deepest reaches";
export const auto THUSTR_17 = "level 17: processing area";
export const auto THUSTR_18 = "level 18: mill";
export const auto THUSTR_19 = "level 19: shipping/respawning";
export const auto THUSTR_20 = "level 20: central processing";

export const auto THUSTR_21 = "level 21: administration center";
export const auto THUSTR_22 = "level 22: habitat";
export const auto THUSTR_23 = "level 23: lunar mining project";
export const auto THUSTR_24 = "level 24: quarry";
export const auto THUSTR_25 = "level 25: baron's den";
export const auto THUSTR_26 = "level 26: ballistyx";
export const auto THUSTR_27 = "level 27: mount pain";
export const auto THUSTR_28 = "level 28: heck";
export const auto THUSTR_29 = "level 29: river styx";
export const auto THUSTR_30 = "level 30: last call";

export const auto THUSTR_31 = "level 31: pharaoh";
export const auto THUSTR_32 = "level 32: caribbean";

export const auto HUSTR_CHATMACRO1 = "I'm ready to kick butt!";
export const auto HUSTR_CHATMACRO2 = "I'm OK.";
export const auto HUSTR_CHATMACRO3 = "I'm not looking too good!";
export const auto HUSTR_CHATMACRO4 = "Help!";
export const auto HUSTR_CHATMACRO5 = "You suck!";
export const auto HUSTR_CHATMACRO6 = "Next time, scumbag...";
export const auto HUSTR_CHATMACRO7 = "Come here!";
export const auto HUSTR_CHATMACRO8 = "I'll take care of it.";
export const auto HUSTR_CHATMACRO9 = "Yes";
export const auto HUSTR_CHATMACRO0 = "No";

export const auto HUSTR_TALKTOSELF1 = "You mumble to yourself";
export const auto HUSTR_TALKTOSELF2 = "Who's there?";
export const auto HUSTR_TALKTOSELF3 = "You scare yourself";
export const auto HUSTR_TALKTOSELF4 = "You start to rave";
export const auto HUSTR_TALKTOSELF5 = "You've lost it...";

export const auto HUSTR_MESSAGESENT = "[Message Sent]";

// The following should NOT be changed unless it seems
// just AWFULLY necessary

export const auto HUSTR_PLRGREEN = "Green: ";
export const auto HUSTR_PLRINDIGO = "Indigo: ";
export const auto HUSTR_PLRBROWN = "Brown: ";
export const auto HUSTR_PLRRED = "Red: ";

export const auto HUSTR_KEYGREEN = sf::Keyboard::Key::G;
export const auto HUSTR_KEYINDIGO = sf::Keyboard::Key::I;
export const auto HUSTR_KEYBROWN = sf::Keyboard::Key::B;
export const auto HUSTR_KEYRED = sf::Keyboard::Key::R;

//
//	AM_map.C
//

export const auto AMSTR_FOLLOWON = "Follow Mode ON";
export const auto AMSTR_FOLLOWOFF = "Follow Mode OFF";

export const auto AMSTR_GRIDON = "Grid ON";
export const auto AMSTR_GRIDOFF = "Grid OFF";

export const auto AMSTR_MARKEDSPOT = "Marked Spot";
export const auto AMSTR_MARKSCLEARED = "All Marks Cleared";

//
//	ST_stuff.C
//

export const auto STSTR_MUS = "Music Change";
export const auto STSTR_NOMUS = "IMPOSSIBLE SELECTION";
export const auto STSTR_DQDON = "Degreelessness Mode On";
export const auto STSTR_DQDOFF = "Degreelessness Mode Off";

export const auto STSTR_KFAADDED = "Very Happy Ammo Added";
export const auto STSTR_FAADDED = "Ammo (no keys) Added";

export const auto STSTR_NCON = "No Clipping Mode ON";
export const auto STSTR_NCOFF = "No Clipping Mode OFF";

export const auto STSTR_BEHOLD = "inVuln, Str, Inviso, Rad, Allmap, or Lite-amp";
export const auto STSTR_BEHOLDX = "Power-up Toggled";

export const auto STSTR_CHOPPERS = "... doesn't suck - GM";
export const auto STSTR_CLEV = "Changing Level...";

//
//	F_Finale.C
//
export auto E1TEXT =                                                                 \
  "Once you beat the big badasses and\n"                                       \
  "clean out the moon base you're supposed\n"                                  \
  "to win, aren't you? Aren't you? Where's\n"                                  \
  "your fat reward and ticket home? What\n"                                    \
  "the hell is this? It's not supposed to\n"                                   \
  "end this way!\n"                                                            \
  "\n"                                                                         \
  "It stinks like rotten meat, but looks\n"                                    \
  "like the lost Deimos base.  Looks like\n"                                   \
  "you're stuck on The Shores of Hell.\n"                                      \
  "The only way out is through.\n"                                             \
  "\n"                                                                         \
  "To continue the DOOM experience, play\n"                                    \
  "The Shores of Hell and its amazing\n"                                       \
  "sequel, Inferno!\n";

export auto E2TEXT =                                                                 \
  "You've done it! The hideous cyber-\n"                                       \
  "demon lord that ruled the lost Deimos\n"                                    \
  "moon base has been slain and you\n"                                         \
  "are triumphant! But ... where are\n"                                        \
  "you? You clamber to the edge of the\n"                                      \
  "moon and look down to see the awful\n"                                      \
  "truth.\n"                                                                   \
  "\n"                                                                         \
  "Deimos floats above Hell itself!\n"                                         \
  "You've never heard of anyone escaping\n"                                    \
  "from Hell, but you'll make the bastards\n"                                  \
  "sorry they ever heard of you! Quickly,\n"                                   \
  "you rappel down to  the surface of\n"                                       \
  "Hell.\n"                                                                    \
  "\n"                                                                         \
  "Now, it's on to the final chapter of\n"                                     \
  "DOOM! -- Inferno.";

export auto E3TEXT =                                                                 \
  "The loathsome spiderdemon that\n"                                           \
  "masterminded the invasion of the moon\n"                                    \
  "bases and caused so much death has had\n"                                   \
  "its ass kicked for all time.\n"                                             \
  "\n"                                                                         \
  "A hidden doorway opens and you enter.\n"                                    \
  "You've proven too tough for Hell to\n"                                      \
  "contain, and now Hell at last plays\n"                                      \
  "fair -- for you emerge from the door\n"                                     \
  "to see the green fields of Earth!\n"                                        \
  "Home at last.\n"                                                            \
  "\n"                                                                         \
  "You wonder what's been happening on\n"                                      \
  "Earth while you were battling evil\n"                                       \
  "unleashed. It's good that no Hell-\n"                                       \
  "spawn could have come through that\n"                                       \
  "door with you ...";

export auto E4TEXT =                                                                 \
  "the spider mastermind must have sent forth\n"                               \
  "its legions of hellspawn before your\n"                                     \
  "final confrontation with that terrible\n"                                   \
  "beast from hell.  but you stepped forward\n"                                \
  "and brought forth eternal damnation and\n"                                  \
  "suffering upon the horde as a true hero\n"                                  \
  "would in the face of something so evil.\n"                                  \
  "\n"                                                                         \
  "besides, someone was gonna pay for what\n"                                  \
  "happened to daisy, your pet rabbit.\n"                                      \
  "\n"                                                                         \
  "but now, you see spread before you more\n"                                  \
  "potential pain and gibbitude as a nation\n"                                 \
  "of demons run amok among our cities.\n"                                     \
  "\n"                                                                         \
  "next stop, hell on earth!";

// after level 6, put this:

export auto C1TEXT =                                                                 \
  "YOU HAVE ENTERED DEEPLY INTO THE INFESTED\n"                                \
  "STARPORT. BUT SOMETHING IS WRONG. THE\n"                                    \
  "MONSTERS HAVE BROUGHT THEIR OWN REALITY\n"                                  \
  "WITH THEM, AND THE STARPORT'S TECHNOLOGY\n"                                 \
  "IS BEING SUBVERTED BY THEIR PRESENCE.\n"                                    \
  "\n"                                                                         \
  "AHEAD, YOU SEE AN OUTPOST OF HELL, A\n"                                     \
  "FORTIFIED ZONE. IF YOU CAN GET PAST IT,\n"                                  \
  "YOU CAN PENETRATE INTO THE HAUNTED HEART\n"                                 \
  "OF THE STARBASE AND FIND THE CONTROLLING\n"                                 \
  "SWITCH WHICH HOLDS EARTH'S POPULATION\n"                                    \
  "HOSTAGE.";

// After level 11, put this:

export auto C2TEXT =                                                                 \
  "YOU HAVE WON! YOUR VICTORY HAS ENABLED\n"                                   \
  "HUMANKIND TO EVACUATE EARTH AND ESCAPE\n"                                   \
  "THE NIGHTMARE.  NOW YOU ARE THE ONLY\n"                                     \
  "HUMAN LEFT ON THE FACE OF THE PLANET.\n"                                    \
  "CANNIBAL MUTATIONS, CARNIVOROUS ALIENS,\n"                                  \
  "AND EVIL SPIRITS ARE YOUR ONLY NEIGHBORS.\n"                                \
  "YOU SIT BACK AND WAIT FOR DEATH, CONTENT\n"                                 \
  "THAT YOU HAVE SAVED YOUR SPECIES.\n"                                        \
  "\n"                                                                         \
  "BUT THEN, EARTH CONTROL BEAMS DOWN A\n"                                     \
  "MESSAGE FROM SPACE: \"SENSORS HAVE LOCATED\n"                               \
  "THE SOURCE OF THE ALIEN INVASION. IF YOU\n"                                 \
  "GO THERE, YOU MAY BE ABLE TO BLOCK THEIR\n"                                 \
  "ENTRY.  THE ALIEN BASE IS IN THE HEART OF\n"                                \
  "YOUR OWN HOME CITY, NOT FAR FROM THE\n"                                     \
  "STARPORT.\" SLOWLY AND PAINFULLY YOU GET\n"                                 \
  "UP AND RETURN TO THE FRAY.";

// After level 20, put this:

export auto C3TEXT =                                                                 \
  "YOU ARE AT THE CORRUPT HEART OF THE CITY,\n"                                \
  "SURROUNDED BY THE CORPSES OF YOUR ENEMIES.\n"                               \
  "YOU SEE NO WAY TO DESTROY THE CREATURES'\n"                                 \
  "ENTRYWAY ON THIS SIDE, SO YOU CLENCH YOUR\n"                                \
  "TEETH AND PLUNGE THROUGH IT.\n"                                             \
  "\n"                                                                         \
  "THERE MUST BE A WAY TO CLOSE IT ON THE\n"                                   \
  "OTHER SIDE. WHAT DO YOU CARE IF YOU'VE\n"                                   \
  "GOT TO GO THROUGH HELL TO GET TO IT?";

// After level 29, put this:

export auto C4TEXT =                                                                 \
  "THE HORRENDOUS VISAGE OF THE BIGGEST\n"                                     \
  "DEMON YOU'VE EVER SEEN CRUMBLES BEFORE\n"                                   \
  "YOU, AFTER YOU PUMP YOUR ROCKETS INTO\n"                                    \
  "HIS EXPOSED BRAIN. THE MONSTER SHRIVELS\n"                                  \
  "UP AND DIES, ITS THRASHING LIMBS\n"                                         \
  "DEVASTATING UNTOLD MILES OF HELL'S\n"                                       \
  "SURFACE.\n"                                                                 \
  "\n"                                                                         \
  "YOU'VE DONE IT. THE INVASION IS OVER.\n"                                    \
  "EARTH IS SAVED. HELL IS A WRECK. YOU\n"                                     \
  "WONDER WHERE BAD FOLKS WILL GO WHEN THEY\n"                                 \
  "DIE, NOW. WIPING THE SWEAT FROM YOUR\n"                                     \
  "FOREHEAD YOU BEGIN THE LONG TREK BACK\n"                                    \
  "HOME. REBUILDING EARTH OUGHT TO BE A\n"                                     \
  "LOT MORE FUN THAN RUINING IT WAS.\n";

// Before level 31, put this:

export auto C5TEXT =                                                                 \
  "CONGRATULATIONS, YOU'VE FOUND THE SECRET\n"                                 \
  "LEVEL! LOOKS LIKE IT'S BEEN BUILT BY\n"                                     \
  "HUMANS, RATHER THAN DEMONS. YOU WONDER\n"                                   \
  "WHO THE INMATES OF THIS CORNER OF HELL\n"                                   \
  "WILL BE.";

// Before level 32, put this:

export auto C6TEXT =                                                                 \
  "CONGRATULATIONS, YOU'VE FOUND THE\n"                                        \
  "SUPER SECRET LEVEL!  YOU'D BETTER\n"                                        \
  "BLAZE THROUGH THIS ONE!\n";

// after map 06

export auto P1TEXT =                                                                 \
  "You gloat over the steaming carcass of the\n"                               \
  "Guardian.  With its death, you've wrested\n"                                \
  "the Accelerator from the stinking claws\n"                                  \
  "of Hell.  You relax and glance around the\n"                                \
  "room.  Damn!  There was supposed to be at\n"                                \
  "least one working prototype, but you can't\n"                               \
  "see it. The demons must have taken it.\n"                                   \
  "\n"                                                                         \
  "You must find the prototype, or all your\n"                                 \
  "struggles will have been wasted. Keep\n"                                    \
  "moving, keep fighting, keep killing.\n"                                     \
  "Oh yes, keep living, too.";

// after map 11

export auto P2TEXT =                                                                 \
  "Even the deadly Arch-Vile labyrinth could\n"                                \
  "not stop you, and you've gotten to the\n"                                   \
  "prototype Accelerator which is soon\n"                                      \
  "efficiently and permanently deactivated.\n"                                 \
  "\n"                                                                         \
  "You're good at that kind of thing.";

// after map 20

export auto P3TEXT =                                                                 \
  "You've bashed and battered your way into\n"                                 \
  "the heart of the devil-hive.  Time for a\n"                                 \
  "Search-and-Destroy mission, aimed at the\n"                                 \
  "Gatekeeper, whose foul offspring is\n"                                      \
  "cascading to Earth.  Yeah, he's bad. But\n"                                 \
  "you know who's worse!\n"                                                    \
  "\n"                                                                         \
  "Grinning evilly, you check your gear, and\n"                                \
  "get ready to give the bastard a little Hell\n"                              \
  "of your own making!";

// after map 30

export auto P4TEXT =                                                                 \
  "The Gatekeeper's evil face is splattered\n"                                 \
  "all over the place.  As its tattered corpse\n"                              \
  "collapses, an inverted Gate forms and\n"                                    \
  "sucks down the shards of the last\n"                                        \
  "prototype Accelerator, not to mention the\n"                                \
  "few remaining demons.  You're done. Hell\n"                                 \
  "has gone back to pounding bad dead folks \n"                                \
  "instead of good live ones.  Remember to\n"                                  \
  "tell your grandkids to put a rocket\n"                                      \
  "launcher in your coffin. If you go to Hell\n"                               \
  "when you die, you'll need it for some\n"                                    \
  "final cleaning-up ...";

// before map 31

export auto P5TEXT =                                                                  \
  "You've found the second-hardest level we\n"                                 \
  "got. Hope you have a saved game a level or\n"                               \
  "two previous.  If not, be prepared to die\n"                                \
  "aplenty. For master marines only.";

// before map 32

export auto P6TEXT =                                                                 \
  "Betcha wondered just what WAS the hardest\n"                                \
  "level we had ready for ya?  Now you know.\n"                                \
  "No one gets out alive.";

export auto T1TEXT =                                                                 \
  "You've fought your way out of the infested\n"                               \
  "experimental labs.   It seems that UAC has\n"                               \
  "once again gulped it down.  With their\n"                                   \
  "high turnover, it must be hard for poor\n"                                  \
  "old UAC to buy corporate health insurance\n"                                \
  "nowadays..\n"                                                               \
  "\n"                                                                         \
  "Ahead lies the military complex, now\n"                                     \
  "swarming with diseased horrors hot to get\n"                                \
  "their teeth into you. With luck, the\n"                                     \
  "complex still has some warlike ordnance\n"                                  \
  "laying around.";

export auto T2TEXT =                                                                 \
  "You hear the grinding of heavy machinery\n"                                 \
  "ahead.  You sure hope they're not stamping\n"                               \
  "out new hellspawn, but you're ready to\n"                                   \
  "ream out a whole herd if you have to.\n"                                    \
  "They might be planning a blood feast, but\n"                                \
  "you feel about as mean as two thousand\n"                                   \
  "maniacs packed into one mad killer.\n"                                      \
  "\n"                                                                         \
  "You don't plan to go down easy.";

export auto T3TEXT =                                                                 \
  "The vista opening ahead looks real damn\n"                                  \
  "familiar. Smells familiar, too -- like\n"                                   \
  "fried excrement. You didn't like this\n"                                    \
  "place before, and you sure as hell ain't\n"                                 \
  "planning to like it now. The more you\n"                                    \
  "brood on it, the madder you get.\n"                                         \
  "Hefting your gun, an evil grin trickles\n"                                  \
  "onto your face. Time to take some names.";

export auto T4TEXT =                                                                 \
  "Suddenly, all is silent, from one horizon\n"                                \
  "to the other. The agonizing echo of Hell\n"                                 \
  "fades away, the nightmare sky turns to\n"                                   \
  "blue, the heaps of monster corpses start \n"                                \
  "to evaporate along with the evil stench \n"                                 \
  "that filled the air. Jeeze, maybe you've\n"                                 \
  "done it. Have you really won?\n"                                            \
  "\n"                                                                         \
  "Something rumbles in the distance.\n"                                       \
  "A blue light begins to glow inside the\n"                                   \
  "ruined skull of the demon-spitter.";

export auto T5TEXT =                                                                 \
  "What now? Looks totally different. Kind\n"                                  \
  "of like King Tut's condo. Well,\n"                                          \
  "whatever's here can't be any worse\n"                                       \
  "than usual. Can it?  Or maybe it's best\n"                                  \
  "to let sleeping gods lie..";

export auto T6TEXT =                                                                 \
  "Time for a vacation. You've burst the\n"                                    \
  "bowels of hell and by golly you're ready\n"                                 \
  "for a break. You mutter to yourself,\n"                                     \
  "Maybe someone else can kick Hell's ass\n"                                   \
  "next time around. Ahead lies a quiet town,\n"                               \
  "with peaceful flowing water, quaint\n"                                      \
  "buildings, and presumably no Hellspawn.\n"                                  \
  "\n"                                                                         \
  "As you step off the transport, you hear\n"                                  \
  "the stomp of a cyberdemon's iron shoe.";

//
// Character cast strings F_FINALE.C
//
export const auto CC_ZOMBIE = "ZOMBIEMAN";
export const auto CC_SHOTGUN = "SHOTGUN GUY";
export const auto CC_HEAVY = "HEAVY WEAPON DUDE";
export const auto CC_IMP = "IMP";
export const auto CC_DEMON = "DEMON";
export const auto CC_LOST = "LOST SOUL";
export const auto CC_CACO = "CACODEMON";
export const auto CC_HELL = "HELL KNIGHT";
export const auto CC_BARON = "BARON OF HELL";
export const auto CC_ARACH = "ARACHNOTRON";
export const auto CC_PAIN = "PAIN ELEMENTAL";
export const auto CC_REVEN = "REVENANT";
export const auto CC_MANCU = "MANCUBUS";
export const auto CC_ARCH = "ARCH-VILE";
export const auto CC_SPIDER = "THE SPIDER MASTERMIND";
export const auto CC_CYBER = "THE CYBERDEMON";
export const auto CC_HERO = "OUR HERO";

//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------


#endif

export const char *endmsg[NUM_QUITMESSAGES + 1] = {
    // DOOM1
    QUITMSG, "please don't leave, there's more\ndemons to toast!",
    "let's beat it -- this is turning\ninto a bloodbath!",
    "i wouldn't leave if i were you.\ndos is much worse.",
    "you're trying to say you like dos\nbetter than me, right?",
    "don't leave yet -- there's a\ndemon around that corner!",
    "ya know, next time you come in here\ni'm gonna toast ya.",
    "go ahead and leave. see if i care."

    // QuitDOOM II messages
    "you want to quit?\nthen, thou hast lost an eighth!",
    "don't go now, there's a \ndimensional shambler waiting\nat the dos "
    "prompt!",
    "get outta here and go back\nto your boring programs.",
    "if i were your boss, i'd \n deathmatch ya in a minute!",
    "look, bud. you leave now\nand you forfeit your body count!",
    "just leave. when you come\nback, i'll be waiting with a bat.",
    "you're lucky i don't smack\nyou for thinking about leaving."

    // FinalDOOM?
    "fuck you, pussy!\nget the fuck out!",
    "you quit and i'll jizz\nin your cystholes!",
    "if you leave, i'll make\nthe lord drink my jizz.",
    "hey, ron! can we say\n'fuck' in the game?",
    "i'd leave: this is just\nmore monsters and levels.\nwhat a load.",
    "suck it down, asshole!\nyou're a fucking wimp!",
    "don't quit now! we're \nstill spending your money!",

    // Internal debug. Different style, too.
    "THIS IS NO MESSAGE!\nPage intentionally left blank."};