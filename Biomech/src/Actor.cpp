#include "Actor.h"

Actor::Actor(fk::SpriteBatch& sb, fk::World& world) :
	fk::Body(world, b2_dynamicBody), p_spriteBatch(sb)
{
	
}
