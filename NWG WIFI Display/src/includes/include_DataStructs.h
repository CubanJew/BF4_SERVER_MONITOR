#ifndef INCLUDE_DATASTRUCTS_H
#define INCLUDE_DATASTRUCTS_H
#include <Arduino.h>

// WEATHER ...
typedef struct {
	int 			high_f[4];
	int 			low_f[4];
	String 		icon[4];  //uint8_t icon[4];
	int 			pop[4];
	double 		qpf_allday_in[4];
  int				ave_humidity[4];
  String 		day[4];
} Forecast;

typedef struct {
	String 	icon;   //uint8_t icon;
	String 	weather;		// weather description
	int 		temp_f;
	int 		relative_humidity;
	int 		feelslike_f;
	double 	precip_1hr_in;
	double 	precip_today_in;
} Conditions;

typedef struct {
	Forecast 	forecast;
	Conditions now;
  //boolean isMetric = false;
} Weather;
// ... END WEATHER




// BF4 game data ...
typedef struct {
  unsigned int T1;  // team 1
  unsigned int T2;  // team 2 score
} Score;

typedef struct {
  uint8_t T1;  // team 1 faction
  uint8_t T2;  // team 2 faction
} Score8_t;

typedef struct {
  Score playerCount;
  Score killSum;
  Score rankSum;
  Score deathSum;
} Stats;

typedef struct {
  Score score;
  Score scoreMax;
  Stats stats;
  unsigned int roundTime;  // round time
  byte players; // # of players
  byte playersQueue;  // players in server queue
  Score8_t faction;    //factions
  uint8_t mode;    // change to ID..
  uint8_t map;  // change to ID
  bool validData; // whether or not data is valid
  bool rushStarted; // whether or not rush round has started. lately Battlelog doesn't always immediately return 'rush' JSON object on new rush map (only have 'teamInfo' object)
	int return_status=5;
} GameData;

// ... end BF4 game data


// BF4 STATS ...
enum Team_S {  TEAM_1, TEAM_2, TEAM_NA};

typedef struct {
  int val = 0;
  Team_S team = TEAM_NA;
} Computed_Stat_Int;

typedef struct {
  double val = 0;
  Team_S team = TEAM_NA;
} Computed_Stat_Double;

typedef struct {
  Computed_Stat_Int     kills;
  Computed_Stat_Double  kd;
  Computed_Stat_Int     rank;
} Stats_Delta;
extern Stats_Delta stats;
// ... end BF4 STATS

#endif
