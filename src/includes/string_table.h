#ifndef _STRING_TABLE_H
#define _STRING_TABLE_H
#define INIT_CONST_STR_C(varName, varVal)  const char varName [] = varVal

#define STRING_TABLE_BUFFER_SIZE  21

// GLOBAL:
#define INVALID                 55
#define KEY_INVALID             66
#define KEY_DUMMYPAD            77      // PAD TO OFFSET ARRAY INDEX TO MATCH JSON DATA NEST DEPTH

// JSON ERROR CODES:
#define STATUS_JSON_SUCCESS                          0
#define STATUS_WIFI_CONNECT_FAIL                     1
#define STATUS_JSON_ERROR_HTTP_CONNECT_FAIL          2
#define STATUS_JSON_ERROR_INVALID_SERVER_URL         3
#define STATUS_JSON_ERROR_HTTP_REQUEST_TIMEOUT       4
#define STATUS_JSON_ERROR_INVALID_RESPONSE           5
#define STATUS_JSON_ERROR_HTTP_REQUEST_LONG_RESPONSE  6
#define STATUS_JSON_ERROR_WIFI_DISCONNECTED           7
#define STATUS_JSON_ERROR_WIFI_ERROR                  8

// ** TODO: OPTIMIZE VARIOUS KEY DEPTH VARIABLES INTO BITS
// EXAMPLEPARSER.H:
#define KEY_2_CURRENTMAP        1
#define KEY_2_WAITINGPLAYERS    2
#define KEY_2_ROUNDTIME         3
#define KEY_2_GAMEMMODE         4
#define KEY_2_CONQUEST          5
#define KEY_2_RUSH              6
#define KEY_2_CAPTURETHEFLAG    7
#define KEY_2_CARRIERASSAULT    8
#define KEY_2_DEATHMATCH        9
#define KEY_2_TEAMINFO          10

// ... teams ...
#define KEY_3_0                 0
#define KEY_3_1                 1
#define KEY_3_2                 2

//  ... mode-specific data ...
#define KEY_4_TICKETS           1
#define KEY_4_TICKETSMAX        2
#define KEY_4_BASES             3
#define KEY_4_BASESMAX          4
#define KEY_4_FLAGS             5
#define KEY_4_FLAGSMAX          6
#define KEY_4_DESTROYEDCRATES   7
#define KEY_4_CARRIERHEALTH     8
#define KEY_4_KILLS             9
#define KEY_4_KILLSMAX          10
#define KEY_4_FACTION           11

// ... player stats ...
#define KEY_6_RANK              1
#define KEY_6_KILLS             2
#define KEY_6_DEATHS            3

// ... team stats ...
#define STAT_KILLS_T1         1
#define STAT_KILLS_T2         2
#define STAT_RANK_T1          3
#define STAT_RANK_T2          4
#define STAT_PLAYERCOUNT_T1   5
#define STAT_PLAYERCOUNT_T2   6
#define STAT_DEATHSUM_T1      7
#define STAT_DEATHSUM_T2      8


// MAP NAMES (33):
#define MAP_ZAVOD       0
#define MAP_LANCANG     1
#define MAP_SIEGE       2
#define MAP_HAINAN      3
#define MAP_LOCKER      4
#define MAP_FLOOD       5
#define MAP_GOLMUD      6
#define MAP_PARCEL      7
#define MAP_ROGUE       8
#define MAP_DAWN        9
#define MAP_GULF        10
#define MAP_CASPIAN     11
#define MAP_METRO       12
#define MAP_FIRESTORM   13
#define MAP_SILK        14
#define MAP_ALTAI       15
#define MAP_GULLIN      16
#define MAP_DRAGON      17
#define MAP_LOST        18
#define MAP_NANSHA      19
#define MAP_WAVE        20
#define MAP_MORTAR      21
#define MAP_PEARL       22
#define MAP_PROPAGANDA  23
#define MAP_LUMPHINI    24
#define MAP_SUNKEN      25
#define MAP_WHITEOUT    26
#define MAP_HAMMER      27
#define MAP_HANGER      28
#define MAP_GIANTS      29
#define MAP_ZAVOD_NIGHT 30
#define MAP_OUTBREAK    31
#define MAP_VALLEY      32
INIT_CONST_STR_C(_MAP_ZAVOD , "Zavod 311");
INIT_CONST_STR_C(_MAP_LANCANG, "Lancang Dam");
INIT_CONST_STR_C(_MAP_SIEGE, "Siege of Shanghai");
INIT_CONST_STR_C(_MAP_HAINAN, "Hainan Resort");
INIT_CONST_STR_C(_MAP_LOCKER, "Operation Locker");
INIT_CONST_STR_C(_MAP_FLOOD, "Flood Zone");
INIT_CONST_STR_C(_MAP_GOLMUD, "Golmud Rush");
INIT_CONST_STR_C(_MAP_PARCEL, "Parcel Storm");
INIT_CONST_STR_C(_MAP_ROGUE, "Rogue Transmission");
INIT_CONST_STR_C(_MAP_DAWN, "Dawnbreaker");
INIT_CONST_STR_C(_MAP_GULF, "Gulf of Oman");
INIT_CONST_STR_C(_MAP_CASPIAN, "Caspian Border");
INIT_CONST_STR_C(_MAP_METRO, "Operation Metro");
INIT_CONST_STR_C(_MAP_FIRESTORM, "Operation Firestorm");
INIT_CONST_STR_C(_MAP_SILK, "Silk Road");
INIT_CONST_STR_C(_MAP_ALTAI, "Altai Range");
INIT_CONST_STR_C(_MAP_GULLIN, "Gullin Peaks");
INIT_CONST_STR_C(_MAP_DRAGON, "Dragon Pass");
INIT_CONST_STR_C(_MAP_LOST, "Lost Island");
INIT_CONST_STR_C(_MAP_NANSHA, "Nansha Strike");
INIT_CONST_STR_C(_MAP_WAVE, "Wave Breaker");
INIT_CONST_STR_C(_MAP_MORTAR, "Operation Mortar");
INIT_CONST_STR_C(_MAP_PEARL, "Pearl Market");
INIT_CONST_STR_C(_MAP_PROPAGANDA, "Propaganda");
INIT_CONST_STR_C(_MAP_LUMPHINI, "Lumphini Garden");
INIT_CONST_STR_C(_MAP_SUNKEN, "Sunken Dragon");
INIT_CONST_STR_C(_MAP_WHITEOUT, "Whiteout");
INIT_CONST_STR_C(_MAP_HAMMER, "Hammerhead");
INIT_CONST_STR_C(_MAP_HANGER, "Hanger 21");
INIT_CONST_STR_C(_MAP_GIANTS, "Giants Karelia");
INIT_CONST_STR_C(_MAP_ZAVOD_NIGHT, "Zavod Night");
INIT_CONST_STR_C(_MAP_OUTBREAK, "Outbreak");
INIT_CONST_STR_C(_MAP_VALLEY, "Dragon Valley");
const char* const STR_TABLE_MAPS[] = {_MAP_ZAVOD, _MAP_LANCANG, _MAP_SIEGE, _MAP_HAINAN, _MAP_LOCKER, _MAP_FLOOD, _MAP_GOLMUD, _MAP_PARCEL, _MAP_ROGUE, _MAP_DAWN, _MAP_GULF, _MAP_CASPIAN, _MAP_METRO, _MAP_FIRESTORM, _MAP_SILK,_MAP_ALTAI, _MAP_GULLIN, _MAP_DRAGON, _MAP_LOST, _MAP_NANSHA, _MAP_WAVE, _MAP_MORTAR, _MAP_PEARL, _MAP_PROPAGANDA, _MAP_LUMPHINI, _MAP_SUNKEN, _MAP_WHITEOUT, _MAP_HAMMER, _MAP_HANGER, _MAP_GIANTS, _MAP_ZAVOD_NIGHT, _MAP_OUTBREAK, _MAP_VALLEY};
//  strcpy_P(str_buffer, (char *)pgm_read_dword(&(STR_TABLE_MAPS[i])));

// GAME MODES (8):
#define MODE_TDM      0
#define MODE_R        1
#define MODE_CTF      2
#define MODE_CA       3
#define MODE_DOM      4
#define MODE_CQ       5
#define MODE_OB       6
#define MODE_GM       7
#define MODE_CL       8
#define MODE_AS       9
#define MODE_SD       10
#define MODE_DE       11
INIT_CONST_STR_C(_MODE_TDM, "TDM");  //INIT_CONST_STR_C(_MODE_TDM, "TD");
INIT_CONST_STR_C(_MODE_R,   "R");
INIT_CONST_STR_C(_MODE_CTF, "CTF");  //INIT_CONST_STR_C(_MODE_CTF, "CF");
INIT_CONST_STR_C(_MODE_CA, "CA");
INIT_CONST_STR_C(_MODE_DOM,"DOM");//INIT_CONST_STR_C(_MODE_DOM,"DO");
INIT_CONST_STR_C(_MODE_CQ, "CQ");
INIT_CONST_STR_C(_MODE_OB, "OB");
INIT_CONST_STR_C(_MODE_GM, "GM");
INIT_CONST_STR_C(_MODE_CL, "CL");
INIT_CONST_STR_C(_MODE_AS, "AS");
INIT_CONST_STR_C(_MODE_SD, "SD");
INIT_CONST_STR_C(_MODE_DE, "DE");
const char* const STR_TABLE_MODES[] = { _MODE_TDM, _MODE_R, _MODE_CTF, _MODE_CA, _MODE_DOM, _MODE_CQ, _MODE_OB, _MODE_GM, _MODE_CL, _MODE_AS, _MODE_SD, _MODE_DE };

// TEAM FACTIONS (3):
#define FACTION_US    0
#define FACTION_RU    1
#define FACTION_CN    2
INIT_CONST_STR_C(_FACTION_US, "US");
INIT_CONST_STR_C(_FACTION_RU, "RU");
INIT_CONST_STR_C(_FACTION_CN, "CN");
const char* const STR_TABLE_FACTIONS[] = { _FACTION_US, _FACTION_RU, _FACTION_CN };

// use: https://github.com/esp8266/Arduino/issues/69
//  strcpy_P(buffer, (char)pgm_read_word(&(string_table[i]))); // Necessary casts and dereferencing, just copy.




#endif