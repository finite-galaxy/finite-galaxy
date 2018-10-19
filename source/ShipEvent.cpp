// ShipEvent.cpp

#include "ShipEvent.h"

#include "Ship.h"

using namespace std;



ShipEvent::ShipEvent(const Government *actor, const shared_ptr<Ship> &target, int type)
	: actorGovernment(actor), target(target), type(type)
{
	if(target)
		targetGovernment = target->GetGovernment();
}



ShipEvent::ShipEvent(const shared_ptr<Ship> &actor, const shared_ptr<Ship> &target, int type)
	: actor(actor), target(target), type(type)
{
	if(actor)
		actorGovernment = actor->GetGovernment();
	if(target)
		targetGovernment = target->GetGovernment();
}



const shared_ptr<Ship> &ShipEvent::Actor() const
{
	return actor;
}



const Government *ShipEvent::ActorGovernment() const
{
	return actorGovernment;
}



const shared_ptr<Ship> &ShipEvent::Target() const
{
	return target;
}



const Government *ShipEvent::TargetGovernment() const
{
	return targetGovernment;
}




int ShipEvent::Type() const
{
	return type;
}
