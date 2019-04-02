/*
Copyright(c) 2016 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

#include "CommonShader"
#include "CommonVertex"

layout (location = 0) in vec4 position;
layout (location = 30) in uvec4 attr1; //color
layout (location = 11) in vec4 t0;

layout (location = 0) out vec4 diffuseColor;
layout (location = 1) out vec4 specularColor;
layout (location = 2) out vec4 globalIllumination;
layout (location = 3) out vec2 texcoord1;

out gl_PerVertex 
{
        vec4 gl_Position;
};

void main() 
{	
	float x = 0;
	float y = 0;
	
	if(position.x > 0)
	{
		x = (position.x / (pushConstants.screenWidth/2));
	}
	else
	{
		x = (position.x);
	}
	
	if(position.y > 0)
	{
		y = (position.y / (pushConstants.screenHeight/2));
	}
	else
	{
		y = (position.y);
	}
	
	gl_Position = vec4((x-1),(y-1),0.0,1.0);

	texcoord1 = t0.xy;

	ColorPair color = CalculateGlobalIllumination(position, vec4(0.0), Convert(attr1), vec4(0.0));

	diffuseColor = color.Diffuse;
	specularColor = color.Specular;

	globalIllumination = color.GlobalIllumination;
}