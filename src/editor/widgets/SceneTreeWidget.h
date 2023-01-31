#ifndef __EDITOR_SCENE_TREE_WIDGET_H__
#define __EDITOR_SCENE_TREE_WIDGET_H__

#include "Widget.h"
#include "nodes/Node.h"

namespace tse{

class SceneTreeWidget : public Widget{

public:
	SceneTreeWidget();
	virtual ~SceneTreeWidget();

	void set_root(Node* root);
	void update_widget();

protected:
	virtual void draw() override;

private:
	Node* m_root;
};

}

#endif // !__EDITOR_SCENE_TREE_H__
