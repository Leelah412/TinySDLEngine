#include "Widget.h"
#include <util/random_string.h>

namespace tse{

Widget::Widget(int x, int y, int w, int h)
	: m_x(x), m_y(y), m_w(w), m_h(h){
	// the problem with making Widget inher from Node is, that it might be possible, that someone might want to use a name for an in-app Node,
	// which is already assigned to an Editor widget; to minimize the possibility of such a thing ever happening,
	// set unique name as widget + node ID + some random string of length 32, that no one would ever dare to use
	set_unique_name("widget_" + get_ID() + random_string(32));
}

Widget::~Widget(){}

void Widget::render(){
	if(!m_show) return;
	draw();
}

void Widget::show_widget(bool show){
	m_show = show;
}

bool Widget::is_visible(){
	return m_show;
}

}
