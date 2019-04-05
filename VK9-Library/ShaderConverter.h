#pragma once

/*
Copyright(c) 2016-2019 Christopher Joseph Dean Schaefer

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

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_sdk_platform.h>
#include <spirv.hpp>
#include <GLSL.std.450.h>

#include "d3d11shader.h"
#include "d3d9.h"

#include<vector>
#include <unordered_map>
#include <map>
#include <stack>
#include <memory>
#include <fstream>

/*
http://timjones.io/blog/archive/2015/09/02/parsing-direct3d-shader-bytecode
https://msdn.microsoft.com/en-us/library/bb219840(VS.85).aspx#Shader_Binary_Format
http://stackoverflow.com/questions/2545704/format-of-compiled-directx9-shader-files
https://msdn.microsoft.com/en-us/library/windows/hardware/ff552891(v=vs.85).aspx
https://github.com/ValveSoftware/ToGL
*/

#ifdef _DEBUG
#define _EXTRA_SHADER_DEBUG_INFO
#endif

#define PACK(c0, c1, c2, c3) \
    (((uint32_t)(uint8_t)(c0) << 24) | \
    ((uint32_t)(uint8_t)(c1) << 16) | \
    ((uint32_t)(uint8_t)(c2) << 8) | \
    ((uint32_t)(uint8_t)(c3)))

#ifndef _MSC_VER
// Used D3D9 constants, missing in MinGW

#define D3DSP_OPCODESPECIFICCONTROL_MASK  0x00ff0000
#define D3DSP_OPCODESPECIFICCONTROL_SHIFT 16

#endif

/*
DXBC allows you to use the register for the first vector in a matrix in matrix instructions.
These enums will allow the requestor to let methods down the stack know which one we want.
*/
#define GIVE_ME_SCALAR 0
#define GIVE_ME_VECTOR_4 1
#define GIVE_ME_VECTOR_3 2
#define GIVE_ME_MATRIX_4X4 3
#define GIVE_ME_MATRIX_3X3 4
#define GIVE_ME_SAMPLER 5
#define GIVE_ME_VECTOR_2 6

/*
Generator's magic number. It is associated with the tool that generated
the module. Its value does not affect any semantics, and is allowed to be 0.
Using a non-0 value is encouraged, and can be registered with
Khronos at https://www.khronos.org/registry/spir-v/api/spir-v.xml.
*/
#define SPIR_V_GENERATORS_NUMBER 0x00000000

//https://msdn.microsoft.com/en-us/library/windows/hardware/ff552738(v=vs.85).aspx
union DestinationParameterToken
{
	struct
	{
		uint32_t RegisterNumber : 11;
		uint32_t RegisterType2 : 2;
		bool UseRelativeAddressing : 1;
		uint32_t Reserved : 2;
		uint32_t WriteMask : 4;
		uint32_t ResultModifier : 4;
		uint32_t ResultShiftScale : 4;
		uint32_t RegisterType1 : 3;
		uint32_t FinalBit : 1;
	};
	uint32_t AllField;
};

union OpcodeDescription
{
	struct
	{
		spv::Op Opcode : 16;
		uint32_t WordCount : 16;
	};
	uint32_t Word;
};

union GeneratorMagicNumber
{
	struct
	{
		uint16_t Version;
		uint16_t Type;
	};
	uint32_t Word;
};

//In DXBC can have float's in the DWORD stream so it was either this or break alias rules.
union Token
{
	float f;
	uint32_t i;
	DestinationParameterToken DestinationParameterToken;
};

struct TypeDescription
{
	spv::Op PrimaryType = spv::OpTypeVoid;
	spv::Op SecondaryType = spv::OpTypeVoid;
	spv::Op TernaryType = spv::OpTypeVoid;
	spv::StorageClass StorageClass = spv::StorageClassOutput;
	uint32_t ComponentCount = 0;
	std::vector<uint32_t> Arguments;

	bool operator ==(const TypeDescription &value) const;
	bool operator !=(const TypeDescription &value) const;
	bool operator <(const TypeDescription &value) const;
};
std::ostream& operator<< (std::ostream& os, TypeDescription& typeDescription);

struct TypeDescriptionComparator
{
	bool operator()(const TypeDescription& a, const TypeDescription& b) const;
};

typedef enum
{
	UNDEF = 0,
	REP,
	LOOP
} Loop_t;

struct LoopIds
{
	Loop_t   LoopType;
	uint32_t VariableId;
	uint32_t CounterId;

	uint32_t PreLoopId;

	uint32_t PreMergeLabelId;

	uint32_t PreExecuteLabelId;

	uint32_t EndLabelId;
};

uint32_t Pack(uint32_t wordCount, spv::Op opcode);
void PutStringInVector(std::string& text, std::vector<uint32_t>& words);
_D3DSHADER_PARAM_REGISTER_TYPE GetRegisterType(uint32_t token);
TypeDescription GetPointerComponentType(const TypeDescription& inputType);
TypeDescription GetComponentType(const TypeDescription& inputType);
TypeDescription GetValueType(const TypeDescription& inputType);
const char* GetRegisterTypeName(_D3DSHADER_PARAM_REGISTER_TYPE token);
const char* GetRegisterTypeName(uint32_t token);
const char* GetUsageName(_D3DDECLUSAGE usage);
uint32_t GetOpcode(uint32_t token);
uint32_t GetOpcodeData(uint32_t token);
uint32_t GetTextureType(uint32_t token);
uint32_t GetRegisterNumber(const Token& token);
uint32_t GetUsage(uint32_t token);
uint32_t GetUsageIndex(uint32_t token);
void PrintTokenInformation(const char* tokenName);
void PrintTokenInformation(const char* tokenName, Token argument1);
void PrintTokenInformation(const char* tokenName, Token result, Token argument1);
void PrintTokenInformation(const char* tokenName, Token result, uint32_t resultId, Token argument1, uint32_t argument1Id);
void PrintTokenInformation(const char* tokenName, Token result, Token argument1, Token argument2);
void PrintTokenInformation(const char* tokenName, Token result, Token argument1, Token argument2, Token argument3);

class ShaderConverter
{
protected:
	vk::Device& mDevice;
public:
	ShaderConverter(vk::Device& device);
	~ShaderConverter();

	vk::UniqueShaderModule Convert(uint32_t* shader);
private:
	std::vector<uint32_t> mInstructions; //used to store the combined instructions for creating a module.

	std::unordered_map<D3DSHADER_PARAM_REGISTER_TYPE, std::unordered_map<uint32_t, uint32_t> > mRegistersById;
	std::unordered_map<D3DSHADER_PARAM_REGISTER_TYPE, std::unordered_map<uint32_t, uint32_t> > mIdsByRegister;

	std::vector<uint32_t> mInputRegisters;
	std::vector<uint32_t> mOutputRegisters;
	std::unordered_map<_D3DDECLUSAGE, uint32_t> mOutputRegisterUsages;
	std::unordered_map<uint32_t, _D3DDECLUSAGE> mUsages;

	std::map<TypeDescription, uint32_t> mTypeIdPairs; //Using map because TypeDescription isn't supported by std::hash
	std::unordered_map<uint32_t, TypeDescription> mIdTypePairs;
	std::unordered_map<uint32_t, uint32_t> mVector4Matrix3X3Pairs;
	std::unordered_map<uint32_t, uint32_t> mVector4Matrix4X4Pairs;
	std::unordered_map<uint32_t, uint32_t> mVector4Vector3Pairs;
	std::unordered_map<uint32_t, std::string> mNameIdPairs;

	std::vector<uint32_t> mCapabilityInstructions;
	std::vector<uint32_t> mExtensionInstructions;
	std::vector<uint32_t> mImportExtendedInstructions;
	std::vector<uint32_t> mMemoryModelInstructions;
	std::vector<uint32_t> mEntryPointInstructions;
	std::vector<uint32_t> mExecutionModeInstructions;

	std::vector<uint32_t> mStringInstructions;
	std::vector<uint32_t> mSourceExtensionInstructions;
	std::vector<uint32_t> mSourceInstructions;
	std::vector<uint32_t> mSourceContinuedInstructions;
	std::vector<uint32_t> mNameInstructions;
	std::vector<uint32_t> mMemberNameInstructions;

	std::vector<uint32_t> mDecorateInstructions;
	std::vector<uint32_t> mMemberDecorateInstructions;
	std::vector<uint32_t> mGroupDecorateInstructions;
	std::vector<uint32_t> mGroupMemberDecorateInstructions;
	std::vector<uint32_t> mDecorationGroupInstructions;

	std::vector<uint32_t> mTypeInstructions;
	std::vector<uint32_t> mFunctionDeclarationInstructions;
	std::vector<uint32_t> mFunctionDefinitionInstructions;

	std::stack<uint32_t> mFalseLabels;
	size_t mFalseLabelCount = 0;
	std::stack<uint32_t> mEndIfLabels;

	std::stack<LoopIds> mLoopIds;

	uint32_t* mBaseToken = nullptr;
	uint32_t* mPreviousToken = nullptr;
	uint32_t* mNextToken = nullptr;
	uint32_t mBaseId = 1;
	uint32_t mNextId = 1;
	uint32_t mTokenOffset = 0;
	uint32_t mMinorVersion = 0;
	uint32_t mMajorVersion = 0;
	uint32_t mPositionRegister = -1;
	uint32_t mEntryPointTypeId = -1;
	uint32_t mEntryPointId = -1;
	bool mIsVertexShader = false;
	uint32_t mGlslExtensionId = 0;
	uint32_t mPositionYId = 0;

	uint32_t m255FloatId = 0;
	uint32_t m255VectorId = 0;
	uint32_t mYFlipVectorId = 0;

	uint32_t m0fId = 0;
	uint32_t m1fId = 0;
	uint32_t mn1fId = 0;

	uint32_t m0vfId = 0;
	uint32_t m1vfId = 0;

	//uint32_t m0Id = 0;
	//uint32_t m1Id = 0;	
	//uint32_t m2Id = 0;
	//uint32_t m3Id = 0;

	uint32_t mConstantIntegerIds[17] = {};

	uint32_t mTemp = 0;

	uint32_t mNumberOfIntegerVectorsInUbo = 16;
	uint32_t mNumberOfBooleansInUbo = 16;
	uint32_t mNumberOfFloatVectorsInUbo = 256;
	uint32_t mUboPointerId = 0;
	uint32_t mRenderStatePointerId = 0;
	uint32_t mTexturesId = 0;
	uint32_t mTextureStagesId = 0;
	uint32_t mTextureStageTypeId = 0;

	uint32_t mTextures[12] = {};

	Token GetNextToken();
	void SkipTokens(uint32_t numberToSkip);
	uint32_t GetNextId();
	void SkipIds(uint32_t numberToSkip);
	uint32_t GetSpirVTypeId(spv::Op registerType, uint32_t id = UINT_MAX);
	uint32_t GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2);
	uint32_t GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2, uint32_t componentCount);
	uint32_t GetSpirVTypeId(spv::Op registerType1, spv::Op registerType2, spv::Op registerType3, uint32_t componentCount);
	uint32_t GetSpirVTypeId(TypeDescription& registerType, uint32_t id = UINT_MAX);
	uint32_t GetNextVersionId(const Token& token);
	uint32_t GetIdByRegister(const Token& token, _D3DSHADER_PARAM_REGISTER_TYPE type = D3DSPR_FORCE_DWORD, _D3DDECLUSAGE usage = D3DDECLUSAGE_TEXCOORD, uint32_t usageIndex = 0);
	void SetIdByRegister(const Token& token, uint32_t id);
	TypeDescription GetTypeByRegister(const Token& token, _D3DDECLUSAGE usage = D3DDECLUSAGE_TEXCOORD);
	uint32_t GetSwizzledId(const Token& token, uint32_t lookingFor);
	uint32_t SwizzlePointer(const Token& token);
	uint32_t ApplyWriteMask(const Token& token, uint32_t inputId, _D3DDECLUSAGE usage = D3DDECLUSAGE_TEXCOORD);

	void GenerateConstantIndices();
	void GenerateStore(const Token& token, uint32_t inputId);
	void GenerateDecoration(uint32_t registerNumber, uint32_t inputId, _D3DDECLUSAGE usage, uint32_t usageIndex, bool isInput);
	void Generate255Constants();
	void GenerateYFlipConstants();
	void GenerateConstantBlock();
	void GenerateRenderStateBlock();
	void GenerateSamplerBlock();
	void GenerateTextureStageBlock();
	void GenerateYFlip();
	void CombineSpirVOpCodes();
	vk::UniqueShaderModule CreateSpirVModule();

	uint32_t FixColor(uint32_t id);
	uint32_t FixPosition(uint32_t id);
	uint32_t ConvertVec4ToVec3(uint32_t id);
	uint32_t ConvertMat4ToMat3(uint32_t id);

	void PushMemberName(uint32_t id, std::string& registerName, uint32_t index);
	void PushName(uint32_t id, std::string& registerName);
	uint32_t PushCompositeExtract(uint32_t baseId, uint32_t index);
	uint32_t PushCompositeExtract(uint32_t resultId, uint32_t baseId, uint32_t index);
	void PushCompositeExtract(uint32_t resultTypeId, uint32_t resultId, uint32_t baseId, uint32_t index);
	uint32_t PushCompositeExtract2(uint32_t baseId, uint32_t index1, uint32_t index2);
	uint32_t PushCompositeExtract2(uint32_t resultId, uint32_t baseId, uint32_t index1, uint32_t index2);
	void PushCompositeExtract2(uint32_t resultTypeId, uint32_t resultId, uint32_t baseId, uint32_t index1, uint32_t index2);
	uint32_t PushAccessChain(uint32_t baseId, uint32_t index);
	uint32_t PushAccessChain(uint32_t resultId, uint32_t baseId, uint32_t index);
	void PushAccessChain(uint32_t resultTypeId, uint32_t resultId, uint32_t baseId, uint32_t indexId);
	void PushInverseSqrt(uint32_t resultTypeId, uint32_t resultId, uint32_t argumentId);
	void PushCos(uint32_t resultTypeId, uint32_t resultId, uint32_t argumentId);
	void PushSin(uint32_t resultTypeId, uint32_t resultId, uint32_t argumentId);
	uint32_t PushLoad(uint32_t pointerId);
	uint32_t PushLoad(uint32_t resultId, uint32_t pointerId);
	void PushLoad(uint32_t resultTypeId, uint32_t resultId, uint32_t pointerId);
	void PushStore(uint32_t pointerId, uint32_t objectId);
	void PushVariable(uint32_t resultTypeId, uint32_t resultId, spv::StorageClass storageClass);

	void Push(spv::Op code);
	void Push(spv::Op code, uint32_t argument1);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, GLSLstd450 argument4, uint32_t argument5);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, GLSLstd450 argument4, uint32_t argument5, uint32_t argument6);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7, uint32_t argument8);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7, uint32_t argument8, uint32_t argument9);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7, uint32_t argument8, uint32_t argument9, uint32_t argument10);
	void Push(spv::Op code, uint32_t argument1, uint32_t argument2, uint32_t argument3, uint32_t argument4, uint32_t argument5, uint32_t argument6, uint32_t argument7, uint32_t argument8, uint32_t argument9, uint32_t argument10, uint32_t argument11);

	//Declare
	void Process_DCL_Pixel();
	void Process_DCL_Vertex();
	void Process_DCL();
	void Process_DEF();
	void Process_DEFI();
	void Process_DEFB();

	//Flow Control Operators
	void Process_IFC(uint32_t extraInfo);
	void Process_IF();
	void Process_ELSE();
	void Process_ENDIF();
	void Process_GENERIC_LOOP();
	void Process_GENERIC_ENDLOOP();
	void Process_REP();
	void Process_ENDREP();
	void Process_LOOP();
	void Process_ENDLOOP();
	void Process_BREAK();

	//Unary Operators
	void Process_NRM();
	void Process_MOV();
	void Process_MOVA();
	void Process_RSQ();
	void Process_RCP();
	void Process_TEXBEM();

	//Binary Operators (mixed unary in here by mistake will sort out some day ... maybe)
	void Process_GenericBinaryOperation(const char* tokenName, spv::Op floatOp, spv::Op integerOp);
	void Process_DST();
	void Process_CRS();
	void Process_POW();
	void Process_MUL();
	void Process_EXP();
	void Process_EXPP();
	void Process_LOG();
	void Process_LOGP();
	void Process_FRC();
	void Process_ABS();
	void Process_ADD();
	void Process_SUB();
	void Process_MIN();
	void Process_MAX();
	void Process_SGE();
	void Process_SLT();
	void Process_DP3();
	void Process_DP4();
	void Process_TEX();
	void Process_TEXKILL();
	void Process_TEXCOORD();

	//matrix operators
	void Process_M4x4();
	void Process_M4x3();
	void Process_M3x4();
	void Process_M3x3();
	void Process_M3x2();

	//ternary operators
	void Process_MAD();
	void Process_CMP();
	void Process_DP2ADD();
	void Process_LRP();
	void Process_SINCOS();
};