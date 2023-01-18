#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

#define TSE_VERTEX_ARRAY_WORLD "world"
#define TSE_DEFAULT_SHADER_NAME "default"
#define TSE_DEFAULT_SHADER_PATH "default_shader.glsl"
#define TSE_DEFAULT_SHADER_MVP_UNIFORM "U_VIEW"
#define TSE_LIGHT_UNIFORM_BUFFER "U_LIGHTS"

template<typename T>
using Ref = std::reference_wrapper<T>;

#endif // !__DEFAULTS_H__
