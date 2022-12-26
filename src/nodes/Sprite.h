#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <nodes/Node.h>
#include <string>
#include <iostream>

typedef enum class __SPRITE_ORIGIN__ {TOP_LEFT, TOP, TOP_RIGHT, LEFT, CENTER, RIGHT, BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT} SPRITE_ORIGIN;

class Sprite : public Node{

public:
	Sprite(SDL_Texture* sprite, SPRITE_ORIGIN origin = SPRITE_ORIGIN::TOP_LEFT, float origin_distance_x = 0, float origin_distance_y = 0);
	Sprite(std::string path, SPRITE_ORIGIN origin = SPRITE_ORIGIN::TOP_LEFT, float origin_distance_x = 0, float origin_distance_y = 0);
	virtual ~Sprite();

	virtual void draw();
	virtual void update(const time_t& delta);

	const SDL_Texture* get_sprite();
	void set_sprite(SDL_Texture* sprite, SPRITE_ORIGIN origin = SPRITE_ORIGIN::TOP_LEFT, float origin_distance_x = 0, float origin_distance_y = 0);
	void set_sprite(std::string path, SPRITE_ORIGIN origin = SPRITE_ORIGIN::TOP_LEFT, float origin_distance_x = 0, float origin_distance_y = 0);

	void set_size(const SDL_FPoint& size) override;
	void set_size(const int& w, const int& h) override;

	SPRITE_ORIGIN get_origin();
	void set_origin(SPRITE_ORIGIN origin);
	SDL_FPoint get_origin_distance();
	void set_origin_distance(SDL_FPoint distance);
	void set_origin_distance(float x = 0, float y = 0);

private:
	void calc_origin();				// helper function to recalculate origin

	SDL_Texture* m_sprite;			// the sprite texture

	SPRITE_ORIGIN m_origin;			// the origin of the sprite
	SDL_FPoint m_origin_distance;	// set origin manually relative to the origin set in "m_origin"
};


#endif