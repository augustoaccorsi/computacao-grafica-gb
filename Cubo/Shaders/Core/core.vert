#version 410 core

layout(location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
out vec2 TexCoord;
out vec3 ourNormal;
out vec3 eye_position, eye_normal;
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
void main() {
   eye_position = vec3(ViewMatrix * ModelMatrix * vec4(aPos, 1.0));
   eye_normal = vec3(ViewMatrix * ModelMatrix * vec4(aNormal, 0.0));
   TexCoord = aTexCoord;
   ourNormal = mat3(ModelMatrix) * aNormal;
   gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(aPos, 1.f);
}