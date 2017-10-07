#include "ExampleParser.h"
#include "JsonListener.h"
#include "includes/string_table.h"

// https://github.com/squix78/json-streaming-parser

GameData *lGame;

// Parsing tools:
uint8_t OBJECT_DEPTH_INDEX = 0;
uint8_t dKeys [] = {KEY_DUMMYPAD, KEY_DUMMYPAD, INVALID, INVALID, INVALID, INVALID, INVALID}; // PAD ARRAY SO INDEX MATCHES JSON OBJECT NEST DEPTH. ALSO DON'T CARE ABOUT DEPTH=1

bool nextValGrab = false; // flag set by key for whether value is of value

// !!! TRY TO SEE IF I CAN REMOVE TICKETSMAX & OTHER MAX
// note: removed "CarrierAssaultLarge" from depdth 2 keyword search array
const char * depth_2_keywordSearch = "currentMap waitingPlayers roundTime gameMode conquest rush captureTheFlag carrierAssault deathmatch teamInfo";   // ** TODO: OPTIMIZE. EASIER TO PROVIDE BLACKLIST THAN WHITELIST   //const char * depth_4_keywordSearch = "tickets ticketsMax bases basesMax flags flagsMax destroyedCrates carrierHealth kills killsMax faction";   // ** TODO: OPTIMIZE. Will kills be pickedup by killsMax? Same for others...
const char * depth_4_keywordSearch = "ticketsMax basesMax flagsMax destroyedCrates carrierHealth killsMax faction";   // ** TODO: OPTIMIZE. Will kills be pickedup by killsMax? Same for others...
const char * depth_6_keywordSearch = "rank kills deaths";
/*
  Score structure:
    OB/CQ/AS/DO/CL  ->   "conquest"         (tickets/ticketsMax)
    CTF:            ->   "captureTheFlag"   (flags/flagsMax)
    CA:             ->   "carrierAssault"   (destroyedCrates/carrierHealth)
    R:              ->   "rush"             (bases/basesMax) & (tickets/ticketsmax) team 1 "defenders" team 2 "attackers"
    TD, SD:         ->   "deathmatch"       (kills/killsMax)
    GM              ->   "gunmaster"        ("max_level" global only)
    DE              ->    n/a (unsupported)
*/

// *********************************************
void  setGameMode(String gm) {
  if (gm == "TeamDeathMatch")              lGame->mode = MODE_TDM;
  else if (gm == "RushLarge")            { lGame->mode = MODE_R;  lGame->rushStarted = false; }
  else if (gm == "CaptureTheFlag")         lGame->mode = MODE_CTF;
  else if (gm == "CarrierAssaultLarge" || gm == "CarrierAssaultSmall")    lGame->mode = MODE_CA;
  else if (gm == "Domination")             lGame->mode = MODE_DOM;
  else if (gm == "ConquestLarge")          lGame->mode = MODE_CQ;
  else if (gm == "Obliteration")           lGame->mode = MODE_OB;
  else if (gm == "GunMaster")              lGame->mode = MODE_GM;
  else if (gm == "Chainlink")              lGame->mode = MODE_CL;
  else if (gm == "AirSuperiority")         lGame->mode = MODE_AS;
  else if (gm == "SquadDeathMatch")        lGame->mode = MODE_SD;
  else if (gm == "Elimination")            lGame->mode = MODE_DE;
  else  lGame->mode = INVALID;
}

//    ** must be in stripped format (discard directories prefix path...)
void setGameMap(String map) {
  if      (map == "MP_Abandoned")   lGame->map = MAP_ZAVOD;
  else if (map == "MP_Damage")      lGame->map = MAP_LANCANG;
  else if (map == "MP_Siege")       lGame->map = MAP_SIEGE;
  else if (map == "MP_Resort")      lGame->map = MAP_HAINAN;
  else if (map == "MP_Prison")      lGame->map = MAP_LOCKER;
  else if (map == "MP_Flooded")     lGame->map = MAP_FLOOD;
  else if (map == "MP_Journey")     lGame->map = MAP_GOLMUD;
  else if (map == "MP_Naval")       lGame->map = MAP_PARCEL;
  else if (map == "MP_TheDish")     lGame->map = MAP_ROGUE;
  else if (map == "MP_Tremors")     lGame->map = MAP_DAWN;
  else if (map == "XP0_Oman")       lGame->map = MAP_GULF;
  else if (map == "XP0_Caspian")    lGame->map = MAP_CASPIAN;
  else if (map == "XP0_Metro")      lGame->map = MAP_METRO;
  else if (map == "XP0_Firestorm")  lGame->map = MAP_FIRESTORM;
  else if (map == "XP1_001")        lGame->map = MAP_SILK;
  else if (map == "XP1_002")        lGame->map = MAP_ALTAI;
  else if (map == "XP1_003")        lGame->map = MAP_GULLIN;
  else if (map == "XP1_004")        lGame->map = MAP_DRAGON;
  else if (map == "XP2_001")        lGame->map = MAP_LOST;
  else if (map == "XP2_002")        lGame->map = MAP_NANSHA;
  else if (map == "XP2_003")        lGame->map = MAP_WAVE;
  else if (map == "XP2_004")        lGame->map = MAP_MORTAR;
  else if (map == "XP3_MarketPl")   lGame->map = MAP_PEARL;
  else if (map == "XP3_Prpganda")   lGame->map = MAP_PROPAGANDA;
  else if (map == "XP3_UrbanGdn")   lGame->map = MAP_LUMPHINI;
  else if (map == "XP3_WtrFront")   lGame->map = MAP_SUNKEN;
  else if (map == "XP4_Arctic")     lGame->map = MAP_WHITEOUT;
  else if (map == "XP4_SubBase")    lGame->map = MAP_HAMMER;
  else if (map == "XP4_Titan")      lGame->map = MAP_HANGER;
  else if (map == "XP4_WlkrFtry")   lGame->map = MAP_GIANTS;
  else if (map == "XP5_Night_01")   lGame->map = MAP_ZAVOD_NIGHT;
  else if (map == "XP6_CMP")        lGame->map = MAP_OUTBREAK;
  else if (map == "XP7_Valley")     lGame->map = MAP_VALLEY;
  else                              lGame->map = INVALID;
}

ExampleListener::ExampleListener(GameData *d)
{
  lGame = d;
  resetStats();
}

// TODO:  implement modes: AS, SD, DE
void ExampleListener::key(String key) {
  switch (OBJECT_DEPTH_INDEX) {
    case 2:
      if (strstr(depth_2_keywordSearch, key.c_str()) != NULL) {
        if (key == "currentMap")          dKeys[2] = KEY_2_CURRENTMAP;
        else if (key == "waitingPlayers") dKeys[2] = KEY_2_WAITINGPLAYERS;
        else if (key == "roundTime")      dKeys[2] = KEY_2_ROUNDTIME;
        else if (key == "gameMode")       dKeys[2] = KEY_2_GAMEMMODE;
        else if (key == "conquest")       dKeys[2] = KEY_2_CONQUEST;
        else if (key == "rush")           {dKeys[2] = KEY_2_RUSH;   lGame->rushStarted = true;}
        else if (key == "captureTheFlag") dKeys[2] = KEY_2_CAPTURETHEFLAG;
        else if (key == "CarrierAssaultLarge" || key == "carrierAssault") dKeys[2] = KEY_2_CARRIERASSAULT;
        else if (key == "deathmatch")     dKeys[2] = KEY_2_DEATHMATCH;
        else if (key == "teamInfo")       dKeys[2] = KEY_2_TEAMINFO;
        else                              dKeys[2] = KEY_INVALID;  // might not be necessary??
        nextValGrab = true;   // is this necesesary??
      }
      break;

    case 3:  // TEAM
      if (key == "0")                       dKeys[3] = KEY_3_0;
      else if (key == "1")                  dKeys[3] = KEY_3_1;
      else if (key == "2")                  dKeys[3] = KEY_3_2;
      else                                  dKeys[3] = KEY_INVALID;

      break;

    case 4:                   // ** todo: optimize value/max into some sort of struct...
      if (strstr(depth_4_keywordSearch, key.c_str()) != NULL) {
        nextValGrab = true;
        if (key == "tickets")               dKeys[4] = KEY_4_TICKETS;
        else if (key == "ticketsMax")       dKeys[4] = KEY_4_TICKETSMAX;
        else if (key == "bases")            dKeys[4] = KEY_4_BASES;
        else if (key == "basesMax")         dKeys[4] = KEY_4_BASESMAX;
        else if (key == "flags")            dKeys[4] = KEY_4_FLAGS;
        else if (key == "flagsMax")         dKeys[4] = KEY_4_FLAGSMAX;
        else if (key == "destroyedCrates")  dKeys[4] = KEY_4_DESTROYEDCRATES;
        else if (key == "carrierHealth")    dKeys[4] = KEY_4_CARRIERHEALTH;
        else if (key == "kills")            dKeys[4] = KEY_4_KILLS;
        else if (key == "killsMax")         dKeys[4] = KEY_4_KILLSMAX;
        else if (key == "faction")          dKeys[4] = KEY_4_FACTION;
        else                                dKeys[4] = KEY_INVALID;
      }
      break;

    case 5:              // PLAYER COUNT + TEAM FACTIONS
      (lGame->players)++;    // is this inadvertantly counting spectators???
      break;

    case 6:
      if (strstr(depth_6_keywordSearch, key.c_str()) != NULL) {
        nextValGrab = true;
        if (key == "rank")          dKeys[6] = KEY_6_RANK;
        else if (key == "kills")    dKeys[6] = KEY_6_KILLS;
        else if (key == "deaths")   dKeys[6] = KEY_6_DEATHS;
        else                        dKeys[6] = KEY_INVALID;
      }
      break;

    default:
      break;
  }
}

//  //0:US  1:RU  2:CN
void setTeamFaction(uint8_t teamNum, uint8_t factionID) {
  if (teamNum == KEY_3_1)
    lGame->faction.T1 = factionID;
  else if (teamNum == KEY_3_2)
    lGame->faction.T2 = factionID;
}

void ExampleListener::value(String value) {
  if (nextValGrab) {
    int iVal = value.toInt();
    switch (OBJECT_DEPTH_INDEX) {
      // ---------------------------------------------------------------------------------------------
      case 2:
        if (dKeys[2] == KEY_2_GAMEMMODE)             setGameMode(value);
        else if (dKeys[2] == KEY_2_CURRENTMAP)       setGameMap(value.substring(value.lastIndexOf('/') + 1 ));
        else if (dKeys[2] == KEY_2_ROUNDTIME)        lGame->roundTime = iVal;
        else if (dKeys[2] == KEY_2_WAITINGPLAYERS)   lGame->playersQueue = iVal;
        break;
      // ---------------------------------------------------------------------------------------------
      case 4:
        // PARSE GAME MODE SPECIFIC INFORMATION
        if (dKeys[2] == KEY_2_CONQUEST) {  // OBLIT + CONQUEST + DOMINATION:
          if (dKeys[4] == KEY_4_TICKETS) {     // get tix
            if (dKeys[3] == KEY_3_1)              lGame->score.T1 = iVal;
            else                                  lGame->score.T2 = iVal;
          } else if (dKeys[4] == KEY_4_TICKETSMAX) {  // get max tickets
            if (dKeys[3] == KEY_3_1)              lGame->scoreMax.T1 = iVal;    // lazy shortcut, just set both team's max score to same value
            else                                  lGame->scoreMax.T2 = iVal;
          }
        }
        else if (dKeys[2] == KEY_2_RUSH) {              // RUSH
          if (dKeys[4] == KEY_4_TICKETS)          lGame->score.T1     = iVal;
          else if (dKeys[4] == KEY_4_TICKETSMAX)  lGame->scoreMax.T1  = iVal;
          else if (dKeys[4] == KEY_4_BASES)       lGame->score.T2     = iVal;
          else if (dKeys[4] == KEY_4_BASESMAX)    lGame->scoreMax.T2  = iVal;
        } else if (dKeys[2] == KEY_2_CAPTURETHEFLAG) {   // CTF
          if (dKeys[4] == KEY_4_FLAGS) {
            if (dKeys[3] == KEY_3_1)              lGame->score.T1 = iVal;
            else                                  lGame->score.T2 = iVal;
          } else if (dKeys[4] == KEY_4_FLAGSMAX) {
            if (dKeys[3] == KEY_3_1)              lGame->scoreMax.T1 = iVal;
            else                                  lGame->scoreMax.T2 = iVal;
          }
        }
        else if (dKeys[2] == KEY_2_CARRIERASSAULT) {   // CARRIER ASSAULT
          if (dKeys[4] == KEY_4_DESTROYEDCRATES) {
            if (dKeys[3] == KEY_3_1)              lGame->score.T1 = iVal;
            else                                  lGame->score.T2 = iVal;
          } else if (dKeys[4] == KEY_4_CARRIERHEALTH) {
            if (dKeys[3] == KEY_3_1)              lGame->scoreMax.T1 = iVal;
            else                                  lGame->scoreMax.T2 = iVal;
          }
        }
        else if (dKeys[2] == KEY_2_DEATHMATCH) {      // TEAM DEATHMATCH
          if (dKeys[4] == KEY_4_KILLS) {            // team 1
            if (dKeys[3] == KEY_3_1)                lGame->score.T1 = iVal;
            else if (dKeys[3] == KEY_3_2)           lGame->score.T2 = iVal;
          }  else if (dKeys[4] == KEY_4_KILLSMAX) { // team2
              if (dKeys[3] == KEY_3_1)              lGame->scoreMax.T1 = iVal;
              else if (dKeys[3] == KEY_3_2)         lGame->scoreMax.T2 = iVal;
          }
        }
        // TEAMINFO -> FACTION
        else if (dKeys[2] == KEY_2_TEAMINFO && dKeys[4] == KEY_4_FACTION) {   // prob. don't need to check for TEAMINFO...
          if (dKeys[3] != KEY_3_0)
            setTeamFaction(dKeys[3], iVal);
        }
        break;
      // ---------------------------------------------------------------------------------------------
      case 6:         // TEAM STATS (KILLS & RANK COMPARISON)
        // TEAM 1
        if (dKeys[3] == KEY_3_1) {
          if (dKeys[6] == KEY_6_RANK) {
              (lGame->stats.playerCount.T1)++;
              lGame->stats.rankSum.T1 += iVal;
          } else if (dKeys[6] == KEY_6_KILLS) {
              lGame->stats.killSum.T1 += iVal;
          } else if (dKeys[6] == KEY_6_DEATHS) {
              lGame->stats.deathSum.T1 += iVal;
          }
          // TEAM 2
        } else if (dKeys[3] == KEY_3_2) {
          if (dKeys[6] == KEY_6_RANK)  {
              (lGame->stats.playerCount.T2)++;
              lGame->stats.rankSum.T2 += iVal;
          } else if (dKeys[6] == KEY_6_KILLS) {
              lGame->stats.killSum.T2 += iVal;
          } else if (dKeys[6] == KEY_6_DEATHS) {
              lGame->stats.deathSum.T2 += iVal;
          }
        }
        break;
      // ---------------------------------------------------------------------------------------------

    }
  }
  nextValGrab = false;
}


void ExampleListener::startObject() {
  OBJECT_DEPTH_INDEX++;
}
void ExampleListener::endObject() {
  OBJECT_DEPTH_INDEX--;
}
void ExampleListener::startDocument() {
  OBJECT_DEPTH_INDEX = 0;
  //resetStats();  // ticketsFirstTeam = true;
}
void ExampleListener::endDocument() {}
void ExampleListener::whitespace(char c) {}
void ExampleListener::startArray() {}
void ExampleListener::endArray() {}

// consider utilizing 2nd argument for team #
// TODO: IS THIS EVEN NEEDED ANYMORE???
int ExampleListener::getTeamStat(uint8_t stat)
{
  switch (stat)
  {
    case STAT_KILLS_T1:
      return lGame->stats.killSum.T1;
      break;
    case STAT_KILLS_T2:
      return lGame->stats.killSum.T2;
      break;
    case STAT_RANK_T1:
      return lGame->stats.rankSum.T1;
      break;
    case STAT_RANK_T2:
      return lGame->stats.rankSum.T2;
      break;
    case STAT_PLAYERCOUNT_T1:
      return lGame->stats.playerCount.T1;
      break;
    case STAT_PLAYERCOUNT_T2:
      return lGame->stats.playerCount.T2;
      break;
    case STAT_DEATHSUM_T1:
      return lGame->stats.deathSum.T1;
      break;
    case STAT_DEATHSUM_T2:
      return lGame->stats.deathSum.T2;
      break;
    default:
      return 0;
      break;
  }
}

void ExampleListener::resetStats()
{
  lGame->players = 0;
  lGame->playersQueue = 0;
  lGame->stats.playerCount.T1 = 0;
  lGame->stats.playerCount.T2 = 0;
  lGame->stats.killSum.T1 = 0;
  lGame->stats.killSum.T2 = 0;
  lGame->stats.rankSum.T1 = 0;
  lGame->stats.rankSum.T2 = 0;
  lGame->stats.deathSum.T1 = 0;
  lGame->stats.deathSum.T2 = 0;
}
