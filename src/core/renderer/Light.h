#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <glm/glm.hpp>

namespace tse{

struct Light{
    // w = 0.0: direction vector
    // w = 1.0: position vector
    glm::vec4 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant = 1.0;
    float linear;
    float quadratic;
};

}



#endif // !__LIGHT_H__
