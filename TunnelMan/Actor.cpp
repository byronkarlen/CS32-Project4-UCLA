#include "Actor.h"
#include "StudentWorld.h"

using namespace std;

/*                          Actor class method implementations:                         */

Actor::Actor(StudentWorld* myWorld, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth) : GraphObject(imageID, startX, startY, dir, size, depth){
    
    m_world = myWorld;
    setVisible(false); //All actors start out as not visible
    setLiveStatus(true); //All actors start out as alive
}

void Actor::setLiveStatus(bool b){
    m_isAlive = b;
}
bool Actor::getLiveStatus() const{
    return m_isAlive;
}

StudentWorld* Actor::getWorld() const{
    return m_world;
}

void Actor::move(){
    if(getDirection() == up && getY() < VIEW_HEIGHT-actorSize)
        moveTo(getX(), getY()+1);
    else if(getDirection() == down && getY() > 0)
        moveTo(getX(), getY() - 1);
    else if(getDirection() == right && getX() < VIEW_WIDTH-actorSize)
        moveTo(getX()+1, getY());
    else if(getDirection() == left && getX() > 0)
        moveTo(getX() - 1, getY());
}



/*                          Earth class method implementations:                 */

Earth::Earth(StudentWorld* myWorld, int startX, int startY) : Actor(myWorld, TID_EARTH, startX, startY, right, 0.25, 3){
    
    setVisible(true); //Earths are set to be visible when they are created
}

char Earth::getGameID() const{
    return 'E'; //Earths have a character ID of 'E'
}



/*                          TunnelMan class method implementations                  */

TunnelMan::TunnelMan(StudentWorld* myWorld) : Actor(myWorld, TID_PLAYER, 30, 60){
    m_numSquirts = 5;
    m_numSonarCharges = 1;
    m_numGoldNuggets = 0;
    m_numBarrelsFound = 0;
    m_hitPoints = 10;
    
    setVisible(true); //TunnelMan starts out as visible
}

char TunnelMan::getGameID() const{
    return 'T';
}

void TunnelMan::doSomething(){
    if(!getLiveStatus()){
        return;
    }
    int x = getX();
    int y = getY();
    StudentWorld* myWorld = getWorld();
    
    if(myWorld->earthAt(x, y)){
        myWorld->removeEarth(x, y);
        myWorld->playSound(SOUND_DIG);
        return;
    }
    
    
    int ch;
    if(myWorld->getKey(ch) == true){
        if(ch == KEY_PRESS_ESCAPE){
            annoy(100);
        }
        else if(ch == KEY_PRESS_SPACE){
//            If they have suffient water in their squirt gun
//            the TunnelMan will fire a Squirt into the oil field
//            The TunnelMan will then reduce their water count by 1.
            if(m_numSquirts > 0){
                introduceSquirt();
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                m_numSquirts--;
            }

        }
        else if(ch == 'Z' || ch == 'z'){
            if(m_numSonarCharges > 0){
                m_numSonarCharges--;
                getWorld()->illuminateOilField(getX(), getY(), 12);
                getWorld()->playSound(SOUND_SONAR);
            }
        }
        else if(ch == KEY_PRESS_TAB){
            if(m_numGoldNuggets > 0){
                
                Gold* g= new Gold(getWorld(), getX(), getY(), false);
                getWorld()->addActor(g);
                m_numGoldNuggets--;
            }

        }
        //ch is one of the arrow keys
        else{
            //*the TunnelMan cannot occupy a square that is less than or equal to a radius of 3 away from the center of any Boulder.
            if(ch == KEY_PRESS_RIGHT){
                if(getDirection() == right){
                    if(!myWorld->boulderWithinRadius3(x+1, y))
                       move();
                }
                else
                    setDirection(right);
            }
            if(ch == KEY_PRESS_LEFT){
                if(getDirection() == left){
                    if(!myWorld->boulderWithinRadius3(x-1, y))
                        move();
                }
                else
                    setDirection(left);
            }
            if(ch == KEY_PRESS_UP){
                if(getDirection() == up){
                    if(!myWorld->boulderWithinRadius3(x, y+1))
                        move();
                }
                else
                    setDirection(up);
            }
            if(ch == KEY_PRESS_DOWN){
                if(getDirection() == down){
                    if(!myWorld->boulderWithinRadius3(x, y-1))
                        move();
                }
                else
                    setDirection(down);
            }
        }
    }
}

void TunnelMan::incrementNumNuggets(){
    m_numGoldNuggets++;
}

int TunnelMan::getNumNuggets(){
    return m_numGoldNuggets;
}

void TunnelMan::incrementNumSonarCharges(){
    m_numSonarCharges ++;
}

int TunnelMan::getNumSonarCharges(){
    return m_numSonarCharges;
}

void TunnelMan::incrementNumSquirts(){
    m_numSquirts += 5;
}
int TunnelMan::getNumSquirts(){
    return m_numSquirts;
}

void TunnelMan::incrementBarrelsFound(){
    m_numBarrelsFound++;
}

int TunnelMan::getNumBarrelsFound(){
    return m_numBarrelsFound;
}

int TunnelMan::getHitPoints(){
    return m_hitPoints;
}
void TunnelMan::annoy(int howMuch){
    m_hitPoints -= howMuch;
    if(m_hitPoints <= 0){
        setLiveStatus(false);
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}

void TunnelMan::introduceSquirt(){
    int startX = getX();
    int startY = getY();
    
    if(getDirection() == up)
        startY += actorSize;
    if(getDirection() == right)
        startX += actorSize;
    if(getDirection() == left)
        startX -= actorSize;
    if(getDirection() == down)
        startY -= actorSize;
    
    if(getWorld()->actorWouldBeWithinField(startX, startY) && !getWorld()->earthAt(startX, startY) && !getWorld()->boulderWithinRadius3(startX, startY)){
        Squirt* s = new Squirt(getWorld(), startX, startY);
        getWorld()->addActor(s);
    }
}



/*                          Boulder class method implementations:                   */

Boulder::Boulder(StudentWorld* myWorld, int startX, int startY) : Actor(myWorld, TID_BOULDER, startX, startY, down, 1, 1.0){
    setVisible(true); //Boulders start out visible
    
    getWorld()->removeEarth(startX, startY);
    state = 0; //Boulders start out in a stable state (0 = stable, 1 = waiting, 2 = falling)
    ticksElapsed = -1; //To track how many ticks have elapsed after entering waiting state. ticksElapsed = -1 when the boulder is not in a waiting state
}

char Boulder::getGameID() const{
    return 'B';
}

void Boulder::doSomething(){
    if(!getLiveStatus())
        return;

    if(state == 0){ //Stable state
        if(boulderCanFall()){
            state = 1; //Waiting state
            ticksElapsed++;
            return;
        }
    }

    if(state == 1){ //Waiting state
        int ticksBeforeBoulderFalls = 30; 
        if(ticksElapsed >= ticksBeforeBoulderFalls){
            state = 2; //Falling state
            getWorld()->playSound(SOUND_FALLING_ROCK);
            ticksElapsed = -1;
        }
        else{
            ticksElapsed++;
        }
        return;
    }
    if(state == 2){//Falling state
        if(boulderCanFall()){
            getWorld()->playSound(SOUND_FALLING_ROCK);
            smushCharacters();
            move();
        }
        else{
            setLiveStatus(false); //Set the boulder to dead
        }
    }
}

void Boulder::smushCharacters(){
    getWorld()->killProtestorsWithinRadius(getX(), getY(), 4);
    
    if(getWorld()->tunnelManWithinRadius(getX(), getY(), 4)){
        getWorld()->getTunnelMan()->annoy(100);
    }
}
bool Boulder::boulderCanFall() const{
    return !getWorld()->willHitBoulderEdgeOrEarth(getX(), getY(), down);
}



/*                      Goodie Class Function Implementations:                  */

Goodie::Goodie(StudentWorld* myWorld, int imageID, int startX, int startY, bool tunnelManCanPickUp, bool temp) : Actor(myWorld, imageID, startX, startY, right, 1.0, 2){
    //Goodies start out as invisible
    m_tunnelManCanPickUp = tunnelManCanPickUp;
    
    m_temporary = temp;
    
    if(m_temporary)
        //TODO: fix this
        m_tickLifeTime = fmax(100, 300 - 10*getWorld()->getLevel());
    else
        m_tickLifeTime = -1;
    m_tickCount = 0;
}

void Goodie::setTickLifeTime(int tickLife){
    m_tickLifeTime = tickLife;
}

void Goodie::doSomething(){
    if(!getLiveStatus())
        return;
    
    if(m_temporary){
        if(m_tickCount >= m_tickLifeTime){
            setLiveStatus(false);
            return;
        }
        else
            m_tickCount++;
    }
    
    if(!isVisible() && getWorld()->tunnelManWithinRadius(getX(), getY(), 4)){
        setVisible(true);
        return;
    }
    
    if(m_tunnelManCanPickUp && getWorld()->tunnelManWithinRadius(getX(), getY(), 3)){
        doSomethingToTunnelMan();
        setLiveStatus(false);
    }
    
    if(!m_tunnelManCanPickUp){
        vector<Actor*> protestors = getWorld()->findProtestorsWithinRadius(getX(), getY(), 3);
        if(protestors.size() != 0){
            doSomethingToProtestor();
            setLiveStatus(false);
        }
    }
}



/*                          Barrel Class Function Implementations                   */

Barrel::Barrel(StudentWorld* myWorld, int startX, int startY) : Goodie(myWorld, TID_BARREL, startX, startY, true, false){
    
    //Barrels start out as invisible
}

char Barrel::getGameID() const{
    return 'O'; //Barrels have a character/game ID of 'O' (Oil)
}

void Barrel::doSomethingToTunnelMan(){
    getWorld()->playSound(SOUND_FOUND_OIL);
    getWorld()->increaseScore(1000);
    getWorld()->getTunnelMan()->incrementBarrelsFound();
}



/*                          Waterpool Class Function Implementations                   */

WaterPool::WaterPool(StudentWorld* myWorld, int startX, int startY) : Goodie(myWorld, TID_WATER_POOL, startX, startY, true, true){
    setVisible(true);
}

char WaterPool::getGameID() const{
    return 'W';
}

void WaterPool::doSomethingToTunnelMan(){
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->getTunnelMan()->incrementNumSquirts();
    getWorld()->increaseScore(100);
    
}



/*                          SonarKit Class Function Implementations                   */

SonarKit::SonarKit(StudentWorld* myWorld) : Goodie(myWorld, TID_SONAR, 0, 60, true, true){
    setVisible(true);
}

char SonarKit::getGameID() const{
    return 'K';
}

void SonarKit::doSomethingToTunnelMan(){
    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->getTunnelMan()->incrementNumSonarCharges();
    getWorld()->increaseScore(75);
}



/*                          Gold Class Function Implementations                   */

Gold::Gold(StudentWorld* myWorld, int startX, int startY, bool tunnelManCanPickUp) : Goodie(myWorld, TID_GOLD, startX, startY, tunnelManCanPickUp, !tunnelManCanPickUp){
    
    if(!tunnelManCanPickUp){
        setVisible(true);
        setTickLifeTime(100);
    }
}

char Gold::getGameID() const{
    return 'G';
}

void Gold::doSomethingToTunnelMan(){

    getWorld()->playSound(SOUND_GOT_GOODIE);
    getWorld()->getTunnelMan()->incrementNumNuggets();
    getWorld()->increaseScore(10);
    
}
void Gold::doSomethingToProtestor(){
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->bribeProtestor(getX(), getY(), 3);
    //The protestor class will handle the points allocated for successfully bribing a protestor
}



/*                          Squirt Class Implementation                                 */
Squirt::Squirt(StudentWorld* myWorld, int startX, int startY) : Actor(myWorld, TID_WATER_SPURT, startX, startY, myWorld->getTunnelMan()->getDirection(), 1.0, 1){
    m_travelDistance = 4; //All squirts start out with an initial travel distance of 4
    setVisible(true);
}

char Squirt::getGameID() const{
    return 'S';
}

void Squirt::doSomething(){
    
    if(getWorld()->squirtProtestorWithinRadius(getX(), getY(), 3)){
        setLiveStatus(false);
        return;
    }
    if(m_travelDistance <= 0){
        setLiveStatus(false);
        return;
    }

    if(getWorld()->willHitBoulderEdgeOrEarth(getX(), getY(),getDirection())){
        setLiveStatus(false);
        return;
    }

    move();
    m_travelDistance--;
}



/*                      Protestor Class function implementations:                       */

Protestor::Protestor(StudentWorld* myWorld, int imageID, int hitPoints) : Actor(myWorld, imageID, 60, 60, left, 1.0, 0){
    
    setVisible(true); //Protestors always start out as visible
    
    m_hitPoints = hitPoints;
    m_numSquaresToMoveInCurrentDirection = generateNumSquaresToMove();
    m_tickCount = 1000; //So that the protestor will do something on the first tick
    m_nonRestingTicksSinceShout = 1000; //So that the protestor will shout at the first opportunity
    m_nonRestingTicksSinceTurn = 0;
    m_leaveTheOilField = false;

}

void Protestor::doSomething(){
   if(!getLiveStatus())
       return;
    
    if(m_leaveTheOilField){
        if(getX() == 60 && getY() == 60){
            setLiveStatus(false);
        }
        else{
            changeDirectionToExit();
            move();
        }
        return;
    }
    
    //TODO: Fix this
    int ticksToWaitBetweenMoves = fmax(0, 3 - (getWorld()->getLevel()/4));
    if(m_tickCount < ticksToWaitBetweenMoves){
        m_tickCount++;
        return;
    }
    
    m_tickCount = 0;
    
    if(withinShoutingDistanceAndFacingTunnelMan()){
        if(m_nonRestingTicksSinceShout >= 15){
            getWorld()->getTunnelMan()->annoy(2);
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            m_nonRestingTicksSinceShout = 0;
        }
        else{
            m_nonRestingTicksSinceShout++;
        }
        m_nonRestingTicksSinceTurn++;
        return;
    }
    
    
    if(canMoveTowardTunnelMan()){
        Direction dOld = getDirection();
        changeDirectionToMoveTowardTunnelMan();
        Direction dNew = getDirection();
        if(justTurned90(dOld, dNew))
            m_nonRestingTicksSinceTurn = -1;

        m_numSquaresToMoveInCurrentDirection = 0;
        move();
        
        m_nonRestingTicksSinceTurn++;
        m_nonRestingTicksSinceShout++;
        return;
    }
    
    
    //The protestor must continue to move around the oil field randomly:
    m_numSquaresToMoveInCurrentDirection--;
    if(m_numSquaresToMoveInCurrentDirection <= 0){
        Direction oldD = getDirection();
        Direction newD;
        do{
            newD = generateRandomDirection();
            setDirection(newD);
        }while(!isViableDirection(newD));
        
        if(justTurned90(oldD, newD))
            m_nonRestingTicksSinceTurn = -1;
        
        m_numSquaresToMoveInCurrentDirection = generateNumSquaresToMove();
    }
    
    if(m_nonRestingTicksSinceTurn >= 200 && atIntersection()){
        m_numSquaresToMoveInCurrentDirection = generateNumSquaresToMove();
        
        Direction dOld = getDirection();
        Direction dNew;
        do{
            dNew = generateRandomDirection();
            setDirection(dNew);
        }while(!isViableDirection(dNew) || !justTurned90(dOld, dNew));
        
        m_nonRestingTicksSinceTurn = -1;
    }
    
    if(isViableDirection(getDirection()))
       move();
    else
       m_numSquaresToMoveInCurrentDirection = 0;
    
    m_nonRestingTicksSinceShout++;
    m_nonRestingTicksSinceTurn++;
}

void Protestor::annoy(int howMuch){
    const int squirtDamage = 2;
    const int boulderDamage = 100;
    
    m_hitPoints -= howMuch;
    if(m_hitPoints <= 0){
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_leaveTheOilField = true;
        if(howMuch == squirtDamage){
            if(getGameID() == 'p'){
                getWorld()->increaseScore(100);
            }
            else if(getGameID() == 'P'){
                getWorld()->increaseScore(250);
            }
        }
        else if(howMuch == boulderDamage)
            getWorld()->increaseScore(500);
        return;
    }
    
    //TODO: Fix
    m_tickCount = fmax(0, 3 - (getWorld()->getLevel()/4)) - fmax(50, 100 - getWorld()->getLevel()*10);
}

void Protestor::bribe(){
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    
    if(getGameID() == 'P'){
        getWorld()->increaseScore(50);
       
        //TODO: Fix
        m_tickCount = fmax(50, 100 - getWorld()->getLevel()*10);
    }
    else{
        getWorld()->increaseScore(25);
        m_leaveTheOilField = true;
    }
    
}

//Private Functions
int Protestor::generateNumSquaresToMove(){
    return rand() % 53 + 8;
}

bool Protestor::withinShoutingDistanceAndFacingTunnelMan(){
    
    int x = getX();
    int y = getY();

    if(!getWorld()->tunnelManWithinRadius(x, y, 4))
        return false;
    
    int tX = getWorld()->getTunnelMan()->getX();
    int tY = getWorld()->getTunnelMan()->getY();
    
    if(getDirection() == up && tY > y)
        return true;
    if(getDirection() == down && tY < y)
        return true;
    if(getDirection() == left && tX < x)
        return true;
    if(getDirection() == right && tX > x)
        return true;
    
    return false;
}

bool Protestor::isViableDirection(Direction d){
    return !getWorld()->willHitBoulderEdgeOrEarth(getX(), getY(), d);
}

bool Protestor::justTurned90(Direction d1, Direction d2){
    if(d1 == up || d1 == down){
        if(d2 == right || d2 == left)
            return true;
        else
            return false;
    }
    else{ //d1 equals left or right
        if(d2 == up || d2 == down)
            return true;
        else
            return false;
    }
}

bool Protestor::atIntersection(){
    Direction d = getDirection();
    if(d == up || d == down){
        if(!isViableDirection(left) && !isViableDirection(right))
            return false;
        else{
            return true;
        }
    }
    else{
        if(!isViableDirection(up) && !isViableDirection(down)){
            return false;
        }
        else{
            return true;
        }
    }
}

GraphObject::Direction Protestor::generateRandomDirection(){
    int i = rand() % 4;
    if(i == 0)
        return up;
    if(i == 1)
        return down;
    if(i == 2)
        return left;
    else
        return right;
        
}
bool Protestor::canMoveTowardTunnelMan(){
    int x = getX();
    int y = getY();
    int tX = getWorld()->getTunnelMan()->getX();
    int tY = getWorld()->getTunnelMan()->getY();
    
    if(x != tX && y != tY)
        return false;
    
    if(x == tX){
        if(y < tY){
            for(int i = 0; i < tY - y; i++){
                if(getWorld()->earthAt(x, y+i) || getWorld()->actorWillOverlapBoulder(x, y+i))
                    return false;
            }
        }
        else{
            for(int i = 0; i < y - tY - 1; i++){ //To account for if the tunnelman is digging
                if(getWorld()->earthAt(x, y-i) || getWorld()->actorWillOverlapBoulder(x, y-i))
                    return false;
            }
        }
    }
    if(y == tY){
        if(x < tX){
            for(int i = 0; i < tX - x; i++){
                if(getWorld()->earthAt(x+i, y) || getWorld()->actorWillOverlapBoulder(x+i, y))
                    return false;
            }
        }
        else{
            for(int i = 0; i < x - tX - 1; i++){ //to account for if the tunnelman is digging
                if(getWorld()->earthAt(x-i, y) || getWorld()->actorWillOverlapBoulder(x-i, y))
                    return false;
            }
        }
    }
    return true;
}


void Protestor::changeDirectionToExit(){
    setDirection(getWorld()->getDirectionToLocation(this, 60, 60));
}



/*                            RegularProtestor Class Function Implementations           */

RegularProtestor::RegularProtestor(StudentWorld* myWorld) : Protestor(myWorld, TID_PROTESTER, 5){}

char RegularProtestor::getGameID() const{
    return 'p';
}



void RegularProtestor::changeDirectionToMoveTowardTunnelMan(){
    faceTunnelMan();
}

void RegularProtestor::faceTunnelMan(){
    int x = getX();
    int y = getY();
    
    int tX = getWorld()->getTunnelMan()->getX();
    int tY = getWorld()->getTunnelMan()->getY();
    
    if(tX > x){
        setDirection(right);
    }
    if(tX < x){
        setDirection(left);
    }
    if(tY < y){
        setDirection(down);
    }
    if(tY > y){
        setDirection(up);
    }
}



/*                              HardcoreProtestor Class Implementation                          */
HardcoreProtestor::HardcoreProtestor(StudentWorld* myWorld) : Protestor(myWorld, TID_HARD_CORE_PROTESTER, 20){}

char HardcoreProtestor::getGameID() const{
    return 'P';
}

bool HardcoreProtestor::canMoveTowardTunnelMan(){
    int maxMovesToTunnelMan = 16 + getWorld()->getLevel()*2;
    return Protestor::canMoveTowardTunnelMan() || getWorld()->isXMovesAwayFromTunnelMan(this, maxMovesToTunnelMan);
}

void HardcoreProtestor::changeDirectionToMoveTowardTunnelMan(){
    setDirection(getWorld()->getDirectionToLocation(this, getWorld()->getTunnelMan()->getX(), getWorld()->getTunnelMan()->getY())); 
}



