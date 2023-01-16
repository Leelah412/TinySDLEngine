#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

namespace tse{

class Framebuffer{

public:
	Framebuffer();
	virtual ~Framebuffer();

	virtual void bind() const;
	virtual void unbind() const;

private:


};

}


#endif // !__FRAMEBUFFER_H__
