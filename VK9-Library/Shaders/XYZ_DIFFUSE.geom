/*
Copyright(c) 2018 Christopher Joseph Dean Schaefer

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

layout (points) in;
layout(triangle_strip, max_vertices = 6) out;

layout (location = 0) in vec4 indiffuseColor[];
layout (location = 1) in vec4 inSpecularColor[];
layout (location = 2) in vec4 inGlobalIllumination[];

layout (location = 0) out vec4 outdiffuseColor;
layout (location = 1) out vec4 outSpecularColor;
layout (location = 2) out vec4 outGlobalIllumination;
layout (location = 3) out vec2 texcoord;

#include "CommonShader"

void main() 
{	
		vec4 position = gl_in[0].gl_Position;
		float calculatedPointSize = renderState.pointSize;

		if(renderState.pointScaleEnable==1)
		{
			float d = sqrt(pow(position.x,2) + pow(position.y,2) + pow(position.z,2));
			calculatedPointSize = 1 * calculatedPointSize * sqrt(1/(renderState.pointScaleA + renderState.pointScaleB * d + renderState.pointScaleC * pow(d,2)));
		}	

		calculatedPointSize = clamp(calculatedPointSize,renderState.pointSizeMinimum,renderState.pointSizeMaximum);

		gl_Position = vec4(position.x - calculatedPointSize/2, position.y + calculatedPointSize/2, position.z, position.w);
		outdiffuseColor = indiffuseColor[0];
		outSpecularColor = inSpecularColor[0];
		outGlobalIllumination = inGlobalIllumination[0];
		texcoord = vec2( 0.0, 1.0 ); 
		EmitVertex();

		gl_Position = vec4(position.x - calculatedPointSize/2, position.y - calculatedPointSize/2, position.z, position.w);
		outdiffuseColor = indiffuseColor[0];
		outSpecularColor = inSpecularColor[0];
		outGlobalIllumination = inGlobalIllumination[0];
		texcoord = vec2( 0.0, 0.0 ); 
		EmitVertex();

		gl_Position = vec4(position.x + calculatedPointSize/2, position.y - calculatedPointSize/2, position.z, position.w);
		outdiffuseColor = indiffuseColor[0];
		outSpecularColor = inSpecularColor[0];
		outGlobalIllumination = inGlobalIllumination[0];
		texcoord = vec2( 1.0, 0.0 ); 
		EmitVertex();

		EndPrimitive();



		gl_Position = vec4(position.x + calculatedPointSize/2, position.y - calculatedPointSize/2, position.z, position.w);
		outdiffuseColor = indiffuseColor[0];
		outSpecularColor = inSpecularColor[0];
		outGlobalIllumination = inGlobalIllumination[0];
		texcoord = vec2( 1.0, 0.0 ); 
		EmitVertex();

		gl_Position = vec4(position.x + calculatedPointSize/2, position.y + calculatedPointSize/2, position.z, position.w);
		outdiffuseColor = indiffuseColor[0];
		outSpecularColor = inSpecularColor[0];
		outGlobalIllumination = inGlobalIllumination[0];
		texcoord = vec2( 1.0, 1.0 );
		EmitVertex();

		gl_Position = vec4(position.x - calculatedPointSize/2, position.y + calculatedPointSize/2, position.z, position.w);
		outdiffuseColor = indiffuseColor[0];
		outSpecularColor = inSpecularColor[0];
		outGlobalIllumination = inGlobalIllumination[0];
		texcoord = vec2( 0.0, 1.0 ); 
		EmitVertex();

		EndPrimitive();
}
