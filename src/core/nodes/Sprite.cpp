#include "Sprite.h"

Sprite::Sprite(SDL_Texture* sprite, SPRITE_ORIGIN origin, float origin_distance_x, float origin_distance_y){
	m_class = "Sprite";
	set_unique_name("sprite_" + to_string(get_ID()));
	cout << "node with unique name " << get_unique_name() << " initialized" << endl;
	set_sprite(sprite, origin, origin_distance_x, origin_distance_y);
}

Sprite::Sprite(std::string path, SPRITE_ORIGIN origin, float origin_distance_x, float origin_distance_y){
	m_class = "Sprite";
	set_unique_name("sprite_" + to_string(get_ID()));
	cout << "node with unique name " << get_unique_name() << " initialized" << endl;
	set_sprite(path, origin, origin_distance_x, origin_distance_y);
}

Sprite::~Sprite(){
	delete m_sprite;
}


void Sprite::draw(){
	SDL_Rect dst;
	dst.x = (int)get_global_position().x;
	dst.y = (int)get_global_position().y;
	dst.w = (int)(get_size().x * get_scale());
	dst.h = (int)(get_size().y * get_scale());

	//SDL_RenderCopyEx(SDL_Data->get_renderer(), m_sprite, NULL, &dst, get_rotation(), NULL, SDL_FLIP_NONE);
}

void Sprite::update(const time_t& delta){

}

const SDL_Texture* Sprite::get_sprite(){
	return m_sprite;
}

void Sprite::set_sprite(SDL_Texture* sprite, SPRITE_ORIGIN origin, float origin_distance_x, float origin_distance_y){
	m_sprite = sprite;
	int x, y;
	SDL_QueryTexture(m_sprite, NULL, NULL, &x, &y);
	set_size((float)x, (float)y);

	// set the position depending on origin settings
	set_origin(origin);
	set_origin_distance(origin_distance_x, origin_distance_y);
}

void Sprite::set_sprite(std::string path, SPRITE_ORIGIN origin, float origin_distance_x, float origin_distance_y){

	SDL_Texture* text;
	//if((text = IMG_LoadTexture(SDL_Data->get_renderer(), path.c_str())) == NULL){
	//	std::cout << SDL_GetError() << std::endl;
	//}
	//set_sprite(text, origin, origin_distance_x, origin_distance_y);
}

const glm::vec2& Sprite::get_size() const{
	return m_size;
}

void Sprite::set_size(const glm::vec2& size){
	m_size = size;
	calc_origin();
}

void Sprite::set_size(int w, int h){
	m_size.x = w;
	m_size.y = h;
	calc_origin();
}

SPRITE_ORIGIN Sprite::get_origin(){
	return m_origin;
}
void Sprite::set_origin(SPRITE_ORIGIN origin){
	m_origin = origin;
	calc_origin();
}
glm::vec2 Sprite::get_origin_distance(){
	return m_origin_distance;
}
void Sprite::set_origin_distance(glm::vec2 distance){
	m_origin_distance = distance;
	calc_origin();
}
void Sprite::set_origin_distance(float x, float y){
	glm::vec2 p = glm::vec2();
	p.x = x;
	p.y = y;
	set_origin_distance(p);
}

void Sprite::calc_origin(){
	int x = get_size().x, y = get_size().y;
	// set new position based on size
	switch(m_origin){
		case SPRITE_ORIGIN::TOP_LEFT:
			set_position_2d(m_origin_distance);
			break;
		case SPRITE_ORIGIN::TOP:
			set_position_2d(-x / 2 + m_origin_distance.x, m_origin_distance.y);
			break;
		case SPRITE_ORIGIN::TOP_RIGHT:
			set_position_2d(-x + m_origin_distance.x, m_origin_distance.y);
			break;
		case SPRITE_ORIGIN::LEFT:
			set_position_2d(m_origin_distance.x, -y / 2 + m_origin_distance.y);
			break;
		case SPRITE_ORIGIN::CENTER:
			set_position_2d(-x / 2 + m_origin_distance.x, -y / 2 + m_origin_distance.y);
			break;
		case SPRITE_ORIGIN::RIGHT:
			set_position_2d(-x + m_origin_distance.x, -y / 2 + m_origin_distance.y);
			break;
		case SPRITE_ORIGIN::BOTTOM_LEFT:
			set_position_2d(m_origin_distance.x, -y + m_origin_distance.y);
			break;
		case SPRITE_ORIGIN::BOTTOM:
			set_position_2d(-x / 2 + m_origin_distance.x, -y + m_origin_distance.y);
			break;
		case SPRITE_ORIGIN::BOTTOM_RIGHT:
			set_position_2d(-x + m_origin_distance.x, -y + m_origin_distance.y);
			break;
		default:
			set_position_2d(m_origin_distance);
			break;
	}
}