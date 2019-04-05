/*
Copyright(c) 2019 Christopher Joseph Dean Schaefer

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

#include "LogManager.h"

static char SeverityText[6][20] =
{
	"[TRACE] ",
	"[DEBUG] ",
	"[INFO] ",
	"[WARNING] ",
	"[ERROR] ",
	"[FATAL] "
};

const std::string mResultStrings[] =
{
	"Unknown",
	"VK_SUCCESS",
	"VK_NOT_READY",
	"VK_TIMEOUT",
	"VK_EVENT_SET",
	"VK_EVENT_RESET",
	"VK_INCOMPLETE",
	"VK_ERROR_OUT_OF_HOST_MEMORY",
	"VK_ERROR_OUT_OF_DEVICE_MEMORY",
	"VK_ERROR_INITIALIZATION_FAILED",
	"VK_ERROR_DEVICE_LOST",
	"VK_ERROR_MEMORY_MAP_FAILED",
	"VK_ERROR_LAYER_NOT_PRESENT",
	"VK_ERROR_EXTENSION_NOT_PRESENT",
	"VK_ERROR_FEATURE_NOT_PRESENT",
	"VK_ERROR_INCOMPATIBLE_DRIVER",
	"VK_ERROR_TOO_MANY_OBJECTS",
	"VK_ERROR_FORMAT_NOT_SUPPORTED",
	"VK_ERROR_FRAGMENTED_POOL",
	"VK_ERROR_SURFACE_LOST_KHR",
	"VK_ERROR_NATIVE_WINDOW_IN_USE_KHR",
	"VK_SUBOPTIMAL_KHR",
	"VK_ERROR_OUT_OF_DATE_KHR",
	"VK_ERROR_INCOMPATIBLE_DISPLAY_KHR",
	"VK_ERROR_VALIDATION_FAILED_EXT",
	"VK_ERROR_INVALID_SHADER_NV",
	"VK_ERROR_OUT_OF_POOL_MEMORY_KHR",
	"VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR",
	"VK_ERROR_NOT_PERMITTED_EXT"
};

std::ostream& operator<< (std::ostream& os, vk::Result result)
{
	switch (result)
	{
	case vk::Result::eSuccess:
		return os << mResultStrings[1];
	case vk::Result::eNotReady:
		return os << mResultStrings[2];
	case vk::Result::eTimeout:
		return os << mResultStrings[3];
	case vk::Result::eEventSet:
		return os << mResultStrings[4];
	case vk::Result::eEventReset:
		return os << mResultStrings[5];
	case vk::Result::eIncomplete:
		return os << mResultStrings[6];
	case vk::Result::eErrorOutOfHostMemory:
		return os << mResultStrings[7];
	case vk::Result::eErrorOutOfDeviceMemory:
		return os << mResultStrings[8];
	case vk::Result::eErrorInitializationFailed:
		return os << mResultStrings[9];
	case vk::Result::eErrorDeviceLost:
		return os << mResultStrings[10];
	case vk::Result::eErrorMemoryMapFailed:
		return os << mResultStrings[11];
	case vk::Result::eErrorLayerNotPresent:
		return os << mResultStrings[12];
	case vk::Result::eErrorExtensionNotPresent:
		return os << mResultStrings[13];
	case vk::Result::eErrorFeatureNotPresent:
		return os << mResultStrings[14];
	case vk::Result::eErrorIncompatibleDriver:
		return os << mResultStrings[15];
	case vk::Result::eErrorTooManyObjects:
		return os << mResultStrings[16];
	case vk::Result::eErrorFormatNotSupported:
		return os << mResultStrings[17];
	case vk::Result::eErrorFragmentedPool:
		return os << mResultStrings[18];
	case vk::Result::eErrorSurfaceLostKHR:
		return os << mResultStrings[19];
	case vk::Result::eErrorNativeWindowInUseKHR:
		return os << mResultStrings[20];
	case vk::Result::eSuboptimalKHR:
		return os << mResultStrings[21];
	case vk::Result::eErrorOutOfDateKHR:
		return os << mResultStrings[22];
	case vk::Result::eErrorIncompatibleDisplayKHR:
		return os << mResultStrings[23];
	case vk::Result::eErrorValidationFailedEXT:
		return os << mResultStrings[24];
	case vk::Result::eErrorInvalidShaderNV:
		return os << mResultStrings[25];
	case vk::Result::eErrorOutOfPoolMemoryKHR:
		return os << mResultStrings[26];
	case vk::Result::eErrorInvalidExternalHandleKHR:
		return os << mResultStrings[27];
	case vk::Result::eErrorNotPermittedEXT:
		return os << mResultStrings[28];
	default:
		return os << mResultStrings[0];
	}
}

std::ostream& operator<< (std::ostream& os, GLSLstd450 code)
{
	switch (code)
	{
	case GLSLstd450Bad: return os << "GLSLstd450Bad";
	case GLSLstd450Round: return os << "GLSLstd450Round";
	case GLSLstd450RoundEven: return os << "GLSLstd450RoundEven";
	case GLSLstd450Trunc: return os << "GLSLstd450Trunc";
	case GLSLstd450FAbs: return os << "GLSLstd450FAbs";
	case GLSLstd450SAbs: return os << "GLSLstd450SAbs";
	case GLSLstd450FSign: return os << "GLSLstd450FSign";
	case GLSLstd450SSign: return os << "GLSLstd450SSign";
	case GLSLstd450Floor: return os << "GLSLstd450Floor";
	case GLSLstd450Ceil: return os << "GLSLstd450Ceil";
	case GLSLstd450Fract: return os << "GLSLstd450Fract";
	case GLSLstd450Radians: return os << "GLSLstd450Radians";
	case GLSLstd450Degrees: return os << "GLSLstd450Degrees";
	case GLSLstd450Sin: return os << "GLSLstd450Sin";
	case GLSLstd450Cos: return os << "GLSLstd450Cos";
	case GLSLstd450Tan: return os << "GLSLstd450Tan";
	case GLSLstd450Asin: return os << "GLSLstd450Asin";
	case GLSLstd450Acos: return os << "GLSLstd450Acos";
	case GLSLstd450Atan: return os << "GLSLstd450Atan";
	case GLSLstd450Sinh: return os << "GLSLstd450Sinh";
	case GLSLstd450Cosh: return os << "GLSLstd450Cosh";
	case GLSLstd450Tanh: return os << "GLSLstd450Tanh";
	case GLSLstd450Asinh: return os << "GLSLstd450Asinh";
	case GLSLstd450Acosh: return os << "GLSLstd450Acosh";
	case GLSLstd450Atanh: return os << "GLSLstd450Atanh";
	case GLSLstd450Atan2: return os << "GLSLstd450Atan2";
	case GLSLstd450Pow: return os << "GLSLstd450Pow";
	case GLSLstd450Exp: return os << "GLSLstd450Exp";
	case GLSLstd450Log: return os << "GLSLstd450Log";
	case GLSLstd450Exp2: return os << "GLSLstd450Exp2";
	case GLSLstd450Log2: return os << "GLSLstd450Log2";
	case GLSLstd450Sqrt: return os << "GLSLstd450Sqrt";
	case GLSLstd450InverseSqrt: return os << "GLSLstd450InverseSqrt";
	case GLSLstd450Determinant: return os << "GLSLstd450Determinant";
	case GLSLstd450MatrixInverse: return os << "GLSLstd450MatrixInverse";
	case GLSLstd450Modf: return os << "GLSLstd450Modf";
	case GLSLstd450ModfStruct: return os << "GLSLstd450ModfStruct";
	case GLSLstd450FMin: return os << "GLSLstd450FMin";
	case GLSLstd450UMin: return os << "GLSLstd450UMin";
	case GLSLstd450SMin: return os << "GLSLstd450SMin";
	case GLSLstd450FMax: return os << "GLSLstd450FMax";
	case GLSLstd450UMax: return os << "GLSLstd450UMax";
	case GLSLstd450SMax: return os << "GLSLstd450SMax";
	case GLSLstd450FClamp: return os << "GLSLstd450FClamp";
	case GLSLstd450UClamp: return os << "GLSLstd450UClamp";
	case GLSLstd450SClamp: return os << "GLSLstd450SClamp";
	case GLSLstd450FMix: return os << "GLSLstd450FMix";
	case GLSLstd450IMix: return os << "GLSLstd450IMix";
	case GLSLstd450Step: return os << "GLSLstd450Step";
	case GLSLstd450SmoothStep: return os << "GLSLstd450SmoothStep";
	case GLSLstd450Fma: return os << "GLSLstd450Fma";
	case GLSLstd450Frexp: return os << "GLSLstd450Frexp";
	case GLSLstd450FrexpStruct: return os << "GLSLstd450FrexpStruct";
	case GLSLstd450Ldexp: return os << "GLSLstd450Ldexp";
	case GLSLstd450PackSnorm4x8: return os << "GLSLstd450PackSnorm4x8";
	case GLSLstd450PackUnorm4x8: return os << "GLSLstd450PackUnorm4x8";
	case GLSLstd450PackSnorm2x16: return os << "GLSLstd450PackSnorm2x16";
	case GLSLstd450PackUnorm2x16: return os << "GLSLstd450PackUnorm2x16";
	case GLSLstd450PackHalf2x16: return os << "GLSLstd450PackHalf2x16";
	case GLSLstd450PackDouble2x32: return os << "GLSLstd450PackDouble2x32";
	case GLSLstd450UnpackSnorm2x16: return os << "GLSLstd450UnpackSnorm2x16";
	case GLSLstd450UnpackUnorm2x16: return os << "GLSLstd450UnpackUnorm2x16";
	case GLSLstd450UnpackHalf2x16: return os << "GLSLstd450UnpackHalf2x16";
	case GLSLstd450UnpackSnorm4x8: return os << "GLSLstd450UnpackSnorm4x8";
	case GLSLstd450UnpackUnorm4x8: return os << "GLSLstd450UnpackUnorm4x8";
	case GLSLstd450UnpackDouble2x32: return os << "GLSLstd450UnpackDouble2x32";
	case GLSLstd450Length: return os << "GLSLstd450Length";
	case GLSLstd450Distance: return os << "GLSLstd450Distance";
	case GLSLstd450Cross: return os << "GLSLstd450Cross";
	case GLSLstd450Normalize: return os << "GLSLstd450Normalize";
	case GLSLstd450FaceForward: return os << "GLSLstd450FaceForward";
	case GLSLstd450Reflect: return os << "GLSLstd450Reflect";
	case GLSLstd450Refract: return os << "GLSLstd450Refract";
	case GLSLstd450FindILsb: return os << "GLSLstd450FindILsb";
	case GLSLstd450FindSMsb: return os << "GLSLstd450FindSMsb";
	case GLSLstd450FindUMsb: return os << "GLSLstd450FindUMsb";
	case GLSLstd450InterpolateAtCentroid: return os << "GLSLstd450InterpolateAtCentroid";
	case GLSLstd450InterpolateAtSample: return os << "GLSLstd450InterpolateAtSample";
	case GLSLstd450InterpolateAtOffset: return os << "GLSLstd450InterpolateAtOffset";
	case GLSLstd450NMin: return os << "GLSLstd450NMin";
	case GLSLstd450NMax: return os << "GLSLstd450NMax";
	case GLSLstd450NClamp: return os << "GLSLstd450NClamp";
	};
	return os << static_cast<std::uint32_t>(code);
}

std::ostream& operator<< (std::ostream& os, spv::Op code)
{
	switch (code)
	{
	case spv::OpNop: return os << "OpNop";
	case spv::OpUndef: return os << "OpUndef";
	case spv::OpSourceContinued: return os << "OpSourceContinued";
	case spv::OpSource: return os << "OpSource";
	case spv::OpSourceExtension: return os << "OpSourceExtension";
	case spv::OpName: return os << "OpName";
	case spv::OpMemberName: return os << "OpMemberName";
	case spv::OpString: return os << "OpString";
	case spv::OpLine: return os << "OpLine";
	case spv::OpExtension: return os << "OpExtension";
	case spv::OpExtInstImport: return os << "OpExtInstImport";
	case spv::OpExtInst: return os << "OpExtInst";
	case spv::OpMemoryModel: return os << "OpMemoryModel";
	case spv::OpEntryPoint: return os << "OpEntryPoint";
	case spv::OpExecutionMode: return os << "OpExecutionMode";
	case spv::OpCapability: return os << "OpCapability";
	case spv::OpTypeVoid: return os << "OpTypeVoid";
	case spv::OpTypeBool: return os << "OpTypeBool";
	case spv::OpTypeInt: return os << "OpTypeInt";
	case spv::OpTypeFloat: return os << "OpTypeFloat";
	case spv::OpTypeVector: return os << "OpTypeVector";
	case spv::OpTypeMatrix: return os << "OpTypeMatrix";
	case spv::OpTypeImage: return os << "OpTypeImage";
	case spv::OpTypeSampler: return os << "OpTypeSampler";
	case spv::OpTypeSampledImage: return os << "OpTypeSampledImage";
	case spv::OpTypeArray: return os << "OpTypeArray";
	case spv::OpTypeRuntimeArray: return os << "OpTypeRuntimeArray";
	case spv::OpTypeStruct: return os << "OpTypeStruct";
	case spv::OpTypeOpaque: return os << "OpTypeOpaque";
	case spv::OpTypePointer: return os << "OpTypePointer";
	case spv::OpTypeFunction: return os << "OpTypeFunction";
	case spv::OpTypeEvent: return os << "OpTypeEvent";
	case spv::OpTypeDeviceEvent: return os << "OpTypeDeviceEvent";
	case spv::OpTypeReserveId: return os << "OpTypeReserveId";
	case spv::OpTypeQueue: return os << "OpTypeQueue";
	case spv::OpTypePipe: return os << "OpTypePipe";
	case spv::OpTypeForwardPointer: return os << "OpTypeForwardPointer";
	case spv::OpConstantTrue: return os << "OpConstantTrue";
	case spv::OpConstantFalse: return os << "OpConstantFalse";
	case spv::OpConstant: return os << "OpConstant";
	case spv::OpConstantComposite: return os << "OpConstantComposite";
	case spv::OpConstantSampler: return os << "OpConstantSampler";
	case spv::OpConstantNull: return os << "OpConstantNull";
	case spv::OpSpecConstantTrue: return os << "OpSpecConstantTrue";
	case spv::OpSpecConstantFalse: return os << "OpSpecConstantFalse";
	case spv::OpSpecConstant: return os << "OpSpecConstant";
	case spv::OpSpecConstantComposite: return os << "OpSpecConstantComposite";
	case spv::OpSpecConstantOp: return os << "OpSpecConstantOp";
	case spv::OpFunction: return os << "OpFunction";
	case spv::OpFunctionParameter: return os << "OpFunctionParameter";
	case spv::OpFunctionEnd: return os << "OpFunctionEnd";
	case spv::OpFunctionCall: return os << "OpFunctionCall";
	case spv::OpVariable: return os << "OpVariable";
	case spv::OpImageTexelPointer: return os << "OpImageTexelPointer";
	case spv::OpLoad: return os << "OpLoad";
	case spv::OpStore: return os << "OpStore";
	case spv::OpCopyMemory: return os << "OpCopyMemory";
	case spv::OpCopyMemorySized: return os << "OpCopyMemorySized";
	case spv::OpAccessChain: return os << "OpAccessChain";
	case spv::OpInBoundsAccessChain: return os << "OpInBoundsAccessChain";
	case spv::OpPtrAccessChain: return os << "OpPtrAccessChain";
	case spv::OpArrayLength: return os << "OpArrayLength";
	case spv::OpGenericPtrMemSemantics: return os << "OpGenericPtrMemSemantics";
	case spv::OpInBoundsPtrAccessChain: return os << "OpInBoundsPtrAccessChain";
	case spv::OpDecorate: return os << "OpDecorate";
	case spv::OpMemberDecorate: return os << "OpMemberDecorate";
	case spv::OpDecorationGroup: return os << "OpDecorationGroup";
	case spv::OpGroupDecorate: return os << "OpGroupDecorate";
	case spv::OpGroupMemberDecorate: return os << "OpGroupMemberDecorate";
	case spv::OpVectorExtractDynamic: return os << "OpVectorExtractDynamic";
	case spv::OpVectorInsertDynamic: return os << "OpVectorInsertDynamic";
	case spv::OpVectorShuffle: return os << "OpVectorShuffle";
	case spv::OpCompositeConstruct: return os << "OpCompositeConstruct";
	case spv::OpCompositeExtract: return os << "OpCompositeExtract";
	case spv::OpCompositeInsert: return os << "OpCompositeInsert";
	case spv::OpCopyObject: return os << "OpCopyObject";
	case spv::OpTranspose: return os << "OpTranspose";
	case spv::OpSampledImage: return os << "OpSampledImage";
	case spv::OpImageSampleImplicitLod: return os << "OpImageSampleImplicitLod";
	case spv::OpImageSampleExplicitLod: return os << "OpImageSampleExplicitLod";
	case spv::OpImageSampleDrefImplicitLod: return os << "OpImageSampleDrefImplicitLod";
	case spv::OpImageSampleDrefExplicitLod: return os << "OpImageSampleDrefExplicitLod";
	case spv::OpImageSampleProjImplicitLod: return os << "OpImageSampleProjImplicitLod";
	case spv::OpImageSampleProjExplicitLod: return os << "OpImageSampleProjExplicitLod";
	case spv::OpImageSampleProjDrefImplicitLod: return os << "OpImageSampleProjDrefImplicitLod";
	case spv::OpImageSampleProjDrefExplicitLod: return os << "OpImageSampleProjDrefExplicitLod";
	case spv::OpImageFetch: return os << "OpImageFetch";
	case spv::OpImageGather: return os << "OpImageGather";
	case spv::OpImageDrefGather: return os << "OpImageDrefGather";
	case spv::OpImageRead: return os << "OpImageRead";
	case spv::OpImageWrite: return os << "OpImageWrite";
	case spv::OpImage: return os << "OpImage";
	case spv::OpImageQueryFormat: return os << "OpImageQueryFormat";
	case spv::OpImageQueryOrder: return os << "OpImageQueryOrder";
	case spv::OpImageQuerySizeLod: return os << "OpImageQuerySizeLod";
	case spv::OpImageQuerySize: return os << "OpImageQuerySize";
	case spv::OpImageQueryLod: return os << "OpImageQueryLod";
	case spv::OpImageQueryLevels: return os << "OpImageQueryLevels";
	case spv::OpImageQuerySamples: return os << "OpImageQuerySamples";
	case spv::OpConvertFToU: return os << "OpConvertFToU";
	case spv::OpConvertFToS: return os << "OpConvertFToS";
	case spv::OpConvertSToF: return os << "OpConvertSToF";
	case spv::OpConvertUToF: return os << "OpConvertUToF";
	case spv::OpUConvert: return os << "OpUConvert";
	case spv::OpSConvert: return os << "OpSConvert";
	case spv::OpFConvert: return os << "OpFConvert";
	case spv::OpQuantizeToF16: return os << "OpQuantizeToF16";
	case spv::OpConvertPtrToU: return os << "OpConvertPtrToU";
	case spv::OpSatConvertSToU: return os << "OpSatConvertSToU";
	case spv::OpSatConvertUToS: return os << "OpSatConvertUToS";
	case spv::OpConvertUToPtr: return os << "OpConvertUToPtr";
	case spv::OpPtrCastToGeneric: return os << "OpPtrCastToGeneric";
	case spv::OpGenericCastToPtr: return os << "OpGenericCastToPtr";
	case spv::OpGenericCastToPtrExplicit: return os << "OpGenericCastToPtrExplicit";
	case spv::OpBitcast: return os << "OpBitcast";
	case spv::OpSNegate: return os << "OpSNegate";
	case spv::OpFNegate: return os << "OpFNegate";
	case spv::OpIAdd: return os << "OpIAdd";
	case spv::OpFAdd: return os << "OpFAdd";
	case spv::OpISub: return os << "OpISub";
	case spv::OpFSub: return os << "OpFSub";
	case spv::OpIMul: return os << "OpIMul";
	case spv::OpFMul: return os << "OpFMul";
	case spv::OpUDiv: return os << "OpUDiv";
	case spv::OpSDiv: return os << "OpSDiv";
	case spv::OpFDiv: return os << "OpFDiv";
	case spv::OpUMod: return os << "OpUMod";
	case spv::OpSRem: return os << "OpSRem";
	case spv::OpSMod: return os << "OpSMod";
	case spv::OpFRem: return os << "OpFRem";
	case spv::OpFMod: return os << "OpFMod";
	case spv::OpVectorTimesScalar: return os << "OpVectorTimesScalar";
	case spv::OpMatrixTimesScalar: return os << "OpMatrixTimesScalar";
	case spv::OpVectorTimesMatrix: return os << "OpVectorTimesMatrix";
	case spv::OpMatrixTimesVector: return os << "OpMatrixTimesVector";
	case spv::OpMatrixTimesMatrix: return os << "OpMatrixTimesMatrix";
	case spv::OpOuterProduct: return os << "OpOuterProduct";
	case spv::OpDot: return os << "OpDot";
	case spv::OpIAddCarry: return os << "OpIAddCarry";
	case spv::OpISubBorrow: return os << "OpISubBorrow";
	case spv::OpUMulExtended: return os << "OpUMulExtended";
	case spv::OpSMulExtended: return os << "OpSMulExtended";
	case spv::OpAny: return os << "OpAny";
	case spv::OpAll: return os << "OpAll";
	case spv::OpIsNan: return os << "OpIsNan";
	case spv::OpIsInf: return os << "OpIsInf";
	case spv::OpIsFinite: return os << "OpIsFinite";
	case spv::OpIsNormal: return os << "OpIsNormal";
	case spv::OpSignBitSet: return os << "OpSignBitSet";
	case spv::OpLessOrGreater: return os << "OpLessOrGreater";
	case spv::OpOrdered: return os << "OpOrdered";
	case spv::OpUnordered: return os << "OpUnordered";
	case spv::OpLogicalEqual: return os << "OpLogicalEqual";
	case spv::OpLogicalNotEqual: return os << "OpLogicalNotEqual";
	case spv::OpLogicalOr: return os << "OpLogicalOr";
	case spv::OpLogicalAnd: return os << "OpLogicalAnd";
	case spv::OpLogicalNot: return os << "OpLogicalNot";
	case spv::OpSelect: return os << "OpSelect";
	case spv::OpIEqual: return os << "OpIEqual";
	case spv::OpINotEqual: return os << "OpINotEqual";
	case spv::OpUGreaterThan: return os << "OpUGreaterThan";
	case spv::OpSGreaterThan: return os << "OpSGreaterThan";
	case spv::OpUGreaterThanEqual: return os << "OpUGreaterThanEqual";
	case spv::OpSGreaterThanEqual: return os << "OpSGreaterThanEqual";
	case spv::OpULessThan: return os << "OpULessThan";
	case spv::OpSLessThan: return os << "OpSLessThan";
	case spv::OpULessThanEqual: return os << "OpULessThanEqual";
	case spv::OpSLessThanEqual: return os << "OpSLessThanEqual";
	case spv::OpFOrdEqual: return os << "OpFOrdEqual";
	case spv::OpFUnordEqual: return os << "OpFUnordEqual";
	case spv::OpFOrdNotEqual: return os << "OpFOrdNotEqual";
	case spv::OpFUnordNotEqual: return os << "OpFUnordNotEqual";
	case spv::OpFOrdLessThan: return os << "OpFOrdLessThan";
	case spv::OpFUnordLessThan: return os << "OpFUnordLessThan";
	case spv::OpFOrdGreaterThan: return os << "OpFOrdGreaterThan";
	case spv::OpFUnordGreaterThan: return os << "OpFUnordGreaterThan";
	case spv::OpFOrdLessThanEqual: return os << "OpFOrdLessThanEqual";
	case spv::OpFUnordLessThanEqual: return os << "OpFUnordLessThanEqual";
	case spv::OpFOrdGreaterThanEqual: return os << "OpFOrdGreaterThanEqual";
	case spv::OpFUnordGreaterThanEqual: return os << "OpFUnordGreaterThanEqual";
	case spv::OpShiftRightLogical: return os << "OpShiftRightLogical";
	case spv::OpShiftRightArithmetic: return os << "OpShiftRightArithmetic";
	case spv::OpShiftLeftLogical: return os << "OpShiftLeftLogical";
	case spv::OpBitwiseOr: return os << "OpBitwiseOr";
	case spv::OpBitwiseXor: return os << "OpBitwiseXor";
	case spv::OpBitwiseAnd: return os << "OpBitwiseAnd";
	case spv::OpNot: return os << "OpNot";
	case spv::OpBitFieldInsert: return os << "OpBitFieldInsert";
	case spv::OpBitFieldSExtract: return os << "OpBitFieldSExtract";
	case spv::OpBitFieldUExtract: return os << "OpBitFieldUExtract";
	case spv::OpBitReverse: return os << "OpBitReverse";
	case spv::OpBitCount: return os << "OpBitCount";
	case spv::OpDPdx: return os << "OpDPdx";
	case spv::OpDPdy: return os << "OpDPdy";
	case spv::OpFwidth: return os << "OpFwidth";
	case spv::OpDPdxFine: return os << "OpDPdxFine";
	case spv::OpDPdyFine: return os << "OpDPdyFine";
	case spv::OpFwidthFine: return os << "OpFwidthFine";
	case spv::OpDPdxCoarse: return os << "OpDPdxCoarse";
	case spv::OpDPdyCoarse: return os << "OpDPdyCoarse";
	case spv::OpFwidthCoarse: return os << "OpFwidthCoarse";
	case spv::OpEmitVertex: return os << "OpEmitVertex";
	case spv::OpEndPrimitive: return os << "OpEndPrimitive";
	case spv::OpEmitStreamVertex: return os << "OpEmitStreamVertex";
	case spv::OpEndStreamPrimitive: return os << "OpEndStreamPrimitive";
	case spv::OpControlBarrier: return os << "OpControlBarrier";
	case spv::OpMemoryBarrier: return os << "OpMemoryBarrier";
	case spv::OpAtomicLoad: return os << "OpAtomicLoad";
	case spv::OpAtomicStore: return os << "OpAtomicStore";
	case spv::OpAtomicExchange: return os << "OpAtomicExchange";
	case spv::OpAtomicCompareExchange: return os << "OpAtomicCompareExchange";
	case spv::OpAtomicCompareExchangeWeak: return os << "OpAtomicCompareExchangeWeak";
	case spv::OpAtomicIIncrement: return os << "OpAtomicIIncrement";
	case spv::OpAtomicIDecrement: return os << "OpAtomicIDecrement";
	case spv::OpAtomicIAdd: return os << "OpAtomicIAdd";
	case spv::OpAtomicISub: return os << "OpAtomicISub";
	case spv::OpAtomicSMin: return os << "OpAtomicSMin";
	case spv::OpAtomicUMin: return os << "OpAtomicUMin";
	case spv::OpAtomicSMax: return os << "OpAtomicSMax";
	case spv::OpAtomicUMax: return os << "OpAtomicUMax";
	case spv::OpAtomicAnd: return os << "OpAtomicAnd";
	case spv::OpAtomicOr: return os << "OpAtomicOr";
	case spv::OpAtomicXor: return os << "OpAtomicXor";
	case spv::OpPhi: return os << "OpPhi";
	case spv::OpLoopMerge: return os << "OpLoopMerge";
	case spv::OpSelectionMerge: return os << "OpSelectionMerge";
	case spv::OpLabel: return os << "OpLabel";
	case spv::OpBranch: return os << "OpBranch";
	case spv::OpBranchConditional: return os << "OpBranchConditional";
	case spv::OpSwitch: return os << "OpSwitch";
	case spv::OpKill: return os << "OpKill";
	case spv::OpReturn: return os << "OpReturn";
	case spv::OpReturnValue: return os << "OpReturnValue";
	case spv::OpUnreachable: return os << "OpUnreachable";
	case spv::OpLifetimeStart: return os << "OpLifetimeStart";
	case spv::OpLifetimeStop: return os << "OpLifetimeStop";
	case spv::OpGroupAsyncCopy: return os << "OpGroupAsyncCopy";
	case spv::OpGroupWaitEvents: return os << "OpGroupWaitEvents";
	case spv::OpGroupAll: return os << "OpGroupAll";
	case spv::OpGroupAny: return os << "OpGroupAny";
	case spv::OpGroupBroadcast: return os << "OpGroupBroadcast";
	case spv::OpGroupIAdd: return os << "OpGroupIAdd";
	case spv::OpGroupFAdd: return os << "OpGroupFAdd";
	case spv::OpGroupFMin: return os << "OpGroupFMin";
	case spv::OpGroupUMin: return os << "OpGroupUMin";
	case spv::OpGroupSMin: return os << "OpGroupSMin";
	case spv::OpGroupFMax: return os << "OpGroupFMax";
	case spv::OpGroupUMax: return os << "OpGroupUMax";
	case spv::OpGroupSMax: return os << "OpGroupSMax";
	case spv::OpReadPipe: return os << "OpReadPipe";
	case spv::OpWritePipe: return os << "OpWritePipe";
	case spv::OpReservedReadPipe: return os << "OpReservedReadPipe";
	case spv::OpReservedWritePipe: return os << "OpReservedWritePipe";
	case spv::OpReserveReadPipePackets: return os << "OpReserveReadPipePackets";
	case spv::OpReserveWritePipePackets: return os << "OpReserveWritePipePackets";
	case spv::OpCommitReadPipe: return os << "OpCommitReadPipe";
	case spv::OpCommitWritePipe: return os << "OpCommitWritePipe";
	case spv::OpIsValidReserveId: return os << "OpIsValidReserveId";
	case spv::OpGetNumPipePackets: return os << "OpGetNumPipePackets";
	case spv::OpGetMaxPipePackets: return os << "OpGetMaxPipePackets";
	case spv::OpGroupReserveReadPipePackets: return os << "OpGroupReserveReadPipePackets";
	case spv::OpGroupReserveWritePipePackets: return os << "OpGroupReserveWritePipePackets";
	case spv::OpGroupCommitReadPipe: return os << "OpGroupCommitReadPipe";
	case spv::OpGroupCommitWritePipe: return os << "OpGroupCommitWritePipe";
	case spv::OpEnqueueMarker: return os << "OpEnqueueMarker";
	case spv::OpEnqueueKernel: return os << "OpEnqueueKernel";
	case spv::OpGetKernelNDrangeSubGroupCount: return os << "OpGetKernelNDrangeSubGroupCount";
	case spv::OpGetKernelNDrangeMaxSubGroupSize: return os << "OpGetKernelNDrangeMaxSubGroupSize";
	case spv::OpGetKernelWorkGroupSize: return os << "OpGetKernelWorkGroupSize";
	case spv::OpGetKernelPreferredWorkGroupSizeMultiple: return os << "OpGetKernelPreferredWorkGroupSizeMultiple";
	case spv::OpRetainEvent: return os << "OpRetainEvent";
	case spv::OpReleaseEvent: return os << "OpReleaseEvent";
	case spv::OpCreateUserEvent: return os << "OpCreateUserEvent";
	case spv::OpIsValidEvent: return os << "OpIsValidEvent";
	case spv::OpSetUserEventStatus: return os << "OpSetUserEventStatus";
	case spv::OpCaptureEventProfilingInfo: return os << "OpCaptureEventProfilingInfo";
	case spv::OpGetDefaultQueue: return os << "OpGetDefaultQueue";
	case spv::OpBuildNDRange: return os << "OpBuildNDRange";
	case spv::OpImageSparseSampleImplicitLod: return os << "OpImageSparseSampleImplicitLod";
	case spv::OpImageSparseSampleExplicitLod: return os << "OpImageSparseSampleExplicitLod";
	case spv::OpImageSparseSampleDrefImplicitLod: return os << "OpImageSparseSampleDrefImplicitLod";
	case spv::OpImageSparseSampleDrefExplicitLod: return os << "OpImageSparseSampleDrefExplicitLod";
	case spv::OpImageSparseSampleProjImplicitLod: return os << "OpImageSparseSampleProjImplicitLod";
	case spv::OpImageSparseSampleProjExplicitLod: return os << "OpImageSparseSampleProjExplicitLod";
	case spv::OpImageSparseSampleProjDrefImplicitLod: return os << "OpImageSparseSampleProjDrefImplicitLod";
	case spv::OpImageSparseSampleProjDrefExplicitLod: return os << "OpImageSparseSampleProjDrefExplicitLod";
	case spv::OpImageSparseFetch: return os << "OpImageSparseFetch";
	case spv::OpImageSparseGather: return os << "OpImageSparseGather";
	case spv::OpImageSparseDrefGather: return os << "OpImageSparseDrefGather";
	case spv::OpImageSparseTexelsResident: return os << "OpImageSparseTexelsResident";
	case spv::OpNoLine: return os << "OpNoLine";
	case spv::OpAtomicFlagTestAndSet: return os << "OpAtomicFlagTestAndSet";
	case spv::OpAtomicFlagClear: return os << "OpAtomicFlagClear";
	case spv::OpImageSparseRead: return os << "OpImageSparseRead";
	case spv::OpSizeOf: return os << "OpSizeOf";
	case spv::OpTypePipeStorage: return os << "OpTypePipeStorage";
	case spv::OpConstantPipeStorage: return os << "OpConstantPipeStorage";
	case spv::OpCreatePipeFromPipeStorage: return os << "OpCreatePipeFromPipeStorage";
	case spv::OpGetKernelLocalSizeForSubgroupCount: return os << "OpGetKernelLocalSizeForSubgroupCount";
	case spv::OpGetKernelMaxNumSubgroups: return os << "OpGetKernelMaxNumSubgroups";
	case spv::OpTypeNamedBarrier: return os << "OpTypeNamedBarrier";
	case spv::OpNamedBarrierInitialize: return os << "OpNamedBarrierInitialize";
	case spv::OpMemoryNamedBarrier: return os << "OpMemoryNamedBarrier";
	case spv::OpModuleProcessed: return os << "OpModuleProcessed";
	case spv::OpExecutionModeId: return os << "OpExecutionModeId";
	case spv::OpDecorateId: return os << "OpDecorateId";
	case spv::OpSubgroupBallotKHR: return os << "OpSubgroupBallotKHR";
	case spv::OpSubgroupFirstInvocationKHR: return os << "OpSubgroupFirstInvocationKHR";
	case spv::OpSubgroupAllKHR: return os << "OpSubgroupAllKHR";
	case spv::OpSubgroupAnyKHR: return os << "OpSubgroupAnyKHR";
	case spv::OpSubgroupAllEqualKHR: return os << "OpSubgroupAllEqualKHR";
	case spv::OpSubgroupReadInvocationKHR: return os << "OpSubgroupReadInvocationKHR";
	case spv::OpGroupIAddNonUniformAMD: return os << "OpGroupIAddNonUniformAMD";
	case spv::OpGroupFAddNonUniformAMD: return os << "OpGroupFAddNonUniformAMD";
	case spv::OpGroupFMinNonUniformAMD: return os << "OpGroupFMinNonUniformAMD";
	case spv::OpGroupUMinNonUniformAMD: return os << "OpGroupUMinNonUniformAMD";
	case spv::OpGroupSMinNonUniformAMD: return os << "OpGroupSMinNonUniformAMD";
	case spv::OpGroupFMaxNonUniformAMD: return os << "OpGroupFMaxNonUniformAMD";
	case spv::OpGroupUMaxNonUniformAMD: return os << "OpGroupUMaxNonUniformAMD";
	case spv::OpGroupSMaxNonUniformAMD: return os << "OpGroupSMaxNonUniformAMD";
	case spv::OpFragmentMaskFetchAMD: return os << "OpFragmentMaskFetchAMD";
	case spv::OpFragmentFetchAMD: return os << "OpFragmentFetchAMD";
	case spv::OpSubgroupShuffleINTEL: return os << "OpSubgroupShuffleINTEL";
	case spv::OpSubgroupShuffleDownINTEL: return os << "OpSubgroupShuffleDownINTEL";
	case spv::OpSubgroupShuffleUpINTEL: return os << "OpSubgroupShuffleUpINTEL";
	case spv::OpSubgroupShuffleXorINTEL: return os << "OpSubgroupShuffleXorINTEL";
	case spv::OpSubgroupBlockReadINTEL: return os << "OpSubgroupBlockReadINTEL";
	case spv::OpSubgroupBlockWriteINTEL: return os << "OpSubgroupBlockWriteINTEL";
	case spv::OpSubgroupImageBlockReadINTEL: return os << "OpSubgroupImageBlockReadINTEL";
	case spv::OpSubgroupImageBlockWriteINTEL: return os << "OpSubgroupImageBlockWriteINTEL";
	case spv::OpDecorateStringGOOGLE: return os << "OpDecorateStringGOOGLE";
	case spv::OpMemberDecorateStringGOOGLE: return os << "OpMemberDecorateStringGOOGLE";
	case spv::OpMax: return os << "OpMax";
	};
	return os << static_cast<std::uint32_t>(code);
}

LogManager* LogManager::mInstance;

LogManager::LogManager(const std::string& filename, SeverityLevel severityLevel)
	: mFileStream(filename),
	mSeverityLevel(severityLevel)
{
}

LogManager::~LogManager() 
{
}

void LogManager::Create(const std::string& filename, SeverityLevel severityLevel)
{
	LogManager::mInstance = new LogManager(filename, severityLevel);
}

void LogManager::Destroy()
{
	delete LogManager::mInstance;
}

LockedStream Log(SeverityLevel severityLevel)
{
	if (severityLevel >= LogManager::mInstance->mSeverityLevel)
	{
		return LockedStream(LogManager::mInstance->mFileStream, LogManager::mInstance->mMutex);
	}
	else
	{
		return LockedStream(LogManager::mInstance->mNullStream, LogManager::mInstance->mMutex);
	}
};