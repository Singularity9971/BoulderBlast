#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init()
{
    
    Level::MazeEntry item;
    int level = getLevel();
    ostringstream oss;
    if(level < 10)
        oss << "level0"<<level<<".dat";
    else
        oss << "level"<<level<<".dat";
    
    if(level == 100)                    //if level is 100 then player has won
        return GWSTATUS_PLAYER_WON;
    
    string curLevel = oss.str();
    Level lev(assetDirectory());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level:: load_fail_bad_format)
    {
        return GWSTATUS_LEVEL_ERROR;
    }
    if(result == Level::load_fail_file_not_found)       //if there are no more levels, then player has won
    {
        return GWSTATUS_PLAYER_WON;
    }
    // otherwise the load was successful and you can access the contents of the level
    
    for(int i = 0; i != 15; i++)
        for(int j = 0; j != 15; j++)
        {
            item = lev.getContentsOf(i, j);
            if(item == Level::player)
            {
                m_player = new Player(i, j, Actor::Direction::right,this);
            }
            else if(item == Level::wall)
            {
                li.push_back(new Wall(i, j, Actor::Direction::none,this));
            }
            else if(item == Level::hole)
            {
                li.push_back(new Hole(i,j,Actor::Direction::none,this));
            }
            else if(item==Level::boulder)
            {
                li.push_back(new Boulder(i,j,Actor::Direction::none,this));
            }
            else if(item == Level::jewel)
            {
                li.push_back(new Jewel(i,j,Actor::Direction::none,this));
                m_jewels++;
            }
            else if(item == Level::extra_life)
            {
                li.push_back(new ExtraLife(i,j,Actor::Direction::none,this));
            }
            else if(item == Level::restore_health)
            {
                li.push_back(new RestoreHealth(i,j,Actor::Direction::none,this));
            }
            else if(item == Level::ammo)
            {
                li.push_back(new Ammo(i,j,Actor::Direction::none,this));
            }
            else if(item == Level::exit)
            {
                check = new Exit(i,j,Actor::Direction::none,this);
                li.push_back(check);
            }
            else if(item == Level::horiz_snarlbot)
            {
                li.push_front(new Snarlbot(i,j,Actor::Direction::right,this));
            }
            else if(item == Level::vert_snarlbot)
            {
                li.push_front(new Snarlbot(i,j,Actor::Direction::down,this));
            }
            else if(item == Level::kleptobot_factory)
            {
                li.push_back(new KleptobotFactory(i,j,this));
            }
            else if(item == Level::angry_kleptobot_factory)
            {
                li.push_back(new AngryKleptobotFactory(i,j,this));
            }
            
        }
    
    return 0;
}

int StudentWorld::move()
{
    
    if(m_player != nullptr && m_player->isAlive())
        m_player->doSomething();
    
    for(list<Actor*>::iterator it = li.begin();it != li.end();it++) //if actor is alive, make it do something
    {
        
        if(!((*it)->isAlive()))
        {
            delete *it;
            it = li.erase(it);
        }
        else
            (*it)->doSomething();
        
        if(!(m_player->isAlive()))
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        if(check != nullptr && check->isComplete())    //if level is complete
        {
            playSound(SOUND_FINISHED_LEVEL);
            return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    bonus > 0 ? bonus-- : bonus = 0;
    
    setDisplayText();                   //set the display text
    
    if(check != nullptr && check->isComplete())
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    else if(m_player->isAlive())
        return GWSTATUS_CONTINUE_GAME;
    else
    {
        return GWSTATUS_PLAYER_DIED;
    }
}

void StudentWorld::cleanUp() //frees dynamic memory and sets bonus to 1000
{
    delete m_player;
    
    list<Actor*>::iterator it = li.begin();
    while(it != li.end())
    {
        delete *it;
        it = li.erase(it);
    }
    m_player = nullptr;
    bonus = 1000;
    m_jewels = 0;
    check = nullptr;
}


void StudentWorld::createNewGoodie(int ID, int x, int y) //creates new goodie at position x,y and increments the int
{
    switch (ID)
    {
        case 2:
        {
            li.push_back(new RestoreHealth(x,y,Actor::none,this));
            
        }
            break;
        case 1:
        {
            li.push_back(new ExtraLife(x,y,Actor::none,this));
        }
            break;
        case 0:
        {
            li.push_back(new Ammo(x,y,Actor::none,this));
        }
            
        default:
            break;
    }
}

void StudentWorld::HitIt(int x, int y)
{
    Actor* p = returnActor(x, y);               //returns actor at poistion x, y and attacks it
    Mortal* toBeHit = dynamic_cast<Mortal*>(p);
    if(toBeHit != nullptr)
    {
    toBeHit->isAttacked();
    if(!(toBeHit->isAlive()))       //if mortal died after being hit, increment score accordingly
    {
        if(dynamic_cast<Snarlbot*>(p) != nullptr)
            increaseScore(100);
        else if(dynamic_cast<Kleptobot*>(p) != nullptr && dynamic_cast<AngryKleptobot*>(p) == nullptr)
            increaseScore(10);
        else if(dynamic_cast<AngryKleptobot*>(p) != nullptr)
            increaseScore(20);
        else if(dynamic_cast<Player*>(p) != nullptr)
            playSound(SOUND_PLAYER_DIE);
    }
    if(dynamic_cast<Player*>(p) != nullptr)     //if player got hit
    {
        playSound(SOUND_PLAYER_IMPACT);
    }
    }
    
}


Actor* StudentWorld::checkIfGoodie(int x, int y)    //checks for all goodies at position x,y except jewel
{
    list<Actor*>::iterator it = li.end();
    --it;
    while(it != li.begin())
    {
        if((*it)->isGoodie())
            if(dynamic_cast<Jewel*>(*it) == nullptr)
                if((*it)->getX() == x && (*it)->getY() == y)
                    return *it;
        
        --it;
    }
    return nullptr;
}

bool StudentWorld::RobotCanFire(int x,int y, Actor::Direction dir) //takes x, y coordinates of robot and checks its line of sight
{
    x = getXcoordOfNext(x, dir);
    y = getYcoordOfNext(y, dir);
    
    while (true)
    {
        Actor* p = returnActor(x, y);
        if(p != nullptr)
        {
            if(dynamic_cast<Player*>(p) != nullptr)
                return true;
            else if(BulletCanPass(p))
            {
                x = getXcoordOfNext(x, dir);
                y = getYcoordOfNext(y, dir);
                continue;
            }
            else
                return false;
        }
        x = getXcoordOfNext(x, dir);
        y = getYcoordOfNext(y, dir);
        
    }
    return false;
    
}

bool StudentWorld::canCreateRobot(int factoryX, int factoryY)   //checks the 3X3 square and returns true if count < 3
{
    
    int count = 0;
    for(list<Actor*>::iterator it = li.begin(); it != li.end();it++)
        if(dynamic_cast<Kleptobot*>(*it)!= nullptr)
        {
            if((*it)->getX() == factoryX && (*it)->getY() == factoryY)
                return false;
            if((*it)->getX() <= factoryX +3 && (*it)->getX() >= factoryX - 3)
                if((*it)->getY() <= factoryY +3 && (*it)->getY() >= factoryY - 3)
                {
                    count++;
                    if(count >= 3)
                        return false;
                }
        }
    
    return true;
}

bool StudentWorld::createNewRobot(Actor* p,int x,int y)  //1 in 50 chance that it creates new robot at position x,y
{
    int random = rand()% 50;
    if(random == 7)
    {
        if(dynamic_cast<AngryKleptobotFactory*>(p) != nullptr)
        {
            li.push_front(new AngryKleptobot(IID_ANGRY_KLEPTOBOT,x,y,this,8));
            return true;
        }
        else
        {
            li.push_front(new Kleptobot(IID_KLEPTOBOT,x,y,this,5));
            return true;
        }
    }
    return false;
}

void StudentWorld::setDisplayText()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    
    ostringstream oss;
    oss.fill('0');oss<<"Score: "<<setw(7)<<score<<endl;
    oss.fill('0');oss<<"  "<<"Level: "<<setw(2)<<level<<endl;
    oss.fill(' ');oss<<"  "<<"Lives: "<<setw(2)<<lives<<endl;
    oss.fill(' ');oss<<"  "<<"Health: "<<setw(3)<<m_player->percentage()<<endl;
    oss.fill(' ');oss<<"%  Ammo: "<<setw(3)<<m_player->ammo()<<endl;
    oss.fill(' ');oss<<"  "<<"Bonus: "<<setw(4)<<bonus<<endl;
    
    string s = oss.str();
    setGameStatText(s);
}

bool StudentWorld::canMove(Actor* q,int x, int y, Actor::Direction dir) const  // to check if actor *q can move to position x, y
{
    
    Actor* p = returnActor(x,y);
    
    if(p == nullptr)
        return true;
    else if(dynamic_cast<Hole*>(p) != nullptr)
        return false;
    else if(p->isImmortal())
        return false;
    else if(p->isMortal() && dynamic_cast<Boulder*>(p) == nullptr)
        return false;
    else if(dynamic_cast<Boulder*>(p) != nullptr)
    {
        if(q == m_player)
        {
            Boulder* a = dynamic_cast<Boulder*>(p);
            if(a->push(dir))
                return true;
            else
                return false;
        }
        else
            return false;
    }
    
    return true;
}

Actor* StudentWorld::returnActor(int x, int y) const //returns player first, then goes through list from beginning
{
    if(m_player->getX() == x && m_player->getY() == y)
        return m_player;
    
    for(list<Actor*>::const_iterator it = li.begin();it!=li.end();it++)
        if((*it)->getX() == x && (*it)->getY() == y)
            if((*it)->isAlive())
                return *it;
    
    return nullptr;
}



// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
