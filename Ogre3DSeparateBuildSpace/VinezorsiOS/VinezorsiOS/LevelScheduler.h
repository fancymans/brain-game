//
//  LevelScheduler.h
//  Vinezors2.0
//
//  Created by Darrin Lin on 8/4/14.
//
//
//________________________________________________________________________________________

#ifndef __Vinezors2_0__LevelScheduler__
#define __Vinezors2_0__LevelScheduler__
//________________________________________________________________________________________

#include <iostream>
#include <fstream>
#include <list>
#include <utility>
#include <vector>
#include <time.h>
#include "LevelSet.h"
#include "PlayerProgress.h"
#include "Util.h"
//________________________________________________________________________________________

struct Bin
{
    Bin(LevelPhase phaseX = PHASE_COLLECT, StageDifficulty difficultyX = DIFFICULTY_EASY, StageDuration durationX = DURATION_NORMAL, bool holdout = false, double nbackShift = 0.0)
    : phaseX(phaseX), difficultyX(difficultyX), durationX(durationX), holdout(holdout), nbackShift(nbackShift)
    {}
    
    LevelPhase phaseX;
    StageDifficulty difficultyX;
    StageDuration durationX;
    bool holdout;
    
    // Not checked in comparison
    double nbackShift;
    
    bool operator==(const Bin rhs) const
    {
        if( (phaseX == rhs.phaseX) && (difficultyX == rhs.difficultyX) && (holdout == rhs.holdout) && (durationX == rhs.durationX))
            return true;
        return false;
    }
};

struct LevelScheduler
{
    // Constructor
    LevelScheduler( double nBackLevelA = 1.0, double nBackLevelB = 1.0, double bBackLevelC = 1.0, double nBackLevelD = 1.0, double nBackLevelE = 1.0, double currentHoldout = 0.0 );
    
    // std::vector< std::vector< std::pair<StageRequest, PlayerProgress> > > schedule;
    // std::vector< std::vector< std::pair<StageRequest, PlayerProgress> > >::iterator scheduleIt;
    
    std::vector< std::pair<StageRequest, PlayerProgress> > tutorialLevels;
    std::vector< std::pair<StageRequest, PlayerProgress> > scheduleHistoryA;
    std::vector< std::pair<StageRequest, PlayerProgress> > scheduleHistoryB;
    std::vector< std::pair<StageRequest, PlayerProgress> > scheduleHistoryC;
    std::vector< std::pair<StageRequest, PlayerProgress> > scheduleHistoryD;
    std::vector< std::pair<StageRequest, PlayerProgress> > scheduleHistoryE;
    std::list<Bin>* binA;
    std::list<Bin>* binB;
    std::list<Bin>* binC;
    std::list<Bin>* binD;
    std::list<Bin>* binE;
    int totalMarbles;
    int timePlayed;
    bool sessionFinished;
    bool sessionFinishedAcknowledged;
    double nBackLevelA;
    double nBackLevelB;
    double nBackLevelC;
    double nBackLevelD;
    double nBackLevelE;
    double scoreCurr;
    double currentHoldout;

    double holdoutOffsetA;
    double holdoutOffsetB;
    double holdoutOffsetD;

    double speedA;
    double speedB;
    double speedC;
    double speedD;
    double speedE;
    bool firstTimeA;
    bool firstTimeB;
    bool firstTimeC;
    bool firstTimeD;
    bool firstTimeE;
    
    int playCount;
    
    // Member functions
    void initTutorialLevels();
    
    StageRequest getRandomLevel( LevelSet* levels );
    void initializeSchedule( LevelSet* levels );
    void populateBins();
    void removeBin(LevelPhase phaseX, StageDifficulty difficultyX, StageDuration durationX, bool hasHoldout);
    std::list<Bin>* pickRandomBin();
    void pickRandomMarble( std::vector<Bin>& choices );
    void setHoldout( std::list<Bin>* b );
    std::vector< std::pair<StageRequest, PlayerProgress> > generateChoices(bool holdoutEnabled, bool newNavEnabled, bool indRecessEnabled,double indRecessNBackLevel, bool holdoutDelayEnabled, float holdoutDelayNumber);
    int rand_num( int lower, int upper );
    int predictAverageStartingSpeed(int initVel);
    
    friend std::ostream& operator<<(std::ostream& out, const LevelScheduler& sch);
    friend std::istream& operator>>(std::istream& in, LevelScheduler& sch);
};

std::ostream& operator<<(std::ostream& out, const LevelScheduler& sch);
std::istream& operator>>(std::istream& in, LevelScheduler& sch);
//________________________________________________________________________________________

#endif /* defined(__Vinezors2_0__LevelScheduler__) */
