#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

//A base class for all the games Objects
class Actor : public GraphObject{
public:
    Actor(StudentWorld* myWorld, int imageID, int startX, int startY, Direction dir = right, double size = 1.0, unsigned int depth = 0);
    
    void setLiveStatus(bool b);
    bool getLiveStatus() const;
    
    StudentWorld* getWorld() const;
    
    virtual char getGameID() const = 0;
    virtual void doSomething() = 0;
    
    //Default annoy function on an actor does nothing
    virtual void annoy(int howMuch){}
    //Default bribe function on an actor does nothing
    virtual void bribe(){}

    //Move the actor one square in it's current direction if it stays within bounds
    void move();
    
    virtual ~Actor(){}
    

private:
    StudentWorld* m_world;
    bool m_isAlive;
};

class Earth : public Actor{
public:
    Earth(StudentWorld* myWorld, int startX, int startY);
    
    //Earth doesn't do anything
    virtual void doSomething(){}
    //Earth has a gameID of 'E'
    virtual char getGameID() const;
};



class TunnelMan : public Actor{
public:
    TunnelMan(StudentWorld* myWorld);
    
    virtual void doSomething();
    virtual char getGameID() const;
    virtual void annoy(int howMuch);
    
    //Getters and Setters
    
    int getNumBarrelsFound();
    void incrementBarrelsFound();
    
    void incrementNumNuggets();
    int getNumNuggets();
    
    void incrementNumSonarCharges();
    int getNumSonarCharges();
    
    void incrementNumSquirts();
    int getNumSquirts();
    
    int getHitPoints();

    virtual ~TunnelMan(){}
    
private:
    int m_numSquirts;
    int m_numSonarCharges;
    int m_numGoldNuggets;
    int m_numBarrelsFound;
    int m_hitPoints;

    void introduceSquirt();
};


class Boulder : public Actor{
public:
    Boulder(StudentWorld* myWorld, int startX, int startY);
    virtual char getGameID() const;
    virtual void doSomething();
    virtual ~Boulder(){}
private:
    int state; //0 for stable, 1 for waiting, 2 for falling
    int ticksElapsed;
    void smushCharacters();
    bool boulderCanFall() const;
};

class Goodie : public Actor{
public:
    Goodie(StudentWorld* myWorld, int imageID, int startX, int startY, bool tunnelManCanPickUp, bool temp);
    
    virtual void doSomething();
    
    virtual void doSomethingToTunnelMan() = 0;
    virtual void doSomethingToProtestor(){}
    
    void setTickLifeTime(int tickLifeTime);
    virtual ~Goodie(){}
    
private:
    bool m_tunnelManCanPickUp;
    bool m_temporary;
    int m_tickLifeTime;
    int m_tickCount;
};

class Barrel : public Goodie{
public:
    Barrel(StudentWorld* myWorld, int startX, int startY);
    virtual char getGameID() const;
    virtual void doSomethingToTunnelMan();
};

class WaterPool : public Goodie{
public:
    WaterPool(StudentWorld* myWorld, int startX, int startY);
    virtual char getGameID() const;
    virtual void doSomethingToTunnelMan();
};

class SonarKit : public Goodie{
public:
    SonarKit(StudentWorld* myWorld);
    virtual char getGameID() const;
    virtual void doSomethingToTunnelMan();
};

class Gold: public Goodie{
public:
    Gold(StudentWorld* myWorld, int startX, int startY, bool tunnelManCanPickUp);
    virtual char getGameID() const;
    virtual void doSomethingToTunnelMan();
    virtual void doSomethingToProtestor();
};


class Squirt : public Actor{
public:
    Squirt(StudentWorld* myWorld, int startX, int startY);
    virtual char getGameID() const;
    
    virtual void doSomething();
    
private:
    int m_travelDistance;
};


class Protestor : public Actor{
public:
    Protestor(StudentWorld* myWorld, int imageID, int hitPoints);
    
    virtual void doSomething();
    
    virtual void bribe();
    virtual bool canMoveTowardTunnelMan();
    virtual void changeDirectionToMoveTowardTunnelMan() = 0;
    
    virtual void annoy(int howMuch);
    
    virtual ~Protestor(){}
    
protected:
    //Returns a number between 8 and 60 inclusive
    int generateNumSquaresToMove();
    //Returns whether the protestor is within shouting distance of the tunnelman, and facing him
    bool withinShoutingDistanceAndFacingTunnelMan();
    //Returns whether the protestor just turned 90 degrees
    bool justTurned90(Direction d1, Direction d2);
    //Returns a random direction
    Direction generateRandomDirection();
    //Returns whether the direction is viable for the protestor to change to
    bool isViableDirection(Direction d);
    //Returns whether the protestor is at an intersection
    bool atIntersection();
    //Changes direction (if needed) so that the protestor is on its way to leave the oil field
    void changeDirectionToExit();
    
private:
    int m_hitPoints;
    int m_numSquaresToMoveInCurrentDirection;
    int m_tickCount;
    int m_nonRestingTicksSinceTurn;
    int m_nonRestingTicksSinceShout;
    bool m_leaveTheOilField;
    
};


class RegularProtestor : public Protestor{
public:
    RegularProtestor(StudentWorld* myWorld);
    virtual char getGameID() const;
    virtual void changeDirectionToMoveTowardTunnelMan();
    
    
private:
    //Changes direction so that the protestor faces the tunnelman
    void faceTunnelMan();
};

class HardcoreProtestor : public Protestor{
public:
    HardcoreProtestor(StudentWorld* myWorld);
    virtual char getGameID() const;
    virtual bool canMoveTowardTunnelMan();
    virtual void changeDirectionToMoveTowardTunnelMan();

    
private:
};

#endif // ACTOR_H_

