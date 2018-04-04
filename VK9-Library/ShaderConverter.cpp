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

#include <vulkan/vk_sdk_platform.h>
#include "d3d9.h"
#include "D3D11Shader.h"
#include "ShaderConverter.h"
#include <boost/log/trivial.hpp>
#include <boost/foreach.hpp>
#include <fstream>

#include "CDevice9.h"
#include "Utilities.h"

/*
http://timjones.io/blog/archive/2015/09/02/parsing-direct3d-shader-bytecode
https://msdn.microsoft.com/en-us/library/bb219840(VS.85).aspx#Shader_Binary_Format
http://stackoverflow.com/questions/2545704/format-of-compiled-directx9-shader-files
https://msdn.microsoft.com/en-us/library/windows/hardware/ff552891(v=vs.85).aspx
https://github.com/ValveSoftware/ToGL
https://www.khronos.org/registry/spir-v/specs/1.2/SPIRV.html
*/

const uint32_t mEndToken = 0x0000FFFF;
const uint16_t mShaderTypePixel = 0xFFFF;
const uint16_t mShaderTypeVertex = 0xFFFE;

#define PACK(c0, c1, c2, c3) \
    (((uint32_t)(uint8_t)(c0) << 24) | \
    ((uint32_t)(uint8_t)(c1) << 16) | \
    ((uint32_t)(uint8_t)(c2) << 8) | \
    ((uint32_t)(uint8_t)(c3)))

/*
Generator's magic number. It is associated with the tool that generated
the module. Its value does not affect any semantics, and is allowed to be 0.
Using a non-0 value is encouraged, and can be registered with
Khronos at https://www.khronos.org/registry/spir-v/api/spir-v.xml.
*/
#define SPIR_V_GENERATORS_NUMBER 0x00000000

ShaderConverter::ShaderConverter(vk::Device& device, ShaderConstantSlots& shaderConstantSlots)
	: mDevice(device), mShaderConstantSlots(shaderConstantSlots)
{

}

ShaderConverter::~ShaderConverter()
{
	if (mConvertedShader.ShaderModule != VK_NULL_HANDLE)
	{
		mDevice.destroyShaderModule(mConvertedShader.ShaderModule, NULL);
		mConvertedShader.ShaderModule = VK_NULL_HANDLE;
	}
}

Token ShaderConverter::GetNextToken()
{
	Token token;

	mPreviousToken++;
	token.i = *(mNextToken++);

	return token;
}

void ShaderConverter::SkipTokens(uint32_t numberToSkip)
{
	mNextToken += numberToSkip;
}

uint32_t ShaderConverter::GetNextId()
{
	return mNextId++;
}

void ShaderConverter::SkipIds(uint32_t numberToSkip)
{
	mNextId += numberToSkip;
}

uint32_t ShaderConverter::GetSpirVTypeId(spv::Op registerType, uint32_t id)
{
	TypeDescription description;

	description.PrimaryType = registerType;
	description.StorageClass = spv::StorageClassInput;

	return GetSpirVTypeId(description, id);
}

uint32_t ShaderConverter::GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2)
{
	TypeDescription description;

	description.PrimaryType = registerType1;
	description.SecondaryType = registerType2;
	description.StorageClass = spv::StorageClassInput;

	return GetSpirVTypeId(description);
}

uint32_t ShaderConverter::GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2, uint32_t componentCount)
{
	TypeDescription description;

	description.PrimaryType = registerType1;
	description.SecondaryType = registerType2;
	description.ComponentCount = componentCount;
	description.StorageClass = spv::StorageClassInput;

	return GetSpirVTypeId(description);
}

uint32_t ShaderConverter::GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2, spv::Op registerType3, uint32_t componentCount)
{
	TypeDescription description;

	description.PrimaryType = registerType1;
	description.SecondaryType = registerType2;
	description.TernaryType = registerType3;
	description.ComponentCount = componentCount;
	description.StorageClass = spv::StorageClassInput;

	return GetSpirVTypeId(description);
}

uint32_t ShaderConverter::GetSpirVTypeId(TypeDescription& registerType, uint32_t id)
{
	uint32_t columnTypeId = 0;
	uint32_t pointerTypeId = 0;
	uint32_t returnTypeId = 0;
	uint32_t sampledTypeId = 0;
	uint32_t id2 = 0;

	for (const auto& type : mTypeIdPairs)
	{
		if (type.first == registerType)
		{
			return type.second;
		}
	}

	if (id == UINT_MAX)
	{
		id = GetNextId();
	}

	mTypeIdPairs[registerType] = id;
	mIdTypePairs[id] = registerType;

	switch (registerType.PrimaryType)
	{
	case spv::OpTypeBool:
		mTypeInstructions.push_back(Pack(2, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		break;
	case spv::OpTypeInt:
		mTypeInstructions.push_back(Pack(4, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(32); //Number of bits.
		mTypeInstructions.push_back(0); //Signedness (0 = unsigned,1 = signed)
		break;
	case spv::OpTypeFloat:
		mTypeInstructions.push_back(Pack(3, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(32); //Number of bits.
		break;
	case spv::OpTypeVector:
		//Matrix and Vector type opcodes are laid out the same but exchange component for column.
	case spv::OpTypeMatrix:
		columnTypeId = GetSpirVTypeId(registerType.SecondaryType);

		mTypeInstructions.push_back(Pack(4, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(columnTypeId); //Component/Column Type
		mTypeInstructions.push_back(registerType.ComponentCount);
		break;
	case spv::OpTypePointer:
		pointerTypeId = GetSpirVTypeId(registerType.SecondaryType, registerType.TernaryType, registerType.ComponentCount);

		mTypeInstructions.push_back(Pack(4, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		if (registerType.SecondaryType == spv::OpTypeSampledImage || registerType.SecondaryType == spv::OpTypeImage)
		{
			mTypeInstructions.push_back(spv::StorageClassUniformConstant); //Storage Class
		}
		else
		{
			mTypeInstructions.push_back(registerType.StorageClass); //Storage Class
		}
		mTypeInstructions.push_back(pointerTypeId); // Type
		break;
	case spv::OpTypeSampler:
		mTypeInstructions.push_back(Pack(2, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		break;
	case spv::OpTypeSampledImage:
	case spv::OpTypeImage:
		id2 = id;
		id = GetNextId();

		sampledTypeId = GetSpirVTypeId(spv::OpTypeFloat);
		mTypeInstructions.push_back(Pack(9, spv::OpTypeImage)); //size,Type
		mTypeInstructions.push_back(id2); //Result (Id)
		mTypeInstructions.push_back(sampledTypeId); //Sampled Type (Id)
		mTypeInstructions.push_back(spv::Dim2D); //dimensionality
		mTypeInstructions.push_back(0); //Depth
		mTypeInstructions.push_back(0); //Arrayed
		mTypeInstructions.push_back(0); //MS
		mTypeInstructions.push_back(1); //Sampled
		mTypeInstructions.push_back(spv::ImageFormatUnknown); //Sampled

		mTypeInstructions.push_back(Pack(3, spv::OpTypeSampledImage)); //size,Type
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(id2); //Type (Id)
		break;
	case spv::OpTypeFunction:
	{
		returnTypeId = GetSpirVTypeId(registerType.SecondaryType);

		mTypeInstructions.push_back(Pack(3 + registerType.Arguments.size(), registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id
		mTypeInstructions.push_back(returnTypeId); //Return Type (Id)

		//Right now there is no comparison on arguments so we are assuming that functions with the same return type are the same. This will need to be expanded later when we're using functions other than the default entry point.
		for (size_t i = 0; i < registerType.Arguments.size(); i++)
		{
			mTypeInstructions.push_back(registerType.Arguments[i]); //Argument Id
		}
	}
	break;
	case spv::OpTypeVoid:
		mTypeInstructions.push_back(Pack(2, registerType.PrimaryType)); //size,Type
		mTypeInstructions.push_back(id); //Id		
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "GetSpirVTypeId - Unsupported data type " << registerType.PrimaryType;
		break;
	}

	mTypeIdPairs[registerType] = id;
	mIdTypePairs[id] = registerType;

	return id;
}

/*
SPIR-V is SSA so this method will generate a new id with the type of the old one when a new "register" is needed.
To handle this result registers will get a new Id each type. The result Id can be used as an input to other operations so this will work fine.
To make sure each call gets the latest id number the lookups must be updated.
*/
uint32_t ShaderConverter::GetNextVersionId(const Token& token)
{
	uint32_t id = GetNextId();

	SetIdByRegister(token, id);

	return id;
}

uint32_t ShaderConverter::GetIdByRegister(const Token& token, _D3DSHADER_PARAM_REGISTER_TYPE type, _D3DDECLUSAGE usage)
{
	D3DSHADER_PARAM_REGISTER_TYPE registerType;
	uint32_t registerNumber = 0;
	TypeDescription description;
	uint32_t id = 0;
	uint32_t typeId = 0;
	std::string registerName;
	size_t stringWordSize;

	if (type == D3DSPR_FORCE_DWORD)
	{
		registerType = GetRegisterType(token.i);
	}
	else
	{
		registerType = type;
	}

	switch (registerType)
	{
	case D3DSPR_CONST2:
		registerNumber = GetRegisterNumber(token) + 2048;
		break;
	case D3DSPR_CONST3:
		registerNumber = GetRegisterNumber(token) + 4096;
		break;
	case D3DSPR_CONST4:
		registerNumber = GetRegisterNumber(token) + 6144;
		break;
	case D3DSPR_TEXTURE:
		registerNumber = GetRegisterNumber(token);

		if (mTextures[registerNumber])
		{
			mTextures[registerNumber];
		}

		break;
	default:
		registerNumber = GetRegisterNumber(token);
		break;
	}

	//If a register has already be declared just return the id.
	boost::container::flat_map<D3DSHADER_PARAM_REGISTER_TYPE, boost::container::flat_map<uint32_t, uint32_t> >::iterator it1 = mIdsByRegister.find(registerType);
	if (it1 != mIdsByRegister.end())
	{
		boost::container::flat_map<uint32_t, uint32_t>::iterator it2 = it1->second.find(registerNumber);
		if (it2 != it1->second.end())
		{
			return it2->second;
		}
	}

	/*
	Registers can be defined simply by using them so anything past this point was used before it was declared.
	We'll be missing some usage information and some other bits so we'll try to piece together what it should be from context. (also guess some stuff)

	The rules are different between ps and vs and even between shader models 1, 2, and 3.
	*/

	switch (registerType)
	{
	case D3DSPR_TEXTURE: //Texture could be texcoord
	case D3DSPR_INPUT:
		id = GetNextId();
		description.PrimaryType = spv::OpTypePointer;
		description.SecondaryType = spv::OpTypeVector;

		//TODO: find a way to tell if this is an integer or float.
		if (usage == D3DDECLUSAGE_COLOR)
		{
			description.TernaryType = spv::OpTypeInt; //Hint says this is color so it should be a single DWORD that Vulkan splits into a uvec4.
		}
		else
		{
			description.TernaryType = spv::OpTypeFloat;
		}
		description.ComponentCount = 4;
		description.StorageClass = spv::StorageClassInput;
		typeId = GetSpirVTypeId(description);

		mIdsByRegister[registerType][registerNumber] = id;
		mRegistersById[registerType][id] = registerNumber;
		mIdTypePairs[id] = description;

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(typeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassInput); //Storage Class

		mInputRegisters.push_back(id);

		if (this->mMajorVersion == 3)
		{
			registerName = "v" + std::to_string(registerNumber);
			stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
			if (registerName.length() % 4 == 0)
			{
				stringWordSize++;
			}
			mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
			mNameInstructions.push_back(id); //target (Id)
			PutStringInVector(registerName, mNameInstructions); //Literal

			GenerateDecoration(registerNumber, id, usage, true);
		}
		else
		{
			if (registerType == D3DSPR_INPUT)
			{
				registerName = "oD" + std::to_string(registerNumber);
				stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
				if (registerName.length() % 4 == 0)
				{
					stringWordSize++;
				}
				mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
				mNameInstructions.push_back(id); //target (Id)
				PutStringInVector(registerName, mNameInstructions); //Literal

				GenerateDecoration(registerNumber, id, D3DDECLUSAGE_COLOR, true);
			}
			else
			{
				registerName = "oT" + std::to_string(registerNumber);
				stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
				if (registerName.length() % 4 == 0)
				{
					stringWordSize++;
				}
				mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
				mNameInstructions.push_back(id); //target (Id)
				PutStringInVector(registerName, mNameInstructions); //Literal

				GenerateDecoration(registerNumber, id, D3DDECLUSAGE_TEXCOORD, true);
			}
		}

		if (this->mIsVertexShader)
		{
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].binding = 0;//element.Stream; TODO:REVISIT
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].location = registerNumber;  //mConvertedShader.mVertexInputAttributeDescriptionCount;
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].offset = 0;//element.Offset;
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32G32B32A32Sfloat;
			mConvertedShader.mVertexInputAttributeDescriptionCount++;
		}

		break;
	case D3DSPR_RASTOUT:
	case D3DSPR_ATTROUT:
	case D3DSPR_COLOROUT:
	case D3DSPR_DEPTHOUT:
	case D3DSPR_OUTPUT:
	case D3DSPR_TEMP:
		/*
		D3DSPR_TEMP is included with the outputs because for pixel shaders r0 is the color output.
		So rather than duplicate everything I put some logic here and there to decide if it's an output or a temp.
		*/

		id = GetNextId();
		description.PrimaryType = spv::OpTypePointer;
		description.SecondaryType = spv::OpTypeVector;
		description.TernaryType = spv::OpTypeFloat; //TODO: find a way to tell if this is an integer or float.
		description.ComponentCount = 4;
		description.StorageClass = spv::StorageClassOutput;
		typeId = GetSpirVTypeId(description);

		mIdsByRegister[registerType][registerNumber] = id;
		mRegistersById[registerType][id] = registerNumber;
		mIdTypePairs[id] = description;

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(typeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(id); //Result (Id)

		if (this->mIsVertexShader && registerType != D3DSPR_TEMP)
		{
			mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class
		}
		else if (!this->mIsVertexShader && registerType == D3DSPR_TEMP) //r0 is used for pixel shader color output because reasons.
		{
			mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class
		}
		else
		{
			mTypeInstructions.push_back(spv::StorageClassPrivate); //Storage Class
		}

		/*
			D3DDECLUSAGE_FOG:
			registerName = "oFog" + std::to_string(registerNumber);
			break;

			D3DDECLUSAGE_PSIZE:
			registerName = "oPts" + std::to_string(registerNumber);
			break;
		*/

		switch (registerType)
		{
		case D3DSPR_RASTOUT:
			mPositionId = id;
			registerName = "oPos" + std::to_string(registerNumber);
			usage = D3DDECLUSAGE_POSITION;
			break;
		case D3DSPR_ATTROUT:
		case D3DSPR_COLOROUT:

			if (registerNumber)
			{
				mColor2Id = id;

				TypeDescription pointerFloatType;
				pointerFloatType.PrimaryType = spv::OpTypePointer;
				pointerFloatType.SecondaryType = spv::OpTypeFloat;
				pointerFloatType.StorageClass = spv::StorageClassOutput;
				uint32_t floatTypeId = GetSpirVTypeId(pointerFloatType);

				mColor2XId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor2XId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor2Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m0Id); //Indexes (Id)

				mColor2YId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor2YId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor2Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m1Id); //Indexes (Id)

				mColor2ZId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor2ZId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor2Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m2Id); //Indexes (Id)

				mColor2WId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor2WId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor2Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m3Id); //Indexes (Id)
			}
			else
			{
				mColor1Id = id;

				TypeDescription pointerFloatType;
				pointerFloatType.PrimaryType = spv::OpTypePointer;
				pointerFloatType.SecondaryType = spv::OpTypeFloat;
				pointerFloatType.StorageClass = spv::StorageClassOutput;
				uint32_t floatTypeId = GetSpirVTypeId(pointerFloatType);

				mColor1XId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor1XId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor1Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m0Id); //Indexes (Id)

				mColor1YId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor1YId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor1Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m1Id); //Indexes (Id)

				mColor1ZId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor1ZId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor1Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m2Id); //Indexes (Id)

				mColor1WId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor1WId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor1Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m3Id); //Indexes (Id)
			}

			registerName = "oD" + std::to_string(registerNumber);
			usage = D3DDECLUSAGE_COLOR;
			break;
		case D3DSPR_TEXCRDOUT:
			registerName = "oT" + std::to_string(registerNumber);
			usage = D3DDECLUSAGE_TEXCOORD;
			break;
		case D3DSPR_TEMP:
			registerName = "r" + std::to_string(registerNumber);
			usage = D3DDECLUSAGE_COLOR;
			break;
		default:
			registerName = "o" + std::to_string(registerNumber);
			break;
		}

		stringWordSize = 3 + (registerName.length() / 4);
		//if (registerName.length() % 4 == 0)
		//{
		//	stringWordSize++;
		//}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(id); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		//mOutputRegisterUsages[(_D3DDECLUSAGE)GetUsage(token.i)] = id;
		if (this->mIsVertexShader && registerType != D3DSPR_TEMP)
		{
			mOutputRegisters.push_back(id);

			GenerateDecoration(registerNumber, id, usage, false);
		}
		else if (!this->mIsVertexShader && registerType == D3DSPR_TEMP) //r0 is used for pixel shader color output because reasons.
		{
			mOutputRegisters.push_back(id);

			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(id); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
			mDecorateInstructions.push_back(0); //Location offset
		}

		break;
	case D3DSPR_CONST:
	case D3DSPR_CONST2:
	case D3DSPR_CONST3:
	case D3DSPR_CONST4:
		id = GetNextId();
		description.PrimaryType = spv::OpTypePointer;
		description.SecondaryType = spv::OpTypeVector;
		description.TernaryType = spv::OpTypeFloat; //TODO: find a way to tell if this is an integer or float.
		description.ComponentCount = 4;
		description.StorageClass = spv::StorageClassInput;
		typeId = GetSpirVTypeId(description);

		mIdsByRegister[registerType][registerNumber] = id;
		mRegistersById[registerType][id] = registerNumber;
		mIdTypePairs[id] = description;

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(typeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassPushConstant); //Storage Class
		break;

	case D3DSPR_SAMPLER:
		id = GetNextId();
		description.PrimaryType = spv::OpTypePointer;
		description.SecondaryType = spv::OpTypeImage;
		description.ComponentCount = 4; //Really this is probably 2 but everything is 1 or 4 so we can swizzle later to get xy.
		description.StorageClass = spv::StorageClassInput;
		typeId = GetSpirVTypeId(description);

		mIdsByRegister[registerType][registerNumber] = id;
		mRegistersById[registerType][id] = registerNumber;
		mIdTypePairs[id] = description;

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(typeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassUniformConstant); //Storage Class

		mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
		mDecorateInstructions.push_back(id); //target (Id)
		mDecorateInstructions.push_back(spv::DecorationBinding); //Decoration Type (Id)
		mDecorateInstructions.push_back(registerNumber); //Location offset

		mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
		mDecorateInstructions.push_back(id); //target (Id)
		mDecorateInstructions.push_back(spv::DecorationDescriptorSet); //Decoration Type (Id)
		mDecorateInstructions.push_back(0); //Location offset

		registerName = "s" + std::to_string(registerNumber);

		stringWordSize = 3 + (registerName.length() / 4);
		//if (registerName.length() % 4 == 0)
		//{
		//	stringWordSize++;
		//}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(id); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		if (!this->mIsVertexShader)
		{
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].binding = registerNumber; //mConvertedShader.mDescriptorSetLayoutBindingCount;
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].descriptorCount = 1;
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].stageFlags = vk::ShaderStageFlagBits::eFragment;
			mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].pImmutableSamplers = NULL;

			mConvertedShader.mDescriptorSetLayoutBindingCount++;
		}

		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "GetIdByRegister - Id not found register " << registerNumber << " (" << registerType << ")";
		break;
	}


	if (!id)
	{
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::GetIdByRegister - Id was zero!";
	}

	return id;
}

void ShaderConverter::SetIdByRegister(const Token& token, uint32_t id)
{
	D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	uint32_t registerNumber = 0;
	TypeDescription description;
	uint32_t typeId = 0;

	switch (registerType)
	{
	case D3DSPR_CONST2:
		registerNumber = GetRegisterNumber(token) + 2048;
		break;
	case D3DSPR_CONST3:
		registerNumber = GetRegisterNumber(token) + 4096;
		break;
	case D3DSPR_CONST4:
		registerNumber = GetRegisterNumber(token) + 6144;
		break;
	default:
		registerNumber = GetRegisterNumber(token);
		break;
	}

	mIdsByRegister[registerType][registerNumber] = id;
	mRegistersById[registerType][id] = registerNumber;
}

TypeDescription ShaderConverter::GetTypeByRegister(const Token& token, _D3DDECLUSAGE usage)
{
	TypeDescription dataType;

	uint32_t id = GetIdByRegister(token, D3DSPR_FORCE_DWORD, usage);

	boost::container::flat_map<uint32_t, TypeDescription>::iterator it2 = mIdTypePairs.find(id);
	if (it2 != mIdTypePairs.end())
	{
		dataType = it2->second;
	}
	else
	{
		dataType.PrimaryType = spv::OpTypeVector;
		if (usage == D3DDECLUSAGE_COLOR)
		{
			dataType.SecondaryType = spv::OpTypeInt;
		}
		else
		{
			dataType.SecondaryType = spv::OpTypeFloat;
		}
		dataType.ComponentCount = 4;
	}

	return dataType;
}

/*
This function assumes a source register.
As such it can write out the conversion instructions and then return the new Id for the caller to use instead of the original source register.
*/
uint32_t ShaderConverter::GetSwizzledId(const Token& token, uint32_t inputId, _D3DSHADER_PARAM_REGISTER_TYPE type, _D3DDECLUSAGE usage)
{
	uint32_t swizzle = token.i & D3DVS_SWIZZLE_MASK;
	uint32_t outputComponentCount = 4; //TODO: figure out how to determine this.
	uint32_t vectorTypeId = 0;
	uint32_t registerNumber = 0;
	TypeDescription typeDescription;
	D3DSHADER_PARAM_REGISTER_TYPE registerType;
	uint32_t dataTypeId;
	std::string registerName;
	size_t stringWordSize;

	registerType = GetRegisterType(token.i);
	typeDescription = GetTypeByRegister(token, usage);

	if (inputId == UINT_MAX)
	{
		inputId = GetIdByRegister(token, type, usage);

		if (typeDescription.PrimaryType == spv::OpTypePointer && (mMajorVersion > 1 || registerType != D3DSPR_TEXTURE || mIsVertexShader))
		{
			//Shift the result type so we get a register instead of a pointer as the output type.
			typeDescription.PrimaryType = typeDescription.SecondaryType;
			typeDescription.SecondaryType = typeDescription.TernaryType;
			typeDescription.TernaryType = spv::OpTypeVoid;
			dataTypeId = GetSpirVTypeId(typeDescription);

			//deference pointer into a register.
			inputId = GetNextId();
			uint32_t tokenId = GetIdByRegister(token);
			mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
			mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
			mFunctionDefinitionInstructions.push_back(inputId); //result (Id)
			mFunctionDefinitionInstructions.push_back(tokenId); //pointer (Id)	

			if (this->mMajorVersion == 3)
			{
				registerName = "v" + std::to_string(registerNumber) + "_loaded";
				stringWordSize = 3 + (registerName.length() / 4);
				//if (registerName.length() % 4 == 0)
				//{
				//	stringWordSize++;
				//}
				mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
				mNameInstructions.push_back(inputId); //target (Id)
				PutStringInVector(registerName, mNameInstructions); //Literal
			}
			else
			{
				if (registerType == D3DSPR_INPUT)
				{
					registerName = "oD" + std::to_string(registerNumber) + "_loaded";
					stringWordSize = 3 + (registerName.length() / 4);
					//if (registerName.length() % 4 == 0)
					//{
					//	stringWordSize++;
					//}
					mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
					mNameInstructions.push_back(inputId); //target (Id)
					PutStringInVector(registerName, mNameInstructions); //Literal
				}
				else if (registerType == D3DSPR_TEMP)
				{
					registerName = "r" + std::to_string(registerNumber) + "_loaded";
					stringWordSize = 3 + (registerName.length() / 4);
					//if (registerName.length() % 4 == 0)
					//{
					//	stringWordSize++;
					//}
					mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
					mNameInstructions.push_back(inputId); //target (Id)
					PutStringInVector(registerName, mNameInstructions); //Literal
				}
				else
				{
					registerName = "oT" + std::to_string(registerNumber) + "_loaded";
					stringWordSize = 3 + (registerName.length() / 4);
					//if (registerName.length() % 4 == 0)
					//{
					//	stringWordSize++;
					//}
					mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
					mNameInstructions.push_back(inputId); //target (Id)
					PutStringInVector(registerName, mNameInstructions); //Literal
				}
			}

			//inputId = GetSwizzledId(token, inputId);

		}
	}

	if (swizzle == 0 || swizzle == D3DVS_NOSWIZZLE || outputComponentCount == 0)
	{
		return inputId; //No swizzle no op.
	}

	uint32_t xSource = swizzle & D3DVS_X_W;
	uint32_t ySource = swizzle & D3DVS_Y_W;
	uint32_t zSource = swizzle & D3DVS_Z_W;
	uint32_t wSource = swizzle & D3DVS_W_W;
	uint32_t outputId = GetNextId();

	//OpVectorShuffle must return a vector and vectors must have at least 2 elements so OpCompositeExtract must be used for a single component swizzle operation.
	if
		(
		((swizzle >> D3DVS_SWIZZLE_SHIFT) == (swizzle >> (D3DVS_SWIZZLE_SHIFT + 2))) &&
			((swizzle >> D3DVS_SWIZZLE_SHIFT) == (swizzle >> (D3DVS_SWIZZLE_SHIFT + 4))) &&
			((swizzle >> D3DVS_SWIZZLE_SHIFT) == (swizzle >> (D3DVS_SWIZZLE_SHIFT + 6)))
			)
	{
		vectorTypeId = GetSpirVTypeId(spv::OpTypeFloat); //Revisit may not be a float

		mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
		mFunctionDefinitionInstructions.push_back(vectorTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(outputId); // Result (Id)
		mFunctionDefinitionInstructions.push_back(inputId); //Composite (Id)

		switch (xSource)
		{
		case D3DVS_X_X:
			mFunctionDefinitionInstructions.push_back(0); //Component Literal
			break;
		case D3DVS_X_Y:
			mFunctionDefinitionInstructions.push_back(1); //Component Literal
			break;
		case D3DVS_X_Z:
			mFunctionDefinitionInstructions.push_back(2); //Component Literal
			break;
		case D3DVS_X_W:
			mFunctionDefinitionInstructions.push_back(3); //Component Literal
			break;
		}

	}
	else //vector
	{
		vectorTypeId = GetSpirVTypeId(spv::OpTypeVector, spv::OpTypeFloat, outputComponentCount); //Revisit may not be a float

		switch (outputComponentCount)
		{ //1 should be covered by the other branch.
		case 2:
			mFunctionDefinitionInstructions.push_back(Pack(5 + outputComponentCount, spv::OpVectorShuffle)); //size,Type
			mFunctionDefinitionInstructions.push_back(vectorTypeId); //Result Type (Id)
			mFunctionDefinitionInstructions.push_back(outputId); // Result (Id)
			mFunctionDefinitionInstructions.push_back(inputId); //Vector1 (Id)
			mFunctionDefinitionInstructions.push_back(inputId); //Vector2 (Id)

			switch (xSource)
			{
			case D3DVS_X_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_X_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_X_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_X_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			switch (ySource)
			{
			case D3DVS_Y_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_Y_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_Y_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_Y_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			break;
		case 3:
			mFunctionDefinitionInstructions.push_back(Pack(5 + outputComponentCount, spv::OpVectorShuffle)); //size,Type
			mFunctionDefinitionInstructions.push_back(vectorTypeId); //Result Type (Id)
			mFunctionDefinitionInstructions.push_back(outputId); // Result (Id)
			mFunctionDefinitionInstructions.push_back(inputId); //Vector1 (Id)
			mFunctionDefinitionInstructions.push_back(inputId); //Vector2 (Id)

			switch (xSource)
			{
			case D3DVS_X_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_X_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_X_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_X_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			switch (ySource)
			{
			case D3DVS_Y_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_Y_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_Y_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_Y_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			switch (zSource)
			{
			case D3DVS_Z_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_Z_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_Z_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_Z_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			break;
		case 4:
			mFunctionDefinitionInstructions.push_back(Pack(5 + outputComponentCount, spv::OpVectorShuffle)); //size,Type
			mFunctionDefinitionInstructions.push_back(vectorTypeId); //Result Type (Id)
			mFunctionDefinitionInstructions.push_back(outputId); // Result (Id)
			mFunctionDefinitionInstructions.push_back(inputId); //Vector1 (Id)
			mFunctionDefinitionInstructions.push_back(inputId); //Vector2 (Id)

			switch (xSource)
			{
			case D3DVS_X_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_X_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_X_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_X_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			switch (ySource)
			{
			case D3DVS_Y_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_Y_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_Y_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_Y_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			switch (zSource)
			{
			case D3DVS_Z_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_Z_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_Z_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_Z_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			switch (wSource)
			{
			case D3DVS_W_X:
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
				break;
			case D3DVS_W_Y:
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
				break;
			case D3DVS_W_Z:
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
				break;
			case D3DVS_W_W:
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
				break;
			}

			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "GetSwizzledId - Unsupported component count  " << outputComponentCount;
			break;
		}

	}

	return outputId;
}

uint32_t ShaderConverter::ApplyWriteMask(const Token& token, uint32_t modifiedId, _D3DDECLUSAGE usage) //Old routine
{
	TypeDescription typeDescription = GetTypeByRegister(token);
	D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	uint32_t originalId = GetIdByRegister(token, registerType, usage);
	uint32_t outputId = modifiedId;
	uint32_t swizzledId = originalId;
	uint32_t outputComponentCount = 4; //TODO: figure out how to determine this.
	uint32_t vectorTypeId = 0;

	if (typeDescription.PrimaryType == spv::OpTypePointer && (mMajorVersion > 1 || registerType != D3DSPR_TEXTURE || mIsVertexShader))
	{
		if (
			(((token.i & D3DSP_WRITEMASK_ALL) == D3DSP_WRITEMASK_ALL) || ((token.i & D3DSP_WRITEMASK_ALL) == 0x00000000))
			||
			((token.i & D3DSP_WRITEMASK_0) && (token.i & D3DSP_WRITEMASK_1) && registerType == D3DSPR_TEXCRDOUT)
			)
		{

			if (originalId == mColor1Id || originalId == mColor2Id)
			{
				uint32_t intTypeId = GetSpirVTypeId(spv::OpTypeInt);
				uint32_t floatTypeId = GetSpirVTypeId(spv::OpTypeFloat);

				uint32_t rId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
				mFunctionDefinitionInstructions.push_back(intTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(rId); //result (Id)
				mFunctionDefinitionInstructions.push_back(modifiedId); //Composite (Id)
				mFunctionDefinitionInstructions.push_back(0); //Indexes (Literal)
				uint32_t r2Id = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertUToF)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(r2Id); //result (Id)
				mFunctionDefinitionInstructions.push_back(rId); //Unsigned Value (Id)
				uint32_t rDividedId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFDiv)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(rDividedId); //result (Id)
				mFunctionDefinitionInstructions.push_back(r2Id); //argument1 (Id)
				mFunctionDefinitionInstructions.push_back(m255FloatId); //argument2 (Id)

				uint32_t gId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
				mFunctionDefinitionInstructions.push_back(intTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(gId); //result (Id)
				mFunctionDefinitionInstructions.push_back(modifiedId); //Composite (Id)
				mFunctionDefinitionInstructions.push_back(1); //Indexes (Literal)
				uint32_t g2Id = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertUToF)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(g2Id); //result (Id)
				mFunctionDefinitionInstructions.push_back(gId); //Unsigned Value (Id)
				uint32_t gDividedId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFDiv)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(gDividedId); //result (Id)
				mFunctionDefinitionInstructions.push_back(g2Id); //argument1 (Id)
				mFunctionDefinitionInstructions.push_back(m255FloatId); //argument2 (Id)

				uint32_t bId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
				mFunctionDefinitionInstructions.push_back(intTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(bId); //result (Id)
				mFunctionDefinitionInstructions.push_back(modifiedId); //Composite (Id)
				mFunctionDefinitionInstructions.push_back(2); //Indexes (Literal)
				uint32_t b2Id = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertUToF)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(b2Id); //result (Id)
				mFunctionDefinitionInstructions.push_back(bId); //Unsigned Value (Id)
				uint32_t bDividedId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFDiv)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(bDividedId); //result (Id)
				mFunctionDefinitionInstructions.push_back(b2Id); //argument1 (Id)
				mFunctionDefinitionInstructions.push_back(m255FloatId); //argument2 (Id)

				uint32_t aId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
				mFunctionDefinitionInstructions.push_back(intTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(aId); //result (Id)
				mFunctionDefinitionInstructions.push_back(modifiedId); //Composite (Id)
				mFunctionDefinitionInstructions.push_back(3); //Indexes (Literal)
				uint32_t a2Id = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertUToF)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(a2Id); //result (Id)
				mFunctionDefinitionInstructions.push_back(aId); //Unsigned Value (Id)
				uint32_t aDividedId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFDiv)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(aDividedId); //result (Id)
				mFunctionDefinitionInstructions.push_back(a2Id); //argument1 (Id)
				mFunctionDefinitionInstructions.push_back(m255FloatId); //argument2 (Id)

				if (originalId == mColor1Id)
				{
					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor1XId); //result (Id)
					mFunctionDefinitionInstructions.push_back(rDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor1YId); //result (Id)
					mFunctionDefinitionInstructions.push_back(gDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor1ZId); //result (Id)
					mFunctionDefinitionInstructions.push_back(bDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor1WId); //result (Id)
					mFunctionDefinitionInstructions.push_back(aDividedId); //argument1 (Id)	
				}
				else
				{
					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor2XId); //result (Id)
					mFunctionDefinitionInstructions.push_back(rDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor2YId); //result (Id)
					mFunctionDefinitionInstructions.push_back(gDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor2ZId); //result (Id)
					mFunctionDefinitionInstructions.push_back(bDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor2WId); //result (Id)
					mFunctionDefinitionInstructions.push_back(aDividedId); //argument1 (Id)	
				}
			}
			else
			{
				mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
				mFunctionDefinitionInstructions.push_back(swizzledId); //result (Id)
				mFunctionDefinitionInstructions.push_back(outputId); //argument1 (Id)	
			}
		}
		else
		{
			outputComponentCount = 0;

			if (token.i & D3DSP_WRITEMASK_0)
			{
				outputComponentCount++;
			}

			if (token.i & D3DSP_WRITEMASK_1)
			{
				outputComponentCount++;
			}

			if (token.i & D3DSP_WRITEMASK_2)
			{
				outputComponentCount++;
			}

			if (token.i & D3DSP_WRITEMASK_3)
			{
				outputComponentCount++;
			}

			if (outputComponentCount == 1)
			{
				TypeDescription pointerType;

				pointerType.PrimaryType = spv::OpTypePointer;
				pointerType.SecondaryType = typeDescription.TernaryType;

				switch (registerType)
				{
				case D3DSPR_INPUT:
					pointerType.StorageClass = spv::StorageClassInput;
					break;
				case D3DSPR_TEMP:
					if (mIsVertexShader)
					{
						pointerType.StorageClass = spv::StorageClassPrivate;
					}
					break;
				default:
					break;
				}

				vectorTypeId = GetSpirVTypeId(pointerType);
			}
			else
			{
				TypeDescription pointerType;

				pointerType.PrimaryType = spv::OpTypePointer;
				pointerType.SecondaryType = spv::OpTypeVector;
				pointerType.TernaryType = typeDescription.TernaryType;
				pointerType.ComponentCount = outputComponentCount;

				switch (registerType)
				{
				case D3DSPR_INPUT:
					pointerType.StorageClass = spv::StorageClassInput;
					break;
				case D3DSPR_TEMP:
					if (mIsVertexShader)
					{
						pointerType.StorageClass = spv::StorageClassPrivate;
					}
					break;
				default:
					break;
				}

				vectorTypeId = GetSpirVTypeId(pointerType);
			}

			swizzledId = GetNextId();

			mFunctionDefinitionInstructions.push_back(Pack(4 + outputComponentCount, spv::OpAccessChain)); //size,Type
			mFunctionDefinitionInstructions.push_back(vectorTypeId); //Result Type (Id)
			mFunctionDefinitionInstructions.push_back(swizzledId); //Result (Id)
			mFunctionDefinitionInstructions.push_back(originalId); //Base (Id)


			if (token.i & D3DSP_WRITEMASK_0)
			{
				mFunctionDefinitionInstructions.push_back(m0Id); //Indexes (Id)
			}

			if (token.i & D3DSP_WRITEMASK_1)
			{
				mFunctionDefinitionInstructions.push_back(m1Id); //Indexes (Id)
			}

			if (token.i & D3DSP_WRITEMASK_2)
			{
				mFunctionDefinitionInstructions.push_back(m2Id); //Indexes (Id)
			}

			if (token.i & D3DSP_WRITEMASK_3)
			{
				mFunctionDefinitionInstructions.push_back(m3Id); //Indexes (Id)
			}

			if (originalId == mColor1Id || originalId == mColor2Id)
			{
				uint32_t compositeTypeId = GetSpirVTypeId(spv::OpTypeVector, spv::OpTypeFloat, 4);
				uint32_t floatTypeId = GetSpirVTypeId(spv::OpTypeFloat);

				uint32_t rId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(rId); //result (Id)
				mFunctionDefinitionInstructions.push_back(modifiedId); //Composite (Id)
				mFunctionDefinitionInstructions.push_back(0); //Indexes (Literal)
				uint32_t r2Id = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertUToF)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(r2Id); //result (Id)
				mFunctionDefinitionInstructions.push_back(rId); //Unsigned Value (Id)
				uint32_t rDividedId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFDiv)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(rDividedId); //result (Id)
				mFunctionDefinitionInstructions.push_back(r2Id); //argument1 (Id)
				mFunctionDefinitionInstructions.push_back(m255FloatId); //argument2 (Id)

				uint32_t gId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(gId); //result (Id)
				mFunctionDefinitionInstructions.push_back(modifiedId); //Composite (Id)
				mFunctionDefinitionInstructions.push_back(1); //Indexes (Literal)
				uint32_t g2Id = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertUToF)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(g2Id); //result (Id)
				mFunctionDefinitionInstructions.push_back(gId); //Unsigned Value (Id)
				uint32_t gDividedId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFDiv)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(gDividedId); //result (Id)
				mFunctionDefinitionInstructions.push_back(g2Id); //argument1 (Id)
				mFunctionDefinitionInstructions.push_back(m255FloatId); //argument2 (Id)

				uint32_t bId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(bId); //result (Id)
				mFunctionDefinitionInstructions.push_back(modifiedId); //Composite (Id)
				mFunctionDefinitionInstructions.push_back(2); //Indexes (Literal)
				uint32_t b2Id = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertUToF)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(b2Id); //result (Id)
				mFunctionDefinitionInstructions.push_back(bId); //Unsigned Value (Id)
				uint32_t bDividedId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFDiv)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(bDividedId); //result (Id)
				mFunctionDefinitionInstructions.push_back(b2Id); //argument1 (Id)
				mFunctionDefinitionInstructions.push_back(m255FloatId); //argument2 (Id)

				uint32_t aId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpCompositeExtract)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(aId); //result (Id)
				mFunctionDefinitionInstructions.push_back(modifiedId); //Composite (Id)
				mFunctionDefinitionInstructions.push_back(3); //Indexes (Literal)
				uint32_t a2Id = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertUToF)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(a2Id); //result (Id)
				mFunctionDefinitionInstructions.push_back(aId); //Unsigned Value (Id)
				uint32_t aDividedId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFDiv)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(aDividedId); //result (Id)
				mFunctionDefinitionInstructions.push_back(a2Id); //argument1 (Id)
				mFunctionDefinitionInstructions.push_back(m255FloatId); //argument2 (Id)

				if (originalId == mColor1Id)
				{
					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor1XId); //result (Id)
					mFunctionDefinitionInstructions.push_back(rDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor1YId); //result (Id)
					mFunctionDefinitionInstructions.push_back(gDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor1ZId); //result (Id)
					mFunctionDefinitionInstructions.push_back(bDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor1WId); //result (Id)
					mFunctionDefinitionInstructions.push_back(aDividedId); //argument1 (Id)	
				}
				else
				{
					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor2XId); //result (Id)
					mFunctionDefinitionInstructions.push_back(rDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor2YId); //result (Id)
					mFunctionDefinitionInstructions.push_back(gDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor2ZId); //result (Id)
					mFunctionDefinitionInstructions.push_back(bDividedId); //argument1 (Id)	

					mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
					mFunctionDefinitionInstructions.push_back(mColor2WId); //result (Id)
					mFunctionDefinitionInstructions.push_back(aDividedId); //argument1 (Id)	
				}
			}
			else
			{
				mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
				mFunctionDefinitionInstructions.push_back(swizzledId); //result (Id)
				mFunctionDefinitionInstructions.push_back(outputId); //argument1 (Id)	
			}
		}
	}
	else
	{
		if ((((token.i & D3DSP_WRITEMASK_ALL) == D3DSP_WRITEMASK_ALL) || ((token.i & D3DSP_WRITEMASK_ALL) == 0x00000000)))
		{
			SetIdByRegister(token, outputId);
		}
		else
		{
			vectorTypeId = GetSpirVTypeId(spv::OpTypeVector, spv::OpTypeFloat, outputComponentCount); //Revisit may not be a float
			swizzledId = GetNextId();
			mFunctionDefinitionInstructions.push_back(Pack(5 + outputComponentCount, spv::OpVectorShuffle)); //size,Type
			mFunctionDefinitionInstructions.push_back(vectorTypeId); //Result Type (Id)
			mFunctionDefinitionInstructions.push_back(swizzledId); // Result (Id)
			mFunctionDefinitionInstructions.push_back(originalId); //Vector1 (Id)
			mFunctionDefinitionInstructions.push_back(modifiedId); //Vector2 (Id)

			if (token.i & D3DSP_WRITEMASK_0)
			{
				mFunctionDefinitionInstructions.push_back(4); //Component Literal
			}
			else
			{
				mFunctionDefinitionInstructions.push_back(0); //Component Literal
			}

			if (token.i & D3DSP_WRITEMASK_1)
			{
				mFunctionDefinitionInstructions.push_back(5); //Component Literal
			}
			else
			{
				mFunctionDefinitionInstructions.push_back(1); //Component Literal
			}

			if (token.i & D3DSP_WRITEMASK_2)
			{
				mFunctionDefinitionInstructions.push_back(6); //Component Literal
			}
			else
			{
				mFunctionDefinitionInstructions.push_back(2); //Component Literal
			}

			if (token.i & D3DSP_WRITEMASK_3)
			{
				mFunctionDefinitionInstructions.push_back(7); //Component Literal
			}
			else
			{
				mFunctionDefinitionInstructions.push_back(3); //Component Literal
			}

			SetIdByRegister(token, swizzledId);
		}
	}

	if (!originalId)
	{
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::ApplyWriteMask - originalId was zero!";
	}

	if (!modifiedId)
	{
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::ApplyWriteMask - modifiedId was zero!";
	}

	if (!swizzledId)
	{
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::ApplyWriteMask - swizzledId was zero!";
	}

	if (!outputId)
	{
		BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::ApplyWriteMask - outputId was zero!";
	}

	return outputId;
}

void ShaderConverter::GenerateYFlip()
{
	if (!mPositionYId)
	{
		return;
	}

	uint32_t typeId = GetSpirVTypeId(spv::OpTypeFloat);

	uint32_t negativeId = GetNextId();
	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(typeId); //Result Type (Id)
	mTypeInstructions.push_back(negativeId); //Result (Id)
	mTypeInstructions.push_back(bit_cast(-1.0f)); //Literal Value

	uint32_t positionYId = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
	mFunctionDefinitionInstructions.push_back(typeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(positionYId); //result (Id)
	mFunctionDefinitionInstructions.push_back(mPositionYId); //pointer (Id)	

	uint32_t resultId = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFMul)); //size,Type
	mFunctionDefinitionInstructions.push_back(typeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
	mFunctionDefinitionInstructions.push_back(positionYId); //argument1 (Id)
	mFunctionDefinitionInstructions.push_back(negativeId); //argument2 (Id)

	mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
	mFunctionDefinitionInstructions.push_back(mPositionYId); //Pointer (Id)
	mFunctionDefinitionInstructions.push_back(resultId); //Object (Id)
}

void ShaderConverter::GeneratePushConstant()
{
	TypeDescription registerPointerType;
	registerPointerType.PrimaryType = spv::OpTypePointer;
	registerPointerType.SecondaryType = spv::OpTypeVector;
	registerPointerType.TernaryType = spv::OpTypeFloat;
	registerPointerType.ComponentCount = 4;
	registerPointerType.StorageClass = spv::StorageClassPushConstant;

	TypeDescription registerType;
	registerType.PrimaryType = spv::OpTypeVector;
	registerType.SecondaryType = spv::OpTypeFloat;
	registerType.ComponentCount = 4;

	std::string registerName;
	uint32_t stringWordSize = 0;
	uint32_t registerPointerTypeId = GetSpirVTypeId(registerPointerType);
	uint32_t registerTypeId = GetSpirVTypeId(registerType);
	uint32_t pushConstantTypeId = GetNextId();
	uint32_t pushConstantPointerTypeId = GetNextId();
	uint32_t pushConstantPointerId = GetNextId();

	mTypeInstructions.push_back(Pack(2 + 4, spv::OpTypeStruct)); //size,Type
	mTypeInstructions.push_back(pushConstantTypeId); //Result (Id)
	mTypeInstructions.push_back(registerTypeId); //Member 0 type (Id)
	mTypeInstructions.push_back(registerTypeId); //Member 1 type (Id)
	mTypeInstructions.push_back(registerTypeId); //Member 2 type (Id)
	mTypeInstructions.push_back(registerTypeId); //Member 3 type (Id)

	//Name and decorate push constant structure type.
	registerName = "PushConstants";
	stringWordSize = 3 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
	mNameInstructions.push_back(pushConstantTypeId); //target (Id)
	PutStringInVector(registerName, mNameInstructions); //Literal

	mDecorateInstructions.push_back(Pack(3, spv::OpDecorate)); //size,Type
	mDecorateInstructions.push_back(pushConstantTypeId); //target (Id)
	mDecorateInstructions.push_back(spv::DecorationBlock); //Decoration Type (Id)

	//Name Members
	registerName = "c0";
	stringWordSize = 4 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpMemberName));
	mNameInstructions.push_back(pushConstantTypeId); //target (Id)
	mNameInstructions.push_back(0); //Member (Literal)
	PutStringInVector(registerName, mNameInstructions); //Literal

	registerName = "c1";
	stringWordSize = 4 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpMemberName));
	mNameInstructions.push_back(pushConstantTypeId); //target (Id)
	mNameInstructions.push_back(1); //Member (Literal)
	PutStringInVector(registerName, mNameInstructions); //Literal

	registerName = "c2";
	stringWordSize = 4 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpMemberName));
	mNameInstructions.push_back(pushConstantTypeId); //target (Id)
	mNameInstructions.push_back(2); //Member (Literal)
	PutStringInVector(registerName, mNameInstructions); //Literal

	registerName = "c3";
	stringWordSize = 4 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpMemberName));
	mNameInstructions.push_back(pushConstantTypeId); //target (Id)
	mNameInstructions.push_back(3); //Member (Literal)
	PutStringInVector(registerName, mNameInstructions); //Literal

	//Set member offsets
	mDecorateInstructions.push_back(Pack(4 + 1, spv::OpMemberDecorate)); //size,Type
	mDecorateInstructions.push_back(pushConstantTypeId); //target (Id)
	mDecorateInstructions.push_back(0); //Member (Literal)
	mDecorateInstructions.push_back(spv::DecorationOffset); //Decoration Type (Id)
	mDecorateInstructions.push_back(0);

	mDecorateInstructions.push_back(Pack(4 + 1, spv::OpMemberDecorate)); //size,Type
	mDecorateInstructions.push_back(pushConstantTypeId); //target (Id)
	mDecorateInstructions.push_back(1); //Member (Literal)
	mDecorateInstructions.push_back(spv::DecorationOffset); //Decoration Type (Id)
	mDecorateInstructions.push_back(16);

	mDecorateInstructions.push_back(Pack(4 + 1, spv::OpMemberDecorate)); //size,Type
	mDecorateInstructions.push_back(pushConstantTypeId); //target (Id)
	mDecorateInstructions.push_back(2); //Member (Literal)
	mDecorateInstructions.push_back(spv::DecorationOffset); //Decoration Type (Id)
	mDecorateInstructions.push_back(32);

	mDecorateInstructions.push_back(Pack(4 + 1, spv::OpMemberDecorate)); //size,Type
	mDecorateInstructions.push_back(pushConstantTypeId); //target (Id)
	mDecorateInstructions.push_back(3); //Member (Literal)
	mDecorateInstructions.push_back(spv::DecorationOffset); //Decoration Type (Id)
	mDecorateInstructions.push_back(48);

	//Create Pointer type and variable
	mTypeInstructions.push_back(Pack(4, spv::OpTypePointer)); //size,Type
	mTypeInstructions.push_back(pushConstantPointerTypeId); //Result (Id)
	mTypeInstructions.push_back(spv::StorageClassPushConstant); //Storage Class
	mTypeInstructions.push_back(pushConstantTypeId); //type (Id)

	mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
	mTypeInstructions.push_back(pushConstantPointerTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
	mTypeInstructions.push_back(pushConstantPointerId); //Result (Id)
	mTypeInstructions.push_back(spv::StorageClassPushConstant); //Storage Class

	registerName = "PC";
	stringWordSize = 3 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
	mNameInstructions.push_back(pushConstantPointerId); //target (Id)
	PutStringInVector(registerName, mNameInstructions); //Literal

	//Create Access Chains
	uint32_t c0 = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
	mFunctionDefinitionInstructions.push_back(registerPointerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(c0); //Result (Id)
	mFunctionDefinitionInstructions.push_back(pushConstantPointerId); //Base (Id)
	mFunctionDefinitionInstructions.push_back(m0Id); //Indexes (Id)

	uint32_t c1 = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
	mFunctionDefinitionInstructions.push_back(registerPointerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(c1); //Result (Id)
	mFunctionDefinitionInstructions.push_back(pushConstantPointerId); //Base (Id)
	mFunctionDefinitionInstructions.push_back(m1Id); //Indexes (Id)

	uint32_t c2 = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
	mFunctionDefinitionInstructions.push_back(registerPointerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(c2); //Result (Id)
	mFunctionDefinitionInstructions.push_back(pushConstantPointerId); //Base (Id)
	mFunctionDefinitionInstructions.push_back(m2Id); //Indexes (Id)

	uint32_t c3 = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
	mFunctionDefinitionInstructions.push_back(registerPointerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(c3); //Result (Id)
	mFunctionDefinitionInstructions.push_back(pushConstantPointerId); //Base (Id)
	mFunctionDefinitionInstructions.push_back(m3Id); //Indexes (Id)

	//Load Access Chains
	uint32_t c0_loaded = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
	mFunctionDefinitionInstructions.push_back(registerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(c0_loaded); //result (Id)
	mFunctionDefinitionInstructions.push_back(c0); //pointer (Id)

	uint32_t c1_loaded = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
	mFunctionDefinitionInstructions.push_back(registerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(c1_loaded); //result (Id)
	mFunctionDefinitionInstructions.push_back(c1); //pointer (Id)	

	uint32_t c2_loaded = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
	mFunctionDefinitionInstructions.push_back(registerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(c2_loaded); //result (Id)
	mFunctionDefinitionInstructions.push_back(c2); //pointer (Id)	

	uint32_t c3_loaded = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
	mFunctionDefinitionInstructions.push_back(registerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(c3_loaded); //result (Id)
	mFunctionDefinitionInstructions.push_back(c3); //pointer (Id)	

	//Remap c0-c3 to push constant.
	mIdsByRegister[D3DSPR_CONST][0] = c0_loaded;
	mRegistersById[D3DSPR_CONST][c0_loaded] = 0;

	mIdsByRegister[D3DSPR_CONST][1] = c1_loaded;
	mRegistersById[D3DSPR_CONST][c1_loaded] = 1;

	mIdsByRegister[D3DSPR_CONST][2] = c2_loaded;
	mRegistersById[D3DSPR_CONST][c2_loaded] = 2;

	mIdsByRegister[D3DSPR_CONST][3] = c3_loaded;
	mRegistersById[D3DSPR_CONST][c3_loaded] = 3;
}

void ShaderConverter::GeneratePostition()
{
	TypeDescription positionType;
	positionType.PrimaryType = spv::OpTypeVector;
	positionType.SecondaryType = spv::OpTypeFloat;
	positionType.ComponentCount = 4;

	TypeDescription positionPointerType;
	positionPointerType.PrimaryType = spv::OpTypePointer;
	positionPointerType.SecondaryType = spv::OpTypeVector;
	positionPointerType.TernaryType = spv::OpTypeFloat;
	positionPointerType.ComponentCount = 4;
	positionPointerType.StorageClass = spv::StorageClassOutput;

	TypeDescription floatPointerType;
	floatPointerType.PrimaryType = spv::OpTypePointer;
	floatPointerType.SecondaryType = spv::OpTypeFloat;
	uint32_t floatPointerTypeId = GetSpirVTypeId(floatPointerType);

	uint32_t floatTypeId = GetSpirVTypeId(spv::OpTypeFloat);
	uint32_t intTypeId = GetSpirVTypeId(spv::OpTypeInt);
	uint32_t positionTypeId = GetSpirVTypeId(positionType);
	uint32_t positionPointerTypeId = GetSpirVTypeId(positionPointerType);
	uint32_t positionStructureTypeId = GetNextId();
	uint32_t positionStructurePointerTypeId = GetNextId();
	uint32_t positionStructurePointerId = GetNextId();
	uint32_t positionPointerId = GetNextId();
	std::string registerName;
	uint32_t stringWordSize = 0;

	m0Id = GetNextId();
	m1Id = GetNextId();
	m2Id = GetNextId();
	m3Id = GetNextId();

	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(intTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m0Id); //Result (Id)
	mTypeInstructions.push_back(0); //Literal Value

	registerName = "int_0";
	stringWordSize = 3 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
	mNameInstructions.push_back(m0Id); //target (Id)
	PutStringInVector(registerName, mNameInstructions); //Literal

	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(intTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m1Id); //Result (Id)
	mTypeInstructions.push_back(1); //Literal Value

	registerName = "int_1";
	stringWordSize = 3 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
	mNameInstructions.push_back(m1Id); //target (Id)
	PutStringInVector(registerName, mNameInstructions); //Literal

	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(intTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m2Id); //Result (Id)
	mTypeInstructions.push_back(2); //Literal Value

	registerName = "int_2";
	stringWordSize = 3 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
	mNameInstructions.push_back(m2Id); //target (Id)
	PutStringInVector(registerName, mNameInstructions); //Literal

	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(intTypeId); //Result Type (Id)
	mTypeInstructions.push_back(m3Id); //Result (Id)
	mTypeInstructions.push_back(3); //Literal Value

	registerName = "int_3";
	stringWordSize = 3 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
	mNameInstructions.push_back(m3Id); //target (Id)
	PutStringInVector(registerName, mNameInstructions); //Literal

	mTypeInstructions.push_back(Pack(2 + 1, spv::OpTypeStruct)); //size,Type
	mTypeInstructions.push_back(positionStructureTypeId); //Result (Id)
	mTypeInstructions.push_back(positionTypeId); //Member 0 type (Id)

	mTypeInstructions.push_back(Pack(4, spv::OpTypePointer)); //size,Type
	mTypeInstructions.push_back(positionStructurePointerTypeId); //Result (Id)
	mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class
	mTypeInstructions.push_back(positionStructureTypeId); //type (Id)

	registerName = "gl_PerVertex";
	stringWordSize = 3 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
	mNameInstructions.push_back(positionStructureTypeId); //target (Id)
	PutStringInVector(registerName, mNameInstructions); //Literal

	registerName = "gl_Position";
	stringWordSize = 4 + (registerName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpMemberName));
	mNameInstructions.push_back(positionStructureTypeId); //target (Id)
	mNameInstructions.push_back(0); //Member (Literal)
	PutStringInVector(registerName, mNameInstructions); //Literal

	//mNameInstructions.push_back(Pack(2, spv::OpName));
	//mNameInstructions.push_back(positionStructurePointerId); //target (Id)

	mDecorateInstructions.push_back(Pack(3, spv::OpDecorate)); //size,Type
	mDecorateInstructions.push_back(positionStructureTypeId); //target (Id)
	mDecorateInstructions.push_back(spv::DecorationBlock); //Decoration Type (Id)

	mDecorateInstructions.push_back(Pack(4 + 1, spv::OpMemberDecorate)); //size,Type
	mDecorateInstructions.push_back(positionStructureTypeId); //target (Id)
	mDecorateInstructions.push_back(0); //Member (Literal)
	mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
	mDecorateInstructions.push_back(spv::BuiltInPosition);

	mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
	mTypeInstructions.push_back(positionStructurePointerTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
	mTypeInstructions.push_back(positionStructurePointerId); //Result (Id)
	mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class

	mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
	mFunctionDefinitionInstructions.push_back(positionPointerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(positionPointerId); //Result (Id)
	mFunctionDefinitionInstructions.push_back(positionStructurePointerId); //Base (Id)
	mFunctionDefinitionInstructions.push_back(m0Id); //Indexes (Id)

	//Updated tracking structures
	mPositionId = positionPointerId;
	mIdsByRegister[D3DSPR_RASTOUT][0] = positionPointerId;
	mRegistersById[D3DSPR_RASTOUT][positionPointerId] = 0;
	mIdTypePairs[positionPointerId] = positionPointerType;

	mOutputRegisters.push_back(positionStructurePointerId);
	mOutputRegisterUsages[D3DDECLUSAGE_POSITION] = positionPointerId;

	//Add an access chain for later flipping.
	mPositionYId = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
	mFunctionDefinitionInstructions.push_back(floatPointerTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(mPositionYId); //Result (Id)
	mFunctionDefinitionInstructions.push_back(positionPointerId); //Base (Id)
	mFunctionDefinitionInstructions.push_back(m1Id); //Indexes (Id)
}

void ShaderConverter::GenerateStore(const Token& token, uint32_t inputId)
{
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(token.i);
	uint32_t resultId = GetNextVersionId(token);
	uint32_t argumentId1 = inputId;

	switch (resultRegisterType)
	{
	case D3DSPR_RASTOUT:
	case D3DSPR_ATTROUT:
	case D3DSPR_COLOROUT:
	case D3DSPR_DEPTHOUT:
	case D3DSPR_OUTPUT:
		mFunctionDefinitionInstructions.push_back(Pack(3, spv::OpStore)); //size,Type
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	default:
		break;
	}
}

void ShaderConverter::GenerateDecoration(uint32_t registerNumber, uint32_t inputId, _D3DDECLUSAGE usage, bool isInput)
{
	if (this->mMajorVersion == 3)
	{
		switch (usage)
		{
		case D3DDECLUSAGE_POSITION:
			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(inputId); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
			mDecorateInstructions.push_back(spv::BuiltInPosition); //Location offset
			break;
		default:
			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(inputId); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
			mDecorateInstructions.push_back(registerNumber); //Location offset
			break;
		}
	}
	else
	{
		if (isInput)
		{
			if (this->mIsVertexShader)
			{
				mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
				mDecorateInstructions.push_back(inputId); //target (Id)
				mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
				mDecorateInstructions.push_back(registerNumber); //Location offset
			}
			else
			{
				switch (usage)
				{
				case D3DDECLUSAGE_POSITION:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
					mDecorateInstructions.push_back(spv::BuiltInPosition); //Location offset
					break;
				case D3DDECLUSAGE_COLOR:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber + 2); //Location offset
					break;
				default:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber + 4); //Location offset
					break;
				}
			}
		}
		else
		{
			if (this->mIsVertexShader)
			{
				switch (usage)
				{
				case D3DDECLUSAGE_POSITION:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
					mDecorateInstructions.push_back(spv::BuiltInPosition); //Location offset
					break;
				case D3DDECLUSAGE_FOG:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(0); //Location offset
					break;
				case D3DDECLUSAGE_PSIZE:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(1); //Location offset
					break;
				case D3DDECLUSAGE_COLOR:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber + 2); //Location offset
					break;
				default:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber + 4); //Location offset
					break;
				}
			}
			else
			{
				switch (usage)
				{
				case D3DDECLUSAGE_POSITION:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationBuiltIn); //Decoration Type (Id)
					mDecorateInstructions.push_back(spv::BuiltInPosition); //Location offset
					break;
				default:
					mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
					mDecorateInstructions.push_back(inputId); //target (Id)
					mDecorateInstructions.push_back(spv::DecorationLocation); //Decoration Type (Id)
					mDecorateInstructions.push_back(registerNumber); //Location offset
					break;
				}
			}
		}
	}
}

void ShaderConverter::Generate255Constants()
{
	uint32_t compositeTypeId = 0;
	uint32_t typeId = 0;
	uint32_t compositeId = GetNextId();
	uint32_t id = GetNextId();

	compositeTypeId = GetSpirVTypeId(spv::OpTypeVector, spv::OpTypeFloat, 4);
	typeId = GetSpirVTypeId(spv::OpTypeFloat);

	mTypeInstructions.push_back(Pack(3 + 1, spv::OpConstant)); //size,Type
	mTypeInstructions.push_back(typeId); //Result Type (Id)
	mTypeInstructions.push_back(id); //Result (Id)
	mTypeInstructions.push_back(bit_cast(255.0f)); //Literal Value

	mTypeInstructions.push_back(Pack(3 + 4, spv::OpConstantComposite)); //size,Type
	mTypeInstructions.push_back(compositeTypeId); //Result Type (Id)
	mTypeInstructions.push_back(compositeId); //Result (Id)
	mTypeInstructions.push_back(id); // Value (Id)
	mTypeInstructions.push_back(id); // Value (Id)
	mTypeInstructions.push_back(id); // Value (Id)
	mTypeInstructions.push_back(id); // Value (Id)

	m255FloatId = id;
	m255VectorId = compositeId;
}

void ShaderConverter::GenerateConstantBlock()
{
	TypeDescription typeDescription; //OpTypeVoid isn't 0 so ={} borks things.
	TypeDescription componentTypeDescription; //OpTypeVoid isn't 0 so ={} borks things.
	uint32_t typeId = 0;
	uint32_t componentTypeId = 0;
	uint32_t specId = 0;

	//--------------Integer-----------------------------
	typeDescription.PrimaryType = spv::OpTypeVector;
	typeDescription.SecondaryType = spv::OpTypeInt;
	typeDescription.ComponentCount = 4;
	typeId = GetSpirVTypeId(typeDescription);

	componentTypeDescription.PrimaryType = spv::OpTypeInt;
	componentTypeDescription.SecondaryType = spv::OpTypeVoid;
	componentTypeDescription.ComponentCount = 0; //default is 0 so using 1 will mess up compare.
	componentTypeId = GetSpirVTypeId(componentTypeDescription);

	for (size_t i = 0; i < 16; i++)
	{
		uint32_t id;
		uint32_t ids[4];

		id = GetNextId();
		for (size_t j = 0; j < 4; j++)
		{
			ids[j] = GetNextId();
			mTypeInstructions.push_back(Pack(3 + 1, spv::OpSpecConstant)); //size,Type
			mTypeInstructions.push_back(componentTypeId); //Result Type (Id)
			mTypeInstructions.push_back(ids[j]); //Result (Id)
			mTypeInstructions.push_back(0); //Literal Value

			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(ids[j]); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationSpecId); //Decoration Type (Id)
			mDecorateInstructions.push_back(specId++);
		}

		mTypeInstructions.push_back(Pack(3 + 4, spv::OpSpecConstantComposite)); //size,Type
		mTypeInstructions.push_back(typeId); //Result Type (Id)
		mTypeInstructions.push_back(id); //Result (Id)
		for (size_t j = 0; j < 4; j++)
		{
			mTypeInstructions.push_back(ids[j]); //Constituents
		}

		std::string registerName = "i" + std::to_string(i);
		size_t stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
		if (registerName.length() % 4 == 0)
		{
			stringWordSize++;
		}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(id); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		mIdsByRegister[D3DSPR_CONSTINT][i] = id;
		mRegistersById[D3DSPR_CONSTINT][id] = i;
	}

	//---------------Boolean------------------------------------
	typeDescription.PrimaryType = spv::OpTypeBool;
	typeDescription.SecondaryType = spv::OpTypeVoid;
	typeDescription.ComponentCount = 0; //default is 0 so using 1 will mess up compare.
	typeId = GetSpirVTypeId(typeDescription);

	for (size_t i = 0; i < 16; i++)
	{
		uint32_t id;

		id = GetNextId();

		mTypeInstructions.push_back(Pack(3 + 1, spv::OpSpecConstant)); //size,Type
		mTypeInstructions.push_back(componentTypeId); //Result Type (Id)
		mTypeInstructions.push_back(id); //Result (Id)
		mTypeInstructions.push_back(0); //Literal Value

		std::string registerName = "b" + std::to_string(i);
		size_t stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
		if (registerName.length() % 4 == 0)
		{
			stringWordSize++;
		}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(id); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
		mDecorateInstructions.push_back(id); //target (Id)
		mDecorateInstructions.push_back(spv::DecorationSpecId); //Decoration Type (Id)
		mDecorateInstructions.push_back(specId++);

		mIdsByRegister[D3DSPR_CONSTBOOL][i] = id;
		mRegistersById[D3DSPR_CONSTBOOL][id] = i;
	}

	//--------------Float-----------------------------
	typeDescription.PrimaryType = spv::OpTypeVector;
	typeDescription.SecondaryType = spv::OpTypeFloat;
	typeDescription.ComponentCount = 4;
	typeId = GetSpirVTypeId(typeDescription);

	componentTypeDescription.PrimaryType = spv::OpTypeFloat;
	componentTypeDescription.SecondaryType = spv::OpTypeVoid;
	componentTypeDescription.ComponentCount = 0; //default is 0 so using 1 will mess up compare.
	componentTypeId = GetSpirVTypeId(componentTypeDescription);

	for (size_t i = 0; i < 256; i++)
	{
		uint32_t id;
		uint32_t ids[4];

		id = GetNextId();
		for (size_t j = 0; j < 4; j++)
		{
			ids[j] = GetNextId();
			mTypeInstructions.push_back(Pack(3 + 1, spv::OpSpecConstant)); //size,Type
			mTypeInstructions.push_back(componentTypeId); //Result Type (Id)
			mTypeInstructions.push_back(ids[j]); //Result (Id)
			//mTypeInstructions.push_back(bit_cast(1.0f)); //Literal Value
			mTypeInstructions.push_back(bit_cast(0.0f)); //Literal Value

			mDecorateInstructions.push_back(Pack(3 + 1, spv::OpDecorate)); //size,Type
			mDecorateInstructions.push_back(ids[j]); //target (Id)
			mDecorateInstructions.push_back(spv::DecorationSpecId); //Decoration Type (Id)
			mDecorateInstructions.push_back(specId++);
		}

		mTypeInstructions.push_back(Pack(3 + 4, spv::OpSpecConstantComposite)); //size,Type
		mTypeInstructions.push_back(typeId); //Result Type (Id)
		mTypeInstructions.push_back(id); //Result (Id)
		for (size_t j = 0; j < 4; j++)
		{
			mTypeInstructions.push_back(ids[j]); //Constituents
		}

		std::string registerName = "c" + std::to_string(i);
		size_t stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
		if (registerName.length() % 4 == 0)
		{
			stringWordSize++;
		}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(id); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		mIdsByRegister[D3DSPR_CONST][i] = id;
		mRegistersById[D3DSPR_CONST][id] = i;
	}
}

void ShaderConverter::CombineSpirVOpCodes()
{
	mInstructions.insert(std::end(mInstructions), std::begin(mCapabilityInstructions), std::end(mCapabilityInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mExtensionInstructions), std::end(mExtensionInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mImportExtendedInstructions), std::end(mImportExtendedInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mMemoryModelInstructions), std::end(mMemoryModelInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mEntryPointInstructions), std::end(mEntryPointInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mExecutionModeInstructions), std::end(mExecutionModeInstructions));

	mInstructions.insert(std::end(mInstructions), std::begin(mStringInstructions), std::end(mStringInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mSourceExtensionInstructions), std::end(mSourceExtensionInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mSourceInstructions), std::end(mSourceInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mSourceContinuedInstructions), std::end(mSourceContinuedInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mNameInstructions), std::end(mNameInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mMemberNameInstructions), std::end(mMemberNameInstructions));

	mInstructions.insert(std::end(mInstructions), std::begin(mDecorateInstructions), std::end(mDecorateInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mMemberDecorateInstructions), std::end(mMemberDecorateInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mGroupDecorateInstructions), std::end(mGroupDecorateInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mGroupMemberDecorateInstructions), std::end(mGroupMemberDecorateInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mDecorationGroupInstructions), std::end(mDecorationGroupInstructions));

	mInstructions.insert(std::end(mInstructions), std::begin(mTypeInstructions), std::end(mTypeInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mFunctionDeclarationInstructions), std::end(mFunctionDeclarationInstructions));
	mInstructions.insert(std::end(mInstructions), std::begin(mFunctionDefinitionInstructions), std::end(mFunctionDefinitionInstructions));

	mCapabilityInstructions.clear();
	mExtensionInstructions.clear();
	mImportExtendedInstructions.clear();
	mMemoryModelInstructions.clear();
	mEntryPointInstructions.clear();
	mExecutionModeInstructions.clear();

	mStringInstructions.clear();
	mSourceExtensionInstructions.clear();
	mSourceInstructions.clear();
	mSourceContinuedInstructions.clear();
	mNameInstructions.clear();
	mMemberNameInstructions.clear();

	mDecorateInstructions.clear();
	mMemberDecorateInstructions.clear();
	mGroupDecorateInstructions.clear();
	mGroupMemberDecorateInstructions.clear();
	mDecorationGroupInstructions.clear();

	mTypeInstructions.clear();
	mFunctionDeclarationInstructions.clear();
	mFunctionDefinitionInstructions.clear();
}

void ShaderConverter::CreateSpirVModule()
{
#ifdef _DEBUG
	//Start Debug Code
	if (!mIsVertexShader)
	{
		std::ofstream outFile("fragment.spv", std::ios::out | std::ios::binary);
		outFile.write((char*)mInstructions.data(), mInstructions.size() * sizeof(uint32_t));
	}
	else
	{
		std::ofstream outFile("vertex.spv", std::ios::out | std::ios::binary);
		outFile.write((char*)mInstructions.data(), mInstructions.size() * sizeof(uint32_t));
	}
	//End Debug Code
#endif

	vk::Result result;
	vk::ShaderModuleCreateInfo moduleCreateInfo;
	moduleCreateInfo.codeSize = mInstructions.size() * sizeof(uint32_t);
	moduleCreateInfo.pCode = mInstructions.data(); //Why is this uint32_t* if the size is in bytes?
	//moduleCreateInfo.flags = 0;
	result = mDevice.createShaderModule(&moduleCreateInfo, nullptr, &mConvertedShader.ShaderModule);

	if (result != vk::Result::eSuccess)
	{
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::CreateSpirVModule vkCreateShaderModule failed with return code of " << GetResultString((VkResult)result);
		return;
	}
}

void ShaderConverter::Process_DCL_Pixel()
{
	Token token = GetNextToken();
	Token registerToken = GetNextToken();
	uint32_t usage = GetUsage(token.i);
	uint32_t usageIndex = GetUsageIndex(token.i);
	uint32_t registerNumber = GetRegisterNumber(registerToken);
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(registerToken.i);
	uint32_t tokenId = GetNextVersionId(registerToken);
	TypeDescription typeDescription;
	uint32_t registerComponents = (registerToken.i & D3DSP_WRITEMASK_ALL) >> 16;
	uint32_t resultTypeId;
	uint32_t textureType;
	std::string registerName;
	size_t stringWordSize;

	typeDescription.PrimaryType = spv::OpTypePointer;
	typeDescription.SecondaryType = spv::OpTypeVector;
	typeDescription.TernaryType = spv::OpTypeFloat;
	typeDescription.StorageClass = spv::StorageClassInput;

	//Magic numbers from ToGL (no whole numbers?)
	switch (registerComponents)
	{
	case 1: //float
		typeDescription.SecondaryType = spv::OpTypeFloat;
		typeDescription.TernaryType = spv::OpTypeVoid;
		typeDescription.ComponentCount = 1;
		break;
	case 3: //vec2
		typeDescription.ComponentCount = 2;
		break;
	case 7: //vec3
		typeDescription.ComponentCount = 3;
		break;
	case 0xF: //vec4
		typeDescription.ComponentCount = 4;
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_DCL - Unsupported component type " << registerComponents;
		break;
	}

	mIdTypePairs[tokenId] = typeDescription;

	switch (registerType)
	{
	case D3DSPR_INPUT:
		typeDescription.StorageClass = spv::StorageClassInput;
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassInput); //Storage Class
		//Optional initializer

		mInputRegisters.push_back(tokenId);

		registerName = "v" + std::to_string(registerNumber);
		stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
		if (registerName.length() % 4 == 0)
		{
			stringWordSize++;
		}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(tokenId); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		GenerateDecoration(registerNumber, tokenId, (_D3DDECLUSAGE)usage, true);
		break;
	case D3DSPR_TEXTURE:
		typeDescription.StorageClass = spv::StorageClassInput;
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassInput); //Storage Class
															 //Optional initializer

		mInputRegisters.push_back(tokenId);

		registerName = "T" + std::to_string(registerNumber);
		stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
		if (registerName.length() % 4 == 0)
		{
			stringWordSize++;
		}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(tokenId); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		GenerateDecoration(registerNumber, tokenId, (_D3DDECLUSAGE)usage, true);
		break;
	case D3DSPR_SAMPLER:
		textureType = GetTextureType(token.i);

		//switch (textureType)
		//{
		//case D3DSTT_2D:
		//	break;
		//case D3DSTT_CUBE:
		//	break;
		//case D3DSTT_VOLUME:
		//	break;
		//case D3DSTT_UNKNOWN:
		//	break;
		//default:
		//	break;
		//}	

		resultTypeId = GetSpirVTypeId(spv::OpTypePointer, spv::OpTypeImage);
		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassUniformConstant); //Storage Class
		//Optional initializer

		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].binding = registerNumber; //mConvertedShader.mDescriptorSetLayoutBindingCount;
		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].descriptorCount = 1;
		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].stageFlags = vk::ShaderStageFlagBits::eFragment;
		mConvertedShader.mDescriptorSetLayoutBinding[mConvertedShader.mDescriptorSetLayoutBindingCount].pImmutableSamplers = nullptr;

		mConvertedShader.mDescriptorSetLayoutBindingCount++;


		/*
		resultTypeId = GetSpirVTypeId(spv::OpTypePointer, spv::OpTypeSampler);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassUniform);
		//Optional initializer
		*/

		break;
	case D3DSPR_TEMP:
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassPrivate); //Storage Class
		//Optional initializer
		break;
	default:
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::Process_DCL_Pixel unsupported register type " << registerType;
		break;
	}

	BOOST_LOG_TRIVIAL(info) << "DCL " << GetUsageName((_D3DDECLUSAGE)usage) << " - " << registerNumber << "(" << GetRegisterTypeName(registerType) << ") ";
}

void ShaderConverter::Process_DCL_Vertex()
{
	Token token = GetNextToken();
	Token registerToken = GetNextToken();
	uint32_t usage = GetUsage(token.i);
	uint32_t usageIndex = GetUsageIndex(token.i);
	uint32_t registerNumber = GetRegisterNumber(registerToken);
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(registerToken.i);
	uint32_t tokenId = GetNextVersionId(registerToken);
	TypeDescription typeDescription;
	uint32_t registerComponents = (registerToken.i & D3DSP_WRITEMASK_ALL) >> 16;
	uint32_t resultTypeId;
	std::string registerName;
	size_t stringWordSize;

	typeDescription.PrimaryType = spv::OpTypePointer;
	typeDescription.SecondaryType = spv::OpTypeVector;
	if (usage == D3DDECLUSAGE_COLOR)
	{
		typeDescription.TernaryType = spv::OpTypeInt;
	}
	else
	{
		typeDescription.TernaryType = spv::OpTypeFloat;
	}

	//Magic numbers from ToGL (no whole numbers?)
	switch (registerComponents)
	{
	case 1: //float
		if (usage == D3DDECLUSAGE_COLOR)
		{
			typeDescription.SecondaryType = spv::OpTypeInt;
		}
		else
		{
			typeDescription.SecondaryType = spv::OpTypeFloat;
		}

		typeDescription.TernaryType = spv::OpTypeVoid;
		typeDescription.ComponentCount = 1;
		break;
	case 3: //vec2
		typeDescription.ComponentCount = 2;
		break;
	case 7: //vec3
		typeDescription.ComponentCount = 3;
		break;
	case 0xF: //vec4
		typeDescription.ComponentCount = 4;
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_DCL - Unsupported component type " << registerComponents;
		break;
	}

	mIdTypePairs[tokenId] = typeDescription;

	switch (registerType)
	{
	case D3DSPR_INPUT:
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassInput); //Storage Class
															 //Optional initializer

		mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].binding = 0;//element.Stream; TODO:REVISIT
		mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].location = mConvertedShader.mVertexInputAttributeDescriptionCount;
		mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].offset = 0;//element.Offset;

		switch (typeDescription.ComponentCount)
		{
		case 1: // 1D float expanded to (value, 0., 0., 1.)
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32Sfloat;
			break;
		case 2:  // 2D float expanded to (value, value, 0., 1.)
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32G32Sfloat;
			break;
		case 3: // 3D float expanded to (value, value, value, 1.)
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32G32B32Sfloat;
			break;
		case 4:  // 4D float
			mConvertedShader.mVertexInputAttributeDescription[mConvertedShader.mVertexInputAttributeDescriptionCount].format = vk::Format::eR32G32B32A32Sfloat;
		default:
			break;
		}

		mConvertedShader.mVertexInputAttributeDescriptionCount++;

		mInputRegisters.push_back(tokenId);

		registerName = "v" + std::to_string(registerNumber);
		stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
		if (registerName.length() % 4 == 0)
		{
			stringWordSize++;
		}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(tokenId); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		GenerateDecoration(registerNumber, tokenId, (_D3DDECLUSAGE)usage, true);
		break;
	case D3DSPR_RASTOUT:
	case D3DSPR_ATTROUT:
	case D3DSPR_COLOROUT:
	case D3DSPR_DEPTHOUT:
	case D3DSPR_OUTPUT:
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassOutput); //Storage Class
		//Optional initializer

		switch (usage)
		{
		case D3DDECLUSAGE_POSITION:
			mPositionRegister = usageIndex; //might need this later.
			mPositionId = tokenId;
			registerName = "oPos" + std::to_string(registerNumber);
			break;
		case D3DDECLUSAGE_FOG:
			registerName = "oFog" + std::to_string(registerNumber);
			break;
		case D3DDECLUSAGE_PSIZE:
			registerName = "oPts" + std::to_string(registerNumber);
			break;
		case D3DDECLUSAGE_COLOR:

			if (registerNumber)
			{
				mColor2Id = tokenId;

				TypeDescription pointerFloatType;
				pointerFloatType.PrimaryType = spv::OpTypePointer;
				pointerFloatType.SecondaryType = spv::OpTypeFloat;
				pointerFloatType.StorageClass = spv::StorageClassOutput;
				uint32_t floatTypeId = GetSpirVTypeId(pointerFloatType);

				mColor2XId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor2XId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor2Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m0Id); //Indexes (Id)

				mColor2YId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor2YId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor2Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m1Id); //Indexes (Id)

				mColor2ZId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor2ZId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor2Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m2Id); //Indexes (Id)

				mColor2WId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor2WId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor2Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m3Id); //Indexes (Id)
			}
			else
			{
				mColor1Id = tokenId;

				TypeDescription pointerFloatType;
				pointerFloatType.PrimaryType = spv::OpTypePointer;
				pointerFloatType.SecondaryType = spv::OpTypeFloat;
				pointerFloatType.StorageClass = spv::StorageClassOutput;
				uint32_t floatTypeId = GetSpirVTypeId(pointerFloatType);

				mColor1XId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor1XId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor1Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m0Id); //Indexes (Id)

				mColor1YId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor1YId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor1Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m1Id); //Indexes (Id)

				mColor1ZId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor1ZId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor1Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m2Id); //Indexes (Id)

				mColor1WId = GetNextId();
				mFunctionDefinitionInstructions.push_back(Pack(4 + 1, spv::OpAccessChain)); //size,Type
				mFunctionDefinitionInstructions.push_back(floatTypeId); //Result Type (Id)
				mFunctionDefinitionInstructions.push_back(mColor1WId); //Result (Id)
				mFunctionDefinitionInstructions.push_back(mColor1Id); //Base (Id)
				mFunctionDefinitionInstructions.push_back(m3Id); //Indexes (Id)
			}

			registerName = "oD" + std::to_string(registerNumber);
			break;
		case D3DDECLUSAGE_TEXCOORD:
			registerName = "oT" + std::to_string(registerNumber);
			break;
		default:
			registerName = "o" + std::to_string(registerNumber);
			break;
		}

		stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
		if (registerName.length() % 4 == 0)
		{
			stringWordSize++;
		}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(tokenId); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		mOutputRegisters.push_back(tokenId);
		mOutputRegisterUsages[(_D3DDECLUSAGE)usage] = tokenId;
		GenerateDecoration(registerNumber, tokenId, (_D3DDECLUSAGE)usage, false);
		break;
	case D3DSPR_TEMP:
		typeDescription.StorageClass = spv::StorageClassPrivate;
		resultTypeId = GetSpirVTypeId(typeDescription);

		mTypeInstructions.push_back(Pack(4, spv::OpVariable)); //size,Type
		mTypeInstructions.push_back(resultTypeId); //ResultType (Id) Must be OpTypePointer with the pointer's type being what you care about.
		mTypeInstructions.push_back(tokenId); //Result (Id)
		mTypeInstructions.push_back(spv::StorageClassPrivate); //Storage Class
		//Optional initializer
		break;
	default:
		BOOST_LOG_TRIVIAL(fatal) << "ShaderConverter::Process_DCL_Vertex unsupported register type " << registerType;
		break;
	}

	BOOST_LOG_TRIVIAL(info) << "DCL " << GetUsageName((_D3DDECLUSAGE)usage) << " - " << registerNumber << "(" << GetRegisterTypeName(registerType) << ") ";
}

void ShaderConverter::Process_DCL()
{
	if (mIsVertexShader)
	{
		if (mMajorVersion >= 3)
		{
			Process_DCL_Vertex();
		}
		else if (mMajorVersion >= 2)
		{
			Process_DCL_Vertex();
		}
		else
		{
			Process_DCL_Vertex();
			BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::Process_DCL unsupported shader version " << mMajorVersion;
		}
	}
	else
	{
		if (mMajorVersion >= 3)
		{
			Process_DCL_Pixel();
		}
		else if (mMajorVersion >= 2)
		{
			Process_DCL_Pixel();
		}
		else
		{
			Process_DCL_Pixel();
			BOOST_LOG_TRIVIAL(warning) << "ShaderConverter::Process_DCL unsupported shader version " << mMajorVersion;
		}
	}
}

void ShaderConverter::Process_DEF()
{
	DWORD literalValue;
	Token token = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	DestinationParameterToken  destinationParameterToken = token.DestinationParameterToken;

	for (size_t i = 0; i < 4; i++)
	{
		literalValue = GetNextToken().i;
		mShaderConstantSlots.FloatConstants[token.DestinationParameterToken.RegisterNumber * 4 + i] = bit_cast(literalValue);
	}

	PrintTokenInformation("DEF", token, token, token);
}

void ShaderConverter::Process_DEFI()
{
	DWORD literalValue;
	Token token = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	DestinationParameterToken  destinationParameterToken = token.DestinationParameterToken;

	for (size_t i = 0; i < 4; i++)
	{
		literalValue = GetNextToken().i;
		mShaderConstantSlots.IntegerConstants[token.DestinationParameterToken.RegisterNumber * 4 + i] = literalValue;
	}

	PrintTokenInformation("DEFI", token, token, token);
}

void ShaderConverter::Process_DEFB()
{
	DWORD literalValue;
	Token token = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE registerType = GetRegisterType(token.i);
	DestinationParameterToken  destinationParameterToken = token.DestinationParameterToken;

	literalValue = GetNextToken().i;
	mShaderConstantSlots.BooleanConstants[token.DestinationParameterToken.RegisterNumber * 4] = literalValue;

	PrintTokenInformation("DEFB", token, token, token);
}

void ShaderConverter::Process_IFC()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;
	uint32_t trueLabelId;
	uint32_t falseLabelId;

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2); //, UINT_MAX, D3DSPR_INPUT
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIEqual)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIEqual)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_IFC - Unsupported data type " << dataType;
		break;
	}

	trueLabelId = GetNextId();
	falseLabelId = GetNextId();

	mFalseLabels.push(falseLabelId);
	mFalseLabelCount++;

	mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpBranchConditional)); //size,Type
	mFunctionDefinitionInstructions.push_back(resultId); //Condition (Id)
	mFunctionDefinitionInstructions.push_back(trueLabelId); //True Label (Id)
	mFunctionDefinitionInstructions.push_back(falseLabelId); //False Label (Id)

	mFunctionDefinitionInstructions.push_back(Pack(2, spv::OpLabel)); //size,Type
	mFunctionDefinitionInstructions.push_back(trueLabelId); //True Label (Id)

	PrintTokenInformation("IFC", argumentToken1, argumentToken2);
}

void ShaderConverter::Process_IF()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;
	uint32_t trueLabelId;
	uint32_t falseLabelId;

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	resultId = GetNextId();
	trueLabelId = GetNextId();
	falseLabelId = GetNextId();

	mFalseLabels.push(falseLabelId);
	mFalseLabelCount++;

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpBranchConditional)); //size,Type
		mFunctionDefinitionInstructions.push_back(argumentId1); //Condition (Id)
		mFunctionDefinitionInstructions.push_back(trueLabelId); //True Label (Id)
		mFunctionDefinitionInstructions.push_back(falseLabelId); //False Label (Id)

		mFunctionDefinitionInstructions.push_back(Pack(2, spv::OpLabel)); //size,Type
		mFunctionDefinitionInstructions.push_back(trueLabelId); //True Label (Id)

		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_IF - Unsupported data type " << dataType;
		break;
	}

	PrintTokenInformation("IF", argumentToken1);
}

void ShaderConverter::Process_ELSE()
{
	uint32_t falseLabelId = mFalseLabels.top();

	mFalseLabels.pop();

	mFunctionDefinitionInstructions.push_back(Pack(2, spv::OpLabel)); //size,Type
	mFunctionDefinitionInstructions.push_back(falseLabelId); //False Label (Id)

	PrintTokenInformation("ELSE");
}

void ShaderConverter::Process_ENDIF()
{
	if (mFalseLabels.size() == mFalseLabelCount)
	{
		Process_ELSE();
	}
	mFalseLabelCount--;

	uint32_t endIfLabelId = GetNextId();

	mFunctionDefinitionInstructions.push_back(Pack(2, spv::OpBranch)); //size,Type
	mFunctionDefinitionInstructions.push_back(endIfLabelId); //End If (Id)

	mFunctionDefinitionInstructions.push_back(Pack(2, spv::OpLabel)); //size,Type
	mFunctionDefinitionInstructions.push_back(endIfLabelId); //End If (Id)

	PrintTokenInformation("ENDIF");
}

void ShaderConverter::Process_NRM()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Normalize); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Normalize); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Normalize); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_NRM - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("NRM", resultToken, argumentToken1);
}

void ShaderConverter::Process_MOV()
{
	TypeDescription typeDescription;
	//spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t resultId;
	uint32_t argumentId1;
	//uint32_t argumentId2;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	//resultId = GetNextVersionId(resultToken);	
	resultId = GetIdByRegister(resultToken);
	typeDescription = GetTypeByRegister(resultToken);

	if (resultId == mColor1Id || resultId == mColor2Id)
	{
		argumentId1 = GetSwizzledId(argumentToken1, UINT_MAX, D3DSPR_FORCE_DWORD, D3DDECLUSAGE_COLOR);
	}
	else
	{
		argumentId1 = GetSwizzledId(argumentToken1);
	}

	//mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	resultId = ApplyWriteMask(resultToken, argumentId1);

	switch (resultRegisterType)
	{
	case D3DSPR_RASTOUT:
	case D3DSPR_ATTROUT:
	case D3DSPR_COLOROUT:
	case D3DSPR_DEPTHOUT:
	case D3DSPR_OUTPUT:
		//Handled by ApplyWriteMask
		break;
	default:
		dataTypeId = GetSpirVTypeId(typeDescription);

		mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpCopyObject)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	}

	PrintTokenInformation("MOV", resultToken, resultId, argumentToken1, argumentId1);
}

void ShaderConverter::Process_MOVA()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = spv::OpTypeInt;
	}
	else
	{
		typeDescription.PrimaryType = spv::OpTypeInt;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	resultId = GetNextId();

	mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpConvertFToS)); //size,Type
	mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
	mFunctionDefinitionInstructions.push_back(argumentId1); //Operand (Id)

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MOVA", resultToken, argumentToken1);
}

void ShaderConverter::Process_RSQ()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450InverseSqrt); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450InverseSqrt); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450InverseSqrt); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_RSQ - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("RSQ", resultToken, argumentToken1);
}


void ShaderConverter::Process_DST()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Distance); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Distance); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Distance); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_DST - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("DST", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_CRS()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Cross); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Cross); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Cross); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_CRS - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("CRS", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_POW()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Pow); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Pow); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Pow); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_POW - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("POW", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_MUL()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIMul)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIMul)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFMul)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MUL - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MUL", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_EXP()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Exp2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Exp2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Exp2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_EXP - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("EXP", resultToken, argumentToken1);
}

void ShaderConverter::Process_EXPP()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Exp2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Exp2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Exp2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_EXPP - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("EXPP", resultToken, argumentToken1);
}

void ShaderConverter::Process_LOG()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Log2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Log2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Log2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_LOG - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("LOG", resultToken, argumentToken1);
}

void ShaderConverter::Process_LOGP()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Log2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Log2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Log2); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_LOGP - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("LOGP", resultToken, argumentToken1);
}

void ShaderConverter::Process_FRC()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2 = 0;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);

	//TODO: create a integer pointer to store the whole part.
	BOOST_LOG_TRIVIAL(warning) << "Process_FRC - Not currently handling integer pointer argument.";

	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450Modf); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id) For some reason I'm required to provide an integer pointer.
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_FRC - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("FRC", resultToken, argumentToken1);
}


void ShaderConverter::Process_ABS()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450SAbs); //Instruction Set (There is no UAbs
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450SAbs); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450FAbs); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MAX - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("ABS", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_ADD()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2); //, UINT_MAX, D3DSPR_INPUT
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIAdd)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIAdd)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFAdd)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_ADD - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("ADD", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_SUB()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpISub)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpISub)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFSub)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_SUB - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("SUB", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_MIN()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450UMin); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450SMin); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450FMin); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MIN - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MIN", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_MAX()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450UMax); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeInt:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450SMax); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	case spv::OpTypeFloat:
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpExtInst)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(mGlslExtensionId); //Instruction Set
		mFunctionDefinitionInstructions.push_back(GLSLstd450::GLSLstd450FMax); //Instruction Set
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)
		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MAX - Unsupported data type " << dataType;
		break;
	}

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("MAX", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_DP3()
{
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	dataTypeId = GetSpirVTypeId(spv::OpTypeFloat);

	TypeDescription typeDescription;
	typeDescription.PrimaryType = spv::OpTypeFloat;
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpDot)); //size,Type
	mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
	mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
	mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("DP3", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_DP4()
{
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t resultId;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	TypeDescription typeDescription;
	typeDescription.PrimaryType = spv::OpTypeFloat;
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
		//TODO: handle target swizzle
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	resultId = GetNextId();

	mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpDot)); //size,Type
	mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
	mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
	mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)

	resultId = ApplyWriteMask(resultToken, resultId);

	PrintTokenInformation("DP4", resultToken, argumentToken1, argumentToken2);
}

void ShaderConverter::Process_TEX()
{
	spv::Op dataType = spv::OpNop;
	uint32_t dataTypeId = 0;
	uint32_t texcoordDataTypeId = 0;
	uint32_t argumentId1 = 0;
	uint32_t argumentId2 = 0;
	uint32_t argumentId1_temp = 0;
	uint32_t argumentId2_temp = 0;
	uint32_t resultId = 0;
	uint32_t resultTypeId = 0;
	std::string registerName;
	uint32_t stringWordSize = 0;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	TypeDescription typeDescription;
	typeDescription.PrimaryType = spv::OpTypeVector;
	typeDescription.SecondaryType = spv::OpTypeFloat;
	typeDescription.ComponentCount = 4;
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	resultTypeId = GetSpirVTypeId(spv::OpTypeImage);

	//typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	//mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.
	dataTypeId = GetSpirVTypeId(typeDescription);

	typeDescription.ComponentCount = 2;
	texcoordDataTypeId = GetSpirVTypeId(typeDescription);

	if (mMajorVersion > 1)
	{
		Token argumentToken1 = GetNextToken();
		_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

		Token argumentToken2 = GetNextToken();
		_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

		argumentId2 = GetSwizzledId(argumentToken1, UINT_MAX, D3DSPR_TEXTURE);
		argumentId1_temp = GetIdByRegister(argumentToken2, D3DSPR_SAMPLER);

		argumentId1 = GetNextId();

		mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
		mFunctionDefinitionInstructions.push_back(resultTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1_temp); //pointer (Id)	

		PrintTokenInformation("TEX_2_0", resultToken, argumentToken1, argumentToken2);
	}
	else if (mMajorVersion == 1 && mMinorVersion >= 4)
	{
		Token argumentToken1 = GetNextToken();
		_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

		argumentId2 = GetSwizzledId(argumentToken1, UINT_MAX, D3DSPR_TEXTURE);
		argumentId1_temp = GetIdByRegister(argumentToken1, D3DSPR_SAMPLER);

		argumentId1 = GetNextId();

		mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
		mFunctionDefinitionInstructions.push_back(resultTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1_temp); //pointer (Id)	

		PrintTokenInformation("TEX_1_4", resultToken, argumentToken1, argumentToken1);
	}
	else
	{
		_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(resultToken.i);

		argumentId2_temp = GetIdByRegister(resultToken, D3DSPR_TEXTURE);
		argumentId1_temp = GetIdByRegister(resultToken, D3DSPR_SAMPLER);

		argumentId1 = GetNextId();
		argumentId2 = GetNextId();

		/*
		Before PS 1.4 this is a single result register which means the image will always be a binding and the texcoord will always an input.
		That means we'll always need to load before doing a OpImageFetch on ps 1.0 through 1.3.
		*/
		mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2_temp); //pointer (Id)	

		mFunctionDefinitionInstructions.push_back(Pack(4, spv::OpLoad)); //size,Type
		mFunctionDefinitionInstructions.push_back(resultTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1_temp); //pointer (Id)	

		PrintTokenInformation("TEX_1_0", resultToken, resultToken, resultToken);
	}

	resultId = GetNextId();

	//I don't know what swizzle will be done but it will likely be a vec4 output so I need to use a shuffle to get a vec2.
	argumentId2_temp = argumentId2;
	argumentId2 = GetNextId();
	mFunctionDefinitionInstructions.push_back(Pack(5 + 2, spv::OpVectorShuffle)); //size,Type
	mFunctionDefinitionInstructions.push_back(texcoordDataTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(argumentId2); // Result (Id)
	mFunctionDefinitionInstructions.push_back(argumentId2_temp); //Vector1 (Id)
	mFunctionDefinitionInstructions.push_back(argumentId2_temp); //Vector2 (Id)
	mFunctionDefinitionInstructions.push_back(0); //Component Literal
	mFunctionDefinitionInstructions.push_back(1); //Component Literal

	//Sample image
	mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpImageSampleImplicitLod)); //size,Type
	mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
	mFunctionDefinitionInstructions.push_back(argumentId1); //Image (Id)
	mFunctionDefinitionInstructions.push_back(argumentId2); //Coordinate (Id)

	if (mMajorVersion == 1)
	{
		registerName = "T" + std::to_string(resultToken.DestinationParameterToken.RegisterNumber);
		stringWordSize = 2 + std::max(registerName.length() / 4, 1U);
		if (registerName.length() % 4 == 0)
		{
			stringWordSize++;
		}
		mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
		mNameInstructions.push_back(resultId); //target (Id)
		PutStringInVector(registerName, mNameInstructions); //Literal

		mTextures[resultToken.DestinationParameterToken.RegisterNumber] = resultId;
	}

	resultId = ApplyWriteMask(resultToken, resultId);
}

/*
I don't know why SPIR-V doesn't have a MAD instruction.
*/
void ShaderConverter::Process_MAD()
{
	TypeDescription typeDescription;
	spv::Op dataType;
	uint32_t dataTypeId;
	uint32_t argumentId1;
	uint32_t argumentId2;
	uint32_t argumentId3;
	uint32_t resultId;
	uint32_t resultId2;

	Token resultToken = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE resultRegisterType = GetRegisterType(resultToken.i);

	Token argumentToken1 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType1 = GetRegisterType(argumentToken1.i);

	Token argumentToken2 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType2 = GetRegisterType(argumentToken2.i);

	Token argumentToken3 = GetNextToken();
	_D3DSHADER_PARAM_REGISTER_TYPE argumentRegisterType3 = GetRegisterType(argumentToken3.i);

	typeDescription = GetTypeByRegister(argumentToken1); //use argument type because result type may not be known.
	mIdTypePairs[mNextId] = typeDescription; //snag next id before increment.

	dataType = typeDescription.PrimaryType;

	//Type could be pointer and matrix so checks are run separately.
	if (typeDescription.PrimaryType == spv::OpTypePointer)
	{
		//Shift the result type so we get a register instead of a pointer as the output type.
		typeDescription.PrimaryType = typeDescription.SecondaryType;
		typeDescription.SecondaryType = typeDescription.TernaryType;
		typeDescription.TernaryType = spv::OpTypeVoid;
	}

	if (typeDescription.PrimaryType == spv::OpTypeMatrix || typeDescription.PrimaryType == spv::OpTypeVector)
	{
		dataType = typeDescription.SecondaryType;
	}

	dataTypeId = GetSpirVTypeId(typeDescription);
	argumentId1 = GetSwizzledId(argumentToken1);
	argumentId2 = GetSwizzledId(argumentToken2);
	argumentId3 = GetSwizzledId(argumentToken3);

	resultId = GetNextId();
	resultId2 = GetNextId();

	switch (dataType)
	{
	case spv::OpTypeBool:
		//Write out multiply
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIMul)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)

		//Write out add
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIAdd)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId2); //result (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId3); //argument2 (Id)

		break;
	case spv::OpTypeInt:
		//Write out multiply
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIMul)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)

		//Write out add
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpIAdd)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId2); //result (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId3); //argument2 (Id)

		break;
	case spv::OpTypeFloat:
		//Write out multiply
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFMul)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //result (Id)
		mFunctionDefinitionInstructions.push_back(argumentId1); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId2); //argument2 (Id)

		//Write out add
		mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFAdd)); //size,Type
		mFunctionDefinitionInstructions.push_back(dataTypeId); //Result Type (Id)
		mFunctionDefinitionInstructions.push_back(resultId2); //result (Id)
		mFunctionDefinitionInstructions.push_back(resultId); //argument1 (Id)
		mFunctionDefinitionInstructions.push_back(argumentId3); //argument2 (Id)

		break;
	default:
		BOOST_LOG_TRIVIAL(warning) << "Process_MAD - Unsupported data type " << dataType;
		break;
	}

	resultId2 = ApplyWriteMask(resultToken, resultId2);

}

ConvertedShader ShaderConverter::Convert(uint32_t* shader)
{
	//mConvertedShader = {};
	mInstructions.clear();

	uint32_t stringWordSize = 0;
	uint32_t token = 0;
	uint32_t instruction = 0;
	mBaseToken = mNextToken = mPreviousToken = shader;

	token = GetNextToken().i;
	mPreviousToken--; //Make Previous token one behind the current token.
	mMajorVersion = D3DSHADER_VERSION_MAJOR(token);
	mMinorVersion = D3DSHADER_VERSION_MINOR(token);

	if ((token & 0xFFFF0000) == 0xFFFF0000)
	{
		mIsVertexShader = false;
		BOOST_LOG_TRIVIAL(info) << "PS " << mMajorVersion << "." << mMinorVersion;
	}
	else
	{
		mIsVertexShader = true;
		BOOST_LOG_TRIVIAL(info) << "VS" << mMajorVersion << "." << mMinorVersion;
	}
	//Probably more info in this word but I'll handle that later.


	//Capability
	mCapabilityInstructions.push_back(Pack(2, spv::OpCapability)); //size,Type
	mCapabilityInstructions.push_back(spv::CapabilityShader); //Capability

	//Import
	mGlslExtensionId = GetNextId();
	std::string importStatement = "GLSL.std.450";
	//The spec says 3+variable but there are only 2 before string literal.
	stringWordSize = 2 + (importStatement.length() / 4);
	if (importStatement.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mExtensionInstructions.push_back(Pack(stringWordSize, spv::OpExtInstImport)); //size,Type
	mExtensionInstructions.push_back(mGlslExtensionId); //Result Id
	PutStringInVector(importStatement, mExtensionInstructions);

	//Memory Model
	mMemoryModelInstructions.push_back(Pack(3, spv::OpMemoryModel)); //size,Type
	mMemoryModelInstructions.push_back(spv::AddressingModelLogical); //Addressing Model
	mMemoryModelInstructions.push_back(spv::MemoryModelGLSL450); //Memory Model

	//mSourceInstructions
	mSourceInstructions.push_back(Pack(3, spv::OpSource)); //size,Type
	mSourceInstructions.push_back(spv::SourceLanguageGLSL); //Source Language
	mSourceInstructions.push_back(400); //Version

	std::string sourceExtension1 = "GL_ARB_separate_shader_objects";
	stringWordSize = 2 + (sourceExtension1.length() / 4);
	if (sourceExtension1.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mSourceExtensionInstructions.push_back(Pack(stringWordSize, spv::OpSourceExtension)); //size,Type
	PutStringInVector(sourceExtension1, mSourceExtensionInstructions);

	std::string sourceExtension2 = "GL_ARB_shading_language_420pack";
	stringWordSize = 2 + (sourceExtension2.length() / 4);
	if (sourceExtension2.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mSourceExtensionInstructions.push_back(Pack(stringWordSize, spv::OpSourceExtension)); //size,Type
	PutStringInVector(sourceExtension2, mSourceExtensionInstructions);

	std::string sourceExtension3 = "GL_GOOGLE_cpp_style_line_directive";
	stringWordSize = 2 + (sourceExtension3.length() / 4);
	if (sourceExtension3.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mSourceExtensionInstructions.push_back(Pack(stringWordSize, spv::OpSourceExtension)); //size,Type
	PutStringInVector(sourceExtension3, mSourceExtensionInstructions);

	std::string sourceExtension4 = "GL_GOOGLE_include_directive";
	stringWordSize = 2 + (sourceExtension4.length() / 4);
	if (sourceExtension4.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mSourceExtensionInstructions.push_back(Pack(stringWordSize, spv::OpSourceExtension)); //size,Type
	PutStringInVector(sourceExtension4, mSourceExtensionInstructions);

	GenerateConstantBlock();

	//Start of entry point
	mEntryPointTypeId = GetNextId();
	mEntryPointId = GetNextId();

	mFunctionDefinitionInstructions.push_back(Pack(5, spv::OpFunction)); //size,Type
	mFunctionDefinitionInstructions.push_back(GetSpirVTypeId(spv::OpTypeVoid)); //Result Type (Id)
	mFunctionDefinitionInstructions.push_back(mEntryPointId); //result (Id)
	mFunctionDefinitionInstructions.push_back(spv::FunctionControlMaskNone); //Function Control
	mFunctionDefinitionInstructions.push_back(mEntryPointTypeId); //Function Type (Id)

	mFunctionDefinitionInstructions.push_back(Pack(2, spv::OpLabel)); //size,Type
	mFunctionDefinitionInstructions.push_back(GetNextId()); //result (Id)

	if (mIsVertexShader)
	{
		GeneratePostition();
	}

	Generate255Constants();

	if (mIsVertexShader)
	{
		GeneratePushConstant();
	}

	//Read DXBC instructions
	while (token != D3DPS_END())
	{
		mTokenOffset = mNextToken - shader;
		token = GetNextToken().i;
		instruction = GetOpcode(token);

		switch (instruction)
		{
		case D3DSIO_NOP:
			//Nothing
			break;
		case D3DSIO_PHASE:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_PHASE.";
			break;
		case D3DSIO_RET:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_RET.";
			break;
		case D3DSIO_ENDLOOP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_ENDLOOP.";
			break;
		case D3DSIO_BREAK:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_BREAK.";
			break;
		case D3DSIO_TEXDEPTH:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXDEPTH.";
			break;
		case D3DSIO_TEXKILL:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXKILL.";
			break;
		case D3DSIO_BEM:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_BEM.";
			break;
		case D3DSIO_TEXBEM:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXBEM.";
			break;
		case D3DSIO_TEXBEML:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXBEML.";
			break;
		case D3DSIO_TEXDP3:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXDP3.";
			break;
		case D3DSIO_TEXDP3TEX:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXDP3TEX.";
			break;
		case D3DSIO_TEXM3x2DEPTH:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x2DEPTH.";
			break;
		case D3DSIO_TEXM3x2TEX:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x2TEX.";
			break;
		case D3DSIO_TEXM3x3:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3.";
			break;
		case D3DSIO_TEXM3x3PAD:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3PAD.";
			break;
		case D3DSIO_TEXM3x3TEX:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3TEX.";
			break;
		case D3DSIO_TEXM3x3VSPEC:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3VSPEC.";
			break;
		case D3DSIO_TEXREG2AR:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXREG2AR.";
			break;
		case D3DSIO_TEXREG2GB:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXREG2GB.";
			break;
		case D3DSIO_TEXREG2RGB:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXREG2RGB.";
			break;
		case D3DSIO_LABEL:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_LABEL.";
			break;
		case D3DSIO_CALL:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_CALL.";
			break;
		case D3DSIO_LOOP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_LOOP.";
			break;
		case D3DSIO_BREAKP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_BREAKP.";
			break;
		case D3DSIO_DSX:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_DSX.";
			break;
		case D3DSIO_DSY:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_DSY.";
			break;
		case D3DSIO_IFC:
			Process_IFC();
			break;
		case D3DSIO_IF:
			Process_IF();
			break;
		case D3DSIO_ELSE:
			Process_ELSE();
			break;
		case D3DSIO_ENDIF:
			Process_ENDIF();
			break;
		case D3DSIO_REP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_REP.";
			break;
		case D3DSIO_ENDREP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_ENDREP.";
			break;
		case D3DSIO_NRM:
			Process_NRM();
			break;
		case D3DSIO_MOVA:
			Process_MOVA();
			break;
		case D3DSIO_MOV:
			Process_MOV();
			break;
		case D3DSIO_RCP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_RCP.";
			break;
		case D3DSIO_RSQ:
			Process_RSQ();
			break;
		case D3DSIO_EXP:
			Process_EXP();
			break;
		case D3DSIO_EXPP:
			Process_EXPP();
			break;
		case D3DSIO_LOG:
			Process_LOG();
			break;
		case D3DSIO_LOGP:
			Process_LOGP();
			break;
		case D3DSIO_FRC:
			Process_FRC();
			break;
		case D3DSIO_LIT:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_LIT.";
			break;
		case D3DSIO_ABS:
			Process_ABS();
			break;
		case D3DSIO_TEXM3x3SPEC:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXM3x3SPEC.";
			break;
		case D3DSIO_M4x4:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_M4x4.";
			break;
		case D3DSIO_M4x3:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_M4x3.";
			break;
		case D3DSIO_M3x4:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_M3x4.";
			break;
		case D3DSIO_M3x3:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_M3x3.";
			break;
		case D3DSIO_M3x2:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_M3x2.";
			break;
		case D3DSIO_CALLNZ:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_CALLNZ.";
			break;
		case D3DSIO_SETP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SETP.";
			break;
		case D3DSIO_BREAKC:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_BREAKC.";
			break;
		case D3DSIO_ADD:
			Process_ADD();
			break;
		case D3DSIO_SUB:
			Process_SUB();
			break;
		case D3DSIO_MUL:
			Process_MUL();
			break;
		case D3DSIO_DP3:
			Process_DP3();
			break;
		case D3DSIO_DP4:
			Process_DP4();
			break;
		case D3DSIO_MIN:
			Process_MIN();
			break;
		case D3DSIO_MAX:
			Process_MAX();
			break;
		case D3DSIO_DST:
			Process_DST();
			break;
		case D3DSIO_SLT:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SLT.";
			break;
		case D3DSIO_SGE:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SGE.";
			break;
		case D3DSIO_CRS:
			Process_CRS();
			break;
		case D3DSIO_POW:
			Process_POW();
			break;
		case D3DSIO_DP2ADD:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_DP2ADD.";
			break;
		case D3DSIO_LRP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_LRP.";
			break;
		case D3DSIO_SGN:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SGN.";
			break;
		case D3DSIO_CND:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_CND.";
			break;
		case D3DSIO_CMP:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_CMP.";
			break;
		case D3DSIO_SINCOS:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_SINCOS.";
			break;
		case D3DSIO_MAD:
			Process_MAD();
			break;
		case D3DSIO_TEXLDD:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXLDD.";
			break;
		case D3DSIO_TEXCOORD:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXCOORD.";
			break;
		case D3DSIO_TEX:
			Process_TEX();
			break;
		case D3DSIO_TEXLDL:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction D3DSIO_TEXLDL.";
			break;
		case D3DSIO_DCL:
			Process_DCL();
			break;
		case D3DSIO_DEFB:
			Process_DEFB();
			break;
		case D3DSIO_DEFI:
			Process_DEFI();
			break;
		case D3DSIO_DEF:
			Process_DEF();
			break;
		case D3DSIO_COMMENT:
			SkipTokens(((token & 0x0fff0000) >> 16));
			break;
		case D3DSIO_END:
			//Nothing
			break;
		default:
			BOOST_LOG_TRIVIAL(warning) << "Unsupported instruction " << instruction << ".";
			break;
		}

	}

	if (mIsVertexShader)
	{
		GenerateYFlip();
	}

	//After inputs & outputs are defined set the function type with the type id defined earlier.
	GetSpirVTypeId(spv::OpTypeFunction, mEntryPointTypeId);

	//End of entry point
	mFunctionDefinitionInstructions.push_back(Pack(1, spv::OpReturn)); //size,Type
	mFunctionDefinitionInstructions.push_back(Pack(1, spv::OpFunctionEnd)); //size,Type

	uint32_t outputIndex = 0;

	//EntryPoint
	std::string entryPointName = "main";
	std::vector<uint32_t> interfaceIds;
	typedef boost::container::flat_map<uint32_t, uint32_t> maptype2;
	for (const auto& inputRegister : mInputRegisters)
	{
		interfaceIds.push_back(inputRegister);
		BOOST_LOG_TRIVIAL(info) << inputRegister << " (Input)";
	}
	for (const auto& outputRegister : mOutputRegisters)
	{
		interfaceIds.push_back(outputRegister);
		BOOST_LOG_TRIVIAL(info) << outputRegister << " (Output)";
	}

	//The spec says 4+variable but there are only 3 before the string literal.
	stringWordSize = 3 + (entryPointName.length() / 4) + interfaceIds.size();
	if (entryPointName.length() % 4 == 0)
	{
		stringWordSize++;
	}
	mEntryPointInstructions.push_back(Pack(stringWordSize, spv::OpEntryPoint)); //size,Type
	if (mIsVertexShader)
	{
		mEntryPointInstructions.push_back(spv::ExecutionModelVertex); //Execution Model
	}
	else
	{
		mEntryPointInstructions.push_back(spv::ExecutionModelFragment); //Execution Model
	}
	mEntryPointInstructions.push_back(mEntryPointId); //Entry Point (Id)
	PutStringInVector(entryPointName, mEntryPointInstructions); //Name
	mEntryPointInstructions.insert(std::end(mEntryPointInstructions), std::begin(interfaceIds), std::end(interfaceIds)); //Interfaces

	//Write entry point name.
	stringWordSize = 3 + (entryPointName.length() / 4);
	mNameInstructions.push_back(Pack(stringWordSize, spv::OpName));
	mNameInstructions.push_back(mEntryPointId); //target (Id)
	PutStringInVector(entryPointName, mNameInstructions); //Literal

	//ExecutionMode
	if (!mIsVertexShader)
	{
		mExecutionModeInstructions.push_back(Pack(3, spv::OpExecutionMode)); //size,Type
		mExecutionModeInstructions.push_back(mEntryPointId); //Entry Point (Id)
		mExecutionModeInstructions.push_back(spv::ExecutionModeOriginUpperLeft); //Execution Mode
	}
	else
	{
		//TODO: figure out what to do for vertex execution mode.
	}


	//Write SPIR-V header
	const GeneratorMagicNumber generatorMagicNumber = { 1,13 };

	mInstructions.push_back(spv::MagicNumber);
	mInstructions.push_back(0x00010000); //spv::Version
	mInstructions.push_back(generatorMagicNumber.Word); //I don't have a generator number ATM.
	mInstructions.push_back(GetNextId()); //Bound
	mInstructions.push_back(0); //Reserved for instruction schema, if needed

	//Dump other opcodes into instruction collection is required order.
	CombineSpirVOpCodes();

	//Pass the word blob to Vulkan to generate a module.
	CreateSpirVModule();

	return mConvertedShader; //Return value optimization don't fail me now.
}
