#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;
class Goodie;
class Actor : public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, int depth, StudentWorld* myWorld, int dir);
    virtual void doSomething() = 0;
    bool alive() const;      //accessor for alive
    void aliveToDead();
    virtual void die();      //kills an actor and uses polymorphism to do the things that an object does before it dies
    StudentWorld* myWorld() const;        //returns a pointer to the StudentWorld object that the actor is in
    
    virtual void pickUpGoodie(Goodie* g);      //immediately returns for all actors except penelope
    virtual void useExit();                   //immediately returns for all actors except penelope and citizens
    virtual void getInfected();               //immediately returns for all non humans
    virtual void activateOn(Actor* target);        //affects the target when appropriate
    
    virtual bool canBlockMovement() const;      //false for everyone except agents and walls
    virtual bool canBeDamagedFlame() const;     //true for everyone except wall and exit and pit, and flame
    virtual bool canBlockFlame() const;         //false for everyone except wall and exit
    virtual bool canBeDamagedPit() const;      //false for non agents
    virtual bool canPickUpGoodie() const;      //false for everyone except Penelope
    virtual bool canTriggerLandmine() const;    //false for all non agents
    virtual bool canUseExit() const;            //false for all non humans
    virtual bool canBeInfected() const;         //false for all non humans
    virtual bool canThreatenCitizen() const;     //returns false for all non zombies
    
private:
    StudentWorld* m_world;
    bool m_alive;
};


class Wall : public Actor
{
public:
    Wall(double startX, double startY, StudentWorld* myWorld);
    virtual void doSomething();
    
    virtual bool canBlockMovement() const;
    virtual bool canBeDamagedFlame() const;
    virtual bool canBlockFlame() const;
};

class Activator : public Actor
{
public:
    Activator(int imageID, double startX, double startY, int depth, StudentWorld* myWorld, int dir);
    virtual void doSomething();
    
};
class Exit : public Activator
{
public:
    Exit(double startX, double startY, StudentWorld* myWorld);
    virtual void activateOn(Actor* target);        //calls useExit
    virtual bool canBeDamagedFlame() const;
    virtual bool canBlockFlame() const;
};

class Pit : public Activator
{
public:
    Pit(double startX, double startY, StudentWorld* myWorld);
    virtual void activateOn(Actor* target);
    virtual bool canBeDamagedFlame() const;
};

class LandMine : public Activator
{
public:
    LandMine(double startX, double startY, StudentWorld* myWorld);
    virtual void doSomething();
    virtual void activateOn(Actor* target);
    virtual void die();
private:
    bool m_active;
    int m_safetyTicks;
};

class Penelope;
class Goodie : public Activator      //virtual class
{
public:
    Goodie(int imageID, double startX, double startY, StudentWorld* myWorld);
    virtual void activateOn(Actor* target);        //Penelope picks up a goodie
    virtual void restock(Penelope* hero) = 0;
};

class VaccineGoodie : public Goodie
{
public:
    VaccineGoodie(double startX, double startY, StudentWorld* myWorld);
    virtual void restock(Penelope* hero);
};

class GasCanGoodie : public Goodie
{
public:
    GasCanGoodie(double startX, double startY, StudentWorld* myWorld);
    virtual void restock(Penelope* hero);
};

class LandMineGoodie : public Goodie
{
public:
    LandMineGoodie(double startX, double startY, StudentWorld* myWorld);
    virtual void restock(Penelope* hero);
};

class Projectile : public Activator
{
public:
    Projectile(int imageID, double startX, double startY, StudentWorld* myWorld, int dir);
    virtual void doSomething();
    virtual bool canBeDamagedFlame() const;
private:
    int m_lifeSpan;
};

class Flame : public Projectile
{
public:
    Flame(double startX, double startY, StudentWorld* myWorld, int dir);
    virtual void activateOn(Actor* target);
};

class Vomit : public Projectile
{
public:
    Vomit(double startX, double startY, StudentWorld* myWorld, int dir);
    virtual void activateOn(Actor* target);
};

class Agent : public Actor
{
public:
    Agent(int imageID, double startX, double startY, StudentWorld* myWorld);
    virtual bool canBlockMovement() const;
    virtual bool canBeDamagedPit() const;
    virtual bool canTriggerLandmine() const;
    
protected:
    void setDirectionTowards(double x, double y, Direction& dir1, Direction& dir2);   //sets direction towards this point. This direction is stored in dir1, dir2 is the alternate
    bool moveIfNothingBlocking(int distance, Direction dir);       //returns true if able to move in direction dir, false it the agent was blocked
    bool paralyzed() const;
    void changeParalysis();
private:
    bool m_paralyzed;
};

class Zombie : public Agent
{
public:
    Zombie(double startX, double startY, StudentWorld* myWorld);
    virtual void doSomething();
    virtual void die();
    virtual bool canThreatenCitizen() const;
protected:
    virtual void newMovementPlan();
    void calculateVomitCoord(double& x, double& y, Direction dir);
    
private:
    int m_plan;
};

class DumbZombie : public Zombie
{
public:
    DumbZombie(double startX, double startY, StudentWorld* myWorld);
    virtual void die();
};

class SmartZombie : public Zombie
{
public:
    SmartZombie(double startX, double startY, StudentWorld* myWorld);
    virtual void die();
    virtual void newMovementPlan();
};

class Human : public Agent
{
public:
    Human(int imageID, double startX, double startY, StudentWorld* myWorld);
    virtual bool canUseExit() const;
    virtual bool canBeInfected() const;
    int getInfLevel();
protected:
    virtual void getInfected();
    void increaseInf();
    void vaccinate();
    bool infected();
private:
    int m_infLevel;
    bool m_infected;
};

class Citizen : public Human
{
public:
    Citizen(double startX, double startY, StudentWorld* myWorld);
    virtual void doSomething();
    virtual void die();
    virtual void useExit();
protected:
    virtual void getInfected();
};

class Penelope : public Human
{
public:
    Penelope(double startX, double startY, StudentWorld* myWorld);
    virtual void doSomething();
    virtual void die();
    virtual bool canPickUpGoodie() const;
    virtual void pickUpGoodie(Goodie* g);
    virtual void useExit();
    
    //accessors
    int getFlames() const;
    int getMines() const;
    int getVaccines() const;
    
    void chargeVaccines();
    void chargeFlames();
    void chargeMines();
private:
    int m_nFlames;
    int m_nMines;
    int m_nVaccines;
};

#endif // ACTOR_H_
