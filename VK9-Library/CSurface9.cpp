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

#include "CSurface9.h"
#include "CDevice9.h"
#include "CTexture9.h"
#include "CCubeTexture9.h"
#include "LogManager.h"
//#include "PrivateTypes.h"

vk::Format ConvertFormat(D3DFORMAT format) noexcept
{
	/*
	https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkFormat
	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172558(v=vs.85).aspx
	*/
	switch (format)
	{
	case D3DFMT_UNKNOWN:
		return  (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_R8G8B8:
		return (vk::Format)VK_FORMAT_R8G8B8_UNORM;
	case D3DFMT_A8R8G8B8:
		return (vk::Format)VK_FORMAT_B8G8R8A8_UNORM;
	case D3DFMT_X8R8G8B8:
		/*
		There is technically no alpha so this may cause problems but I don't see anything that is a 100% match for this format in Vulkan.
		*/
		return (vk::Format)VK_FORMAT_B8G8R8A8_UNORM; //B8G8R8X8_UNORM
	case D3DFMT_R5G6B5:
		return (vk::Format)VK_FORMAT_R5G6B5_UNORM_PACK16; //Reall this is VK_FORMAT_B5G6R5_UNORM_PACK16 but Nvidia doesn't support that format as an attachment so we'll do it the other way around and remap.
	case D3DFMT_X1R5G5B5:
		return (vk::Format)VK_FORMAT_B5G5R5A1_UNORM_PACK16; //B5G5R5X1_UNORM
	case D3DFMT_A1R5G5B5:
		return (vk::Format)VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	case D3DFMT_A4R4G4B4:
		return (vk::Format)VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	case D3DFMT_R3G3B2:
		return (vk::Format)VK_FORMAT_UNDEFINED; //B2G3R3_UNORM:
	case D3DFMT_A8:
		return (vk::Format)VK_FORMAT_R8_UNORM; //A8_UNORM
	case D3DFMT_A8R3G3B2:
		return (vk::Format)VK_FORMAT_UNDEFINED; //B2G3R3A8_UNORM
	case D3DFMT_X4R4G4B4:
		return (vk::Format)VK_FORMAT_UNDEFINED; //B4G4R4X4_UNORM
	case D3DFMT_A2B10G10R10:
		return (vk::Format)VK_FORMAT_UNDEFINED; //R10G10B10A2_UNORM
	case D3DFMT_A8B8G8R8:
		return (vk::Format)VK_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_X8B8G8R8:
		return (vk::Format)VK_FORMAT_R8G8B8A8_UNORM; //R8G8B8X8_UNORM
	case D3DFMT_G16R16:
		return (vk::Format)VK_FORMAT_R16G16_UNORM;
	case D3DFMT_A2R10G10B10:
		return (vk::Format)VK_FORMAT_UNDEFINED; //B10G10R10A2_UNORM
	case D3DFMT_A16B16G16R16:
		return (vk::Format)VK_FORMAT_R16G16B16A16_UNORM;
	case D3DFMT_A8P8:
		return (vk::Format)VK_FORMAT_R8G8_UNORM; //P8_UINT_A8_UNORM
	case D3DFMT_P8:
		return (vk::Format)VK_FORMAT_R8_UINT; //P8_UINT
	case D3DFMT_L8:
		return (vk::Format)VK_FORMAT_R8_UNORM;
	case D3DFMT_A8L8:
		return (vk::Format)VK_FORMAT_R8G8_UNORM; //L8A8_UNORM
	case D3DFMT_A4L4:
		return (vk::Format)VK_FORMAT_UNDEFINED; //L4A4_UNORM
	case D3DFMT_V8U8:
		return (vk::Format)VK_FORMAT_R8G8_SNORM;
	case D3DFMT_L6V5U5:
		return (vk::Format)VK_FORMAT_UNDEFINED; //R5G5_SNORM_L6_UNORM
	case D3DFMT_X8L8V8U8:
		return (vk::Format)VK_FORMAT_UNDEFINED; //R8G8_SNORM_L8X8_UNORM
	case D3DFMT_Q8W8V8U8:
		return (vk::Format)VK_FORMAT_R8G8B8A8_SNORM;
	case D3DFMT_V16U16:
		return (vk::Format)VK_FORMAT_R16G16_SNORM;
	case D3DFMT_A2W10V10U10:
		return (vk::Format)VK_FORMAT_UNDEFINED; //R10G10B10_SNORM_A2_UNORM
	case D3DFMT_UYVY:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_R8G8_B8G8:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_YUY2:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_G8R8_G8B8:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_DXT1:
		return (vk::Format)VK_FORMAT_BC1_RGB_UNORM_BLOCK; //VK_FORMAT_BC1_RGB_UNORM_BLOCK
	case D3DFMT_DXT2:
		return (vk::Format)VK_FORMAT_BC2_UNORM_BLOCK; //VK_FORMAT_BC2_UNORM_BLOCK
	case D3DFMT_DXT3:
		return (vk::Format)VK_FORMAT_BC3_UNORM_BLOCK; //VK_FORMAT_BC3_UNORM_BLOCK
	case D3DFMT_DXT4:
		return (vk::Format)VK_FORMAT_BC4_UNORM_BLOCK; //VK_FORMAT_BC4_UNORM_BLOCK
	case D3DFMT_DXT5:
		return (vk::Format)VK_FORMAT_BC5_UNORM_BLOCK; //VK_FORMAT_BC5_UNORM_BLOCK
	case D3DFMT_D16_LOCKABLE:
		return (vk::Format)VK_FORMAT_D16_UNORM; //D16_LOCKABLE
	case D3DFMT_D32:
		return (vk::Format)VK_FORMAT_D32_SFLOAT; //D32_UNORM
	case D3DFMT_D15S1:
		return (vk::Format)VK_FORMAT_UNDEFINED; //S1_UINT_D15_UNORM
	case D3DFMT_D24S8:
		return (vk::Format)VK_FORMAT_D24_UNORM_S8_UINT;
	case D3DFMT_D24X8:
		return (vk::Format)VK_FORMAT_D24_UNORM_S8_UINT; //X8D24_UNORM
	case D3DFMT_D24X4S4:
		return (vk::Format)VK_FORMAT_UNDEFINED; //S4X4_UINT_D24_UNORM
	case D3DFMT_D16:
		return (vk::Format)VK_FORMAT_D16_UNORM;
	case D3DFMT_D32F_LOCKABLE:
		return (vk::Format)VK_FORMAT_D32_SFLOAT;
	case D3DFMT_D24FS8:
		return (vk::Format)VK_FORMAT_D24_UNORM_S8_UINT; //S8_UINT_D24_SFLOAT
#if !defined(D3D_DISABLE_9EX)
	case D3DFMT_D32_LOCKABLE:
		return (vk::Format)VK_FORMAT_D32_SFLOAT;
	case D3DFMT_S8_LOCKABLE:
		return (vk::Format)VK_FORMAT_S8_UINT;
#endif // !D3D_DISABLE_9EX
	case D3DFMT_L16:
		return (vk::Format)VK_FORMAT_R16_UNORM; //L16_UNORM
	case D3DFMT_VERTEXDATA:
		return (vk::Format)VK_FORMAT_UNDEFINED; //VERTEXDATA
	case D3DFMT_INDEX16:
		return (vk::Format)VK_FORMAT_R16_UINT;
	case D3DFMT_INDEX32:
		return (vk::Format)VK_FORMAT_R32_UINT;
	case D3DFMT_Q16W16V16U16:
		return (vk::Format)VK_FORMAT_R16G16B16A16_SNORM;
	case D3DFMT_MULTI2_ARGB8:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_R16F:
		return (vk::Format)VK_FORMAT_R16_SFLOAT;
	case D3DFMT_G16R16F:
		return (vk::Format)VK_FORMAT_R16G16_SFLOAT;
	case D3DFMT_A16B16G16R16F:
		return (vk::Format)VK_FORMAT_R16G16B16A16_SFLOAT;
	case D3DFMT_R32F:
		return (vk::Format)VK_FORMAT_R32_SFLOAT;
	case D3DFMT_G32R32F:
		return (vk::Format)VK_FORMAT_R32G32_SFLOAT;
	case D3DFMT_A32B32G32R32F:
		return (vk::Format)VK_FORMAT_R32G32B32A32_SFLOAT;
	case D3DFMT_CxV8U8:
		return (vk::Format)VK_FORMAT_R8G8_SNORM;
#if !defined(D3D_DISABLE_9EX)
	case D3DFMT_A1:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_A2B10G10R10_XR_BIAS:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	case D3DFMT_BINARYBUFFER:
		return (vk::Format)VK_FORMAT_UNDEFINED;
#endif // !D3D_DISABLE_9EX
	case D3DFMT_FORCE_DWORD:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	default:
		return (vk::Format)VK_FORMAT_UNDEFINED;
	}
}

size_t SizeOf(vk::Format format) noexcept
{
	switch (format)
	{
	case vk::Format::eUndefined: //VK_FORMAT_UNDEFINED,
		return 0;
	case vk::Format::eR4G4UnormPack8: //VK_FORMAT_R4G4_UNORM_PACK8,
		return 1;
	case vk::Format::eR4G4B4A4UnormPack16: //VK_FORMAT_R4G4B4A4_UNORM_PACK16,
		return 2;
	case vk::Format::eB4G4R4A4UnormPack16: //VK_FORMAT_B4G4R4A4_UNORM_PACK16,
		return 2;
	case vk::Format::eR5G6B5UnormPack16: //VK_FORMAT_R5G6B5_UNORM_PACK16,
		return 2;
	case vk::Format::eB5G6R5UnormPack16: //VK_FORMAT_B5G6R5_UNORM_PACK16,
		return 2;
	case vk::Format::eR5G5B5A1UnormPack16: //VK_FORMAT_R5G5B5A1_UNORM_PACK16,
		return 2;
	case vk::Format::eB5G5R5A1UnormPack16: //VK_FORMAT_B5G5R5A1_UNORM_PACK16,
		return 2;
	case vk::Format::eA1R5G5B5UnormPack16: //VK_FORMAT_A1R5G5B5_UNORM_PACK16,
		return 2;
	case vk::Format::eR8Unorm: //VK_FORMAT_R8_UNORM,
		return 1;
	case vk::Format::eR8Snorm: //VK_FORMAT_R8_SNORM,
		return 1;
	case vk::Format::eR8Uscaled: //VK_FORMAT_R8_USCALED,
		return 1;
	case vk::Format::eR8Sscaled: //VK_FORMAT_R8_SSCALED,
		return 1;
	case vk::Format::eR8Uint: //VK_FORMAT_R8_UINT,
		return 1;
	case vk::Format::eR8Sint: //VK_FORMAT_R8_SINT,
		return 1;
	case vk::Format::eR8Srgb: //VK_FORMAT_R8_SRGB,
		return 1;
	case vk::Format::eR8G8Unorm: //VK_FORMAT_R8G8_UNORM,
		return 2;
	case vk::Format::eR8G8Snorm: //VK_FORMAT_R8G8_SNORM,
		return 2;
	case vk::Format::eR8G8Uscaled: //VK_FORMAT_R8G8_USCALED,
		return 2;
	case vk::Format::eR8G8Sscaled: //VK_FORMAT_R8G8_SSCALED,
		return 2;
	case vk::Format::eR8G8Uint: //VK_FORMAT_R8G8_UINT,
		return 2;
	case vk::Format::eR8G8Sint: //VK_FORMAT_R8G8_SINT,
		return 2;
	case vk::Format::eR8G8Srgb: //VK_FORMAT_R8G8_SRGB,
		return 2;
	case vk::Format::eR8G8B8Unorm: //VK_FORMAT_R8G8B8_UNORM,
		return 3;
	case vk::Format::eR8G8B8Snorm: //VK_FORMAT_R8G8B8_SNORM,
		return 3;
	case vk::Format::eR8G8B8Uscaled: //VK_FORMAT_R8G8B8_USCALED,
		return 3;
	case vk::Format::eR8G8B8Sscaled: //VK_FORMAT_R8G8B8_SSCALED,
		return 3;
	case vk::Format::eR8G8B8Uint: //VK_FORMAT_R8G8B8_UINT,
		return 3;
	case vk::Format::eR8G8B8Sint: //VK_FORMAT_R8G8B8_SINT,
		return 3;
	case vk::Format::eR8G8B8Srgb: //VK_FORMAT_R8G8B8_SRGB,
		return 3;
	case vk::Format::eB8G8R8Unorm: //VK_FORMAT_B8G8R8_UNORM,
		return 3;
	case vk::Format::eB8G8R8Snorm: //VK_FORMAT_B8G8R8_SNORM,
		return 3;
	case vk::Format::eB8G8R8Uscaled: //VK_FORMAT_B8G8R8_USCALED,
		return 3;
	case vk::Format::eB8G8R8Sscaled: //VK_FORMAT_B8G8R8_SSCALED,
		return 3;
	case vk::Format::eB8G8R8Uint: //VK_FORMAT_B8G8R8_UINT,
		return 3;
	case vk::Format::eB8G8R8Sint: //VK_FORMAT_B8G8R8_SINT,
		return 3;
	case vk::Format::eB8G8R8Srgb: //VK_FORMAT_B8G8R8_SRGB,
		return 3;
	case vk::Format::eR8G8B8A8Unorm: //VK_FORMAT_R8G8B8A8_UNORM,
		return 4;
	case vk::Format::eR8G8B8A8Snorm: //VK_FORMAT_R8G8B8A8_SNORM,
		return 4;
	case vk::Format::eR8G8B8A8Uscaled: //VK_FORMAT_R8G8B8A8_USCALED,
		return 4;
	case vk::Format::eR8G8B8A8Sscaled: //VK_FORMAT_R8G8B8A8_SSCALED,
		return 4;
	case vk::Format::eR8G8B8A8Uint: //VK_FORMAT_R8G8B8A8_UINT,
		return 4;
	case vk::Format::eR8G8B8A8Sint: //VK_FORMAT_R8G8B8A8_SINT,
		return 4;
	case vk::Format::eR8G8B8A8Srgb: //VK_FORMAT_R8G8B8A8_SRGB,
		return 4;
	case vk::Format::eB8G8R8A8Unorm: //VK_FORMAT_B8G8R8A8_UNORM,
		return 4;
	case vk::Format::eB8G8R8A8Snorm: //VK_FORMAT_B8G8R8A8_SNORM,
		return 4;
	case vk::Format::eB8G8R8A8Uscaled: //VK_FORMAT_B8G8R8A8_USCALED,
		return 4;
	case vk::Format::eB8G8R8A8Sscaled: //VK_FORMAT_B8G8R8A8_SSCALED,
		return 4;
	case vk::Format::eB8G8R8A8Uint: //VK_FORMAT_B8G8R8A8_UINT,
		return 4;
	case vk::Format::eB8G8R8A8Sint: //VK_FORMAT_B8G8R8A8_SINT,
		return 4;
	case vk::Format::eB8G8R8A8Srgb: //VK_FORMAT_B8G8R8A8_SRGB,
		return 4;
	case vk::Format::eA8B8G8R8UnormPack32: //VK_FORMAT_A8B8G8R8_UNORM_PACK32,
		return 4;
	case vk::Format::eA8B8G8R8SnormPack32: //VK_FORMAT_A8B8G8R8_SNORM_PACK32,
		return 4;
	case vk::Format::eA8B8G8R8UscaledPack32: //VK_FORMAT_A8B8G8R8_USCALED_PACK32,
		return 4;
	case vk::Format::eA8B8G8R8SscaledPack32: //VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
		return 4;
	case vk::Format::eA8B8G8R8UintPack32: //VK_FORMAT_A8B8G8R8_UINT_PACK32,
		return 4;
	case vk::Format::eA8B8G8R8SintPack32: //VK_FORMAT_A8B8G8R8_SINT_PACK32,
		return 4;
	case vk::Format::eA8B8G8R8SrgbPack32: //VK_FORMAT_A8B8G8R8_SRGB_PACK32,
		return 4;
	case vk::Format::eA2R10G10B10UnormPack32: //VK_FORMAT_A2R10G10B10_UNORM_PACK32,
		return 4;
	case vk::Format::eA2R10G10B10SnormPack32: //VK_FORMAT_A2R10G10B10_SNORM_PACK32,
		return 4;
	case vk::Format::eA2R10G10B10UscaledPack32: //VK_FORMAT_A2R10G10B10_USCALED_PACK32,
		return 4;
	case vk::Format::eA2R10G10B10SscaledPack32: //VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
		return 4;
	case vk::Format::eA2R10G10B10UintPack32: //VK_FORMAT_A2R10G10B10_UINT_PACK32,
		return 4;
	case vk::Format::eA2R10G10B10SintPack32: //VK_FORMAT_A2R10G10B10_SINT_PACK32,
		return 4;
	case vk::Format::eA2B10G10R10UnormPack32: //VK_FORMAT_A2B10G10R10_UNORM_PACK32,
		return 4;
	case vk::Format::eA2B10G10R10SnormPack32: //VK_FORMAT_A2B10G10R10_SNORM_PACK32,
		return 4;
	case vk::Format::eA2B10G10R10UscaledPack32: //VK_FORMAT_A2B10G10R10_USCALED_PACK32,
		return 4;
	case vk::Format::eA2B10G10R10SscaledPack32: //VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
		return 4;
	case vk::Format::eA2B10G10R10UintPack32: //VK_FORMAT_A2B10G10R10_UINT_PACK32,
		return 4;
	case vk::Format::eA2B10G10R10SintPack32: //VK_FORMAT_A2B10G10R10_SINT_PACK32,
		return 4;
	case vk::Format::eR16Unorm: //VK_FORMAT_R16_UNORM,
		return 2;
	case vk::Format::eR16Snorm: //VK_FORMAT_R16_SNORM,
		return 2;
	case vk::Format::eR16Uscaled: //VK_FORMAT_R16_USCALED,
		return 2;
	case vk::Format::eR16Sscaled: //VK_FORMAT_R16_SSCALED,
		return 2;
	case vk::Format::eR16Uint: //VK_FORMAT_R16_UINT,
		return 2;
	case vk::Format::eR16Sint: //VK_FORMAT_R16_SINT,
		return 2;
	case vk::Format::eR16Sfloat: //VK_FORMAT_R16_SFLOAT,
		return 2;
	case vk::Format::eR16G16Unorm: //VK_FORMAT_R16G16_UNORM,
		return 4;
	case vk::Format::eR16G16Snorm: //VK_FORMAT_R16G16_SNORM,
		return 4;
	case vk::Format::eR16G16Uscaled: //VK_FORMAT_R16G16_USCALED,
		return 4;
	case vk::Format::eR16G16Sscaled: //VK_FORMAT_R16G16_SSCALED,
		return 4;
	case vk::Format::eR16G16Uint: //VK_FORMAT_R16G16_UINT,
		return 4;
	case vk::Format::eR16G16Sint: //VK_FORMAT_R16G16_SINT,
		return 4;
	case vk::Format::eR16G16Sfloat: //VK_FORMAT_R16G16_SFLOAT,
		return 4;
	case vk::Format::eR16G16B16Unorm: //VK_FORMAT_R16G16B16_UNORM,
		return 6;
	case vk::Format::eR16G16B16Snorm: //VK_FORMAT_R16G16B16_SNORM
		return 6;
	case vk::Format::eR16G16B16Uscaled: //VK_FORMAT_R16G16B16_USCALED,
		return 6;
	case vk::Format::eR16G16B16Sscaled: //VK_FORMAT_R16G16B16_SSCALED,
		return 6;
	case vk::Format::eR16G16B16Uint: //VK_FORMAT_R16G16B16_UINT,
		return 6;
	case vk::Format::eR16G16B16Sint: //VK_FORMAT_R16G16B16_SINT,
		return 6;
	case vk::Format::eR16G16B16Sfloat: //VK_FORMAT_R16G16B16_SFLOAT,
		return 6;
	case vk::Format::eR16G16B16A16Unorm: //VK_FORMAT_R16G16B16A16_UNORM,
		return 8;
	case vk::Format::eR16G16B16A16Snorm: //VK_FORMAT_R16G16B16A16_SNORM,
		return 8;
	case vk::Format::eR16G16B16A16Uscaled: //VK_FORMAT_R16G16B16A16_USCALED,
		return 8;
	case vk::Format::eR16G16B16A16Sscaled: //VK_FORMAT_R16G16B16A16_SSCALED,
		return 8;
	case vk::Format::eR16G16B16A16Uint: //VK_FORMAT_R16G16B16A16_UINT,
		return 8;
	case vk::Format::eR16G16B16A16Sint: //VK_FORMAT_R16G16B16A16_SINT,
		return 8;
	case vk::Format::eR16G16B16A16Sfloat: //VK_FORMAT_R16G16B16A16_SFLOAT,
		return 8;
	case vk::Format::eR32Uint: //VK_FORMAT_R32_UINT,
		return 4;
	case vk::Format::eR32Sint: //VK_FORMAT_R32_SINT,
		return 4;
	case vk::Format::eR32Sfloat: //VK_FORMAT_R32_SFLOAT,
		return 4;
	case vk::Format::eR32G32Uint: //VK_FORMAT_R32G32_UINT,
		return 8;
	case vk::Format::eR32G32Sint: //VK_FORMAT_R32G32_SINT,
		return 8;
	case vk::Format::eR32G32Sfloat: //VK_FORMAT_R32G32_SFLOAT,
		return 8;
	case vk::Format::eR32G32B32Uint: //VK_FORMAT_R32G32B32_UINT,
		return 12;
	case vk::Format::eR32G32B32Sint: //VK_FORMAT_R32G32B32_SINT,
		return 12;
	case vk::Format::eR32G32B32Sfloat: //VK_FORMAT_R32G32B32_SFLOAT,
		return 12;
	case vk::Format::eR32G32B32A32Uint: //VK_FORMAT_R32G32B32A32_UINT,
		return 16;
	case vk::Format::eR32G32B32A32Sint: //VK_FORMAT_R32G32B32A32_SINT,
		return 16;
	case vk::Format::eR32G32B32A32Sfloat: //VK_FORMAT_R32G32B32A32_SFLOAT,
		return 16;
	case vk::Format::eR64Uint: //VK_FORMAT_R64_UINT,
		return 8;
	case vk::Format::eR64Sint: //VK_FORMAT_R64_SINT,
		return 8;
	case vk::Format::eR64Sfloat: //VK_FORMAT_R64_SFLOAT,
		return 8;
	case vk::Format::eR64G64Uint: //VK_FORMAT_R64G64_UINT,
		return 16;
	case vk::Format::eR64G64Sint: //VK_FORMAT_R64G64_SINT,
		return 16;
	case vk::Format::eR64G64Sfloat: //VK_FORMAT_R64G64_SFLOAT,
		return 16;
	case vk::Format::eR64G64B64Uint: //VK_FORMAT_R64G64B64_UINT,
		return 24;
	case vk::Format::eR64G64B64Sint: //VK_FORMAT_R64G64B64_SINT,
		return 24;
	case vk::Format::eR64G64B64Sfloat: //VK_FORMAT_R64G64B64_SFLOAT,
		return 24;
	case vk::Format::eR64G64B64A64Uint: //VK_FORMAT_R64G64B64A64_UINT,
		return 32;
	case vk::Format::eR64G64B64A64Sint: //VK_FORMAT_R64G64B64A64_SINT,
		return 32;
	case vk::Format::eR64G64B64A64Sfloat: //VK_FORMAT_R64G64B64A64_SFLOAT,
		return 32;
	case vk::Format::eB10G11R11UfloatPack32: //VK_FORMAT_B10G11R11_UFLOAT_PACK32,
	case vk::Format::eE5B9G9R9UfloatPack32: //VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
	case vk::Format::eD16Unorm: //VK_FORMAT_D16_UNORM,
		return 2;
	case vk::Format::eX8D24UnormPack32: //VK_FORMAT_X8_D24_UNORM_PACK32,
	case vk::Format::eD32Sfloat: //VK_FORMAT_D32_SFLOAT,
		return 4;
	case vk::Format::eS8Uint: //VK_FORMAT_S8_UINT,
		return 1;
	case vk::Format::eD16UnormS8Uint: //VK_FORMAT_D16_UNORM_S8_UINT,
		return 3;
	case vk::Format::eD24UnormS8Uint: //VK_FORMAT_D24_UNORM_S8_UINT,
		return 5;
	case vk::Format::eD32SfloatS8Uint: //VK_FORMAT_D32_SFLOAT_S8_UINT,
	case vk::Format::eBc1RgbUnormBlock: //VK_FORMAT_BC1_RGB_UNORM_BLOCK,
	case vk::Format::eBc1RgbSrgbBlock: //VK_FORMAT_BC1_RGB_SRGB_BLOCK,
	case vk::Format::eBc1RgbaUnormBlock: //VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
	case vk::Format::eBc1RgbaSrgbBlock: //VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
	case vk::Format::eBc2UnormBlock: //VK_FORMAT_BC2_UNORM_BLOCK,
	case vk::Format::eBc2SrgbBlock: //VK_FORMAT_BC2_SRGB_BLOCK,
	case vk::Format::eBc3UnormBlock: //VK_FORMAT_BC3_UNORM_BLOCK,
	case vk::Format::eBc3SrgbBlock: //VK_FORMAT_BC3_SRGB_BLOCK,
	case vk::Format::eBc4UnormBlock: //VK_FORMAT_BC4_UNORM_BLOCK,
	case vk::Format::eBc4SnormBlock: //VK_FORMAT_BC4_SNORM_BLOCK,
	case vk::Format::eBc5UnormBlock: //VK_FORMAT_BC5_UNORM_BLOCK,
	case vk::Format::eBc5SnormBlock: //VK_FORMAT_BC5_SNORM_BLOCK,
	case vk::Format::eBc6HUfloatBlock: //VK_FORMAT_BC6H_UFLOAT_BLOCK,
	case vk::Format::eBc6HSfloatBlock: //VK_FORMAT_BC6H_SFLOAT_BLOCK,
	case vk::Format::eBc7UnormBlock: //VK_FORMAT_BC7_UNORM_BLOCK,
	case vk::Format::eBc7SrgbBlock: //VK_FORMAT_BC7_SRGB_BLOCK,
	case vk::Format::eEtc2R8G8B8UnormBlock: //VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
	case vk::Format::eEtc2R8G8B8SrgbBlock: //VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
	case vk::Format::eEtc2R8G8B8A1UnormBlock: //VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
	case vk::Format::eEtc2R8G8B8A1SrgbBlock: //VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
	case vk::Format::eEtc2R8G8B8A8UnormBlock: //VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
	case vk::Format::eEtc2R8G8B8A8SrgbBlock: //VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
	case vk::Format::eEacR11UnormBlock: //VK_FORMAT_EAC_R11_UNORM_BLOCK,
	case vk::Format::eEacR11SnormBlock: //VK_FORMAT_EAC_R11_SNORM_BLOCK,
	case vk::Format::eEacR11G11UnormBlock: //VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
	case vk::Format::eEacR11G11SnormBlock: //VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
	case vk::Format::eAstc4x4UnormBlock: //VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
	case vk::Format::eAstc4x4SrgbBlock: //VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
	case vk::Format::eAstc5x4UnormBlock: //VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
	case vk::Format::eAstc5x4SrgbBlock: //VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
	case vk::Format::eAstc5x5UnormBlock: //VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
	case vk::Format::eAstc5x5SrgbBlock: //VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
	case vk::Format::eAstc6x5UnormBlock: //VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
	case vk::Format::eAstc6x5SrgbBlock: //VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
	case vk::Format::eAstc6x6UnormBlock: //VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
	case vk::Format::eAstc6x6SrgbBlock: //VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
	case vk::Format::eAstc8x5UnormBlock: //VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
	case vk::Format::eAstc8x5SrgbBlock: //VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
	case vk::Format::eAstc8x6UnormBlock: //VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
	case vk::Format::eAstc8x6SrgbBlock: //VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
	case vk::Format::eAstc8x8UnormBlock: //VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
	case vk::Format::eAstc8x8SrgbBlock: //VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
	case vk::Format::eAstc10x5UnormBlock: //VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
	case vk::Format::eAstc10x5SrgbBlock: //VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
	case vk::Format::eAstc10x6UnormBlock: //VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
	case vk::Format::eAstc10x6SrgbBlock: //VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
	case vk::Format::eAstc10x8UnormBlock: //VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
	case vk::Format::eAstc10x8SrgbBlock: //VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
	case vk::Format::eAstc10x10UnormBlock: //VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
	case vk::Format::eAstc10x10SrgbBlock: //VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
	case vk::Format::eAstc12x10UnormBlock: //VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
	case vk::Format::eAstc12x10SrgbBlock: //VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
	case vk::Format::eAstc12x12UnormBlock: //VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
	case vk::Format::eAstc12x12SrgbBlock: //VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
	case vk::Format::eG8B8G8R8422Unorm: //VK_FORMAT_G8B8G8R8_422_UNORM,
	//case vk::Format::eG8B8G8R8422UnormKHR: //VK_FORMAT_G8B8G8R8_422_UNORM,
	case vk::Format::eB8G8R8G8422Unorm: //VK_FORMAT_B8G8R8G8_422_UNORM,
	//case vk::Format::eB8G8R8G8422UnormKHR: //VK_FORMAT_B8G8R8G8_422_UNORM,
	case vk::Format::eG8B8R83Plane420Unorm: //VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
	//case vk::Format::eG8B8R83Plane420UnormKHR: //VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
	case vk::Format::eG8B8R82Plane420Unorm: //VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
	//case vk::Format::eG8B8R82Plane420UnormKHR: //VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
	case vk::Format::eG8B8R83Plane422Unorm: //VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
	//case vk::Format::eG8B8R83Plane422UnormKHR: //VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
	case vk::Format::eG8B8R82Plane422Unorm: //VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
	//case vk::Format::eG8B8R82Plane422UnormKHR: //VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
	case vk::Format::eG8B8R83Plane444Unorm: //VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
	//case vk::Format::eG8B8R83Plane444UnormKHR: //VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
	case vk::Format::eR10X6UnormPack16: //VK_FORMAT_R10X6_UNORM_PACK16,
	//case vk::Format::eR10X6UnormPack16KHR: //VK_FORMAT_R10X6_UNORM_PACK16,
	case vk::Format::eR10X6G10X6Unorm2Pack16: //VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
	//case vk::Format::eR10X6G10X6Unorm2Pack16KHR: //VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
	case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16: //VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
	//case vk::Format::eR10X6G10X6B10X6A10X6Unorm4Pack16KHR: //VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
	case vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16: //VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
	//case vk::Format::eG10X6B10X6G10X6R10X6422Unorm4Pack16KHR: //VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
	case vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16: //VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
	//case vk::Format::eB10X6G10X6R10X6G10X6422Unorm4Pack16KHR: //VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
	case vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16: //VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
	//case vk::Format::eG10X6B10X6R10X63Plane420Unorm3Pack16KHR: //VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
	case vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16: //VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
	//case vk::Format::eG10X6B10X6R10X62Plane420Unorm3Pack16KHR: //VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
	case vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16: //VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
	//case vk::Format::eG10X6B10X6R10X63Plane422Unorm3Pack16KHR: //VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
	case vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16: //VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
	//case vk::Format::eG10X6B10X6R10X62Plane422Unorm3Pack16KHR: //VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
	case vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16: //VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
	//case vk::Format::eG10X6B10X6R10X63Plane444Unorm3Pack16KHR: //VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
	case vk::Format::eR12X4UnormPack16: //VK_FORMAT_R12X4_UNORM_PACK16,
	//case vk::Format::eR12X4UnormPack16KHR: //VK_FORMAT_R12X4_UNORM_PACK16,
	case vk::Format::eR12X4G12X4Unorm2Pack16: //VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
	//case vk::Format::eR12X4G12X4Unorm2Pack16KHR: //VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
	case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16: //VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
	//case vk::Format::eR12X4G12X4B12X4A12X4Unorm4Pack16KHR: //VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
	case vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16: //VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
	//case vk::Format::eG12X4B12X4G12X4R12X4422Unorm4Pack16KHR: //VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
	case vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16: //VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
	//case vk::Format::eB12X4G12X4R12X4G12X4422Unorm4Pack16KHR: //VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
	case vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16: //VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
	//case vk::Format::eG12X4B12X4R12X43Plane420Unorm3Pack16KHR: //VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
	case vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16: //VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
	//case vk::Format::eG12X4B12X4R12X42Plane420Unorm3Pack16KHR: //VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
	case vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16: //VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
	//case vk::Format::eG12X4B12X4R12X43Plane422Unorm3Pack16KHR: //VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
	case vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16: //VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
	//case vk::Format::eG12X4B12X4R12X42Plane422Unorm3Pack16KHR: //VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
	case vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16: //VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
	//case vk::Format::eG12X4B12X4R12X43Plane444Unorm3Pack16KHR: //VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
	case vk::Format::eG16B16G16R16422Unorm: //VK_FORMAT_G16B16G16R16_422_UNORM,
	//case vk::Format::eG16B16G16R16422UnormKHR: //VK_FORMAT_G16B16G16R16_422_UNORM,
	case vk::Format::eB16G16R16G16422Unorm: //VK_FORMAT_B16G16R16G16_422_UNORM,
	//case vk::Format::eB16G16R16G16422UnormKHR: //VK_FORMAT_B16G16R16G16_422_UNORM,
	case vk::Format::eG16B16R163Plane420Unorm: //VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
	//case vk::Format::eG16B16R163Plane420UnormKHR: //VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
	case vk::Format::eG16B16R162Plane420Unorm: //VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
	//case vk::Format::eG16B16R162Plane420UnormKHR: //VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
	case vk::Format::eG16B16R163Plane422Unorm: //VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
	//case vk::Format::eG16B16R163Plane422UnormKHR: //VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
	case vk::Format::eG16B16R162Plane422Unorm: //VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
	//case vk::Format::eG16B16R162Plane422UnormKHR: //VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
	case vk::Format::eG16B16R163Plane444Unorm: //VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
	//case vk::Format::eG16B16R163Plane444UnormKHR: //VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
	case vk::Format::ePvrtc12BppUnormBlockIMG: //VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
	case vk::Format::ePvrtc14BppUnormBlockIMG: //VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
	case vk::Format::ePvrtc22BppUnormBlockIMG: //VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
	case vk::Format::ePvrtc24BppUnormBlockIMG: //VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
	case vk::Format::ePvrtc12BppSrgbBlockIMG: //VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
	case vk::Format::ePvrtc14BppSrgbBlockIMG: //VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
	case vk::Format::ePvrtc22BppSrgbBlockIMG: //VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
	case vk::Format::ePvrtc24BppSrgbBlockIMG: //VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG
	default:
		return 4;
	}
}

CSurface9::CSurface9(CDevice9* Device, CTexture9* Texture, UINT Width, UINT Height, DWORD Usage, UINT Levels, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, BOOL Lockable, D3DPOOL pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mCubeTexture(nullptr),
	mTexture(Texture),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mLevels(Levels),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mLockable(Lockable),
	mPool(pool),
	mSharedHandle(pSharedHandle)
{
	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

	if (mCubeTexture != nullptr)
	{
		mCubeTexture->AddRef();
	}
	else if (mTexture != nullptr)
	{
		mTexture->AddRef();
	}

	{
		const vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
		const vk::ImageUsageFlags usage = ((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment) | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;
		const vk::MemoryPropertyFlags required_props = vk::MemoryPropertyFlagBits::eDeviceLocal;

		const vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setFormat(ConvertFormat(mFormat))
			.setExtent({ mWidth, mHeight, 1 })
			.setMipLevels(1)
			.setArrayLayers(1)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setTiling(tiling)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setQueueFamilyIndexCount(0)
			.setPQueueFamilyIndices(nullptr)
			.setInitialLayout(vk::ImageLayout::ePreinitialized);
		mImage = mDevice->mDevice->createImageUnique(imageCreateInfo);

		vk::MemoryRequirements memoryRequirements;
		mDevice->mDevice->getImageMemoryRequirements(mImage.get(), &memoryRequirements);

		mImageMemoryAllocateInfo.setAllocationSize(memoryRequirements.size);
		mImageMemoryAllocateInfo.setMemoryTypeIndex(0);

		auto pass = mDevice->FindMemoryTypeFromProperties(memoryRequirements.memoryTypeBits, required_props, &mImageMemoryAllocateInfo.memoryTypeIndex);

		mImageDeviceMemory = mDevice->mDevice->allocateMemoryUnique(mImageMemoryAllocateInfo);

		mDevice->mDevice->bindImageMemory(mImage.get(), mImageDeviceMemory.get(), 0);

		//Now transition this thing from init to attachment ready.
		SetImageLayout(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eColorAttachmentOptimal));
	}

	ResetViewAndStagingBuffer();
}

CSurface9::CSurface9(CDevice9* Device, CCubeTexture9* Texture, UINT Width, UINT Height, DWORD Usage, UINT Levels, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, BOOL Lockable, D3DPOOL pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mCubeTexture(Texture),
	mTexture(nullptr),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mLevels(Levels),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mLockable(Lockable),
	mPool(pool),
	mSharedHandle(pSharedHandle)
{
	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

	if (mCubeTexture != nullptr)
	{
		mCubeTexture->AddRef();
	}
	else if (mTexture != nullptr)
	{
		mTexture->AddRef();
	}

	{
		const vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
		const vk::ImageUsageFlags usage = ((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment) | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc;
		const vk::MemoryPropertyFlags required_props = vk::MemoryPropertyFlagBits::eDeviceLocal;

		const vk::ImageCreateInfo imageCreateInfo = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setFormat(ConvertFormat(mFormat))
			.setExtent({ mWidth, mHeight, 1 })
			.setMipLevels(1)
			.setArrayLayers(1)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setTiling(tiling)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setQueueFamilyIndexCount(0)
			.setPQueueFamilyIndices(nullptr)
			.setInitialLayout(vk::ImageLayout::ePreinitialized);
		mImage = mDevice->mDevice->createImageUnique(imageCreateInfo);

		vk::MemoryRequirements memoryRequirements;
		mDevice->mDevice->getImageMemoryRequirements(mImage.get(), &memoryRequirements);

		mImageMemoryAllocateInfo.setAllocationSize(memoryRequirements.size);
		mImageMemoryAllocateInfo.setMemoryTypeIndex(0);

		auto pass = mDevice->FindMemoryTypeFromProperties(memoryRequirements.memoryTypeBits, required_props, &mImageMemoryAllocateInfo.memoryTypeIndex);

		mImageDeviceMemory = mDevice->mDevice->allocateMemoryUnique(mImageMemoryAllocateInfo);

		mDevice->mDevice->bindImageMemory(mImage.get(), mImageDeviceMemory.get(), 0);

		//Now transition this thing from init to attachment ready.
		SetImageLayout(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eColorAttachmentOptimal));
	}

	ResetViewAndStagingBuffer();
}

CSurface9::CSurface9(CDevice9* Device, vk::Image& image, UINT Width, UINT Height, DWORD Usage, UINT Levels, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, BOOL Lockable, D3DPOOL pool, HANDLE *pSharedHandle)
	: mDevice(Device),
	mImage(vk::UniqueImage(image, *Device->mDevice)),
	mWidth(Width),
	mHeight(Height),
	mUsage(Usage),
	mLevels(Levels),
	mFormat(Format),
	mMultiSample(MultiSample),
	mMultisampleQuality(MultisampleQuality),
	mDiscard(Discard),
	mLockable(Lockable),
	mPool(pool),
	mSharedHandle(pSharedHandle)
{
	if (mFormat == D3DFMT_D16_LOCKABLE || mFormat == D3DFMT_D32 || mFormat == D3DFMT_D15S1 || mFormat == D3DFMT_D24S8 || mFormat == D3DFMT_D24X8 || mFormat == D3DFMT_D24X4S4 || mFormat == D3DFMT_D16)
	{
		mUsage = D3DUSAGE_DEPTHSTENCIL;
	}

	if (mCubeTexture != nullptr)
	{
		mCubeTexture->AddRef();
	}
	else if (mTexture != nullptr)
	{
		mTexture->AddRef();
	}

	//Now transition this thing from init to attachment ready.
	SetImageLayout(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eColorAttachmentOptimal));

	ResetViewAndStagingBuffer();
}

CSurface9::~CSurface9()
{
	if (mUsage != D3DUSAGE_DEPTHSTENCIL) //Depth stencil doesn't have a texture.
	{
		if (mCubeTexture != nullptr)
		{
			mCubeTexture->Release();
		}
		else if (mTexture != nullptr)
		{
			mTexture->Release();
		}
	}
}

ULONG CSurface9::PrivateAddRef(void)
{
	return InterlockedIncrement(&mPrivateReferenceCount);
}

ULONG CSurface9::PrivateRelease(void)
{
	ULONG ref = InterlockedDecrement(&mPrivateReferenceCount);

	if (ref == 0 && mReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

void CSurface9::SetImageLayout(vk::ImageLayout newLayout)
{
	mDevice->BeginRecordingUtilityCommands();
	{
		const vk::PipelineStageFlags src_stages = ((mImageLayout == vk::ImageLayout::eTransferSrcOptimal || mImageLayout == vk::ImageLayout::eTransferDstOptimal) ? vk::PipelineStageFlagBits::eTransfer : vk::PipelineStageFlagBits::eTopOfPipe);
		const vk::PipelineStageFlags dest_stages = ((newLayout == vk::ImageLayout::eTransferSrcOptimal || newLayout == vk::ImageLayout::eTransferDstOptimal) ? vk::PipelineStageFlagBits::eTransfer : ((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::PipelineStageFlagBits::eEarlyFragmentTests : vk::PipelineStageFlagBits::eColorAttachmentOutput));

		auto DstAccessMask = [](vk::ImageLayout const &layout)
		{
			vk::AccessFlags flags;

			switch (layout) {
			case vk::ImageLayout::eTransferDstOptimal:
				// Make sure anything that was copying from this image has completed
				flags = vk::AccessFlagBits::eTransferWrite;
				break;
			case vk::ImageLayout::eColorAttachmentOptimal:
				flags = vk::AccessFlagBits::eColorAttachmentWrite;
				break;
			case vk::ImageLayout::eDepthStencilAttachmentOptimal:
				flags = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
				break;
			case vk::ImageLayout::eShaderReadOnlyOptimal:
				// Make sure any Copy or CPU writes to image are flushed
				flags = vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eInputAttachmentRead;
				break;
			case vk::ImageLayout::eTransferSrcOptimal:
				flags = vk::AccessFlagBits::eTransferRead;
				break;
			case vk::ImageLayout::ePresentSrcKHR:
				flags = vk::AccessFlagBits::eMemoryRead;
				break;
			default:
				break;
			}

			return flags;
		};

		auto const barrier = vk::ImageMemoryBarrier()
			.setSrcAccessMask(vk::AccessFlagBits())
			.setDstAccessMask(DstAccessMask(newLayout))
			.setOldLayout(mImageLayout)
			.setNewLayout(newLayout)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(mImage.get())
			.setSubresourceRange(vk::ImageSubresourceRange(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor), 0, 1, 0, 1));

		mDevice->mCurrentUtilityCommandBuffer.pipelineBarrier(src_stages, dest_stages, vk::DependencyFlagBits(), 0, nullptr, 0, nullptr, 1, &barrier);

	}
	mDevice->StopRecordingUtilityCommands();

	mImageLayout = newLayout;
}

void CSurface9::ResetViewAndStagingBuffer()
{
	{
		auto const viewInfo = vk::ImageViewCreateInfo()
			.setImage(mImage.get())
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(ConvertFormat(mFormat))
			.setSubresourceRange(vk::ImageSubresourceRange(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor), 0, 1, 0, 1));
		mImageView = mDevice->mDevice->createImageViewUnique(viewInfo);
	}

	{
		auto const bufferInfo = vk::BufferCreateInfo().setSize(mWidth * mHeight * SizeOf(ConvertFormat(mFormat))).setUsage(vk::BufferUsageFlagBits::eTransferSrc);

		mStagingBuffer = mDevice->mDevice->createBufferUnique(bufferInfo);

		vk::MemoryRequirements mem_reqs;
		mDevice->mDevice->getBufferMemoryRequirements(mStagingBuffer.get(), &mem_reqs);

		auto mem_alloc = vk::MemoryAllocateInfo().setAllocationSize(mem_reqs.size).setMemoryTypeIndex(0);
		mDevice->FindMemoryTypeFromProperties(mem_reqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, &mem_alloc.memoryTypeIndex);

		mStagingBufferMemory = mDevice->mDevice->allocateMemoryUnique(mem_alloc);

		mDevice->mDevice->bindBufferMemory(mStagingBuffer.get(), mStagingBufferMemory.get(), 0);
	}
}

ULONG STDMETHODCALLTYPE CSurface9::AddRef(void)
{
	return InterlockedIncrement(&mReferenceCount);
}

HRESULT STDMETHODCALLTYPE CSurface9::QueryInterface(REFIID riid, void  **ppv)
{
	if (ppv == nullptr)
	{
		return E_POINTER;
	}

	if (IsEqualGUID(riid, IID_IDirect3DSurface9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IDirect3DResource9))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	if (IsEqualGUID(riid, IID_IUnknown))
	{
		(*ppv) = this;
		this->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CSurface9::Release(void)
{
	ULONG ref = InterlockedDecrement(&mReferenceCount);

	if (ref == 0 && mPrivateReferenceCount == 0)
	{
		delete this;
	}

	return ref;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDevice(IDirect3DDevice9** ppDevice)
{
	mDevice->AddRef();
	(*ppDevice) = (IDirect3DDevice9*)mDevice;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSurface9::FreePrivateData(REFGUID refguid)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::FreePrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

DWORD STDMETHODCALLTYPE CSurface9::GetPriority()
{
	//TODO: Implement.

	Log(warning) << "CSurface9::GetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::GetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

D3DRESOURCETYPE STDMETHODCALLTYPE CSurface9::GetType()
{
	return D3DRTYPE_SURFACE;
}

void STDMETHODCALLTYPE CSurface9::PreLoad()
{
	//TODO: Implement.

	Log(warning) << "CSurface9::PreLoad is not implemented!" << std::endl;

	return;
}

DWORD STDMETHODCALLTYPE CSurface9::SetPriority(DWORD PriorityNew)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::SetPriority is not implemented!" << std::endl;

	return 1;
}

HRESULT STDMETHODCALLTYPE CSurface9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::SetPrivateData is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetContainer(REFIID riid, void** ppContainer)
{
	this->AddRef();

	//TODO: Implement.

	Log(warning) << "CSurface9::GetContainer is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDC(HDC* phdc)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::GetDC is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::GetDesc(D3DSURFACE_DESC* pDesc)
{
	pDesc->Format = this->mFormat;
	pDesc->Type = D3DRTYPE_SURFACE;
	pDesc->Usage = this->mUsage;
	pDesc->Pool = this->mPool;
	pDesc->MultiSampleType = this->mMultiSample;
	pDesc->MultiSampleQuality = this->mMultisampleQuality;
	pDesc->Width = this->mWidth;
	pDesc->Height = this->mHeight;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSurface9::LockRect(D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags)
{
	if (!mData)
	{
		mData = mDevice->mDevice->mapMemory(mStagingBufferMemory.get(), (vk::DeviceSize)0, VK_WHOLE_SIZE);
	}

	size_t formatSize = SizeOf(ConvertFormat(mFormat));

	pLockedRect->Pitch = mWidth * formatSize;

	char* bytes = (char*)mData;
	if (pRect != nullptr)
	{
		bytes += (pLockedRect->Pitch * pRect->top);
		bytes += (formatSize * pRect->left);
	}

	pLockedRect->pBits = (void*)bytes;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSurface9::ReleaseDC(HDC hdc)
{
	//TODO: Implement.

	Log(warning) << "CSurface9::ReleaseDC is not implemented!" << std::endl;

	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CSurface9::UnlockRect()
{
	if (mData)
	{
		mDevice->mDevice->unmapMemory(mStagingBufferMemory.get());
		mData = nullptr;
	}

	mDevice->BeginRecordingUtilityCommands();
	{
		{
			this->SetImageLayout(vk::ImageLayout::eTransferDstOptimal);

			auto const subresource = vk::ImageSubresourceLayers()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setMipLevel(0)
				.setBaseArrayLayer(0)
				.setLayerCount(1);

			auto const copy_region =
				vk::BufferImageCopy()
				.setBufferOffset(0)
				.setBufferRowLength(mWidth)
				.setBufferImageHeight(mHeight)
				.setImageSubresource(subresource)
				.setImageOffset({ 0, 0, 0 })
				.setImageExtent({ mWidth, mHeight, 1 });

			mDevice->mCurrentUtilityCommandBuffer.copyBufferToImage(mStagingBuffer.get(), mImage.get(), vk::ImageLayout::eTransferDstOptimal, 1, &copy_region);

			SetImageLayout(((mUsage == D3DUSAGE_DEPTHSTENCIL) ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eColorAttachmentOptimal));
		}

		if (mTexture)
		{
			mTexture->SetImageLayout(vk::ImageLayout::eTransferDstOptimal);

			auto const subresource = vk::ImageSubresourceLayers()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setMipLevel(mMipIndex)
				.setBaseArrayLayer(mTargetLayer)
				.setLayerCount(1);

			auto const copy_region =
				vk::BufferImageCopy()
				.setBufferOffset(0)
				.setBufferRowLength(mWidth)
				.setBufferImageHeight(mHeight)
				.setImageSubresource(subresource)
				.setImageOffset({ 0, 0, 0 })
				.setImageExtent({ mWidth, mHeight, 1 });

			mDevice->mCurrentUtilityCommandBuffer.copyBufferToImage(mStagingBuffer.get(), mTexture->mImage.get(), vk::ImageLayout::eTransferDstOptimal, 1, &copy_region);

			mTexture->SetImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

			if (mMipIndex == 0 && (mTexture->mUsage & D3DUSAGE_AUTOGENMIPMAP) == D3DUSAGE_AUTOGENMIPMAP)
			{
				mTexture->GenerateMipSubLevels();
			}
		}
		else if (mCubeTexture)
		{
			mCubeTexture->SetImageLayout(vk::ImageLayout::eTransferDstOptimal);

			auto const subresource = vk::ImageSubresourceLayers()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setMipLevel(mMipIndex)
				.setBaseArrayLayer(mTargetLayer)
				.setLayerCount(1);

			auto const copy_region =
				vk::BufferImageCopy()
				.setBufferOffset(0)
				.setBufferRowLength(mWidth)
				.setBufferImageHeight(mHeight)
				.setImageSubresource(subresource)
				.setImageOffset({ 0, 0, 0 })
				.setImageExtent({ mWidth, mHeight, 1 });

			mDevice->mCurrentUtilityCommandBuffer.copyBufferToImage(mStagingBuffer.get(), mCubeTexture->mImage.get(), vk::ImageLayout::eTransferDstOptimal, 1, &copy_region);

			mCubeTexture->SetImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

			if (mMipIndex == 0 && (mCubeTexture->mUsage & D3DUSAGE_AUTOGENMIPMAP) == D3DUSAGE_AUTOGENMIPMAP)
			{
				mCubeTexture->GenerateMipSubLevels();
			}
		}
	}
	mDevice->StopRecordingUtilityCommands();

	return S_OK;
}