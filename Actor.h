#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include "GameWorld.h"
#include <iostream>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction dir,StudentWorld* ss)      //Actor constructor takes a pointer to Studentworld
    :m_student(ss),GraphObject(imageID,startX,startY,dir)
    {
        setVisible(true);           //make every object visible when constructed
        m_alive = true;
        Mortal = false;
        Goodie = false;
        Immortal = false;
        
    }
    virtual void doSomething() = 0;
    virtual bool isAlive() const       //returns true if actor is alive
    {
        return m_alive;
    }
    
    void setGoodie()                   //called in goodie's constructor
    {
        Goodie = true;
    }
    
    void setImmortal()                  //called in Immortal's constructor
    {
        Immortal = true;
    }
    
    void setMortal()                    //called in mortal's constructor
    {
        Mortal = true;
    }
    
    bool isGoodie() const
    {
        return Goodie;
    }
    
    bool isMortal() const
    {
        return Mortal;
    }
    
    bool isImmortal() const
    {
        return Immortal;
    }
    
    StudentWorld* getWorld() const   //returns pointer to StudentWorld
    {
        return m_student;
    }
    
    virtual void setDead()          //sets bool to false
    {
        m_alive = false;
    }
    
    virtual ~Actor()                //make every actor invisible when it's destroyed
    {
        setVisible(false);
    }
private:
    StudentWorld* m_student;
    bool m_alive;
    bool Mortal;
    bool Goodie;
    bool Immortal;
    
};

class Immortal : public Actor           //objects which can't be destroyed, that is wall and factories
{
public:
    Immortal(int imageID, int startX, int startY, Direction dir,StudentWorld* ss)
    :Actor(imageID,startX,startY,dir,ss)
    {
        setImmortal();
    }
    
    virtual void doSomething() = 0;
    
    
    void setDead()
    {}
    
    virtual ~Immortal()
    {
        
    }
    
};

class Mortal : public Actor            //objects which can be damaged, abstract class
{
public:
    Mortal(int imageID, int startX, int startY, Direction dir,StudentWorld* ss,int life)
    :Actor(imageID,startX,startY,dir,ss)
    {
        m_points = life;            //constructor takes in an int to represent their life and sets them to Mortal
        setMortal();
    }
    
    virtual void isAttacked()       //if a mortal object is hit, decrement its health by 2 points
    {
        m_points=m_points-2;
    }
    
    
    virtual bool isAlive() const     //mortal is alive if its health is > 0
    {
        return m_points > 0 ? true : false;
    }
    
    void restoreHealth()          //only ever called on player, sets health back to 20
    {
        m_points = 20;
    }
    
    double health() const          //returns current health
    {
        return m_points;
    }
    
    virtual void setDead()          //set health to 0
    {
        m_points = 0;
    }
    
    virtual void doSomething()=0;
    
    virtual ~Mortal()
    {}
private:
    int m_points;
};

class Wall : public Immortal            //wall object, which is immortal and does nothing
{
public:
    Wall(int startX, int startY, Direction dir,StudentWorld* ss)
    :Immortal(IID_WALL,startX,startY,dir,ss)
    {}
    
    virtual void doSomething()
    {}
    
    virtual ~Wall()
    {}
    
};

class Player : public Mortal
{
public:
    Player(int StartX, int StartY, Direction dir,StudentWorld* ss)
    :Mortal(IID_PLAYER,StartX,StartY,dir,ss,20)
    {
        m_ammo = 20;            //constructor sets ammo to 20
    }
    
    virtual void doSomething();
    
    double percentage() const       //returns how mouch percentage of health player has left
    {
        std::cout.setf(std::ios::fixed);
        std::cout.precision(0);
        return (health()/20)*100;
    }
    
    int ammo() const                //returns current ammo of player
    {
        return m_ammo;
    }
    
    virtual void increaseAmmo()             //increase ammo of player by 20 points, used when player picks up ammo goodie
    {
        m_ammo+=20;
    }
    
    
    virtual ~Player()
    {}
private:
    int m_ammo;
};

class Bullet : public Actor         //bullet is a separate object derived straight from actor
{
public:
    Bullet(int StartX, int StartY, Direction dir,StudentWorld* ss)
    :Actor(IID_BULLET,StartX,StartY,dir,ss)
    {
    }
    virtual void doSomething();
    
    virtual ~Bullet()
    {}

};

class Hole : public Actor       //hole is a separate object as well which does nothing
{
public:
    Hole(int StartX, int StartY, Direction dir,StudentWorld* ss)
    :Actor(IID_HOLE,StartX,StartY,dir,ss)
    {
    }
    
    virtual void doSomething()
    {
        
    }
    
    virtual ~Hole()
    {
        
    }
    
private:

};

class Boulder : public Mortal       //boulder is mortal as it can be hit, its life is 10 points
{
public:
    Boulder(int StartX, int StartY, Direction dir,StudentWorld* ss)
    :Mortal(IID_BOULDER,StartX,StartY,dir,ss,10)
    {
    }
    
    virtual ~Boulder()
    {}
    
    
    virtual void doSomething()
    {}
    
    virtual bool push(Direction);
    
};

class Goodie : public Actor         //goodie represents jewel,ammo,extralife and restoreHealth
{
public:
    Goodie(int imageID, int startX, int startY, Direction dir,StudentWorld* ss,int increase)
    :Actor(imageID,startX, startY, dir, ss)
    {
        m_increase = increase;              //since every goodie when collected gives bonus points, these points are taken and stored in the
        setGoodie();                        //constructor for every goodie
    }
    
    virtual ~Goodie(){}
    
    virtual void doSomething();
    
    virtual void dummy() = 0;

private:
    int m_increase;
};

class Jewel : public Goodie
{
public:
    Jewel(int startX, int startY, Direction dir,StudentWorld* ss)
    :Goodie(IID_JEWEL,startX, startY, dir, ss,50)
    {}
    
    virtual void doSomething();
    
    void dummy()
    {}
    
    virtual ~Jewel()
    {}

};

class ExtraLife : public Goodie
{
public:
    ExtraLife(int startX, int startY, Direction dir,StudentWorld* ss)
    :Goodie(IID_EXTRA_LIFE,startX, startY, dir, ss,1000)
    {}
    
    void dummy()
    {}
    
    virtual ~ExtraLife()
    {}
    
    virtual void doSomething();
    
};

class RestoreHealth : public Goodie
{
public:
    RestoreHealth(int startX, int startY, Direction dir,StudentWorld* ss)
    :Goodie(IID_RESTORE_HEALTH,startX, startY, dir, ss,500)
    {}
    
    void dummy()
    {}
    
    virtual ~RestoreHealth()
    {}
    
    virtual void doSomething();

};

class Ammo : public Goodie
{
public:
    Ammo(int startX, int startY, Direction dir,StudentWorld* ss)
    :Goodie(IID_AMMO,startX, startY, dir, ss,100)
    {}
    
    void dummy()
    {}
    
    virtual ~Ammo()
    {}
    
    virtual void doSomething();
    
};

class Exit : public Actor           //exit is a separate object derived from actor
{
public:
    Exit(int startX, int startY, Direction dir,StudentWorld* ss)
    :Actor(IID_EXIT,startX,startY,dir,ss)
    {
        visible = false;            //make exit invisible
        setVisible(visible);
        levelComplete = false;      //bool to check if levelIsComplete that is number of jewels left are 0
        checker = 0;                //checker int makes sure exit is made visible only once
    }
    
    virtual ~Exit()
    {}
    
    virtual void doSomething();
    
    bool isVisible() const      //returns if exit is visible or not
    {
        return visible;
    }
    
    void makeVisible()          //makes exit visible, called when number of jewels equal 0
    {
        visible = true;
        setVisible(visible);
    }
    
    bool isComplete() const     //returns whether level is complete or not
    {
        return levelComplete;
    }
    
private:
    bool visible;
    bool levelComplete;
    int checker;
    

};

class Robot : public Mortal     //robots are mortal objects
{
public:
    Robot(int imageID,int startX,int startY,Direction dir,StudentWorld* ss,int life)
    :Mortal(imageID,startX,startY,dir,ss,life)
    {
        m_ticks = setTicks();       //sets int tempticks to the number of ticks as per the current level
        temp_ticks = 0;             //sets the int temp_ticks to 0
    }
    
    virtual void isAttacked();
    
    int setTicks();
    
    int getTicks() const
    {
        return m_ticks;
    }
    
    void increaseTick()           //increases temp_ticks, called after every time robot rests
    {
        temp_ticks++;
    }
    
    virtual void doSomething() = 0;
    
    int getTempTicks() const      //returns tempticks
    {
        return temp_ticks;
    }
    
    bool fire(int x, int y, Direction dir); //fire function checks and fires bullet, returns true if a bullet is formed
    
    void setTicksToZero()                   //sets tempticks to 0, called after every time a robot does something
    {
        temp_ticks = 0;
    }
        
    virtual ~Robot()
    {}
    
private:
    int m_ticks;
    int temp_ticks;
    
};

class Snarlbot : public Robot
{
public:
    Snarlbot(int startX,int startY,Direction dir,StudentWorld* ss)
    :Robot(IID_SNARLBOT,startX,startY,dir,ss,10)
    {
        m_direction = dir;          //snarlbot has a direction member variable
    }
    
    virtual void doSomething();
    
    
    Direction changeDirection() //function which reverses direction based on current direction
    {
        switch (m_direction)
        {
            case up:
                m_direction = down;
                break;
            case down:
                m_direction = up;
                break;
            case left:
                m_direction = right;
                break;
            case right:
                m_direction = left;
                break;
            default:
                break;
        }
        return m_direction;
    }
        
    Direction getTheDirection() const //gets direction member variable of the snarlbot
    {
        return m_direction;
    }
    
    virtual ~Snarlbot(){}
    
    
private:
    Direction m_direction;
    
};


class Kleptobot : public Robot      //kleptobot class which represents a normal kleptobot
{
public:
    Kleptobot(int imageID,int startX,int startY, StudentWorld* ss,int points)
    :Robot(imageID,startX,startY,right,ss,points)
    {
        distanceBeforeTurning = randomNumber();             //sets the distance as a random number between 1 and 6
        Goodie = false;                                     //bool which tells whether the robot has a goodie or not
        storeId = -99;                                      //int which stores id of goodie, is initialized to junk value
        temp = 0;                                           //temp int which increments everytime the kleptobot moves
    }
    
    virtual void doSomething();
    
    void Move(int x, int y)                         //moves to x, y, increases temp and sets the ticks to 0
    {
        moveTo(x, y);
        incTemp();
        setTicksToZero();
    }
    
    bool hasGoodie() const
    {
        return Goodie;
    }
    
    void pickedUpGoodie()                                   //sets goodie bool to true, means robot has picked up a goodie
    {
        Goodie = true;
    }
    
    int getTemp() const                                     //returns the temp int
    {
        return temp;
    }
    
    void incTemp()                                          //increases the temp int
    {
        temp++;
    }
    
    
    void reset()                                            //resets temp value
    {
        temp = 0;
    }
        
    bool doesItPickUpGoodie(Actor* q)
    {
        if(hasGoodie())
            return false;
        int random = rand() % 10;
        if(random == 7)                             //this guarantees a 1 in 7 chance
        {
            if(dynamic_cast<Ammo*>(q) != nullptr)    //id = 0 for ammo
                storeId = 0;
            else if(dynamic_cast<ExtraLife*>(q) != nullptr)  //id = 1 for extralife
                storeId = 1;
            else if(dynamic_cast<RestoreHealth*>(q) != nullptr)   //id = 2 for restorehealth
                storeId = 2;
            return true;
        }
        
        return false;
    }
    
    Direction getRandomDirection() const   //returns a random direction
    {
        Direction dir = none;
        int random = rand() % 4+1;
        switch (random)
        {
            case 1:
                dir = down;
                break;
            case 2:
                dir = up;
                break;
            case 3:
                dir = right;
                break;
            case 4:
                dir = left;
                break;
            default:
                break;
        }
        
        return dir;

    }
    
    virtual int randomNumber() const   //returns random number between 1 and 6
    {
        return rand() % 6 + 1;
    }
    
    bool setProperDirection();
    
    virtual void setDistance()          //sets the distanceBeforeTurning to a random number
    {
        distanceBeforeTurning = randomNumber();
    }
    
    int getDistance() const
    {
        return distanceBeforeTurning;
    }
    
    virtual ~Kleptobot();

private:
    int distanceBeforeTurning;
    bool Goodie;
    int storeId;
    int temp;
};

class KleptobotFactory : public Immortal        //immortal object
{
public:
    KleptobotFactory(int startX,int startY,StudentWorld* ss)
    :Immortal(IID_ROBOT_FACTORY,startX,startY,none,ss)
    {}
    
    virtual void doSomething();
    
    virtual bool createRobot();
        
    ~KleptobotFactory()
    {}

};

class AngryKleptobotFactory : public KleptobotFactory   //derived from kleptobotfactory as both do the same thing
{
public:
    AngryKleptobotFactory(int startX,int startY,StudentWorld* ss)
    :KleptobotFactory(startX,startY,ss)
    {}
    
    virtual bool createRobot();
        
    virtual ~AngryKleptobotFactory()
    {}
    
};

class AngryKleptobot : public Kleptobot             //derived from kleptobot
{
public:
    AngryKleptobot(int imageID,int startX,int startY,StudentWorld* ss,int points)
    :Kleptobot(imageID,startX,startY,ss,points)
    {}
    
    virtual void doSomething();
    
    virtual ~AngryKleptobot()
    {}
    
};



#endif // ACTOR_H_
