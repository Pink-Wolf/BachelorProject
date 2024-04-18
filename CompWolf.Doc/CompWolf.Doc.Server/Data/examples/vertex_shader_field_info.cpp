#include <dimensions> // float2
#include <shaders> // shader_field_info, combine_shader_fields
#include <utility> // offsetof

struct vertex
{
    float2 position;
    float2 uv;
};
template<> struct shader_field_info<vertex> : public combine_shader_fields<
    type_value_pair<float2, offsetof(vertex, position)>,
    type_value_pair<float2, offsetof(vertex, uv)>
> {};