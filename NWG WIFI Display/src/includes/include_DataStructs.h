#ifndef INCLUDE_DATASTRUCTS_H
#define INCLUDE_DATASTRUCTS_H

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

} GameData;

#endif
