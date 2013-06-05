#include "BehaviourStateLogic.h"

BehaviourStateLogic* BehaviourStateLogic::getInstance() {
    //if (!Instance) {
    static BehaviourStateLogic* Instance = new BehaviourStateLogic();
    //}
    return Instance;
}


void BehaviourStateLogic::checkDistanceToBall() {
    states[IS_CLOSEST_TO_BALL] = Blackboard->TeamInfo->howManyCloserToBall() == 0;
    states[IS_SECOND_FROM_BALL] = Blackboard->TeamInfo->howManyCloserToBall() == 1;
    states[IS_FURTHEST_FROM_BALL] = Blackboard->TeamInfo->howManyCloserToBall() == 2;
}
    
void BehaviourStateLogic::checkVisibilityOfBall() {
    states[BALL_IS_SEEN] = Blackboard->Objects->mobileFieldObjects[FieldObjects::FO_BALL].TimeSinceLastSeen() < 90.0; //XXX: timing hack, magic number
    
    //use this for seeing the ball as a team
    vector<TeamPacket::SharedBall> balls = Blackboard->TeamInfo->getSharedBalls();
    states[TEAM_SEES_BALL] = false;
    for (int i = 0; i < balls.size(); i++) {
        states[TEAM_SEES_BALL] = states[TEAM_SEES_BALL] or balls[i].TimeSinceLastSeen < 90.0; //XXX: timing hack, magic number
    }
    
    states[BALL_IS_LOST] = Blackboard->Objects->mobileFieldObjects[FieldObjects::FO_BALL].lost();
}

void BehaviourStateLogic::checkGameState() {
    if (!states[GAME_STATE_PENALISED] and Blackboard->GameInfo->getCurrentState()) {
        timeUnPenalised = Blackboard->Sensors->GetTimeStamp();
    }
    states[GAME_STATE_PENALISED] = Blackboard->GameInfo->getCurrentState() == GameInformation::PenalisedState;
    states[GAME_STATE_POSITIONING] = Blackboard->GameInfo->getCurrentState() == GameInformation::SetState;
    states[GAME_STATE_READY] = Blackboard->GameInfo->getCurrentState() == GameInformation::ReadyState;
    
    //manage time since we've resumed play
    if (states[GAME_STATE_READY] and Blackboard->GameInfo->getCurrentState() == GameInformation::PlayingState) {
        timeGameStarted = Blackboard->Sensors->GetTimeStamp();
    }
    states[GAME_STATE_PLAYING] = Blackboard->GameInfo->getCurrentState() == GameInformation::PlayingState;
    states[GAME_STATE_KICKOFF] = Blackboard->GameInfo->haveKickOff();
    states[GAME_STATE_KICKING_OFF] = Blackboard->GameInfo->haveKickOff() and timeSinceGameStarted() < 15000.0; //XXX: timing hack, magic number
    states[GAME_STATE_END] = Blackboard->GameInfo->getCurrentState() == GameInformation::Finished;
}

void BehaviourStateLogic::checkMyMovement() {
    
    //XXX: not sure if in_position needs help
    Blackboard->Sensors->get(NUSensorsData::MotionKickActive, states[IS_KICKING]);
    states[IS_APPROACHING_BALL] =  Navigation->getInstance()->getCurrentCommand() == Navigation::GOTOOBJECT;
    states[IS_IN_POSITION] = Navigation->getInstance()->getCurrentCommand() == Navigation::GOTOPOINT and
                             Navigation->getInstance()->isStopped();
    states[IS_GOAL_KEEPER] = Blackboard->GameInfo->getPlayerNumber() == m_GoalKeeper;
}

void BehaviourStateLogic::checkFallen() {
    //manage time since falling/getting up in a state-based manner
    
    if (!states[IS_FALLEN_OVER] and Blackboard->Sensors->isFallen()) {
        timeFallen = Blackboard->Sensors->GetTimeStamp();
    } else if (states[IS_FALLEN_OVER] and !Blackboard->Sensors->isFallen()) {
        timeGetup = Blackboard->Sensors->GetTimeStamp();
    }
    
    //set all sensors
    states[IS_FALLEN_OVER] = Blackboard->Sensors->isFallen();
    states[IS_GETTING_UP] = timeSinceFallen() < 7000.0; //XXX: timing hack, magic number
    states[JUST_GOT_UP] = !states[IS_GETTING_UP] and timeSinceGetup < 10000.0; //XXX: timing hack, magic number
    
    states[IS_PICKED_UP] = false; //XXX: check with steve how to set this
}

void BehaviourStateLogic::update() {
    checkDistanceToBall();
    checkVisibilityOfBall();
    checkGameState();
    checkMyMovement();
    checkFallen();
}


