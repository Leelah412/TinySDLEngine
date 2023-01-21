#ifndef __LIGHT_NODE_H__
#define __LIGHT_NODE_H__

#include "Node.h"
#include "SceneLoader.h"
#include "renderer/RenderManager.h"
#include "renderer/Light.h"

USE_RENDER_MANAGER

namespace tse{

REGISTER_NODE(LightNode)

class LightNode : public Node{

public:
	LightNode();
	LightNode(Light* light);
	LightNode(const Light& light);
	virtual ~LightNode();

	virtual void set_light(Light* light);
	virtual void set_light(const Light& light);

	virtual void update_global_position() override;
	virtual void update_global_rotation() override;

	// Turn the light on
	virtual void turn_on();
	// Turn the light off
	virtual void turn_off();

private:
	Light* m_light = nullptr;
	
};

}


#endif // !__LIGHT_NODE_H__
