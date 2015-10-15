#include "Actor.h"
#include "StudentWorld.h"
#include <cstdlib>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

void Player::doSomething()                      
{
    if(!isAlive())
        return;
    
    int key = 0;
    if(getWorld()->getKey(key))
    {
        int x = getX();
        int y = getY();
        Direction dir = none;
        switch (key)                //Switch statement to record direction and adjacent x and y coordinates according to the direction
        {
            case KEY_PRESS_DOWN:
            {
                dir= down;
                x = getWorld()->getXcoordOfNext(x, dir);
                y = getWorld()->getYcoordOfNext(y, dir);
            }
                break;
            case KEY_PRESS_LEFT:
            {
                dir = left;
                x= getWorld()->getXcoordOfNext(x, dir);
                y= getWorld()->getYcoordOfNext(y, dir);
            }
                break;
                
            case KEY_PRESS_RIGHT:
            {
                dir = right;
                x=getWorld()->getXcoordOfNext(x, dir);
                y=getWorld()->getYcoordOfNext(y, dir);
            }
                break;
                
            case KEY_PRESS_UP:
            {
                dir=up;
                x=getWorld()->getXcoordOfNext(x, dir);
                y=getWorld()->getYcoordOfNext(y, dir);
            }
                break;
                
            case KEY_PRESS_ESCAPE:
            {
                setDead();
                return;
            }
                break;
                
            case KEY_PRESS_SPACE:
            {
                if(m_ammo <= 0)                                     //return if player is out of ammo
                    return;
                m_ammo--;                                               //decrement ammo after firing
                x = getWorld()->getXcoordOfNext(x, getDirection());
                y = getWorld()->getYcoordOfNext(y, getDirection());
                getWorld()->fireBullet(this, x, y, getDirection());         //fire a bullet in front of the player
                return;
                
            }
                break;
                
            default:
                break;
        }
        if(getDirection() != dir)           //change direction if player has been given new direction
            setDirection(dir);
        if(getWorld()->canMove(this,x, y, getDirection()))        //move the player in the rquired direction if it is able to move
            moveTo(x, y);
    }
}

void Bullet::doSomething()
{
    if(!isAlive())
        return;
    
    int x = getWorld()->getXcoordOfNext(getX(), getDirection());
    int y = getWorld()->getYcoordOfNext(getY(), getDirection());
    Actor* q = getWorld()->returnActor(getX(), getY());         //return actor at bullet's current position
    Actor* p = getWorld()->returnActor(x, y);                   //return actor at bullet's next position
    
    if(q != nullptr && !getWorld()->BulletCanPass(q))   //checks bullet's current position
    {
        setDead();                                          //kill bullet if it hits something it can't pass
        setVisible(false);
        if(getWorld()->canBeHit(getX(), getY()))            //if actor can be hit, then hit it
            getWorld()->HitIt(getX(), getY());
        return;
    }
    
    moveTo(x, y);                                       //moves bullet to adjacent position

    if(p != nullptr && !getWorld()->BulletCanPass(p))    //checks bullet's next position
    {
        setDead();
        setVisible(false);
        if(getWorld()->canBeHit(getX(), getY()))
            getWorld()->HitIt(getX(), getY());
        return;
    }
    
    
}


bool Boulder::push(Direction dir)
{
    int a = getWorld()->getXcoordOfNext(getX(), dir);
    int b = getWorld()->getYcoordOfNext(getY(), dir);
    Actor* p = getWorld()->returnActor(a, b);               //return actor at Boulder's adjacent position
    if(p==nullptr)
    {
        moveTo(a, b);                                       //move boulder if adjacent position is empty
        return true;
    }
    else if(dynamic_cast<Hole*>(p) != nullptr)   //else if there is a hole, destroy both the hole and the boulder
    {
        p->setDead();
        moveTo(a, b);
        setDead();
        return true;
    }
    else
        return false;
}



void Goodie::doSomething()
{
    getWorld()->increaseScore(m_increase);              //increase score after getting goodie
    setDead();                                          //destroy goodie after taking it
    getWorld()->playSound(SOUND_GOT_GOODIE);            //play sound
}

void ExtraLife::doSomething()
{
    
    if(getWorld()->returnPlayer()->getX() == getX() && getWorld()->returnPlayer()->getY() == getY())
    {
        Goodie::doSomething();
        getWorld()->incLives();                 //if player is on the goodie, apply the goodie's bonus
    }
}

void RestoreHealth::doSomething()
{
    if(!isAlive())
        return;

    if(getWorld()->returnPlayer()->getX() == getX() && getWorld()->returnPlayer()->getY() == getY())
    {
        Goodie::doSomething();
        getWorld()->returnPlayer()->restoreHealth();   //restore health of the player
    }
}

void Ammo::doSomething()
{
    if(!isAlive())
        return;

    if(getWorld()->returnPlayer()->getX() == getX() && getWorld()->returnPlayer()->getY() == getY())
    {
        Goodie::doSomething();
        getWorld()->returnPlayer()->increaseAmmo();     //increase ammo of the player
    }
}

void Jewel::doSomething()
{
    if(!isAlive())
        return;

    if(getWorld()->returnPlayer()->getX() == getX() && getWorld()->returnPlayer()->getY() == getY())
    {
        Goodie::doSomething();
        getWorld()->decreaseJewels();       //decrease the number of jewels in the world
    }
}

void Exit::doSomething()
{
    if(getWorld()->returnPlayer()->getX() == getX() && getWorld()->returnPlayer()->getY() == getY() && isVisible())
    {
        getWorld()->playSound(SOUND_FINISHED_LEVEL);               //if player is on the exit and it's visible, complete level
        getWorld()->increaseScore(2000 + getWorld()->getBonus());
        levelComplete = true;
        return;
    }
    
    if(getWorld()->numJewels() == 0)                //if jewels are finished, make the level visible and play the sound once
    {
        if(checker == 1)
            return;
        getWorld()->playSound(SOUND_REVEAL_EXIT);
        makeVisible();
        checker++;
    }
}

void Robot::isAttacked()                            //calls the base class' isAttacked function and plays the appropriate sound
{
    Mortal::isAttacked();
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
    if(!isAlive())
        getWorld()->playSound(SOUND_ROBOT_DIE);
}

int Robot::setTicks()                               //function to set the ticks
{
    int x = (28 - getWorld()->getLevel())/4;
    
    return x > 3 ? x : 3;
}

bool Robot::fire(int x, int y, Direction dir)           //fires a bullet if the robot can fire in that direction
{
    if(getWorld()->RobotCanFire(x, y, dir))
    {
        int a = getWorld()->getXcoordOfNext(x, dir);
        int b = getWorld()->getYcoordOfNext(y, dir);
        getWorld()->fireBullet(this, a, b, dir);
        return true;
        
    }
    return false;
}

void Snarlbot::doSomething()
{
    if(!isAlive())
        return;
    
    if(getTicks() != getTempTicks())            //turn to rest
    {
        increaseTick();
        return;
    }
    if(fire(getX(), getY(), getDirection()))            //function to check if it can fire and then fire the bullet
    {
        setTicksToZero();
        return;
    }

    int x = getWorld()->getXcoordOfNext(getX(), getDirection());
    int y = getWorld()->getYcoordOfNext(getY(), getDirection());
    
    if(getWorld()->canMove(this,x, y,getDirection()))           //else move in the adjacent direction if possible
        moveTo(x, y);
    else
        setDirection(changeDirection());                        //else change the direction
    
    setTicksToZero();                                           //set ticks to 0 after moving
}



bool Kleptobot::setProperDirection()
{
    
    int arr[] = {0,0,0,0};
    
    while(arr[0] == 0 || arr[1] == 0 || arr[2] == 0 || arr[3] == 0)  //checks if kleptobot can move in any direction and change its direction if                                                it can
    {
    Direction dir = getRandomDirection();                       //get a random direction and adjacent position coordinates
    int x = getWorld()->getXcoordOfNext(getX(), dir);
    int y = getWorld()->getYcoordOfNext(getY(), dir);

    switch (dir)
        {
        case up:
            if(arr[0] == 0)                                     //set array value to 1 if robot cannot move in that direction
            {
                if(getWorld()->canMove(this, x, y, dir))
                {
                    setDirection(dir);
                    return true;}
                else
                    arr[0] = 1;
            }
            break;
        case down:
            if(arr[1] == 0)
            {
                if(getWorld()->canMove(this, x, y, dir))
                {
                    setDirection(dir);
                    return true;}
                else
                    arr[1] = 1;
            }
            break;
        case left:
            if(arr[2] == 0)
            {
                if(getWorld()->canMove(this, x, y, dir))
                {
                    setDirection(dir);
                    return true;}
                else
                    arr[2] = 1;
            }
            break;
        case right:
            if(arr[3] == 0)
            {
                if(getWorld()->canMove(this, x, y, dir))
                {
                    setDirection(dir);
                    return true;}
                else
                    arr[3] = 1;
            }
            break;
        default:
            break;
            
        }
    }
    return false;

}

void Kleptobot::doSomething()
{
    if(!isAlive())
        return;
    
    if(getTicks() != getTempTicks())
    {
        increaseTick();
        return;
    }
    
    int x = getWorld()->getXcoordOfNext(getX(), getDirection());
    int y = getWorld()->getYcoordOfNext(getY(), getDirection());
    
    if(getTemp() < getDistance())                           //if it hasn't moved DistanceBeforeTurning steps
    {
        Actor* p = getWorld()->checkIfGoodie(getX(), getY());               //check if robot is standing on goodie
        if(p != nullptr)
        {
            if(doesItPickUpGoodie(p))                               //if it is, then check if it actually picks it up(1/10 chance)
            {
                p->setDead();
                getWorld()->playSound(SOUND_ROBOT_MUNCH);           //if it does, destory the goodie and play sound
                pickedUpGoodie();
                setTicksToZero();
            }
            else
            {
                if(getWorld()->canMove(this, x, y, getDirection()))     //else if it doesn't pick up the goodie, move one step in current direction
                {
                    Move(x, y);
                }
                else
                {
                    if(setProperDirection())                         //else change direction and then move in the new direction
                    {
                        int a = getWorld()->getXcoordOfNext(getX(), getDirection());
                        int b = getWorld()->getYcoordOfNext(getY(), getDirection());
                        Move(a, b);
                        reset();
                    }

                }
            }
            
        }
        else                                //else if robot is not on a goodie
        {
            if(getWorld()->canMove(this, x, y, getDirection()))         //move in current direction, if it can
            {
                Move(x, y);
            }
            else
            {
                if(setProperDirection())                        //else move in a new direction
                {
                    int a = getWorld()->getXcoordOfNext(getX(), getDirection());
                    int b = getWorld()->getYcoordOfNext(getY(), getDirection());
                    Move(a, b);
                    reset();
                    
                }
                                
            }
            
        }
    }
    

    else                                //else if robot has already moved distanceBeforeTurning steps, change direction and move in new direction
    {
        if(setProperDirection())
        {
            int a = getWorld()->getXcoordOfNext(getX(), getDirection());
            int b = getWorld()->getYcoordOfNext(getY(), getDirection());
            Move(a, b);
            reset();
        }
    }


}

bool KleptobotFactory::createRobot()                //function to check if robot has been created by factory
{
    if(getWorld()->createNewRobot(this,getX(),getY()))
        return true;
    else
        return false;
}


void KleptobotFactory::doSomething()            //if factory can create robot and creates it, play sound
{
    if(getWorld()->canCreateRobot(getX(), getY()))
        if(createRobot())
            getWorld()->playSound(SOUND_ROBOT_BORN);
}



Kleptobot::~Kleptobot()                     //if kleptobot had a goodie, create new goodie when it dies
{
    if(hasGoodie())
        getWorld()->createNewGoodie(storeId, getX(), getY());
}

bool AngryKleptobotFactory::createRobot()       //same as kleptobot factory, but pointer being passed in the function will be different
{
    if(getWorld()->createNewRobot(this,getX(),getY()))
        return true;
    else
        return false;
}

void AngryKleptobot::doSomething()      //if angry bot can fire, then it fires else it does what a normal kleptobot will do
{
    if(!isAlive())
        return;
    
    if(getTicks() != getTempTicks())
    {
        increaseTick();
        return;
    }
    
    if(fire(getX(), getY(), getDirection()))
    {
        setTicksToZero();
        return;
    }
    else
        Kleptobot::doSomething();

}
























