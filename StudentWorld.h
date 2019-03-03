#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Penelope;
class Actor;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    virtual ~StudentWorld();

    void addActor(Actor* a);
    bool anythingBlockingMovement(Actor* mover, double finalX, double finalY);
    bool isFlameBlocked(double x, double y);
    
    
    bool heroOverlapping(Actor* a1) const;      //checks is hero is overlapping a1
    bool actorsOverlapping(Actor* a1, Actor* a2) const;     //checks if 2 different actors are overlapping
    bool anythingOverlapping(double x, double y);     //checks if any actors are overlapping an actor at (x,y) (used for vaccine drop)
    
    bool locateNearestHuman(double x, double y, double& otherX, double& otherY, double& distance);
    void finishLevel();
    bool levelFinished() const;
    
    void activateOnActorsOverlapping(Actor* a);      //if other actors are overalapping actor a, affect them somehow
    
    int nCitizens() const;
    void recordCitizenGone();
    void distanceToPenelope(double x, double y, double& otherX, double& otherY, double& distance);
    bool distanceToClosestZombie(double x, double y, double& otherX, double& otherY, double& distance);
private:
    bool blocking(double x1, double y1, double x2, double y2);       //determines if objects at (x1, y1) and (x2, y2) block each other
    int m_nCitizens;
    bool m_level_finished;
    Penelope* m_hero;
    list<Actor*> m_actors;
};
#endif // STUDENTWORLD_H_
