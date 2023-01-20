#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <nodes/Node.h>
#include <string>
#include <iostream>

typedef enum class __SPRITE_ORIGIN__ {TOP_LEFT, TOP, TOP_RIGHT, LEFT, CENTER, RIGHT, BOTTOM_LEFT, BOTTOM, BOTTOM_RIGHT} SPRITE_ORIGIN;

REGISTER_NODE(Sprite)

class Sprite : public Node{

public:
	Sprite();
	Sprite(SDL_Texture* sprite, SPRITE_ORIGIN origin = SPRITE_ORIGIN::TOP_LEFT, float origin_distance_x = 0, float origin_distance_y = 0);
	Sprite(std::string path, SPRITE_ORIGIN origin = SPRITE_ORIGIN::TOP_LEFT, float origin_distance_x = 0, float origin_distance_y = 0);
	virtual ~Sprite();

	virtual void draw() override;
	virtual void update(const time_t& delta) override;

	const SDL_Texture* get_sprite();
	void set_sprite(SDL_Texture* sprite, SPRITE_ORIGIN origin = SPRITE_ORIGIN::TOP_LEFT, float origin_distance_x = 0, float origin_distance_y = 0);
	void set_sprite(std::string path, SPRITE_ORIGIN origin = SPRITE_ORIGIN::TOP_LEFT, float origin_distance_x = 0, float origin_distance_y = 0);

	virtual const glm::vec2& get_size() const;
	virtual void set_size(const glm::vec2& size);
	virtual void set_size(int w, int h);

	SPRITE_ORIGIN get_origin();
	void set_origin(SPRITE_ORIGIN origin);
	glm::vec2 get_origin_distance();
	void set_origin_distance(glm::vec2 distance);
	void set_origin_distance(float x = 0, float y = 0);

private:
	void calc_origin();					// helper function to recalculate origin

	SDL_Texture* m_sprite;				// Sprite texture
	glm::vec2 m_size = glm::vec2();		// Sprite size

	SPRITE_ORIGIN m_origin;				// Sprite origin of the sprite
	glm::vec2 m_origin_distance;		// Set origin manually relative to "m_origin"
};


#endif