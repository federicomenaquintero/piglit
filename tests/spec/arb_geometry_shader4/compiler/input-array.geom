// [config]
// expect_result: pass
// glsl_version: 1.10
// check_link: true
// require_extensions: GL_ARB_geometry_shader4
// [end config]

#version 110
#extension GL_ARB_geometry_shader4: enable

varying in vec4 foo[3];

void main()
{
}
