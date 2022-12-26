#include "CollisionBox.h"

CollisionBox::CollisionBox(){
	m_hitbox = {0,0,0,0};
}
CollisionBox::CollisionBox(SDL_FRect hitbox){
	set_hitbox(hitbox);
}
CollisionBox::CollisionBox(float x, float y, float w, float h){
	set_hitbox(x, y, w, h);
}
CollisionBox::~CollisionBox(){

}

SDL_FRect CollisionBox::get_hitbox(){
	return m_hitbox;
}
void CollisionBox::set_hitbox(SDL_FRect rect){
	m_hitbox = rect;
	// also assign the nodes in case someone decides to get the points instead of the box
	SDL_FPoint tl, tr, bl, br;
	tl.x = rect.x;			tl.y = rect.y;
	tr.x = rect.x + rect.w; tr.y = rect.y;
	bl.x = rect.x;			bl.y = rect.y + rect.h;
	br.x = rect.x + rect.w; br.y = rect.y + rect.h;
	m_polygon = {tl, tr, br, bl};
	m_aabb = rect;
}
void CollisionBox::set_hitbox(float x, float y, float w, float h){
	set_hitbox({x,y,w,h});
}

void CollisionBox::set_polygon(vector<SDL_FPoint> polygon){
	// only accept paths, that form an exactly rectangular shape
}