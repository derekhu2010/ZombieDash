#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <list>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_level_finished(false), m_nCitizens(0), m_hero(nullptr)
{
}

int StudentWorld::init()
{
    Level lev(assetPath());
    ostringstream levelString;
    levelString.fill('0');
    levelString << "level" << setw(2) << getLevel()<< ".txt";
    Level::LoadResult result = lev.loadLevel(levelString.str());
    
    if (result == Level::load_fail_file_not_found || getLevel() == 100)
    {
        //cerr << "level did not load" << endl;
        return GWSTATUS_PLAYER_WON;
    }
    else if (result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if (result == Level::load_success)
    {
        //cerr << "Level loaded" << endl;
        Level::MazeEntry temp;
        for(int row = 0; row < LEVEL_HEIGHT; row++)
        {
            for(int col = 0; col < LEVEL_WIDTH; col++)
            {
                temp = lev.getContentsOf(row, col);
                switch (temp)
                {
                    case Level::wall:
                        m_actors.push_back(new Wall(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        break;
                    case Level::exit:
                        m_actors.push_back(new Exit(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        break;
                    case Level::pit:
                        m_actors.push_back(new Pit(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        break;
                    case Level::vaccine_goodie:
                        m_actors.push_back(new VaccineGoodie(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        break;
                    case Level::gas_can_goodie:
                        m_actors.push_back(new GasCanGoodie(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        break;
                    case Level::landmine_goodie:
                        m_actors.push_back(new LandMineGoodie(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        break;
                    case Level::dumb_zombie:
                        m_actors.push_back(new DumbZombie(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        break;
                    case Level::smart_zombie:
                        m_actors.push_back(new SmartZombie(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        break;
                    case Level::citizen:
                        m_actors.push_back(new Citizen(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this));
                        m_nCitizens++;
                        break;
                    case Level::player:
                        m_hero = new Penelope(row*SPRITE_WIDTH, col*SPRITE_HEIGHT, this);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_hero->doSomething();
    
    list<Actor*>::iterator it;
    for(it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if((*it)->alive())
            (*it)->doSomething();
        if(!m_hero->alive())
            return GWSTATUS_PLAYER_DIED;
    }
    
    it = m_actors.begin();      //get rid of actors that died in this tick
    while(it != m_actors.end())
    {
        if( !((*it)->alive()) )
        {
            delete *it;
            it = m_actors.erase(it);
        }
        else
            it++;
    }
    
    ostringstream stats;
    stats << "Score: " << getScore() << "  Level: " << getLevel() << "  Lives: " << getLives() <<
    "  Vaccines: " << m_hero->getVaccines() << "  Flames: " << m_hero->getFlames() << "  Mines: " <<
    m_hero->getMines() << "  Infected: " << m_hero->getInfLevel();
    
    setGameStatText(stats.str());
    
    if(levelFinished())
        return GWSTATUS_FINISHED_LEVEL;
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_hero;
    m_hero = nullptr;
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        delete (*it);
        it = m_actors.erase(it);
    }
    m_level_finished = false;
    m_nCitizens = 0;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

void StudentWorld::addActor(Actor *a)
{
    m_actors.push_back(a);
}

bool StudentWorld::anythingBlockingMovement(Actor* mover, double finalX, double finalY)      //finalX and finalY is where the mover is intending to move
{
    if(mover != m_hero && m_hero->alive())      //check if Penelope is blocking movement
    {
        if(blocking(finalX, finalY, m_hero->getX(), m_hero->getY()))
            return true;      //Penelope is blocking movement
    }
    
    list<Actor*>::iterator it;
    for(it = m_actors.begin(); it != m_actors.end(); it++)       //check if other actors that can block movement (zombies, walls, etc) are blocking movement
    {
        if( (*it)->canBlockMovement() && mover != (*it) && (*it)->alive())
        {
            if(blocking(finalX, finalY, (*it)->getX(), (*it)->getY()))
                return true;      //Another actor is blocking movement
        }
    }
    return false;
}

bool StudentWorld::isFlameBlocked(double x, double y)
{
    list<Actor*>::iterator it = m_actors.begin();
    while(it != m_actors.end())
    {
        if( (*it)->alive() && (*it)->canBlockFlame() && blocking(x, y, (*it)->getX(), (*it)->getY() ))
            return true;
        it++;
    }
    return false;
}

bool StudentWorld::blocking(double x1, double y1, double x2, double y2)
{
    double distanceX = x1- x2;
    double distanceY = y1 - y2;
    return (distanceX < SPRITE_WIDTH && distanceX > -SPRITE_WIDTH) && (distanceY < SPRITE_HEIGHT && distanceY > -SPRITE_HEIGHT);
}
bool StudentWorld::heroOverlapping(Actor* a1) const
{
    return actorsOverlapping(a1, m_hero);
}

bool StudentWorld::actorsOverlapping(Actor* a1, Actor* a2) const
{
    if (a1 == a2)     //an object cannot overlap itself
        return false;
    int distanceX = a1->getX() - a2->getX();
    int distanceY = a1->getY() - a2->getY();
    if (distanceX * distanceX + distanceY * distanceY <= 100)
        return true;
    return false;
}

bool StudentWorld::anythingOverlapping(double x, double y)
{
    double distanceX, distanceY;
    list<Actor*>::iterator it = m_actors.begin();
    while(it != m_actors.end())
    {
        distanceX = (*it)->getX() - x;
        distanceY = (*it)->getY() - y;
        if (distanceX * distanceX + distanceY * distanceY <= 100)
            return true;
        it++;
    }
    return false;
}

bool StudentWorld::locateNearestHuman(double x, double y, double& otherX, double& otherY, double &distance)
{
    list<Actor*>::iterator it = m_actors.begin();
    bool anyHumans = false;
    double current;
    while(it != m_actors.end())
    {
        if((*it)->canBeInfected() && (*it)->alive())      //only humans can be infected
        {
            anyHumans = true;
            current = sqrt( ((x-(*it)->getX()) * (x-(*it)->getX())) + ((y-(*it)->getY()) * (y-(*it)->getY())) );
            if(current < distance)
            {
                otherX = (*it)->getX();
                otherY = (*it)->getY();
                distance = current;
            }
        }
        it++;
    }
    
    if(m_hero->alive())
    {
        anyHumans = true;
        current = sqrt( ((x-m_hero->getX()) * (x-m_hero->getX())) + ((y-m_hero->getY()) * (y-m_hero->getY())) );
        if (current < distance)
        {
            otherX = m_hero->getX();
            otherY = m_hero->getY();
            distance = current;
        }
    }
    return anyHumans;
}


void StudentWorld::finishLevel()
{
    m_level_finished = true;
}

bool StudentWorld::levelFinished() const
{
    return m_level_finished;
}

void StudentWorld::activateOnActorsOverlapping(Actor *a)
{
    if(heroOverlapping(a))
        a->activateOn(m_hero);
    
    list<Actor*>::iterator it = m_actors.begin();
    while(it != m_actors.end())
    {
        if(actorsOverlapping(*it, a))      //if an actor is overlapping a, let a affect it accordingly
            a->activateOn(*it);
        it++;
    }
}

int StudentWorld::nCitizens() const
{
    return m_nCitizens;
}

void StudentWorld::recordCitizenGone()
{
    m_nCitizens--;
}

void StudentWorld::distanceToPenelope(double x, double y, double &otherX, double &otherY, double &distance)
{
    distance = sqrt( ((x-m_hero->getX()) * (x-m_hero->getX())) + ((y-m_hero->getY()) * (y-m_hero->getY())) );
    otherX = m_hero->getX();
    otherY = m_hero->getY();
}

bool StudentWorld::distanceToClosestZombie(double x, double y, double &otherX, double &otherY, double &distance)
{
    list<Actor*>::iterator it = m_actors.begin();
    bool anyZombies = false;
    double current;
    while(it != m_actors.end())
    {
        if((*it)->canThreatenCitizen())      //only zombies can be threaten citizens
        {
            anyZombies = true;
            current = sqrt( ((x-(*it)->getX()) * (x-(*it)->getX())) + ((y-(*it)->getY()) * (y-(*it)->getY())) );
            if(current < distance)
            {
                otherX = (*it)->getX();
                otherY = (*it)->getY();
                distance = current;
            }
        }
        it++;
    }
    return anyZombies;
}
