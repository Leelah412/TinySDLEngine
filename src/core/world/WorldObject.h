#ifndef __WORLD_OBJECT__
#define __WORLD_OBJECT__

#include <math/Vector.h>

class WorldObject{

public:
	WorldObject();
	virtual ~WorldObject();

	virtual Vector2 get_position();
	virtual float get_position_x();
	virtual float get_position_y();
	virtual void set_position(const Vector2& position);
	virtual void set_position(const float& x, const float& y);
	virtual void set_position_x(const float& x);
	virtual void set_position_y(const float& y);

	virtual float get_scale();
	virtual void set_scale(const float& scale);

	virtual float get_rotation();
	virtual void set_rotation(const float& rotation);

private:
	Vector2 m_position;
	float m_rotation;
	float m_scale;
	// TODO: add transform matrix
};

#endif // !__WORLD_OBJECT__
