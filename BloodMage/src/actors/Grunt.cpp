#include "Grunt.h"


Grunt::Grunt(fk::SpriteBatch* sbPtr, b2World* wPtr, fk::ActorDef& ad) : Actor(sbPtr, wPtr, ad) {

}
Grunt::~Grunt() {

}
void Grunt::think(std::vector<fk::Actor*>& actorPtrs, fk::Camera* camPtr) {

}
void Grunt::updateBody() {

}
void Grunt::updateSprite() {
	(*p_sbPtr)[p_spriteIDs[0]].setPosition(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[0]].setRotationAxis(p_bodyPtr->GetPosition().x, p_bodyPtr->GetPosition().y);
	(*p_sbPtr)[p_spriteIDs[0]].canvas.rotationAngle = p_bodyPtr->GetAngle();
}