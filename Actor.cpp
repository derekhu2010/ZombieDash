#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

////////////////////////
//ACTOR IMPLEMENTATION//
////////////////////////
Actor::Actor(int imageID, double startX, double startY, int depth, StudentWorld* myWorld, int dir)
: GraphObject(imageID, startX, startY, dir, depth, 1.0)
{
    m_world = myWorld;
    m_alive = true;
}

bool Actor::alive() const
{
    return m_alive;
}
void Actor::die()
{
    aliveToDead();
}

void Actor::aliveToDead()
{
    m_alive = false;
}

StudentWorld* Actor::myWorld() const
{
    return m_world;
}

bool Actor::canBlockMovement() const
{
    return false;      //most actors cannot block movement
}

bool Actor::canBeDamagedFlame() const
{
    return true;      //most actors are damaged by flame
}

bool Actor::canBeDamagedPit() const
{
    return false;      //most actors cannot be damaged by the pit
}

bool Actor::canPickUpGoodie() const
{
    return false;
}

bool Actor::canTriggerLandmine() const
{
    return false;
}

bool Actor::canUseExit() const
{
    return false;
}

bool Actor::canBeInfected() const
{
    return false;
}

bool Actor::canThreatenCitizen() const
{
    return false;
}

bool Actor::canBlockFlame() const
{
    return false;
}

void Actor::pickUpGoodie(Goodie *g)
{
    return;
}
void Actor::useExit()
{
    return;
}

void Actor::getInfected()
{
    return;
}
void Actor::activateOn(Actor *target)
{
    return;
}

///////////////////////
//WALL IMPLEMENTATION//
///////////////////////
Wall::Wall(double startX, double startY, StudentWorld* myWorld)
: Actor(IID_WALL, startX, startY, 0, myWorld, right) {}

void Wall::doSomething()
{
    return;
}

bool Wall::canBlockMovement() const
{
    return true;
}

bool Wall::canBeDamagedFlame() const
{
    return false;
}

bool Wall::canBlockFlame() const
{
    return true;
}
////////////////////////////
//ACTIVATOR IMPLEMENTATION//
////////////////////////////
Activator::Activator(int imageID, double startX, double startY, int depth, StudentWorld* myWorld, int dir)
: Actor(imageID, startX, startY, depth, myWorld, dir) {}

void Activator::doSomething()
{
    if(!alive())
        return;
    myWorld()->activateOnActorsOverlapping(this);
}

///////////////////////
//EXIT IMPLEMENTATION//
///////////////////////
Exit::Exit(double startX, double startY, StudentWorld* myWorld)
: Activator(IID_EXIT, startX, startY, 1, myWorld, right) {}

void Exit::activateOn(Actor *target)
{
    if(target->canUseExit())
        target->useExit();
}

bool Exit::canBeDamagedFlame() const
{
    return false;
}

bool Exit::canBlockFlame() const
{
    return true;
}
//////////////////////
//PIT IMPLEMENTATION//
//////////////////////
Pit::Pit(double startX, double startY, StudentWorld* myWorld)
: Activator(IID_PIT, startX, startY, 0, myWorld, right) {}

void Pit::activateOn(Actor *target)
{
    if(target->alive() && target->canBeDamagedPit())
        target->die();
}

bool Pit::canBeDamagedFlame() const
{
    return false;
}

///////////////////////////
//LANDMINE IMPLEMENTATION//
///////////////////////////
LandMine::LandMine(double startX, double startY, StudentWorld* myWorld)
: Activator(IID_LANDMINE, startX, startY, 1, myWorld, right)
{
    m_active = false;
    m_safetyTicks = 30;
}

void LandMine::doSomething()
{
    if(!alive())
        return;
    if(!m_active)
    {
        m_safetyTicks--;
        if(m_safetyTicks == 0)
        {
            m_active = true;
            return;
        }
        return;
    }
    Activator::doSomething();
}

void LandMine::activateOn(Actor *target)
{
    if(target->canTriggerLandmine())
    {
        die();
    }
}

void LandMine::die()
{
    aliveToDead();
    myWorld()->playSound(SOUND_LANDMINE_EXPLODE);
    for(int row = -1; row <= 1; row++)
    {
        for(int col = -1; col <= 1; col++)
        {
            if(!(myWorld()->isFlameBlocked(getX() + col * SPRITE_WIDTH, getY() + row * SPRITE_HEIGHT)))
                myWorld()->addActor(new Flame(getX() + col * SPRITE_WIDTH, getY() + row * SPRITE_WIDTH, myWorld(), up) );
        }
    }
    myWorld()->addActor(new Pit(getX(), getY(), myWorld()));
    
}

//////////////////////////
//GOODIE IMPLEMENTATIONS//
//////////////////////////
Goodie::Goodie(int imageID, double startX, double startY, StudentWorld* myWorld)
: Activator(imageID, startX, startY, 1, myWorld, right) {}

void Goodie::activateOn(Actor *target)
{
    if(target->canPickUpGoodie())
    {
        target->pickUpGoodie(this);
        myWorld()->increaseScore(50);
        aliveToDead();
        myWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

VaccineGoodie::VaccineGoodie(double startX, double startY, StudentWorld* myWorld)
: Goodie(IID_VACCINE_GOODIE, startX, startY, myWorld) {};

void VaccineGoodie::restock(Penelope* hero)
{
    hero->chargeVaccines();
}

GasCanGoodie::GasCanGoodie(double startX, double startY, StudentWorld* myWorld)
: Goodie(IID_GAS_CAN_GOODIE, startX, startY, myWorld) {};

void GasCanGoodie::restock(Penelope *hero)
{
    hero->chargeFlames();
}

LandMineGoodie::LandMineGoodie(double startX, double startY, StudentWorld* myWorld)
: Goodie(IID_LANDMINE_GOODIE, startX, startY, myWorld) {};

void LandMineGoodie::restock(Penelope *hero)
{
    hero->chargeMines();
}

/////////////////////////////
//PROJECTILE IMPLEMENTATION//
/////////////////////////////
Projectile::Projectile(int imageID, double startX, double startY, StudentWorld* myWorld, int dir)
: Activator(imageID, startX, startY, 0, myWorld, dir)
{
    m_lifeSpan = 3;
}

void Projectile::doSomething()
{
    if(!alive())
        return;
    m_lifeSpan--;
    if(m_lifeSpan == 0)
    {
        aliveToDead();
        return;
    }
    Activator::doSomething();
}

bool Projectile::canBeDamagedFlame() const
{
    return false;
}

////////////////////////
//FLAME IMPLEMENTATION//
////////////////////////
Flame::Flame(double startX, double startY, StudentWorld* myWorld, int dir)
: Projectile(IID_FLAME, startX, startY, myWorld, dir) {
}

void Flame::activateOn(Actor *target)
{
    if(target->alive() && target->canBeDamagedFlame())
        target->die();
}
////////////////////////
//VOMIT IMPLEMENTATION//
////////////////////////
Vomit::Vomit(double startX, double startY, StudentWorld* myWorld, int dir)
: Projectile(IID_VOMIT, startX, startY, myWorld, dir) {};

void Vomit::activateOn(Actor *target)
{
    if(target->alive() && target->canBeInfected())
    {
        target->getInfected();
    }
}

////////////////////////
//AGENT IMPLEMENTATION//
////////////////////////
Agent::Agent(int imageID, double startX, double startY, StudentWorld* myWorld)
: Actor(imageID, startX, startY, 0, myWorld, right)
{
    m_paralyzed = false;
}

bool Agent::canBeDamagedPit() const
{
    return true;
}

bool Agent::canBlockMovement() const
{
    return true;
}

bool Agent::canTriggerLandmine() const
{
    return true;
}

bool Agent::moveIfNothingBlocking(int distance, Direction dir)
{
    switch(dir)
    {
        case left:
            setDirection(left);
            if(! myWorld()->anythingBlockingMovement(this, getX()-distance, getY()) )
            {
                moveTo(getX()-distance, getY());
                break;
            }
            return false;
        case right:
            setDirection(right);
            if(! myWorld()->anythingBlockingMovement(this, getX()+distance, getY()) )
            {
                moveTo(getX()+distance, getY());
                break;
            }
            return false;
        case up:
            setDirection(up);
            if(! myWorld()->anythingBlockingMovement(this, getX(), getY()+distance) )
            {
                moveTo(getX(), getY()+distance);
                break;
            }
            return false;
        case down:
            setDirection(down);
            if(! myWorld()->anythingBlockingMovement(this, getX(), getY()-distance) )
            {
                moveTo(getX(), getY()-distance);
                break;
            }
            return false;
    }
    return true;
}

void Agent::setDirectionTowards(double x, double y, Direction& dir1, Direction& dir2)
{
    Direction hor, ver;     //the two directions a smart zombie can move in if it is close to a human
        if(x > getX())
            hor = right;
        else if(x < getX())
            hor = left;
        if(y > getY())
            ver = up;
        else if(y < getY())
            ver = down;
        
        if (x == getX())     //same collum
            hor = ver;         //no possible horizontal movement
        if (y == getY())     //same row
            ver = hor;        //no possible vertical movement
        
        if(randInt(1,2) == 1)
        {
            dir1 = hor;
            dir2 = ver;
        }
        else
        {
            dir1 = ver;
            dir2 = hor;
        }
    setDirection(dir1);
}

bool Agent::paralyzed() const
{
    return m_paralyzed;
}

void Agent::changeParalysis()
{
    m_paralyzed = !m_paralyzed;
}


/////////////////////////
//ZOMBIE IMPLEMENTATION//
/////////////////////////
Zombie::Zombie(double startX, double startY, StudentWorld* myWorld)
: Agent(IID_ZOMBIE, startX, startY, myWorld)
{
    m_plan = 0;
}

void Zombie::doSomething()
{
    if(!alive())
        return;
    if(paralyzed())
    {
        changeParalysis();
        return;
    }
    
    //vomit
    double vomitX = getX();
    double vomitY = getY();
    double humanX, humanY;
    double closestHuman = sqrt(VIEW_WIDTH*VIEW_WIDTH + VIEW_HEIGHT * VIEW_HEIGHT) ;
    calculateVomitCoord(vomitX, vomitY, getDirection());
    if(myWorld()->locateNearestHuman(vomitX, vomitY, humanX, humanY, closestHuman) && closestHuman <= 10 && randInt(1, 3) == 1)
    {
        myWorld()->addActor(new Vomit(vomitX, vomitY, myWorld(), getDirection()));
        myWorld()->playSound(SOUND_ZOMBIE_VOMIT);
        changeParalysis();
        return;
    }
    
    //or move
    if(m_plan ==0)
        newMovementPlan();
    if(moveIfNothingBlocking(1, getDirection()))
        m_plan--;
    else      //something blocked the zombie so we need the movement plan is terminated
        m_plan = 0;
    changeParalysis();
}

void Zombie::die()
{
    aliveToDead();
    myWorld()->playSound(SOUND_ZOMBIE_DIE);
}

bool Zombie::canThreatenCitizen() const
{
    return true;
}

void Zombie::newMovementPlan()
{
    m_plan = randInt(3, 10);
    setDirection(randInt(0, 3) * 90);
}


void Zombie::calculateVomitCoord(double& x, double& y, Direction dir)
{
    switch (dir)
    {
        case right:
            x = getX() + SPRITE_WIDTH;
            break;
        case left:
            x = getX() - SPRITE_WIDTH;
            break;
        case up:
            y  = getY() + SPRITE_HEIGHT;
            break;
        case down:
            y = getY() - SPRITE_HEIGHT;
            break;
        default:
            break;
    }
}


//////////////////////////////
//DUMB ZOMBIE IMPLEMENTATION//
//////////////////////////////
DumbZombie::DumbZombie(double startX, double startY, StudentWorld* myWorld)
: Zombie(startX, startY, myWorld) {}

void DumbZombie::die()
{
    Zombie::die();
    myWorld()->increaseScore(1000);
    
    if(randInt(1, 10) == 1)
    {
        Direction dir = randInt(0, 3) * 90;
        double x = getX();
        double y = getY();
        switch (dir)
        {
            case right:
                x += SPRITE_WIDTH;
                break;
            case left:
                x -= SPRITE_WIDTH;
                break;
            case up:
                y += SPRITE_HEIGHT;
                break;
            case down:
                y -= SPRITE_HEIGHT;
                break;
            default:
                break;
        }
        if(! myWorld()->anythingOverlapping(x, y) )
            myWorld()->addActor(new VaccineGoodie(x,y,myWorld()));
    }
}
///////////////////////////////
//SMART ZOMBIE IMPLEMENTATION//
///////////////////////////////
SmartZombie::SmartZombie(double startX, double startY, StudentWorld* myWorld)
: Zombie(startX, startY, myWorld) {}

void SmartZombie::die()
{
    Zombie::die();
    myWorld()->increaseScore(2000);
}

void SmartZombie::newMovementPlan()
{
    Zombie::newMovementPlan();      //sets a distance and a random direction
    double humanX, humanY;
    double distance = sqrt(VIEW_WIDTH * VIEW_WIDTH + VIEW_HEIGHT * VIEW_HEIGHT);
    myWorld()->locateNearestHuman(getX(), getY(), humanX, humanY, distance);
    Direction dir1, dir2;
    if(distance <= 80)
        setDirectionTowards(humanX, humanY, dir1, dir2);
}
///////////////////////////
//HUMAN IMPLEMENTATION//
///////////////////////////
Human::Human(int imageID, double startX, double startY, StudentWorld* myWorld)
: Agent(imageID, startX, startY, myWorld)
{
    m_infLevel = 0;
    m_infected = false;
}

bool Human::canUseExit() const
{
    return true;
}

bool Human::canBeInfected() const
{
    return true;
}

void Human::getInfected()
{
    
    m_infected = true;
}

void Human::increaseInf()
{
    m_infLevel++;
}

void Human::vaccinate()
{
    m_infLevel = 0;
    m_infected = false;
}

int Human::getInfLevel()
{
    return m_infLevel;
}

bool Human::infected()
{
    return m_infected;
}

//////////////////////////
//CITIZEN IMPLEMENTATION//
//////////////////////////
Citizen::Citizen(double startX, double startY, StudentWorld* myWorld)
: Human(IID_CITIZEN, startX, startY, myWorld) {}

void Citizen::doSomething()
{
    if(!alive())
        return;
    if(infected())
        increaseInf();
    if(getInfLevel() == 500)
    {
        aliveToDead();
        myWorld()->playSound(SOUND_ZOMBIE_BORN);
        myWorld()->increaseScore(-1000);
        myWorld()->recordCitizenGone();
        if(randInt(1, 10) > 7)
            myWorld()->addActor(new SmartZombie(getX(), getY(), myWorld()));
        else
            myWorld()->addActor(new DumbZombie(getX(), getY(), myWorld()));
        return;
    }
    if(paralyzed())
    {
        changeParalysis();
        return;
    }
    
    double dist_p, heroX, heroY, zombieX, zombieY;
    double dist_z = 256;
    
    myWorld()->distanceToPenelope(getX(), getY(), heroX, heroY, dist_p);
    myWorld()->distanceToClosestZombie(getX(), getY(), zombieX, zombieY, dist_z);
    Direction dir1, dir2;
    
    if(dist_p < dist_z && dist_p <=80)        //this brings the citizen towards penelope
    {
        setDirectionTowards(heroX, heroY, dir1, dir2);
        if(moveIfNothingBlocking(2, dir1))      //move in direction dir1 if possible and then return
        {
            changeParalysis();
            return;
        }
        else if(moveIfNothingBlocking(2, dir2))      //otherwise move in dir2, still towards the hero and return
        {
            changeParalysis();
            return;
        }
    }
    
    
    if(dist_z <= 80)       //if dir1 and fir 2 are both blocked, runaway from zombies
    {
        double dist_z_postMove;     //stores the distance after a potential move
        double dist_largest = dist_z;      //stores the greatest possible distance from the nearest zombie in one move
        Direction moveThisWay;
        for(int i = 0; i < 4; i++)
        {
            dist_z_postMove = VIEW_WIDTH;        //reset dist_z_postMove because it is passed by reference
            Direction dir = i*90;
            switch(dir)
            {
                case right:
                    if(myWorld()->anythingBlockingMovement(this, getX() + 2, getY()))
                        break;
                    myWorld()->distanceToClosestZombie(getX() + 2, getY(), zombieX, zombieY, dist_z_postMove);
                    break;
                    
                case up:
                    if(myWorld()->anythingBlockingMovement(this, getX(), getY() + 2))
                        break;
                    else
                        myWorld()->distanceToClosestZombie(getX(), getY() + 2, zombieX, zombieY, dist_z_postMove);
                    break;
                    
                case left:
                    if(myWorld()->anythingBlockingMovement(this, getX() - 2, getY()))
                        break;
                    else
                        myWorld()->distanceToClosestZombie(getX() - 2, getY(), zombieX, zombieY, dist_z_postMove);
                    break;
                    
                case down:
                    if(myWorld()->anythingBlockingMovement(this, getX(), getY() - 2))
                        break;
                    else
                        myWorld()->distanceToClosestZombie(getX(), getY() - 2, zombieX, zombieY, dist_z_postMove);
                    break;
                    
            }
            if(dist_z_postMove != VIEW_WIDTH && dist_z_postMove > dist_largest)      //first boolean expression checks fo the case that dist_z_postMove is unchanged
            {
                dist_largest = dist_z_postMove;
                moveThisWay = dir;
            }
        }
        if (dist_largest == dist_z)       //i.e. none of the 4 directions of movement moved our citizen further, so just do nothing
        {
            changeParalysis();
            return;
        }
        else
        {
            moveIfNothingBlocking(2, moveThisWay);         //otherwise move in the direction that takes the citizen the fursthest
            changeParalysis();
        }
    }
}

void Citizen::die()     //by flame or pit
{
    aliveToDead();
    myWorld()->playSound(SOUND_CITIZEN_DIE);
    myWorld()->recordCitizenGone();
    myWorld()->increaseScore(-1000);
    
}

void Citizen::useExit()
{
    myWorld()->playSound(SOUND_CITIZEN_SAVED);
    myWorld()->increaseScore(500);
    myWorld()->recordCitizenGone();
    aliveToDead();
}

void Citizen::getInfected()
{
    if(infected())
        return;
    Human::getInfected();
    myWorld()->playSound(SOUND_CITIZEN_INFECTED);
}

///////////////////////////
//PENELOPE IMPLEMENTATION//
///////////////////////////
Penelope::Penelope(double startX, double startY, StudentWorld* myWorld)
: Human (IID_PLAYER, startX, startY, myWorld)
{
    m_nMines = 0;
    m_nFlames = 0;
    m_nVaccines = 0;
}

void Penelope::doSomething()
{
    if(!alive())
        return;
    if(infected())
    {
        increaseInf();
    }
    if(getInfLevel() == 500)
    {
        die();
        return;
    }
    
    int ch;
    if(myWorld()->getKey(ch))
    {
        switch(ch)
        {
            case KEY_PRESS_LEFT:
                moveIfNothingBlocking(4, left);
                break;
            case KEY_PRESS_RIGHT:
                moveIfNothingBlocking(4, right);
                break;
            case KEY_PRESS_UP:
                moveIfNothingBlocking(4, up);
                break;
            case KEY_PRESS_DOWN:
                moveIfNothingBlocking(4, down);
                break;
             
            case KEY_PRESS_SPACE:
                if(m_nFlames > 0)
                {
                    m_nFlames--;
                    myWorld()->playSound(SOUND_PLAYER_FIRE);
                    Direction dir = getDirection();
                    int x, y;
                    for(int i = 1; i<=3; i++)
                    {
                        switch(dir)
                        {
                            case up:
                                x = getX();
                                y = getY() + i * SPRITE_HEIGHT;
                                break;
                            case down:
                                x = getX();
                                y = getY() - i * SPRITE_HEIGHT;
                                break;
                            case right:
                                x = getX() + i * SPRITE_WIDTH;
                                y = getY();
                                break;
                            case left:
                                x = getX() - i * SPRITE_WIDTH;
                                y = getY();
                                break;
                        }
                        
                        if(myWorld()->isFlameBlocked(x, y))
                            break;
                        myWorld()->addActor(new Flame(x, y, myWorld(), getDirection() ));
                    }
                }
                break;
                
            case KEY_PRESS_TAB:
                if(m_nMines > 0)
                {
                    m_nMines--;
                    myWorld()->addActor(new LandMine(getX(), getY(), myWorld() ));
                }
                break;
                
            case KEY_PRESS_ENTER:
                if(m_nVaccines > 0)
                {
                    m_nVaccines--;
                    vaccinate();
                }
        }
    }
    
}

void Penelope::die()
{
    myWorld()->decLives();
    aliveToDead();
    myWorld()->playSound(SOUND_PLAYER_DIE);
}


bool Penelope::canPickUpGoodie() const
{
    return true;
}
void Penelope::pickUpGoodie(Goodie *g)
{
    g->restock(this);
}

void Penelope::useExit()
{
    if(myWorld()->nCitizens() == 0)
    {
        myWorld()->finishLevel();
        myWorld()->playSound(SOUND_LEVEL_FINISHED);
    }
}

int Penelope::getFlames() const
{
    return m_nFlames;
}

int Penelope::getMines() const
{
    return m_nMines;
}

int Penelope::getVaccines() const
{
    return m_nVaccines;
}

void Penelope::chargeVaccines()
{
    m_nVaccines+=1;
}

void Penelope::chargeFlames()
{
    m_nFlames+=5;
}

void Penelope::chargeMines()
{
    m_nMines+=2;
}
