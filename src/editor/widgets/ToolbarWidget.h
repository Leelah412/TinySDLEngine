#ifndef __EDITOR_TOOLBAR_WIDGET_H__
#define __EDITOR_TOOLBAR_WIDGET_H__

#include "Widget.h"

namespace tse{

class ToolbarWidget : public Widget{

public:
	ToolbarWidget();
	virtual ~ToolbarWidget();

protected:
	virtual void draw() override;

private:

};

}


#endif // !__EDITOR_TOOLBAR_WIDGET_H__
