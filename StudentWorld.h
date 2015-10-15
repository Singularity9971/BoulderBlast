#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <list>
#include <sstream>
#include <iomanip>
using namespace std;


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
        m_player = nullptr;             //sets pointer which points to player = to nullptr
        bonus = 1000;                   //sets the bonus value to 1000
        m_jewels = 0;                   //sets the number of jewels to 0
        check = nullptr;                //sets the exit pointer to nullptr
	}

    void createNewGoodie(int ID, int x, int y);   //to create appropriate goodie at position x,y
    
    virtual int init();
	    
    
    void fireBullet(Actor* p, int x, int y, Actor::Direction dir)     //plays appropriate sound and calls the newBullet function
    {
        if(p == m_player)
            playSound(SOUND_PLAYER_FIRE);
        else
            playSound(SOUND_ENEMY_FIRE);
        
        li.push_back(new Bullet(x,y,dir,this));

    }
    
    void HitIt(int x, int y);               //hits actor at position x,y
        
    Actor* checkIfGoodie(int x, int y);                   //loops through list to check if actor at position x,y is a goodie
        
    bool RobotCanFire(int x,int y, Actor::Direction dir);   //checks the robot's line of sight
        
    bool canCreateRobot(int factoryX, int factoryY);        //checks if factory can create a robot
    
    bool createNewRobot(Actor* p,int x,int y);              //1 in 50 chance of creating robot, returns true if robot is formed
    
    int getXcoordOfNext(int x,Actor::Direction dir) const   //return adjacent x position
    {
        if(dir == Actor::up || dir == Actor::down)
            return x;
        else if(dir == Actor::right)
            return x+1;
        else
            return x-1;
    }
    
    int getYcoordOfNext(int y,Actor::Direction dir) const  //return adjacent y position
    {
        if(dir == Actor::right || dir == Actor::left)
            return y;
        else if(dir == Actor::up)
            return y+1;
        else
            return y-1;
    }
    

    Player* returnPlayer() const
    {
        return m_player;
    }
    
    void decreaseJewels()       //decrease number of jewels
    {
        m_jewels--;
    }
    
    int numJewels() const       //return number of jewels
    {
        return m_jewels;
    }
    
    int getBonus() const        //return the bonus points left in the level
    {
        return bonus;
    }
    
    void setDisplayText();
        
    bool BulletCanPass(Actor* p)    //return true if bullet can pass through the actor, that is hole,goodies and bullet
    {
        if(p == nullptr)
            return true;
        if(!p->isImmortal() && !p->isMortal())
            return true;
        
        return false;
    }
    
    
    virtual int move();
	    
    
    virtual void cleanUp();
	    
    
    bool canMove(Actor* q,int x, int y, Actor::Direction dir) const; //checks if the actor can move to position x,y
    
    bool canBeHit(int x, int y) const       //checks if actor at position can be damaged by a bullet
    {
        Actor* p = returnActor(x, y);
        
            if(p != nullptr && p->isMortal())
                return true;
        
        return false;
    }
    
    Actor* returnActor(int x, int y) const;  //returns actor at position x,y
        
    
    ~StudentWorld()
    {
        cleanUp();
    }
    
private:
    list<Actor*> li;
    Player* m_player;
    int bonus;
    int m_jewels;
    Exit* check;
    
};

#endif // STUDENTWORLD_H_
