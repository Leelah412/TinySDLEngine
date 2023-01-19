#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <glm/glm.hpp>

namespace tse{

struct Light{
    // w = 0.0: direction vector
    // w = 1.0: position vector
    glm::vec4 direction;

    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;

    float constant = 1.0;
    float linear;
    float quadratic;
    float dummy = 0.0;
};

}



#endif // !__LIGHT_H__
