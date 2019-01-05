// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

#include <chrono>
#include <thread>

#include "Perf_ProcessQueue.h"
#include "Perf_CommandStreamManager.h"

#include "CStateBlock9.h"
#include "CPixelShader9.h"
#include "CVertexShader9.h"
#include "CIndexBuffer9.h"
#include "CVertexBuffer9.h"
#include "CTexture9.h"
#include "CCubeTexture9.h"
#include "CVolumeTexture9.h"
#include "CSurface9.h"
#include "CDevice9.h"
#include "CVolume9.h"

#include "Utilities.h"

void ProcessQueue(CommandStreamManager* commandStreamManager)
{
	size_t count = 0;

	Sleep(100);
	while (commandStreamManager->IsRunning)
	{
		WorkItem* workItem = nullptr;

		if (commandStreamManager->mWorkItems.Pop(workItem, count))
		{
			//try
			//{
			switch (workItem->WorkItemType)
			{
			case Instance_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateInstance();
			}
			break;
			case Instance_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyInstance(workItem->Id);
			}
			break;
			case Device_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateDevice(workItem->Id, workItem->Argument1);

				//TODO: revisit
				commandStreamManager->mResult = vk::Result::eSuccess;
			}
			break;
			case Device_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyDevice(workItem->Id);
			}
			break;
			case VertexBuffer_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateVertexBuffer(workItem->Id, workItem->Argument1);
			}
			break;
			case VertexBuffer_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyVertexBuffer(workItem->Id);
			}
			break;
			case IndexBuffer_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateIndexBuffer(workItem->Id, workItem->Argument1);
			}
			break;
			case IndexBuffer_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyIndexBuffer(workItem->Id);
			}
			break;
			case Texture_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateTexture(workItem->Id, workItem->Argument1);
			}
			break;
			case Texture_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyTexture(workItem->Id);
			}
			break;
			case CubeTexture_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateCubeTexture(workItem->Id, workItem->Argument1);
			}
			break;
			case CubeTexture_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyCubeTexture(workItem->Id);
			}
			break;
			case VolumeTexture_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateVolumeTexture(workItem->Id, workItem->Argument1);
			}
			break;
			case VolumeTexture_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyVolumeTexture(workItem->Id);
			}
			break;
			case Surface_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateSurface(workItem->Id, workItem->Argument1);
			}
			break;
			case Surface_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroySurface(workItem->Id);
			}
			break;
			case Volume_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateVolume(workItem->Id, workItem->Argument1);
			}
			break;
			case Volume_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyVolume(workItem->Id);
			}
			break;
			case Shader_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateShader(workItem->Id, workItem->Argument1, workItem->Argument2, workItem->Argument3);
			}
			break;
			case Shader_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyShader(workItem->Id);
			}
			break;
			case Query_Create:
			{
				commandStreamManager->mRenderManager.mStateManager.CreateQuery(workItem->Id, workItem->Argument1);
			}
			break;
			case Query_Destroy:
			{
				commandStreamManager->mRenderManager.mStateManager.DestroyQuery(workItem->Id);
			}
			break;
			case Device_SetRenderTarget:
			{
				CDevice9* device9 = bit_cast<CDevice9*>(workItem->Caller);
				DWORD RenderTargetIndex = bit_cast<DWORD>(workItem->Argument1);
				CSurface9* pRenderTarget = bit_cast<CSurface9*>(workItem->Argument2);

				commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id]->SetRenderTarget(device9, RenderTargetIndex, pRenderTarget);
			}
			break;
			case Device_SetDepthStencilSurface:
			{
				CDevice9* device9 = bit_cast<CDevice9*>(workItem->Caller);
				CSurface9* pNewZStencil = bit_cast<CSurface9*>(workItem->Argument1);

				commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id]->SetDepthStencilSurface(device9, pNewZStencil);
			}
			break;
			case Device_Clear:
			{
				DWORD Count = bit_cast<DWORD>(workItem->Argument1);
				D3DRECT* pRects = bit_cast<D3DRECT*>(workItem->Argument2);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument3);
				D3DCOLOR Color = bit_cast<D3DCOLOR>(workItem->Argument4);
				float Z = bit_cast<float>(workItem->Argument5);
				DWORD Stencil = bit_cast<DWORD>(workItem->Argument6);

				commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id]->Clear(Count, pRects, Flags, Color, Z, Stencil);
			}
			break;
			case Device_BeginScene:
			{
				auto& renderManager = commandStreamManager->mRenderManager;
				auto& realDevice = renderManager.mStateManager.mDevices[workItem->Id];
				if (!realDevice->mDeviceState.mRenderTarget->mIsSceneStarted)
				{
					realDevice->StartScene(false, false, false);
				}
			}
			break;
			case Device_Present:
			{
				CDevice9* device9 = bit_cast<CDevice9*>(workItem->Caller);
				RECT* pSourceRect = bit_cast<RECT*>(workItem->Argument1);
				RECT* pDestRect = bit_cast<RECT*>(workItem->Argument2);
				HWND hDestWindowOverride = bit_cast<HWND>(workItem->Argument3);
				RGNDATA* pDirtyRegion = bit_cast<RGNDATA*>(workItem->Argument4);

				if (!hDestWindowOverride)
				{
					hDestWindowOverride = device9->mFocusWindow;
				}

				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				commandStreamManager->mResult = commandStreamManager->mRenderManager.Present(realDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

				//This isn't truely atomic but worst case that some extra buffers will be created.
				commandStreamManager->mFrameBit = !commandStreamManager->mFrameBit;
			}
			break;
			case Device_BeginStateBlock:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CDevice9* device = bit_cast<CDevice9*>(workItem->Argument1);

				realDevice->mCurrentStateRecording = new CStateBlock9(device);
			}
			break;
			case Device_DrawIndexedPrimitive:
			{
				D3DPRIMITIVETYPE Type = bit_cast<D3DPRIMITIVETYPE>(workItem->Argument1);
				INT BaseVertexIndex = bit_cast<INT>(workItem->Argument2);
				UINT MinIndex = bit_cast<UINT>(workItem->Argument3);
				UINT NumVertices = bit_cast<UINT>(workItem->Argument4);
				UINT StartIndex = bit_cast<UINT>(workItem->Argument5);
				UINT PrimitiveCount = bit_cast<UINT>(workItem->Argument6);

				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				commandStreamManager->mRenderManager.DrawIndexedPrimitive(realDevice, Type, BaseVertexIndex, MinIndex, NumVertices, StartIndex, PrimitiveCount);
			}
			break;
			case Device_DrawPrimitive:
			{
				D3DPRIMITIVETYPE PrimitiveType = bit_cast<D3DPRIMITIVETYPE>(workItem->Argument1);
				UINT StartVertex = bit_cast<UINT>(workItem->Argument2);
				UINT PrimitiveCount = bit_cast<UINT>(workItem->Argument3);

				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				commandStreamManager->mRenderManager.DrawPrimitive(realDevice, PrimitiveType, StartVertex, PrimitiveCount);
			}
			break;
			case Device_EndStateBlock:
			{
				IDirect3DStateBlock9** ppSB = bit_cast<IDirect3DStateBlock9**>(workItem->Argument1);
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];

				(*ppSB) = realDevice->mCurrentStateRecording;
				realDevice->mCurrentStateRecording = nullptr;
			}
			break;
			case Device_GetDisplayMode:
			{
				UINT iSwapChain = bit_cast<UINT>(workItem->Argument1);
				D3DDISPLAYMODE* pMode = bit_cast<D3DDISPLAYMODE*>(workItem->Argument2);
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];

				if (iSwapChain)
				{
					//TODO: Implement.
					Log(warning) << "ProcessQueue multiple swapchains are not implemented!" << std::endl;
				}
				else
				{
					auto& colorSurface = realDevice->mDeviceState.mRenderTarget->mColorSurface;

					pMode->Height = colorSurface->mExtent.height;
					pMode->Width = colorSurface->mExtent.width;
					pMode->RefreshRate = 60; //fake it till you make it.
					pMode->Format = ConvertFormat(colorSurface->mRealFormat);
				}
			}
			break;
			case Device_GetDisplayModeEx:
			{
				UINT iSwapChain = bit_cast<UINT>(workItem->Argument1);
				D3DDISPLAYMODEEX* pMode = bit_cast<D3DDISPLAYMODEEX*>(workItem->Argument2);
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];

				if (iSwapChain)
				{
					//TODO: Implement.
					Log(warning) << "ProcessQueue multiple swapchains are not implemented!" << std::endl;
				}
				else
				{
					auto& colorSurface = realDevice->mDeviceState.mRenderTarget->mColorSurface;

					pMode->Size = sizeof(D3DDISPLAYMODEEX);
					pMode->Height = colorSurface->mExtent.height;
					pMode->Width = colorSurface->mExtent.width;
					pMode->RefreshRate = 60; //fake it till you make it.
					pMode->Format = ConvertFormat(colorSurface->mRealFormat);
					pMode->ScanLineOrdering = D3DSCANLINEORDERING_UNKNOWN;
				}
			}
			break;
			case Device_GetFVF:
			{
				DWORD* pFVF = bit_cast<DWORD*>(workItem->Argument1);
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];

				(*pFVF) = realDevice->mDeviceState.mFVF;
			}
			break;
			case Device_GetLight:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Index = bit_cast<DWORD>(workItem->Argument1);
				D3DLIGHT9* pLight = bit_cast<D3DLIGHT9*>(workItem->Argument2);
				auto& light = realDevice->mDeviceState.mShaderState.mLights[Index];

				pLight->Type = (*(D3DLIGHTTYPE*)light.Type);
				pLight->Diffuse = (*(D3DCOLORVALUE*)light.Diffuse);
				pLight->Specular = (*(D3DCOLORVALUE*)light.Specular);
				pLight->Ambient = (*(D3DCOLORVALUE*)light.Ambient);

				pLight->Position = (*(D3DVECTOR*)light.Position);
				pLight->Direction = (*(D3DVECTOR*)light.Direction);

				pLight->Range = light.Range;
				pLight->Falloff = light.Falloff;
				pLight->Attenuation0 = light.Attenuation0;
				pLight->Attenuation1 = light.Attenuation1;
				pLight->Attenuation2 = light.Attenuation2;
				pLight->Theta = light.Theta;
				pLight->Phi = light.Phi;
			}
			break;
			case Device_GetLightEnable:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Index = bit_cast<DWORD>(workItem->Argument1);
				BOOL* pEnable = bit_cast<BOOL*>(workItem->Argument2);

				(*pEnable) = realDevice->mDeviceState.mShaderState.mLights[Index].IsEnabled;
			}
			break;
			case Device_GetMaterial:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DMATERIAL9* pMaterial = bit_cast<D3DMATERIAL9*>(workItem->Argument1);

				(*pMaterial) = realDevice->mDeviceState.mShaderState.mMaterial;
			}
			break;
			case Device_GetNPatchMode:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				FLOAT* output = bit_cast<FLOAT*>(workItem->Argument1);

				(*output) = realDevice->mDeviceState.mNSegments;
			}
			break;
			case Device_GetPixelShader:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DPixelShader9** ppShader = bit_cast<IDirect3DPixelShader9**>(workItem->Argument1);

				(*ppShader) = (IDirect3DPixelShader9*)realDevice->mDeviceState.mPixelShader;
			}
			break;
			case Device_GetPixelShaderConstantB:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				BOOL* pConstantData = bit_cast<BOOL*>(workItem->Argument2);
				UINT BoolCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					pConstantData[i] = slots.BooleanConstants[StartRegister + i];
				}
			}
			break;
			case Device_GetPixelShaderConstantF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				float* pConstantData = bit_cast<float*>(workItem->Argument2);
				UINT Vector4fCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4fCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					pConstantData[i] = slots.FloatConstants[startIndex + i];
				}
			}
			break;
			case Device_GetPixelShaderConstantI:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				int* pConstantData = bit_cast<int*>(workItem->Argument2);
				UINT Vector4iCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4iCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					pConstantData[i] = slots.IntegerConstants[startIndex + i];
				}
			}
			break;
			case Device_GetRenderState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DRENDERSTATETYPE State = bit_cast<D3DRENDERSTATETYPE>(workItem->Argument1);
				DWORD* pValue = bit_cast<DWORD*>(workItem->Argument2);

				ShaderState* constants = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					constants = &realDevice->mCurrentStateRecording->mDeviceState.mShaderState;
				}
				else
				{
					constants = &realDevice->mDeviceState.mShaderState;
				}

				switch (State)
				{
				case D3DRS_ZENABLE:
					(*pValue) = constants->mRenderState.zEnable;
					break;
				case D3DRS_FILLMODE:
					(*pValue) = constants->mRenderState.fillMode;
					break;
				case D3DRS_SHADEMODE:
					(*pValue) = constants->mRenderState.shadeMode;
					break;
				case D3DRS_ZWRITEENABLE:
					(*pValue) = constants->mRenderState.zWriteEnable;
					break;
				case D3DRS_ALPHATESTENABLE:
					(*pValue) = constants->mRenderState.alphaTestEnable;
					break;
				case D3DRS_LASTPIXEL:
					(*pValue) = constants->mRenderState.lastPixel;
					break;
				case D3DRS_SRCBLEND:
					(*pValue) = constants->mRenderState.sourceBlend;
					break;
				case D3DRS_DESTBLEND:
					(*pValue) = constants->mRenderState.destinationBlend;
					break;
				case D3DRS_CULLMODE:
					(*pValue) = constants->mRenderState.cullMode;
					break;
				case D3DRS_ZFUNC:
					(*pValue) = constants->mRenderState.zFunction;
					break;
				case D3DRS_ALPHAREF:
					(*pValue) = constants->mRenderState.alphaReference;
					break;
				case D3DRS_ALPHAFUNC:
					(*pValue) = constants->mRenderState.alphaFunction;
					break;
				case D3DRS_DITHERENABLE:
					(*pValue) = constants->mRenderState.ditherEnable;
					break;
				case D3DRS_ALPHABLENDENABLE:
					(*pValue) = constants->mRenderState.alphaBlendEnable;
					break;
				case D3DRS_FOGENABLE:
					(*pValue) = constants->mRenderState.fogEnable;
					break;
				case D3DRS_SPECULARENABLE:
					(*pValue) = constants->mRenderState.specularEnable;
					break;
				case D3DRS_FOGCOLOR:
					(*pValue) = constants->mRenderState.fogColor;
					break;
				case D3DRS_FOGTABLEMODE:
					(*pValue) = constants->mRenderState.fogTableMode;
					break;
				case D3DRS_FOGSTART:
					(*pValue) = bit_cast(constants->mRenderState.fogStart);
					break;
				case D3DRS_FOGEND:
					(*pValue) = bit_cast(constants->mRenderState.fogEnd);
					break;
				case D3DRS_FOGDENSITY:
					(*pValue) = bit_cast(constants->mRenderState.fogDensity);
					break;
				case D3DRS_RANGEFOGENABLE:
					(*pValue) = constants->mRenderState.rangeFogEnable;
					break;
				case D3DRS_STENCILENABLE:
					(*pValue) = constants->mRenderState.stencilEnable;
					break;
				case D3DRS_STENCILFAIL:
					(*pValue) = constants->mRenderState.stencilFail;
					break;
				case D3DRS_STENCILZFAIL:
					(*pValue) = constants->mRenderState.stencilZFail;
					break;
				case D3DRS_STENCILPASS:
					(*pValue) = constants->mRenderState.stencilPass;
					break;
				case D3DRS_STENCILFUNC:
					(*pValue) = constants->mRenderState.stencilFunction;
					break;
				case D3DRS_STENCILREF:
					(*pValue) = constants->mRenderState.stencilReference;
					break;
				case D3DRS_STENCILMASK:
					(*pValue) = constants->mRenderState.stencilMask;
					break;
				case D3DRS_STENCILWRITEMASK:
					(*pValue) = constants->mRenderState.stencilWriteMask;
					break;
				case D3DRS_TEXTUREFACTOR:
					(*pValue) = constants->mRenderState.textureFactor;
					break;
				case D3DRS_WRAP0:
					(*pValue) = constants->mRenderState.wrap0;
					break;
				case D3DRS_WRAP1:
					(*pValue) = constants->mRenderState.wrap1;
					break;
				case D3DRS_WRAP2:
					(*pValue) = constants->mRenderState.wrap2;
					break;
				case D3DRS_WRAP3:
					(*pValue) = constants->mRenderState.wrap3;
					break;
				case D3DRS_WRAP4:
					(*pValue) = constants->mRenderState.wrap4;
					break;
				case D3DRS_WRAP5:
					(*pValue) = constants->mRenderState.wrap5;
					break;
				case D3DRS_WRAP6:
					(*pValue) = constants->mRenderState.wrap6;
					break;
				case D3DRS_WRAP7:
					(*pValue) = constants->mRenderState.wrap7;
					break;
				case D3DRS_CLIPPING:
					(*pValue) = constants->mRenderState.clipping;
					break;
				case D3DRS_LIGHTING:
					(*pValue) = constants->mRenderState.lighting;
					break;
				case D3DRS_AMBIENT:
					(*pValue) = constants->mRenderState.ambient;
					break;
				case D3DRS_FOGVERTEXMODE:
					(*pValue) = constants->mRenderState.fogVertexMode;
					break;
				case D3DRS_COLORVERTEX:
					(*pValue) = constants->mRenderState.colorVertex;
					break;
				case D3DRS_LOCALVIEWER:
					(*pValue) = constants->mRenderState.localViewer;
					break;
				case D3DRS_NORMALIZENORMALS:
					(*pValue) = constants->mRenderState.normalizeNormals;
					break;
				case D3DRS_DIFFUSEMATERIALSOURCE:
					(*pValue) = constants->mRenderState.diffuseMaterialSource;
					break;
				case D3DRS_SPECULARMATERIALSOURCE:
					(*pValue) = constants->mRenderState.specularMaterialSource;
					break;
				case D3DRS_AMBIENTMATERIALSOURCE:
					(*pValue) = constants->mRenderState.ambientMaterialSource;
					break;
				case D3DRS_EMISSIVEMATERIALSOURCE:
					(*pValue) = constants->mRenderState.emissiveMaterialSource;
					break;
				case D3DRS_VERTEXBLEND:
					(*pValue) = constants->mRenderState.vertexBlend;
					break;
				case D3DRS_CLIPPLANEENABLE:
					(*pValue) = constants->mRenderState.clipPlaneEnable;
					break;
				case D3DRS_POINTSIZE:
					(*pValue) = constants->mRenderState.pointSize;
					break;
				case D3DRS_POINTSIZE_MIN:
					(*pValue) = bit_cast(constants->mRenderState.pointSizeMinimum);
					break;
				case D3DRS_POINTSPRITEENABLE:
					(*pValue) = constants->mRenderState.pointSpriteEnable;
					break;
				case D3DRS_POINTSCALEENABLE:
					(*pValue) = constants->mRenderState.pointScaleEnable;
					break;
				case D3DRS_POINTSCALE_A:
					(*pValue) = bit_cast(constants->mRenderState.pointScaleA);
					break;
				case D3DRS_POINTSCALE_B:
					(*pValue) = bit_cast(constants->mRenderState.pointScaleB);
					break;
				case D3DRS_POINTSCALE_C:
					(*pValue) = bit_cast(constants->mRenderState.pointScaleC);
					break;
				case D3DRS_MULTISAMPLEANTIALIAS:
					(*pValue) = constants->mRenderState.multisampleAntiAlias;
					break;
				case D3DRS_MULTISAMPLEMASK:
					(*pValue) = constants->mRenderState.multisampleMask;
					break;
				case D3DRS_PATCHEDGESTYLE:
					(*pValue) = constants->mRenderState.patchEdgeStyle;
					break;
				case D3DRS_DEBUGMONITORTOKEN:
					(*pValue) = constants->mRenderState.debugMonitorToken;
					break;
				case D3DRS_POINTSIZE_MAX:
					(*pValue) = bit_cast(constants->mRenderState.pointSizeMaximum);
					break;
				case D3DRS_INDEXEDVERTEXBLENDENABLE:
					(*pValue) = constants->mRenderState.indexedVertexBlendEnable;
					break;
				case D3DRS_COLORWRITEENABLE:
					(*pValue) = constants->mRenderState.colorWriteEnable;
					break;
				case D3DRS_TWEENFACTOR:
					(*pValue) = bit_cast(constants->mRenderState.tweenFactor);
					break;
				case D3DRS_BLENDOP:
					(*pValue) = constants->mRenderState.blendOperation;
					break;
				case D3DRS_POSITIONDEGREE:
					(*pValue) = constants->mRenderState.positionDegree;
					break;
				case D3DRS_NORMALDEGREE:
					(*pValue) = constants->mRenderState.normalDegree;
					break;
				case D3DRS_SCISSORTESTENABLE:
					(*pValue) = constants->mRenderState.scissorTestEnable;
					break;
				case D3DRS_SLOPESCALEDEPTHBIAS:
					(*pValue) = bit_cast(constants->mRenderState.slopeScaleDepthBias);
					break;
				case D3DRS_ANTIALIASEDLINEENABLE:
					(*pValue) = constants->mRenderState.antiAliasedLineEnable;
					break;
				case D3DRS_MINTESSELLATIONLEVEL:
					(*pValue) = bit_cast(constants->mRenderState.minimumTessellationLevel);
					break;
				case D3DRS_MAXTESSELLATIONLEVEL:
					(*pValue) = bit_cast(constants->mRenderState.maximumTessellationLevel);
					break;
				case D3DRS_ADAPTIVETESS_X:
					(*pValue) = bit_cast(constants->mRenderState.adaptivetessX);
					break;
				case D3DRS_ADAPTIVETESS_Y:
					(*pValue) = bit_cast(constants->mRenderState.adaptivetessY);
					break;
				case D3DRS_ADAPTIVETESS_Z:
					(*pValue) = bit_cast(constants->mRenderState.adaptivetessZ);
					break;
				case D3DRS_ADAPTIVETESS_W:
					(*pValue) = bit_cast(constants->mRenderState.adaptivetessW);
					break;
				case D3DRS_ENABLEADAPTIVETESSELLATION:
					(*pValue) = constants->mRenderState.enableAdaptiveTessellation;
					break;
				case D3DRS_TWOSIDEDSTENCILMODE:
					(*pValue) = constants->mRenderState.twoSidedStencilMode;
					break;
				case D3DRS_CCW_STENCILFAIL:
					(*pValue) = constants->mRenderState.ccwStencilFail;
					break;
				case D3DRS_CCW_STENCILZFAIL:
					(*pValue) = constants->mRenderState.ccwStencilZFail;
					break;
				case D3DRS_CCW_STENCILPASS:
					(*pValue) = constants->mRenderState.ccwStencilPass;
					break;
				case D3DRS_CCW_STENCILFUNC:
					(*pValue) = constants->mRenderState.ccwStencilFunction;
					break;
				case D3DRS_COLORWRITEENABLE1:
					(*pValue) = constants->mRenderState.colorWriteEnable1;
					break;
				case D3DRS_COLORWRITEENABLE2:
					(*pValue) = constants->mRenderState.colorWriteEnable2;
					break;
				case D3DRS_COLORWRITEENABLE3:
					(*pValue) = constants->mRenderState.colorWriteEnable3;
					break;
				case D3DRS_BLENDFACTOR:
					(*pValue) = constants->mRenderState.blendFactor;
					break;
				case D3DRS_SRGBWRITEENABLE:
					(*pValue) = constants->mRenderState.srgbWriteEnable;
					break;
				case D3DRS_DEPTHBIAS:
					(*pValue) = bit_cast(constants->mRenderState.depthBias);
					break;
				case D3DRS_WRAP8:
					(*pValue) = constants->mRenderState.wrap8;
					break;
				case D3DRS_WRAP9:
					(*pValue) = constants->mRenderState.wrap9;
					break;
				case D3DRS_WRAP10:
					(*pValue) = constants->mRenderState.wrap10;
					break;
				case D3DRS_WRAP11:
					(*pValue) = constants->mRenderState.wrap11;
					break;
				case D3DRS_WRAP12:
					(*pValue) = constants->mRenderState.wrap12;
					break;
				case D3DRS_WRAP13:
					(*pValue) = constants->mRenderState.wrap13;
					break;
				case D3DRS_WRAP14:
					(*pValue) = constants->mRenderState.wrap14;
					break;
				case D3DRS_WRAP15:
					(*pValue) = constants->mRenderState.wrap15;
					break;
				case D3DRS_SEPARATEALPHABLENDENABLE:
					(*pValue) = constants->mRenderState.separateAlphaBlendEnable;
					break;
				case D3DRS_SRCBLENDALPHA:
					(*pValue) = constants->mRenderState.sourceBlendAlpha;
					break;
				case D3DRS_DESTBLENDALPHA:
					(*pValue) = constants->mRenderState.destinationBlendAlpha;
					break;
				case D3DRS_BLENDOPALPHA:
					(*pValue) = constants->mRenderState.blendOperationAlpha;
					break;
				default:
					Log(warning) << "ProcessQueue unknown state! " << State << std::endl;
					break;
				}
			}
			break;
			case Device_GetSamplerState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Sampler = bit_cast<D3DRENDERSTATETYPE>(workItem->Argument1);
				D3DSAMPLERSTATETYPE Type = bit_cast<D3DSAMPLERSTATETYPE>(workItem->Argument2);
				DWORD* pValue = bit_cast<DWORD*>(workItem->Argument3);

				(*pValue) = realDevice->mDeviceState.mSamplerStates[Sampler][Type];
			}
			break;
			case Device_GetScissorRect:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				RECT* pRect = bit_cast<RECT*>(workItem->Argument1);

				(*pRect) = realDevice->mDeviceState.m9Scissor;
			}
			break;
			case Device_GetStreamSource:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StreamNumber = bit_cast<UINT>(workItem->Argument1);
				IDirect3DVertexBuffer9** ppStreamData = bit_cast<IDirect3DVertexBuffer9**>(workItem->Argument2);
				UINT* pOffsetInBytes = bit_cast<UINT*>(workItem->Argument3);
				UINT* pStride = bit_cast<UINT*>(workItem->Argument4);

				StreamSource& value = realDevice->mDeviceState.mStreamSources[StreamNumber];

				(*ppStreamData) = (IDirect3DVertexBuffer9*)value.StreamData;
				/*
				Vulkan wants 64bit uint but d3d9 uses 32bit uint. This cast just keeps compiler from complaining.
				This should be safe because only 32bit can be set and d3d9 is x86 only so endianness issues shouldn't come into play.
				*/
				(*pOffsetInBytes) = (UINT)value.OffsetInBytes;
				(*pStride) = value.Stride;
			}
			break;
			case Device_GetTexture:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Stage = bit_cast<DWORD>(workItem->Argument1);
				IDirect3DBaseTexture9** ppTexture = bit_cast<IDirect3DBaseTexture9**>(workItem->Argument2);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				(*ppTexture) = state->mTextures[Stage];
			}
			break;
			case Device_GetTextureStageState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Stage = bit_cast<DWORD>(workItem->Argument1);
				D3DTEXTURESTAGESTATETYPE Type = bit_cast<D3DTEXTURESTAGESTATETYPE>(workItem->Argument2);
				DWORD* pValue = bit_cast<DWORD*>(workItem->Argument3);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				switch (Type)
				{
				case D3DTSS_COLOROP:
					(*pValue) = state->mShaderState.mTextureStages[Stage].colorOperation;
					break;
				case D3DTSS_COLORARG1:
					(*pValue) = state->mShaderState.mTextureStages[Stage].colorArgument1;
					break;
				case D3DTSS_COLORARG2:
					(*pValue) = state->mShaderState.mTextureStages[Stage].colorArgument2;
					break;
				case D3DTSS_ALPHAOP:
					(*pValue) = state->mShaderState.mTextureStages[Stage].alphaOperation;
					break;
				case D3DTSS_ALPHAARG1:
					(*pValue) = state->mShaderState.mTextureStages[Stage].alphaArgument1;
					break;
				case D3DTSS_ALPHAARG2:
					(*pValue) = state->mShaderState.mTextureStages[Stage].alphaArgument2;
					break;
				case D3DTSS_BUMPENVMAT00:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapMatrix00;
					break;
				case D3DTSS_BUMPENVMAT01:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapMatrix01;
					break;
				case D3DTSS_BUMPENVMAT10:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapMatrix10;
					break;
				case D3DTSS_BUMPENVMAT11:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapMatrix11;
					break;
				case D3DTSS_TEXCOORDINDEX:
					(*pValue) = state->mShaderState.mTextureStages[Stage].texureCoordinateIndex;
					break;
				case D3DTSS_BUMPENVLSCALE:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapScale;
					break;
				case D3DTSS_BUMPENVLOFFSET:
					(*pValue) = state->mShaderState.mTextureStages[Stage].bumpMapOffset;
					break;
				case D3DTSS_TEXTURETRANSFORMFLAGS:
					(*pValue) = state->mShaderState.mTextureStages[Stage].textureTransformationFlags;
					break;
				case D3DTSS_COLORARG0:
					(*pValue) = state->mShaderState.mTextureStages[Stage].colorArgument0;
					break;
				case D3DTSS_ALPHAARG0:
					(*pValue) = state->mShaderState.mTextureStages[Stage].alphaArgument0;
					break;
				case D3DTSS_RESULTARG:
					(*pValue) = state->mShaderState.mTextureStages[Stage].Result;
					break;
				case D3DTSS_CONSTANT:
					(*pValue) = state->mShaderState.mTextureStages[Stage].Constant;
				default:
					break;
				}
			}
			break;
			case Device_GetTransform:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DTRANSFORMSTATETYPE State = bit_cast<D3DTRANSFORMSTATETYPE>(workItem->Argument1);
				D3DMATRIX* pMatrix = bit_cast<D3DMATRIX*>(workItem->Argument2);

				auto& deviceState = realDevice->mDeviceState;

				switch (State)
				{
				case D3DTS_WORLD:
					(*pMatrix) = deviceState.mTransforms[State];
					break;
				case D3DTS_VIEW:
					(*pMatrix) = deviceState.mTransforms[State];
					break;
				case D3DTS_PROJECTION:
					(*pMatrix) = deviceState.mTransforms[State];
					break;
				case D3DTS_TEXTURE0:
					(*pMatrix) = deviceState.mShaderState.mTextureStages[0].textureTransformationMatrix;
					break;
				case D3DTS_TEXTURE1:
					(*pMatrix) = deviceState.mShaderState.mTextureStages[1].textureTransformationMatrix;
					break;
				case D3DTS_TEXTURE2:
					(*pMatrix) = deviceState.mShaderState.mTextureStages[2].textureTransformationMatrix;
					break;
				case D3DTS_TEXTURE3:
					(*pMatrix) = deviceState.mShaderState.mTextureStages[3].textureTransformationMatrix;
					break;
				case D3DTS_TEXTURE4:
					(*pMatrix) = deviceState.mShaderState.mTextureStages[4].textureTransformationMatrix;
					break;
				case D3DTS_TEXTURE5:
					(*pMatrix) = deviceState.mShaderState.mTextureStages[5].textureTransformationMatrix;
					break;
				case D3DTS_TEXTURE6:
					(*pMatrix) = deviceState.mShaderState.mTextureStages[6].textureTransformationMatrix;
					break;
				case D3DTS_TEXTURE7:
					(*pMatrix) = deviceState.mShaderState.mTextureStages[7].textureTransformationMatrix;
					break;
				default:
					Log(warning) << "Unknown transformation type " << State << std::endl;
					break;
				}
			}
			break;
			case Device_GetVertexDeclaration:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DVertexDeclaration9** ppDecl = bit_cast<IDirect3DVertexDeclaration9**>(workItem->Argument1);

				(*ppDecl) = (IDirect3DVertexDeclaration9*)realDevice->mDeviceState.mVertexDeclaration;
			}
			break;
			case Device_GetVertexShader:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DVertexShader9** ppShader = bit_cast<IDirect3DVertexShader9**>(workItem->Argument1);

				(*ppShader) = (IDirect3DVertexShader9*)realDevice->mDeviceState.mVertexShader;
			}
			break;
			case Device_GetVertexShaderConstantB:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				BOOL* pConstantData = bit_cast<BOOL*>(workItem->Argument2);
				UINT BoolCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					pConstantData[i] = slots.BooleanConstants[StartRegister + i];
				}
			}
			break;
			case Device_GetVertexShaderConstantF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				float* pConstantData = bit_cast<float*>(workItem->Argument2);
				UINT Vector4fCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4fCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					pConstantData[i] = slots.FloatConstants[startIndex + i];
				}
			}
			break;
			case Device_GetVertexShaderConstantI:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				int* pConstantData = bit_cast<int*>(workItem->Argument2);
				UINT Vector4iCount = bit_cast<UINT>(workItem->Argument3);

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4iCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					pConstantData[i] = slots.IntegerConstants[startIndex + i];
				}
			}
			break;
			case Device_GetViewport:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DVIEWPORT9* pViewport = bit_cast<D3DVIEWPORT9*>(workItem->Argument1);

				(*pViewport) = realDevice->mDeviceState.m9Viewport;
			}
			break;
			case Device_LightEnable:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD LightIndex = bit_cast<DWORD>(workItem->Argument1);
				BOOL bEnable = bit_cast<BOOL>(workItem->Argument2);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				state->mShaderState.mLights[LightIndex].IsEnabled = bEnable;
				state->mAreLightsDirty = true;
			}
			break;
			case Device_Reset:
			{
				auto& stateManager = commandStreamManager->mRenderManager.mStateManager;
				HWND FocusWindow = bit_cast<HWND>(workItem->Argument1);

				stateManager.mSwapChains.erase(FocusWindow);
			}
			break;
			case Device_SetFVF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD FVF = bit_cast<DWORD>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.mFVF = FVF;
					deviceState.mVertexDeclaration = nullptr;

					deviceState.mHasFVF = true;
					deviceState.mHasVertexDeclaration = false;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.mFVF = FVF;
					deviceState.mVertexDeclaration = nullptr;

					deviceState.mHasFVF = true;
					deviceState.mHasVertexDeclaration = false;
				}
			}
			break;
			case Device_SetIndices:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DIndexBuffer9* pIndexData = bit_cast<IDirect3DIndexBuffer9*>(workItem->Argument1);
				DeviceState* state = nullptr;
				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				if (pIndexData != nullptr)
				{
					auto realIndex = commandStreamManager->mRenderManager.mStateManager.mIndexBuffers[((CIndexBuffer9*)pIndexData)->mId];

					state->mIndexBuffer = realIndex.get();
					state->mOriginalIndexBuffer = (CIndexBuffer9*)pIndexData;
					state->mHasIndexBuffer = true;
				}
				else
				{
					state->mIndexBuffer = nullptr;
					state->mOriginalIndexBuffer = nullptr;
					state->mHasIndexBuffer = false;
				}
			}
			break;
			case Device_GetIndices:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DIndexBuffer9** pIndexData = bit_cast<IDirect3DIndexBuffer9**>(workItem->Argument1);

				(*pIndexData) = realDevice->mDeviceState.mOriginalIndexBuffer;
			}
			break;
			case Device_SetLight:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Index = bit_cast<DWORD>(workItem->Argument1);
				D3DLIGHT9* pLight = bit_cast<D3DLIGHT9*>(workItem->Argument2);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				state->mAreLightsDirty = true;

				Light light = {};

				light.Type = pLight->Type;
				light.Diffuse[0] = pLight->Diffuse.r;
				light.Diffuse[1] = pLight->Diffuse.g;
				light.Diffuse[2] = pLight->Diffuse.b;
				light.Diffuse[3] = pLight->Diffuse.a;

				light.Specular[0] = pLight->Specular.r;
				light.Specular[1] = pLight->Specular.g;
				light.Specular[2] = pLight->Specular.b;
				light.Specular[3] = pLight->Specular.a;

				light.Ambient[0] = pLight->Ambient.r;
				light.Ambient[1] = pLight->Ambient.g;
				light.Ambient[2] = pLight->Ambient.b;
				light.Ambient[3] = pLight->Ambient.a;

				light.Position[0] = pLight->Position.x;
				light.Position[1] = pLight->Position.y;
				light.Position[2] = pLight->Position.z;
				//No need to set [3] because structure is init with {} so it's already 0.

				light.Direction[0] = pLight->Direction.x;
				light.Direction[1] = pLight->Direction.y;
				light.Direction[2] = pLight->Direction.z;
				//No need to set [3] because structure is init with {} so it's already 0.

				light.Range = pLight->Range;
				light.Falloff = pLight->Falloff;
				light.Attenuation0 = pLight->Attenuation0;
				light.Attenuation1 = pLight->Attenuation1;
				light.Attenuation2 = pLight->Attenuation2;
				light.Theta = pLight->Theta;
				light.Phi = pLight->Phi;


				light.IsEnabled = state->mShaderState.mLights[Index].IsEnabled;
				state->mShaderState.mLights[Index] = light;
			}
			break;
			case Device_SetMaterial:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DMATERIAL9* pMaterial = bit_cast<D3DMATERIAL9*>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.mShaderState.mMaterial = (*pMaterial);
					deviceState.mIsMaterialDirty = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.mShaderState.mMaterial = (*pMaterial);
					deviceState.mIsMaterialDirty = true;
				}
			}
			break;
			case Device_SetNPatchMode:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				float nSegments = bit_cast<float>(workItem->Argument1);

				if (nSegments > 0.0f)
				{
					Log(warning) << "ProcessQueue nPatch greater than zero not supported." << std::endl;
				}

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					realDevice->mCurrentStateRecording->mDeviceState.mNSegments = nSegments;
				}
				else
				{
					realDevice->mDeviceState.mNSegments = nSegments;
				}
			}
			break;
			case Device_SetPixelShader:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DPixelShader9* pShader = bit_cast<IDirect3DPixelShader9*>(workItem->Argument1);

				if (pShader != nullptr)
				{
					pShader->AddRef();
				}

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					realDevice->mCurrentStateRecording->mDeviceState.mPixelShader = (CPixelShader9*)pShader;
					realDevice->mCurrentStateRecording->mDeviceState.mHasPixelShader = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					if (deviceState.mPixelShader != nullptr)
					{
						deviceState.mPixelShader->Release();
					}

					deviceState.mPixelShader = (CPixelShader9*)pShader;
					deviceState.mHasPixelShader = true;
				}
			}
			break;
			case Device_SetPixelShaderConstantB:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				BOOL* pConstantData = bit_cast<BOOL*>(workItem->Argument2);
				UINT BoolCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mArePixelShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					slots.BooleanConstants[StartRegister + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetPixelShaderConstantF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				float* pConstantData = bit_cast<float*>(workItem->Argument2);
				UINT Vector4fCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mArePixelShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4fCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					slots.FloatConstants[startIndex + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetPixelShaderConstantI:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				int* pConstantData = bit_cast<int*>(workItem->Argument2);
				UINT Vector4iCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mArePixelShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mPixelShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4iCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					slots.IntegerConstants[startIndex + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetRenderState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DRENDERSTATETYPE State = bit_cast<D3DRENDERSTATETYPE>(workItem->Argument1);
				DWORD Value = bit_cast<DWORD>(workItem->Argument2);

				ShaderState* constants = nullptr;
				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					constants = &realDevice->mCurrentStateRecording->mDeviceState.mShaderState;
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					constants = &realDevice->mDeviceState.mShaderState;
					state = &realDevice->mDeviceState;
				}

				state->mIsRenderStateDirty = true;

				switch (State)
				{
				case D3DRS_ZENABLE:
					constants->mRenderState.zEnable = Value;
					state->wasDsaGroupModified = true;
					state->wasMultisampleGroupModified = true;
					state->mIsZBiasDirty = true;
					break;
				case D3DRS_FILLMODE:
					constants->mRenderState.fillMode = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_SHADEMODE:
					constants->mRenderState.shadeMode = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_ZWRITEENABLE:
					constants->mRenderState.zWriteEnable = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_ALPHATESTENABLE:
					constants->mRenderState.alphaTestEnable = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_LASTPIXEL:
					constants->mRenderState.lastPixel = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_SRCBLEND:
					constants->mRenderState.sourceBlend = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_DESTBLEND:
					constants->mRenderState.destinationBlend = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_CULLMODE:
					constants->mRenderState.cullMode = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_ZFUNC:
					constants->mRenderState.zFunction = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_ALPHAREF:
					constants->mRenderState.alphaReference = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_ALPHAFUNC:
					constants->mRenderState.alphaFunction = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_DITHERENABLE:
					constants->mRenderState.ditherEnable = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_ALPHABLENDENABLE:
					constants->mRenderState.alphaBlendEnable = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_FOGENABLE:
					constants->mRenderState.fogEnable = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasFogGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_SPECULARENABLE:
					constants->mRenderState.specularEnable = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_FOGCOLOR:
					constants->mRenderState.fogColor = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_FOGTABLEMODE:
					constants->mRenderState.fogTableMode = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasFogGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_FOGSTART:
					constants->mRenderState.fogStart = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_FOGEND:
					constants->mRenderState.fogEnd = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_FOGDENSITY:
					constants->mRenderState.fogDensity = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					state->wasPixelShaderConstantGroupModified = true;
					break;
				case D3DRS_RANGEFOGENABLE:
					constants->mRenderState.rangeFogEnable = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_STENCILENABLE:
					constants->mRenderState.stencilEnable = Value;
					state->wasDsaGroupModified = true;
					state->wasMultisampleGroupModified = true;
					break;
				case D3DRS_STENCILFAIL:
					constants->mRenderState.stencilFail = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILZFAIL:
					constants->mRenderState.stencilZFail = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILPASS:
					constants->mRenderState.stencilPass = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILFUNC:
					constants->mRenderState.stencilFunction = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILREF:
					constants->mRenderState.stencilReference = Value;
					state->hasStencilReference = true;
					break;
				case D3DRS_STENCILMASK:
					constants->mRenderState.stencilMask = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_STENCILWRITEMASK:
					constants->mRenderState.stencilWriteMask = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_TEXTUREFACTOR:
					constants->mRenderState.textureFactor = Value;
					state->hasTextureFactor = true;
					break;
				case D3DRS_WRAP0:
					constants->mRenderState.wrap0 = Value;
					state->hasWrap0 = true;
					break;
				case D3DRS_WRAP1:
					constants->mRenderState.wrap1 = Value;
					state->hasWrap1 = true;
					break;
				case D3DRS_WRAP2:
					constants->mRenderState.wrap2 = Value;
					state->hasWrap2 = true;
					break;
				case D3DRS_WRAP3:
					constants->mRenderState.wrap3 = Value;
					state->hasWrap3 = true;
					break;
				case D3DRS_WRAP4:
					constants->mRenderState.wrap4 = Value;
					state->hasWrap4 = true;
					break;
				case D3DRS_WRAP5:
					constants->mRenderState.wrap5 = Value;
					state->hasWrap5 = true;
					break;
				case D3DRS_WRAP6:
					constants->mRenderState.wrap6 = Value;
					state->hasWrap6 = true;
					break;
				case D3DRS_WRAP7:
					constants->mRenderState.wrap7 = Value;
					state->hasWrap7 = true;
					break;
				case D3DRS_CLIPPING:
					constants->mRenderState.clipping = Value;
					state->hasClipping = true;
					break;
				case D3DRS_LIGHTING:
					constants->mRenderState.lighting = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_AMBIENT:
					constants->mRenderState.ambient = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_FOGVERTEXMODE:
					constants->mRenderState.fogVertexMode = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_COLORVERTEX:
					constants->mRenderState.colorVertex = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_LOCALVIEWER:
					constants->mRenderState.localViewer = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_NORMALIZENORMALS:
					constants->mRenderState.normalizeNormals = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_DIFFUSEMATERIALSOURCE:
					constants->mRenderState.diffuseMaterialSource = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_SPECULARMATERIALSOURCE:
					constants->mRenderState.specularMaterialSource = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_AMBIENTMATERIALSOURCE:
					constants->mRenderState.ambientMaterialSource = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_EMISSIVEMATERIALSOURCE:
					constants->mRenderState.emissiveMaterialSource = Value;
					state->wasFixedFunctionLightingGroupModified = true;
					break;
				case D3DRS_VERTEXBLEND:
					constants->mRenderState.vertexBlend = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_CLIPPLANEENABLE:
					constants->mRenderState.clipPlaneEnable = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_POINTSIZE:
					constants->mRenderState.pointSize = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_POINTSIZE_MIN:
					constants->mRenderState.pointSizeMinimum = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_POINTSPRITEENABLE:
					constants->mRenderState.pointSpriteEnable = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_POINTSCALEENABLE:
					constants->mRenderState.pointScaleEnable = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_POINTSCALE_A:
					constants->mRenderState.pointScaleA = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_POINTSCALE_B:
					constants->mRenderState.pointScaleB = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_POINTSCALE_C:
					constants->mRenderState.pointScaleC = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_MULTISAMPLEANTIALIAS:
					constants->mRenderState.multisampleAntiAlias = Value;
					state->wasMultisampleGroupModified = true;
					break;
				case D3DRS_MULTISAMPLEMASK:
					constants->mRenderState.multisampleMask = Value;
					state->hasMultisampleMask = true;
					break;
				case D3DRS_PATCHEDGESTYLE:
					constants->mRenderState.patchEdgeStyle = Value;
					state->hasPatchEdgeStyle = true;
					break;
				case D3DRS_DEBUGMONITORTOKEN:
					constants->mRenderState.debugMonitorToken = Value;
					state->hasDebugMonitorToken = true;
					break;
				case D3DRS_POINTSIZE_MAX:
					constants->mRenderState.pointSizeMaximum = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_INDEXEDVERTEXBLENDENABLE:
					constants->mRenderState.indexedVertexBlendEnable = Value;
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_COLORWRITEENABLE:
					constants->mRenderState.colorWriteEnable = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_TWEENFACTOR:
					constants->mRenderState.tweenFactor = bit_cast(Value);
					state->wasFixedFunctionOtherGroupModified = true;
					break;
				case D3DRS_BLENDOP:
					constants->mRenderState.blendOperation = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_POSITIONDEGREE:
					constants->mRenderState.positionDegree = Value;
					state->hasPositionDegree = true;
					break;
				case D3DRS_NORMALDEGREE:
					constants->mRenderState.normalDegree = Value;
					state->hasNormalDegree = true;
					break;
				case D3DRS_SCISSORTESTENABLE:
					constants->mRenderState.scissorTestEnable = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_SLOPESCALEDEPTHBIAS:
					constants->mRenderState.slopeScaleDepthBias = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					state->mIsZBiasDirty = true;
					break;
				case D3DRS_ANTIALIASEDLINEENABLE:
					constants->mRenderState.antiAliasedLineEnable = Value;
					state->wasRasterizerGroupModified = true;
					break;
				case D3DRS_MINTESSELLATIONLEVEL:
					constants->mRenderState.minimumTessellationLevel = bit_cast(Value);
					state->hasMinimumTessellationLevel = true;
					break;
				case D3DRS_MAXTESSELLATIONLEVEL:
					constants->mRenderState.maximumTessellationLevel = bit_cast(Value);
					state->hasMaximumTessellationLevel = true;
					break;
				case D3DRS_ADAPTIVETESS_X:
					constants->mRenderState.adaptivetessX = bit_cast(Value);
					state->hasAdaptivetessX = true;
					break;
				case D3DRS_ADAPTIVETESS_Y:
					constants->mRenderState.adaptivetessY = bit_cast(Value);
					state->hasAdaptivetessY = true;
					break;
				case D3DRS_ADAPTIVETESS_Z:
					constants->mRenderState.adaptivetessZ = bit_cast(Value);
					state->hasAdaptivetessZ = true;
					break;
				case D3DRS_ADAPTIVETESS_W:
					constants->mRenderState.adaptivetessW = bit_cast(Value);
					state->hasAdaptivetessW = true;
					break;
				case D3DRS_ENABLEADAPTIVETESSELLATION:
					constants->mRenderState.enableAdaptiveTessellation = Value;
					state->hasEnableAdaptiveTessellation = true;
					break;
				case D3DRS_TWOSIDEDSTENCILMODE:
					constants->mRenderState.twoSidedStencilMode = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_CCW_STENCILFAIL:
					constants->mRenderState.ccwStencilFail = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_CCW_STENCILZFAIL:
					constants->mRenderState.ccwStencilZFail = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_CCW_STENCILPASS:
					constants->mRenderState.ccwStencilPass = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_CCW_STENCILFUNC:
					constants->mRenderState.ccwStencilFunction = Value;
					state->wasDsaGroupModified = true;
					break;
				case D3DRS_COLORWRITEENABLE1:
					constants->mRenderState.colorWriteEnable1 = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_COLORWRITEENABLE2:
					constants->mRenderState.colorWriteEnable2 = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_COLORWRITEENABLE3:
					constants->mRenderState.colorWriteEnable3 = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_BLENDFACTOR:
					constants->mRenderState.blendFactor = Value;
					state->hasBlendFactor = true;
					break;
				case D3DRS_SRGBWRITEENABLE:
					constants->mRenderState.srgbWriteEnable = Value;
					state->hasSrgbWriteEnable = true;
					break;
				case D3DRS_DEPTHBIAS:
					constants->mRenderState.depthBias = bit_cast(Value);
					state->wasRasterizerGroupModified = true;
					state->mIsZBiasDirty = true;
					break;
				case D3DRS_WRAP8:
					constants->mRenderState.wrap8 = Value;
					state->hasWrap8 = true;
					break;
				case D3DRS_WRAP9:
					constants->mRenderState.wrap9 = Value;
					state->hasWrap9 = true;
					break;
				case D3DRS_WRAP10:
					constants->mRenderState.wrap10 = Value;
					state->hasWrap10 = true;
					break;
				case D3DRS_WRAP11:
					constants->mRenderState.wrap11 = Value;
					state->hasWrap11 = true;
					break;
				case D3DRS_WRAP12:
					constants->mRenderState.wrap12 = Value;
					state->hasWrap12 = true;
					break;
				case D3DRS_WRAP13:
					constants->mRenderState.wrap13 = Value;
					state->hasWrap13 = true;
					break;
				case D3DRS_WRAP14:
					constants->mRenderState.wrap14 = Value;
					state->hasWrap14 = true;
					break;
				case D3DRS_WRAP15:
					constants->mRenderState.wrap15 = Value;
					state->hasWrap15 = true;
					break;
				case D3DRS_SEPARATEALPHABLENDENABLE:
					constants->mRenderState.separateAlphaBlendEnable = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_SRCBLENDALPHA:
					constants->mRenderState.sourceBlendAlpha = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_DESTBLENDALPHA:
					constants->mRenderState.destinationBlendAlpha = Value;
					state->wasBlendGroupModified = true;
					break;
				case D3DRS_BLENDOPALPHA:
					constants->mRenderState.blendOperationAlpha = Value;
					state->wasBlendGroupModified = true;
					break;
				default:
					Log(warning) << "ProcessQueue unknown state! " << State << std::endl;
					break;
				}
			}
			break;
			case Device_SetSamplerState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Sampler = bit_cast<DWORD>(workItem->Argument1);
				D3DSAMPLERSTATETYPE Type = bit_cast<D3DSAMPLERSTATETYPE>(workItem->Argument2);
				DWORD Value = bit_cast<DWORD>(workItem->Argument3);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				state->mSamplerStates[Sampler][Type] = Value;

				//Mark the index as updated.
				state->mSamplerStates[Sampler][0] = 1;
			}
			break;
			case Device_SetScissorRect:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				RECT* pRect = bit_cast<RECT*>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.m9Scissor = (*pRect);

					deviceState.mScissor.extent.width = deviceState.m9Scissor.right;
					deviceState.mScissor.extent.height = deviceState.m9Scissor.bottom;
					deviceState.mScissor.offset.x = deviceState.m9Scissor.left;
					deviceState.mScissor.offset.y = deviceState.m9Scissor.top;

					deviceState.mIsScissorDirty = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.m9Scissor = (*pRect);

					deviceState.mScissor.extent.width = deviceState.m9Scissor.right;
					deviceState.mScissor.extent.height = deviceState.m9Scissor.bottom;
					deviceState.mScissor.offset.x = deviceState.m9Scissor.left;
					deviceState.mScissor.offset.y = deviceState.m9Scissor.top;

					deviceState.mIsScissorDirty = true;
				}
			}
			break;
			case Device_SetStreamSource:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StreamNumber = bit_cast<UINT>(workItem->Argument1);
				IDirect3DVertexBuffer9* pStreamData = bit_cast<IDirect3DVertexBuffer9*>(workItem->Argument2);
				UINT OffsetInBytes = bit_cast<UINT>(workItem->Argument3);
				UINT Stride = bit_cast<UINT>(workItem->Argument4);

				CVertexBuffer9* streamData = (CVertexBuffer9*)pStreamData;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);
					deviceState.mAreStreamSourcesDirty = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.mStreamSources[StreamNumber] = StreamSource(StreamNumber, streamData, OffsetInBytes, Stride);
					deviceState.mAreStreamSourcesDirty = true;
				}
			}
			break;
			case Device_SetTexture:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Sampler = bit_cast<DWORD>(workItem->Argument1);
				IDirect3DBaseTexture9* pTexture = bit_cast<IDirect3DBaseTexture9*>(workItem->Argument2);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				//state->mIsRenderStateDirty = true;
				//state->mAreTextureStagesDirty = true;
				state->mAreTexturesDirty = true;

				state->mTextures[Sampler] = pTexture;
			}
			break;
			case Device_SetTextureStageState:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				DWORD Stage = bit_cast<DWORD>(workItem->Argument1);
				D3DTEXTURESTAGESTATETYPE Type = bit_cast<D3DTEXTURESTAGESTATETYPE>(workItem->Argument2);
				DWORD Value = bit_cast<DWORD>(workItem->Argument3);

				DeviceState* state = nullptr;

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					state = &realDevice->mCurrentStateRecording->mDeviceState;
				}
				else
				{
					state = &realDevice->mDeviceState;
				}

				state->mAreTextureStagesDirty = true;

				switch (Type)
				{
				case D3DTSS_COLOROP:
					state->mShaderState.mTextureStages[Stage].colorOperation = Value;
					break;
				case D3DTSS_COLORARG1:
					state->mShaderState.mTextureStages[Stage].colorArgument1 = Value;
					break;
				case D3DTSS_COLORARG2:
					state->mShaderState.mTextureStages[Stage].colorArgument2 = Value;
					break;
				case D3DTSS_ALPHAOP:
					state->mShaderState.mTextureStages[Stage].alphaOperation = Value;
					break;
				case D3DTSS_ALPHAARG1:
					state->mShaderState.mTextureStages[Stage].alphaArgument1 = Value;
					break;
				case D3DTSS_ALPHAARG2:
					state->mShaderState.mTextureStages[Stage].alphaArgument2 = Value;
					break;
				case D3DTSS_BUMPENVMAT00:
					state->mShaderState.mTextureStages[Stage].bumpMapMatrix00 = bit_cast(Value);
					break;
				case D3DTSS_BUMPENVMAT01:
					state->mShaderState.mTextureStages[Stage].bumpMapMatrix01 = bit_cast(Value);
					break;
				case D3DTSS_BUMPENVMAT10:
					state->mShaderState.mTextureStages[Stage].bumpMapMatrix10 = bit_cast(Value);
					break;
				case D3DTSS_BUMPENVMAT11:
					state->mShaderState.mTextureStages[Stage].bumpMapMatrix11 = bit_cast(Value);
					break;
				case D3DTSS_TEXCOORDINDEX:
					state->mShaderState.mTextureStages[Stage].texureCoordinateIndex = Value;
					break;
				case D3DTSS_BUMPENVLSCALE:
					state->mShaderState.mTextureStages[Stage].bumpMapScale = bit_cast(Value);
					break;
				case D3DTSS_BUMPENVLOFFSET:
					state->mShaderState.mTextureStages[Stage].bumpMapOffset = bit_cast(Value);
					break;
				case D3DTSS_TEXTURETRANSFORMFLAGS:
					state->mShaderState.mTextureStages[Stage].textureTransformationFlags = Value;
					break;
				case D3DTSS_COLORARG0:
					state->mShaderState.mTextureStages[Stage].colorArgument0 = Value;
					break;
				case D3DTSS_ALPHAARG0:
					state->mShaderState.mTextureStages[Stage].alphaArgument0 = Value;
					break;
				case D3DTSS_RESULTARG:
					state->mShaderState.mTextureStages[Stage].Result = Value;
					break;
				case D3DTSS_CONSTANT:
					state->mShaderState.mTextureStages[Stage].Constant = Value;
					break;
				default:
					break;
				}
			}
			break;
			case Device_SetTransform:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				D3DTRANSFORMSTATETYPE State = bit_cast<D3DTRANSFORMSTATETYPE>(workItem->Argument1);
				D3DMATRIX* pMatrix = bit_cast<D3DMATRIX*>(workItem->Argument2);


				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					switch (State)
					{
					case D3DTS_WORLD:
						deviceState.mTransforms[State] = (*pMatrix);
						deviceState.mHasTransformsChanged = true;
						break;
					case D3DTS_VIEW:
						deviceState.mTransforms[State] = (*pMatrix);
						deviceState.mHasTransformsChanged = true;
						break;
					case D3DTS_PROJECTION:
						deviceState.mTransforms[State] = (*pMatrix);
						deviceState.mHasTransformsChanged = true;
						break;
					case D3DTS_TEXTURE0:
						deviceState.mShaderState.mTextureStages[0].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE1:
						deviceState.mShaderState.mTextureStages[1].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE2:
						deviceState.mShaderState.mTextureStages[2].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE3:
						deviceState.mShaderState.mTextureStages[3].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE4:
						deviceState.mShaderState.mTextureStages[4].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE5:
						deviceState.mShaderState.mTextureStages[5].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE6:
						deviceState.mShaderState.mTextureStages[6].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE7:
						deviceState.mShaderState.mTextureStages[7].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					default:
						Log(warning) << "Unknown transformation type " << State << std::endl;
						break;
					}
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					switch (State)
					{
					case D3DTS_WORLD:
						deviceState.mTransforms[State] = (*pMatrix);
						deviceState.mHasTransformsChanged = true;
						break;
					case D3DTS_VIEW:
						deviceState.mTransforms[State] = (*pMatrix);
						deviceState.mHasTransformsChanged = true;
						break;
					case D3DTS_PROJECTION:
						deviceState.mTransforms[State] = (*pMatrix);
						deviceState.mHasTransformsChanged = true;
						break;
					case D3DTS_TEXTURE0:
						deviceState.mShaderState.mTextureStages[0].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE1:
						deviceState.mShaderState.mTextureStages[1].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE2:
						deviceState.mShaderState.mTextureStages[2].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE3:
						deviceState.mShaderState.mTextureStages[3].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE4:
						deviceState.mShaderState.mTextureStages[4].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE5:
						deviceState.mShaderState.mTextureStages[5].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE6:
						deviceState.mShaderState.mTextureStages[6].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					case D3DTS_TEXTURE7:
						deviceState.mShaderState.mTextureStages[7].textureTransformationMatrix = (*pMatrix);
						deviceState.mAreTextureStagesDirty = true;
						break;
					default:
						Log(warning) << "Unknown transformation type " << State << std::endl;
						break;
					}
				}
			}
			break;
			case Device_SetVertexDeclaration:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DVertexDeclaration9* pDecl = bit_cast<IDirect3DVertexDeclaration9*>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;
					deviceState.mFVF = 0;

					deviceState.mHasVertexDeclaration = true;
					deviceState.mHasFVF = false;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.mVertexDeclaration = (CVertexDeclaration9*)pDecl;
					deviceState.mFVF = 0;

					deviceState.mHasVertexDeclaration = true;
					deviceState.mHasFVF = false;
				}
			}
			break;
			case Device_SetVertexShader:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DVertexShader9* pShader = bit_cast<IDirect3DVertexShader9*>(workItem->Argument1);

				if (pShader != nullptr)
				{
					pShader->AddRef();
				}

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;
					//Log(info) << "Recorded VertexShader"<< std::endl;
					deviceState.mVertexShader = (CVertexShader9*)pShader;
					deviceState.mHasVertexShader = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;
					if (deviceState.mVertexShader != nullptr)
					{
						deviceState.mVertexShader->Release();
					}

					deviceState.mVertexShader = (CVertexShader9*)pShader;
					deviceState.mHasVertexShader = true;
				}
			}
			break;
			case Device_SetVertexShaderConstantB:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				BOOL* pConstantData = bit_cast<BOOL*>(workItem->Argument2);
				UINT BoolCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mAreVertexShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				for (size_t i = 0; i < BoolCount; i++)
				{
					slots.BooleanConstants[StartRegister + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetVertexShaderConstantF:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				float* pConstantData = bit_cast<float*>(workItem->Argument2);
				UINT Vector4fCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mAreVertexShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4fCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					slots.FloatConstants[startIndex + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetVertexShaderConstantI:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				UINT StartRegister = bit_cast<UINT>(workItem->Argument1);
				int* pConstantData = bit_cast<int*>(workItem->Argument2);
				UINT Vector4iCount = bit_cast<UINT>(workItem->Argument3);

				realDevice->mDeviceState.mAreVertexShaderSlotsDirty = true;

				auto& slots = realDevice->mDeviceState.mVertexShaderConstantSlots;
				uint32_t startIndex = (StartRegister * 4);
				uint32_t length = (Vector4iCount * 4);
				for (size_t i = 0; i < length; i++)
				{
					slots.IntegerConstants[startIndex + i] = pConstantData[i];
				}
			}
			break;
			case Device_SetViewport:
			{
				auto& renderManager = commandStreamManager->mRenderManager;
				auto& realDevice = renderManager.mStateManager.mDevices[workItem->Id];

				D3DVIEWPORT9* pViewport = bit_cast<D3DVIEWPORT9*>(workItem->Argument1);

				if (realDevice->mCurrentStateRecording != nullptr)
				{
					auto& deviceState = realDevice->mCurrentStateRecording->mDeviceState;

					deviceState.m9Viewport = (*pViewport);

					deviceState.mViewport.x = (float)deviceState.m9Viewport.X - 0.5f;
					deviceState.mViewport.y = (float)deviceState.m9Viewport.Y - 0.5f;
					deviceState.mViewport.width = (float)deviceState.m9Viewport.Width - 0.5f;
					deviceState.mViewport.height = (float)deviceState.m9Viewport.Height - 0.5f;
					deviceState.mViewport.minDepth = deviceState.m9Viewport.MinZ;
					deviceState.mViewport.maxDepth = deviceState.m9Viewport.MaxZ;

					deviceState.mIsViewportDirty = true;
				}
				else
				{
					auto& deviceState = realDevice->mDeviceState;

					deviceState.m9Viewport = (*pViewport);

					deviceState.mViewport.x = (float)deviceState.m9Viewport.X - 0.5f;
					deviceState.mViewport.y = (float)deviceState.m9Viewport.Y - 0.5f;
					deviceState.mViewport.width = (float)deviceState.m9Viewport.Width - 0.5f;
					deviceState.mViewport.height = (float)deviceState.m9Viewport.Height - 0.5f;
					deviceState.mViewport.minDepth = deviceState.m9Viewport.MinZ;
					deviceState.mViewport.maxDepth = deviceState.m9Viewport.MaxZ;

					deviceState.mIsViewportDirty = true;
				}
			}
			break;
			case Device_StretchRect:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DSurface9* pSourceSurface = bit_cast<IDirect3DSurface9*>(workItem->Argument1);
				RECT* pSourceRect = bit_cast<RECT*>(workItem->Argument2);
				IDirect3DSurface9* pDestSurface = bit_cast<IDirect3DSurface9*>(workItem->Argument3);
				RECT* pDestRect = bit_cast<RECT*>(workItem->Argument4);
				D3DTEXTUREFILTERTYPE Filter = bit_cast<D3DTEXTUREFILTERTYPE>(workItem->Argument5);

				commandStreamManager->mRenderManager.StretchRect(realDevice, pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
			}
			case Device_UpdateSurface:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DSurface9* pSourceSurface = bit_cast<IDirect3DSurface9*>(workItem->Argument1);
				RECT* pSourceRect = bit_cast<RECT*>(workItem->Argument2);
				IDirect3DSurface9* pDestinationSurface = bit_cast<IDirect3DSurface9*>(workItem->Argument2);
				POINT* pDestinationPoint = bit_cast<POINT*>(workItem->Argument2);

				commandStreamManager->mRenderManager.UpdateSurface(realDevice, pSourceSurface, pSourceRect, pDestinationSurface, pDestinationPoint);
			}
			case Device_UpdateTexture:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				IDirect3DBaseTexture9* pSourceTexture = bit_cast<IDirect3DBaseTexture9*>(workItem->Argument1);
				IDirect3DBaseTexture9* pDestinationTexture = bit_cast<IDirect3DBaseTexture9*>(workItem->Argument2);

				commandStreamManager->mRenderManager.UpdateTexture(realDevice, pSourceTexture, pDestinationTexture);
			}
			break;
			case Device_GetAvailableTextureMem:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CDevice9* device9 = (CDevice9*)workItem->Caller;
				//                                                                                            Bytes     KB      MB
				device9->mAvailableTextureMemory = (((realDevice->mEstimatedMemory - realDevice->mEstimatedMemoryUsed) / 1024) / 1024);
			}
			break;
			case Instance_GetAdapterIdentifier:
			{
				UINT Adapter = bit_cast<UINT>(workItem->Argument1);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument2);
				D3DADAPTER_IDENTIFIER9* pIdentifier = bit_cast<D3DADAPTER_IDENTIFIER9*>(workItem->Argument3);
				auto instance = commandStreamManager->mRenderManager.mStateManager.mInstances[workItem->Id];
				auto device = instance->mPhysicalDevices[Adapter];

				vk::PhysicalDeviceProperties properties;

				device.getProperties(&properties);

				(*pIdentifier) = {}; //zero it out.

				strcpy(pIdentifier->DeviceName, "Radeon (TM) RX 480 Graphics"); //properties.deviceName
				strcpy(pIdentifier->Driver, "aticfx64.dll,aticfx64.dll,aticfx64.dll,aticfx32,aticfx32,aticfx32,atiumd64.dll,atidxx64.dll,atidxx64.dll,atiumdag,atidxx32,atidxx32,atiumdva,atiumd6a.cap,atitmm64.dll"); //revisit
				strcpy(pIdentifier->Description, "VK9 Emulated Device"); //revisit
				pIdentifier->VendorId = 0x1002;  //properties.vendorID;
				pIdentifier->DeviceId = 0x67DF;  //properties.deviceID;
				pIdentifier->SubSysId = 0x94801682;
				pIdentifier->Revision = 0x00C7;
				pIdentifier->WHQLLevel = 1;

				pIdentifier->DriverVersion.QuadPart = properties.driverVersion;

				//little endian assumption.
				CustomGUID guid = { 0xD7B71EE2 , 0x249F , 0x11CF , 0x3275 , 0x17B57BC2D835 };
				pIdentifier->DeviceIdentifier = bit_cast<GUID>(guid);

			}
			break;
			case Instance_GetDeviceCaps:
			{
				UINT Adapter = bit_cast<UINT>(workItem->Argument1);
				D3DDEVTYPE DeviceType = bit_cast<D3DDEVTYPE>(workItem->Argument2);
				D3DCAPS9* pCaps = bit_cast<D3DCAPS9*>(workItem->Argument3);
				auto instance = commandStreamManager->mRenderManager.mStateManager.mInstances[workItem->Id];
				auto device = instance->mPhysicalDevices[Adapter];

				vk::PhysicalDeviceProperties properties;
				vk::PhysicalDeviceFeatures features;

				device.getProperties(&properties);
				device.getFeatures(&features);

				/*
				https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkPhysicalDeviceProperties
				https://msdn.microsoft.com/en-us/library/windows/desktop/bb172513(v=vs.85).aspx
				https://msdn.microsoft.com/en-us/library/windows/desktop/bb172635(v=vs.85).aspx
				https://msdn.microsoft.com/en-us/library/windows/desktop/bb172591(v=vs.85).aspx
				*/

				(*pCaps) = {};

				//Re-worked Caps based off of SwiftGL, ToGL, Nine, Wine, MS Docs, and local implementation.
				pCaps->DeviceType = DeviceType;

				pCaps->AdapterOrdinal = 0;

				pCaps->Caps = D3DCAPS_READ_SCANLINE; //Based on ToGL & Swift this is DX7 stuff.

				pCaps->Caps2 = D3DCAPS2_CANMANAGERESOURCE |
					/* D3DCAPS2_CANSHARERESOURCE | */
					/* D3DCAPS2_CANCALIBRATEGAMMA | */
					D3DCAPS2_DYNAMICTEXTURES |
					D3DCAPS2_FULLSCREENGAMMA |
					D3DCAPS2_CANAUTOGENMIPMAP;

				pCaps->Caps3 = D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD |
					D3DCAPS3_COPY_TO_VIDMEM |
					D3DCAPS3_COPY_TO_SYSTEMMEM |
					D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION;

				pCaps->Caps3 = D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD |
					D3DCAPS3_COPY_TO_VIDMEM |
					D3DCAPS3_COPY_TO_SYSTEMMEM |
					D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION;

				pCaps->PresentationIntervals = D3DPRESENT_INTERVAL_DEFAULT |
					D3DPRESENT_INTERVAL_ONE |
					D3DPRESENT_INTERVAL_TWO |
					D3DPRESENT_INTERVAL_THREE |
					D3DPRESENT_INTERVAL_FOUR |
					D3DPRESENT_INTERVAL_IMMEDIATE;

				pCaps->CursorCaps = D3DCURSORCAPS_COLOR | D3DCURSORCAPS_LOWRES;

				pCaps->DevCaps = D3DDEVCAPS_CANBLTSYSTONONLOCAL |
					D3DDEVCAPS_CANRENDERAFTERFLIP |
					D3DDEVCAPS_DRAWPRIMITIVES2 |
					D3DDEVCAPS_DRAWPRIMITIVES2EX |
					D3DDEVCAPS_DRAWPRIMTLVERTEX |
					D3DDEVCAPS_EXECUTESYSTEMMEMORY |
					D3DDEVCAPS_EXECUTEVIDEOMEMORY |
					D3DDEVCAPS_HWRASTERIZATION |
					D3DDEVCAPS_HWTRANSFORMANDLIGHT |
					/*D3DDEVCAPS_NPATCHES |*/
					D3DDEVCAPS_PUREDEVICE |
					/*D3DDEVCAPS_QUINTICRTPATCHES |*/
					/*D3DDEVCAPS_RTPATCHES |*/
					/*D3DDEVCAPS_RTPATCHHANDLEZERO |*/
					/*D3DDEVCAPS_SEPARATETEXTUREMEMORIES |*/
					D3DDEVCAPS_TEXTURENONLOCALVIDMEM |
					/* D3DDEVCAPS_TEXTURESYSTEMMEMORY |*/
					D3DDEVCAPS_TEXTUREVIDEOMEMORY |
					D3DDEVCAPS_TLVERTEXSYSTEMMEMORY |
					D3DDEVCAPS_TLVERTEXVIDEOMEMORY;

				pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_MASKZ |	// Device can enable and disable modification of the depth buffer on pixel operations.
					D3DPMISCCAPS_CULLNONE |						// The driver does not perform triangle culling. This corresponds to the D3DCULL_NONE member of the D3DCULL enumerated type.
					D3DPMISCCAPS_CULLCW |						// The driver supports clockwise triangle culling through the D3DRS_CULLMODE state. (This applies only to triangle primitives.) This flag corresponds to the D3DCULL_CW member of the D3DCULL enumerated type.
					D3DPMISCCAPS_CULLCCW |						// The driver supports counterclockwise culling through the D3DRS_CULLMODE state. (This applies only to triangle primitives.) This flag corresponds to the D3DCULL_CCW member of the D3DCULL enumerated type.
					D3DPMISCCAPS_COLORWRITEENABLE |				// Device supports per-channel writes for the render-target color buffer through the D3DRS_COLORWRITEENABLE state.
					D3DPMISCCAPS_CLIPPLANESCALEDPOINTS |		// Device correctly clips scaled points of size greater than 1.0 to user-defined clipping planes.
					D3DPMISCCAPS_CLIPTLVERTS |					// Device clips post-transformed vertex primitives. Specify D3DUSAGE_DONOTCLIP when the pipeline should not do any clipping. For this case, additional software clipping may need to be performed at draw time, requiring the vertex buffer to be in system memory.
					D3DPMISCCAPS_TSSARGTEMP |					// Device supports D3DTA for temporary register.
					D3DPMISCCAPS_BLENDOP |						// Device supports alpha-blending operations other than D3DBLENDOP_ADD.
				//	D3DPMISCCAPS_NULLREFERENCE |				// A reference device that does not render.
					D3DPMISCCAPS_INDEPENDENTWRITEMASKS |		// Device supports independent write masks for multiple element textures or multiple render targets.
					D3DPMISCCAPS_PERSTAGECONSTANT |				// Device supports per-stage constants. See D3DTSS_CONSTANT in D3DTEXTURESTAGESTATETYPE.
					D3DPMISCCAPS_FOGANDSPECULARALPHA |			// Device supports separate fog and specular alpha. Many devices use the specular alpha channel to store the fog factor.
					D3DPMISCCAPS_SEPARATEALPHABLEND |			// Device supports separate blend settings for the alpha channel.
					D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS |		// Device supports different bit depths for multiple render targets.
					D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING |	// Device supports post-pixel shader operations for multiple render targets.
					D3DPMISCCAPS_FOGVERTEXCLAMPED;				// Device clamps fog blend factor per vertex.

				pCaps->RasterCaps = D3DPRASTERCAPS_ANISOTROPY |	// Device supports anisotropic filtering.
					D3DPRASTERCAPS_COLORPERSPECTIVE |			// Device iterates colors perspective correctly.
				//	D3DPRASTERCAPS_DITHER |						// Device can dither to improve color resolution.
					D3DPRASTERCAPS_DEPTHBIAS |					// Device supports legacy depth bias. For true depth bias, see D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS.
					D3DPRASTERCAPS_FOGRANGE |					// Device supports range-based fog. In range-based fog, the distance of an object from the viewer is used to compute fog effects, not the depth of the object (that is, the z-coordinate) in the scene.
					D3DPRASTERCAPS_FOGTABLE |					// Device calculates the fog value by referring to a lookup table containing fog values that are indexed to the depth of a given pixel.
					D3DPRASTERCAPS_FOGVERTEX |					// Device calculates the fog value during the lighting operation and interpolates the fog value during rasterization.
					D3DPRASTERCAPS_MIPMAPLODBIAS |				// Device supports level of detail (LOD) bias adjustments. These bias adjustments enable an application to make a mipmap appear crisper or less sharp than it normally would. For more information about LOD bias in mipmaps, see D3DSAMP_MIPMAPLODBIAS.
				//	D3DPRASTERCAPS_MULTISAMPLE_TOGGLE |			// Device supports toggling multisampling on and off between IDirect3DDevice9::BeginScene and IDirect3DDevice9::EndScene (using D3DRS_MULTISAMPLEANTIALIAS).
					D3DPRASTERCAPS_SCISSORTEST |				// Device supports scissor test. See Scissor Test.
					D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS |		// Device performs true slope-scale based depth bias. This is in contrast to the legacy style D3DPRASTERCAPS_DEPTHBIAS.
				//	D3DPRASTERCAPS_WBUFFER |					// Device supports depth buffering using w.
					D3DPRASTERCAPS_WFOG |						// Device supports w-based fog. W-based fog is used when a perspective projection matrix is specified, but affine projections still use z-based fog. The system considers a projection matrix that contains a nonzero value in the [3][4] element to be a perspective projection matrix.
				//	D3DPRASTERCAPS_ZBUFFERLESSHSR |				// Device can perform hidden-surface removal (HSR) without requiring the application to sort polygons and without requiring the allocation of a depth-buffer. This leaves more video memory for textures. The method used to perform HSR is hardware-dependent and is transparent to the application. Z-bufferless HSR is performed if no depth-buffer surface is associated with the rendering-target surface and the depth-buffer comparison test is enabled (that is, when the state value associated with the D3DRS_ZENABLE enumeration constant is set to TRUE).
					D3DPRASTERCAPS_ZFOG |						// Device supports z-based fog.
					D3DPRASTERCAPS_ZTEST;						// Device can perform z-test operations. This effectively renders a primitive and indicates whether any z pixels have been rendered.

				pCaps->ZCmpCaps = D3DPCMPCAPS_NEVER |
					D3DPCMPCAPS_LESS |
					D3DPCMPCAPS_EQUAL |
					D3DPCMPCAPS_LESSEQUAL |
					D3DPCMPCAPS_GREATER |
					D3DPCMPCAPS_NOTEQUAL |
					D3DPCMPCAPS_GREATEREQUAL |
					D3DPCMPCAPS_ALWAYS;

				pCaps->SrcBlendCaps = D3DPBLENDCAPS_BLENDFACTOR |		// The driver supports both D3DBLEND_BLENDFACTOR and D3DBLEND_INVBLENDFACTOR. See D3DBLEND.
					D3DPBLENDCAPS_BOTHINVSRCALPHA |	// Source blend factor is (1-As,1-As,1-As,1-As) and destination blend factor is (As,As,As,As); the destination blend selection is overridden.
					D3DPBLENDCAPS_BOTHSRCALPHA |	// The driver supports the D3DBLEND_BOTHSRCALPHA blend mode. (This blend mode is obsolete. For more information, see D3DBLEND.)
					D3DPBLENDCAPS_DESTALPHA |		// Blend factor is (Ad, Ad, Ad, Ad).
					D3DPBLENDCAPS_DESTCOLOR |		// Blend factor is (Rd, Gd, Bd, Ad).
					D3DPBLENDCAPS_INVDESTALPHA |	// Blend factor is (1-Ad, 1-Ad, 1-Ad, 1-Ad).
					D3DPBLENDCAPS_INVDESTCOLOR |	// Blend factor is (1-Rd, 1-Gd, 1-Bd, 1-Ad).
					D3DPBLENDCAPS_INVSRCALPHA |		// Blend factor is (1-As, 1-As, 1-As, 1-As).
					D3DPBLENDCAPS_INVSRCCOLOR |		// Blend factor is (1-Rs, 1-Gs, 1-Bs, 1-As).
					D3DPBLENDCAPS_ONE |				// Blend factor is (1, 1, 1, 1).
					D3DPBLENDCAPS_SRCALPHA |		// Blend factor is (As, As, As, As).
					D3DPBLENDCAPS_SRCALPHASAT |		// Blend factor is (f, f, f, 1); f = min(As, 1-Ad).
					D3DPBLENDCAPS_SRCCOLOR |		// Blend factor is (Rs, Gs, Bs, As).
					D3DPBLENDCAPS_ZERO;				// Blend factor is (0, 0, 0, 0).

				pCaps->DestBlendCaps = pCaps->SrcBlendCaps;

				pCaps->AlphaCmpCaps = D3DPCMPCAPS_NEVER |
					D3DPCMPCAPS_LESS |
					D3DPCMPCAPS_EQUAL |
					D3DPCMPCAPS_LESSEQUAL |
					D3DPCMPCAPS_GREATER |
					D3DPCMPCAPS_NOTEQUAL |
					D3DPCMPCAPS_GREATEREQUAL |
					D3DPCMPCAPS_ALWAYS;

				pCaps->ShadeCaps = D3DPSHADECAPS_COLORGOURAUDRGB |
					D3DPSHADECAPS_SPECULARGOURAUDRGB |
					D3DPSHADECAPS_ALPHAGOURAUDBLEND |
					D3DPSHADECAPS_FOGGOURAUD;

				pCaps->TextureCaps = D3DPTEXTURECAPS_ALPHA |	// Alpha in texture pixels is supported.
					D3DPTEXTURECAPS_ALPHAPALETTE |				// Device can draw alpha from texture palettes.
					D3DPTEXTURECAPS_CUBEMAP |					// Supports cube textures.
				//	D3DPTEXTURECAPS_CUBEMAP_POW2 |				// Device requires that cube texture maps have dimensions specified as powers of two.
					D3DPTEXTURECAPS_MIPCUBEMAP |				// Device supports mipmapped cube textures.
					D3DPTEXTURECAPS_MIPMAP |					// Device supports mipmapped textures.
					D3DPTEXTURECAPS_MIPVOLUMEMAP |				// Device supports mipmapped volume textures.
				//	D3DPTEXTURECAPS_NONPOW2CONDITIONAL |		// Conditionally supports the use of 2-D textures with dimensions that are not powers of two. A device that exposes this capability can use such a texture if all of the following requirements are met...
				//	D3DPTEXTURECAPS_NOPROJECTEDBUMPENV |		// Device does not support a projected bump-environment loopkup operation in programmable and fixed function shaders.
					D3DPTEXTURECAPS_PERSPECTIVE |				// Perspective correction texturing is supported.
				//	D3DPTEXTURECAPS_POW2 |						// All textures must have widths and heights specified as powers of two. This requirement does not apply to either cube textures or volume textures.
					D3DPTEXTURECAPS_PROJECTED |					// Supports the D3DTTFF_PROJECTED texture transformation flag. When applied, the device divides transformed texture coordinates by the last texture coordinate. If this capability is present, then the projective divide occurs per pixel. If this capability is not present, but the projective divide needs to occur anyway, then it is performed on a per-vertex basis by the Direct3D runtime.
				//	D3DPTEXTURECAPS_SQUAREONLY |				// All textures must be square.
					D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE |	// Texture indices are not scaled by the texture size prior to interpolation.
					D3DPTEXTURECAPS_VOLUMEMAP;					// Device supports volume textures.
				//	D3DPTEXTURECAPS_VOLUMEMAP_POW2;				// Device requires that volume texture maps have dimensions specified as powers of two.

				pCaps->TextureFilterCaps = D3DPTFILTERCAPS_MAGFPOINT |	// Device supports per-stage point-sample filtering for magnifying textures. The point-sample magnification filter is represented by the D3DTEXF_POINT member of the D3DTEXTUREFILTERTYPE enumerated type.
					D3DPTFILTERCAPS_MAGFLINEAR |						// Device supports per-stage bilinear interpolation filtering for magnifying mipmaps. The bilinear interpolation mipmapping filter is represented by the D3DTEXF_LINEAR member of the D3DTEXTUREFILTERTYPE enumerated type.
				//	D3DPTFILTERCAPS_MAGFANISOTROPIC |					// Device supports per-stage anisotropic filtering for magnifying textures. The anisotropic magnification filter is represented by the D3DTEXF_ANISOTROPIC member of the D3DTEXTUREFILTERTYPE enumerated type.
				//	D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD |					// Device supports per-stage pyramidal sample filtering for magnifying textures. The pyramidal magnifying filter is represented by the D3DTEXF_PYRAMIDALQUAD member of the D3DTEXTUREFILTERTYPE enumerated type.
				//	D3DPTFILTERCAPS_MAGFGAUSSIANQUAD |					// Device supports per-stage Gaussian quad filtering for magnifying textures.
					D3DPTFILTERCAPS_MINFPOINT |							// Device supports per-stage point-sample filtering for minifying textures. The point-sample minification filter is represented by the D3DTEXF_POINT member of the D3DTEXTUREFILTERTYPE enumerated type.
					D3DPTFILTERCAPS_MINFLINEAR |						// Device supports per-stage linear filtering for minifying textures. The linear minification filter is represented by the D3DTEXF_LINEAR member of the D3DTEXTUREFILTERTYPE enumerated type.
					D3DPTFILTERCAPS_MINFANISOTROPIC |					// Device supports per-stage anisotropic filtering for minifying textures. The anisotropic minification filter is represented by the D3DTEXF_ANISOTROPIC member of the D3DTEXTUREFILTERTYPE enumerated type.
				//	D3DPTFILTERCAPS_MINFPYRAMIDALQUAD |					// Device supports per-stage pyramidal sample filtering for minifying textures.
				//	D3DPTFILTERCAPS_MINFGAUSSIANQUAD |					// Device supports per-stage Gaussian quad filtering for minifying textures.
					D3DPTFILTERCAPS_MIPFPOINT |							// Device supports per-stage point-sample filtering for mipmaps. The point-sample mipmapping filter is represented by the D3DTEXF_POINT member of the D3DTEXTUREFILTERTYPE enumerated type.
					D3DPTFILTERCAPS_MIPFLINEAR;							// Device supports per-stage bilinear interpolation filtering for mipmaps. The bilinear interpolation mipmapping filter is represented by the D3DTEXF_LINEAR member of the D3DTEXTUREFILTERTYPE enumerated type.

				pCaps->CubeTextureFilterCaps = pCaps->TextureFilterCaps;

				pCaps->VolumeTextureFilterCaps = pCaps->TextureFilterCaps;

				pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_BORDER |	// Device supports setting coordinates outside the range [0.0, 1.0] to the border color, as specified by the D3DSAMP_BORDERCOLOR texture-stage state.
					D3DPTADDRESSCAPS_CLAMP |							// Device can clamp textures to addresses.
					D3DPTADDRESSCAPS_INDEPENDENTUV |					// Device can separate the texture-addressing modes of the u and v coordinates of the texture. This ability corresponds to the D3DSAMP_ADDRESSU and D3DSAMP_ADDRESSV render-state values.
					D3DPTADDRESSCAPS_MIRROR |							// Device can mirror textures to addresses.
					D3DPTADDRESSCAPS_MIRRORONCE |						// Device can take the absolute value of the texture coordinate (thus, mirroring around 0) and then clamp to the maximum value.
					D3DPTADDRESSCAPS_WRAP;								// Device can wrap textures to addresses.

				pCaps->VolumeTextureAddressCaps = pCaps->TextureAddressCaps;

				pCaps->LineCaps =
					D3DLINECAPS_ALPHACMP |
					D3DLINECAPS_BLEND |
					D3DLINECAPS_TEXTURE |
					D3DLINECAPS_ZTEST |
					D3DLINECAPS_FOG |
					D3DLINECAPS_ANTIALIAS;

				pCaps->MaxTextureWidth = properties.limits.maxImageDimension2D;
				pCaps->MaxTextureHeight = pCaps->MaxTextureWidth;
				pCaps->MaxVolumeExtent = properties.limits.maxImageDimension3D;
				pCaps->MaxTextureRepeat = 32768;
				pCaps->MaxTextureAspectRatio = 0; //pCaps->MaxTextureWidth;
				pCaps->MaxAnisotropy = features.samplerAnisotropy;

				//TODO: revisit guard values.
				pCaps->MaxVertexW = 1e10f;
				pCaps->GuardBandLeft = -1e9f;
				pCaps->GuardBandTop = -1e9f;
				pCaps->GuardBandRight = 1e9f;
				pCaps->GuardBandBottom = 1e9f;
				pCaps->ExtentsAdjust = 0.0f;

				pCaps->StencilCaps =
					D3DSTENCILCAPS_KEEP |
					D3DSTENCILCAPS_ZERO |
					D3DSTENCILCAPS_REPLACE |
					D3DSTENCILCAPS_INCRSAT |
					D3DSTENCILCAPS_DECRSAT |
					D3DSTENCILCAPS_INVERT |
					D3DSTENCILCAPS_INCR |
					D3DSTENCILCAPS_DECR |
					D3DSTENCILCAPS_TWOSIDED;

				pCaps->FVFCaps =
					8 | /* 8 textures max */
					/*D3DFVFCAPS_DONOTSTRIPELEMENTS |*/
					D3DFVFCAPS_PSIZE;

				pCaps->TextureOpCaps = D3DTEXOPCAPS_DISABLE |
					D3DTEXOPCAPS_SELECTARG1 |
					D3DTEXOPCAPS_SELECTARG2 |
					D3DTEXOPCAPS_MODULATE |
					D3DTEXOPCAPS_MODULATE2X |
					D3DTEXOPCAPS_MODULATE4X |
					D3DTEXOPCAPS_ADD |
					D3DTEXOPCAPS_ADDSIGNED |
					D3DTEXOPCAPS_ADDSIGNED2X |
					D3DTEXOPCAPS_SUBTRACT |
					D3DTEXOPCAPS_ADDSMOOTH |
					D3DTEXOPCAPS_BLENDDIFFUSEALPHA |
					D3DTEXOPCAPS_BLENDTEXTUREALPHA |
					D3DTEXOPCAPS_BLENDFACTORALPHA |
					D3DTEXOPCAPS_BLENDTEXTUREALPHAPM |
					D3DTEXOPCAPS_BLENDCURRENTALPHA |
					D3DTEXOPCAPS_PREMODULATE |
					D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR |
					D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA |
					D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR |
					D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA |
					D3DTEXOPCAPS_BUMPENVMAP |
					D3DTEXOPCAPS_BUMPENVMAPLUMINANCE |
					D3DTEXOPCAPS_DOTPRODUCT3 |
					D3DTEXOPCAPS_MULTIPLYADD |
					D3DTEXOPCAPS_LERP;

				pCaps->MaxTextureBlendStages = 8;
				pCaps->MaxSimultaneousTextures = 8;

				pCaps->VertexProcessingCaps = D3DVTXPCAPS_TEXGEN |
					D3DVTXPCAPS_TEXGEN_SPHEREMAP |
					D3DVTXPCAPS_MATERIALSOURCE7 |
					D3DVTXPCAPS_DIRECTIONALLIGHTS |
					D3DVTXPCAPS_POSITIONALLIGHTS |
					D3DVTXPCAPS_LOCALVIEWER |
					D3DVTXPCAPS_TWEENING;
				/*D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER*/

				pCaps->MaxActiveLights = 8;
				pCaps->MaxUserClipPlanes = 6; //TODO: I need to look into this one.
				pCaps->MaxVertexBlendMatrices = 3; //Nine & Swift uses 4 but I only have 3 slots with my binding mapping.
				pCaps->MaxVertexBlendMatrixIndex = 8;
				pCaps->MaxPointSize = properties.limits.pointSizeRange[1]; //[0] min [1] max
				pCaps->MaxPrimitiveCount = 0x555555;
				pCaps->MaxVertexIndex = 0xFFFFFF;
				pCaps->MaxStreams = std::max((uint32_t)1,std::min(properties.limits.maxVertexInputBindings, (uint32_t)16));
				pCaps->MaxStreamStride = std::min(properties.limits.maxVertexInputBindingStride, (uint32_t)65536);
				pCaps->VertexShaderVersion = D3DVS_VERSION(3, 0);
				pCaps->PixelShaderVersion = D3DPS_VERSION(3, 0);
				pCaps->MaxVertexShaderConst = 256;
				pCaps->PixelShader1xMaxValue = 65504.f;

				pCaps->DevCaps2 = D3DDEVCAPS2_STREAMOFFSET |
					D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET |
					D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES;
				/*D3DDEVCAPS2_DMAPNPATCH |*/
				/*D3DDEVCAPS2_ADAPTIVETESSRTPATCH |*/
				/*D3DDEVCAPS2_ADAPTIVETESSNPATCH |*/
				/*D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH*/

				pCaps->MasterAdapterOrdinal = 0;
				pCaps->AdapterOrdinalInGroup = 0;
				pCaps->NumberOfAdaptersInGroup = 1; //TODO: need to look into groups.

				//TODO: need to look at these in a few more implementations.
				pCaps->MaxNpatchTessellationLevel = 0.0f;
				pCaps->Reserved5 = 0;

				pCaps->DeclTypes = D3DDTCAPS_UBYTE4 |
					D3DDTCAPS_UBYTE4N |
					D3DDTCAPS_SHORT2N |
					D3DDTCAPS_SHORT4N |
					D3DDTCAPS_USHORT2N |
					D3DDTCAPS_USHORT4N |
					D3DDTCAPS_UDEC3 |
					D3DDTCAPS_DEC3N |
					D3DDTCAPS_FLOAT16_2 |
					D3DDTCAPS_FLOAT16_4;

				//TODO: Look into real target limits. All of my cases use one or two targets.
				pCaps->NumSimultaneousRTs = std::max((uint32_t)1,properties.limits.maxColorAttachments);

				pCaps->StretchRectFilterCaps = D3DPTFILTERCAPS_MINFPOINT |
					D3DPTFILTERCAPS_MINFLINEAR |
					D3DPTFILTERCAPS_MAGFPOINT |
					D3DPTFILTERCAPS_MAGFLINEAR;

				pCaps->VS20Caps.Caps = D3DVS20CAPS_PREDICATION;
				pCaps->VS20Caps.DynamicFlowControlDepth = D3DVS20_MAX_DYNAMICFLOWCONTROLDEPTH;
				pCaps->VS20Caps.NumTemps = D3DVS20_MAX_NUMTEMPS;
				pCaps->VS20Caps.StaticFlowControlDepth = D3DVS20_MAX_STATICFLOWCONTROLDEPTH;

				pCaps->PS20Caps.Caps = D3DPS20CAPS_ARBITRARYSWIZZLE |
					D3DPS20CAPS_GRADIENTINSTRUCTIONS |
					D3DPS20CAPS_PREDICATION |
					D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT |
					D3DPS20CAPS_NODEPENDENTREADLIMIT;
				pCaps->PS20Caps.DynamicFlowControlDepth = D3DPS20_MAX_DYNAMICFLOWCONTROLDEPTH;
				pCaps->PS20Caps.NumTemps = D3DPS20_MAX_NUMTEMPS;
				pCaps->PS20Caps.StaticFlowControlDepth = D3DPS20_MAX_STATICFLOWCONTROLDEPTH;
				pCaps->PS20Caps.NumInstructionSlots = D3DPS20_MAX_NUMINSTRUCTIONSLOTS;

				pCaps->VertexTextureFilterCaps = pCaps->TextureFilterCaps &
					~(D3DPTFILTERCAPS_MIPFPOINT |
						D3DPTFILTERCAPS_MIPFPOINT);

				//I divide by MAX_DXBC_PHASES because for 1.x they can use PHASE to upgrade to 2.0 and both phases have their own limit.
				DWORD estimatedMaxInstructions = (SPIR_V_MAX_ID / ESTIMATED_SPIR_V_PER_DXBC_OP / MAX_DXBC_PHASES);
				pCaps->MaxVertexShader30InstructionSlots = std::max((DWORD)D3DMIN30SHADERINSTRUCTIONS, std::min((DWORD)D3DMAX30SHADERINSTRUCTIONS, estimatedMaxInstructions));
				pCaps->MaxPixelShader30InstructionSlots = std::max((DWORD)D3DMIN30SHADERINSTRUCTIONS, std::min((DWORD)D3DMAX30SHADERINSTRUCTIONS, estimatedMaxInstructions));

				pCaps->MaxVShaderInstructionsExecuted = std::max((DWORD)65535, pCaps->MaxVertexShader30InstructionSlots * 32);
				pCaps->MaxPShaderInstructionsExecuted = std::max((DWORD)65535, pCaps->MaxPixelShader30InstructionSlots * 32);
			}
			break;
			case VertexBuffer_Lock:
			{
				auto& realVertexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mVertexBuffers[workItem->Id]);

				UINT OffsetToLock = bit_cast<UINT>(workItem->Argument1);
				UINT SizeToLock = bit_cast<UINT>(workItem->Argument2);
				VOID** ppbData = bit_cast<VOID**>(workItem->Argument3);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument4);
				size_t lastId = (size_t)workItem->Argument5;

				if (lastId != workItem->Id)
				{
					//Make sure buffer gets bound because it could be new.
					realVertexBuffer.mRealDevice->mDeviceState.mAreStreamSourcesDirty = true;

					if (!(Flags & D3DLOCK_DISCARD))
					{
						auto& oldVertexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mVertexBuffers[lastId]);
						oldVertexBuffer.mRealDevice->CopyBuffer(oldVertexBuffer.mBuffer, realVertexBuffer.mBuffer, realVertexBuffer.mAllocationInfo.size);
					}
				}

				(*ppbData) = realVertexBuffer.Lock(OffsetToLock);
			}
			break;
			case VertexBuffer_Unlock:
			{
				auto& realVertexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mVertexBuffers[workItem->Id]);

				realVertexBuffer.Unlock();
			}
			break;
			case IndexBuffer_Lock:
			{
				auto& realIndexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mIndexBuffers[workItem->Id]);
				UINT OffsetToLock = bit_cast<UINT>(workItem->Argument1);
				UINT SizeToLock = bit_cast<UINT>(workItem->Argument2);
				VOID** ppbData = bit_cast<VOID**>(workItem->Argument3);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument4);
				size_t lastId = (size_t)workItem->Argument5;

				if (lastId != workItem->Id)
				{
					//Make sure buffer gets bound because it could be new.
					realIndexBuffer.mRealDevice->mDeviceState.mAreStreamSourcesDirty = true;

					if (!(Flags & D3DLOCK_DISCARD))
					{
						auto& oldRealIndexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mIndexBuffers[lastId]);
						realIndexBuffer.mRealDevice->CopyBuffer(oldRealIndexBuffer.mBuffer, realIndexBuffer.mBuffer, realIndexBuffer.mAllocationInfo.size);
					}
				}

				(*ppbData) = realIndexBuffer.Lock(OffsetToLock);
			}
			break;
			case IndexBuffer_Unlock:
			{
				auto& realIndexBuffer = (*commandStreamManager->mRenderManager.mStateManager.mIndexBuffers[workItem->Id]);

				realIndexBuffer.Unlock();
			}
			break;
			case StateBlock_Create:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CStateBlock9* stateBlock = bit_cast<CStateBlock9*>(workItem->Argument1);

				MergeState(realDevice->mDeviceState, stateBlock->mDeviceState, stateBlock->mType, false);
			}
			break;
			case StateBlock_Capture:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CStateBlock9* stateBlock = bit_cast<CStateBlock9*>(workItem->Argument1);

				/*
				Capture only captures the current state of state that has already been recorded (eg update not insert)
				https://msdn.microsoft.com/en-us/library/windows/desktop/bb205890(v=vs.85).aspx
				*/
				MergeState(realDevice->mDeviceState, stateBlock->mDeviceState, stateBlock->mType, true);
			}
			break;
			case StateBlock_Apply:
			{
				auto& realDevice = commandStreamManager->mRenderManager.mStateManager.mDevices[workItem->Id];
				CStateBlock9* stateBlock = bit_cast<CStateBlock9*>(workItem->Argument1);

				MergeState(stateBlock->mDeviceState, realDevice->mDeviceState, stateBlock->mType, false);

				if (realDevice->mRenderTargets.size())
				{
					auto& renderTarget = realDevice->mRenderTargets[0];
					if (renderTarget != nullptr)
					{
						auto& constants = realDevice->mDeviceState.mShaderState;
						auto surface = renderTarget->mColorSurface;
						if (surface != nullptr)
						{
							auto& extent = surface->mExtent;
							constants.mRenderState.screenWidth = extent.width;
							constants.mRenderState.screenHeight = extent.height;
						}
					}
				}

				if (stateBlock->mType == D3DSBT_ALL)
				{
					realDevice->mDeviceState.mHasTransformsChanged = true;
				}
			}
			break;
			case Texture_GenerateMipSubLevels:
			{
				auto& texture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[workItem->Id]);
				auto& realDevice = texture.mRealDevice;
				CTexture9* texture9 = bit_cast<CTexture9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				vk::Result result;
				vk::PipelineStageFlags sourceStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::PipelineStageFlags destinationStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::CommandBuffer commandBuffer;
				vk::Filter realFilter = ConvertFilter(texture9->mMipFilter);

				vk::CommandBufferAllocateInfo commandBufferInfo;
				commandBufferInfo.commandPool = realDevice->mCommandPool;
				commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
				commandBufferInfo.commandBufferCount = 1;

				result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result) << std::endl;
					break;
				}

				vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
				commandBufferInheritanceInfo.renderPass = nullptr;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = nullptr;
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags = 0;
				//commandBufferInheritanceInfo.pipelineStatistics = 0;

				vk::CommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				result = commandBuffer.begin(&commandBufferBeginInfo);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result) << std::endl;
					break;
				}

				vk::ImageMemoryBarrier imageMemoryBarrier;
				//imageMemoryBarrier.srcAccessMask = 0;
				//imageMemoryBarrier.dstAccessMask = 0;

				vk::ImageSubresourceRange mipSubRange;
				mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
				mipSubRange.levelCount = 1;
				mipSubRange.layerCount = 1;

				/*
				I'm debating whether or not to have the population of the image here. If I don't I'll end up creating another command for that. On the other hand this method should purely populate the other levels as per the spec.
				*/

				// Transition zero mip level to transfer source
				//mipSubRange.baseMipLevel = 0;

				//imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				//imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				//imageMemoryBarrier.image = mImage;
				//imageMemoryBarrier.subresourceRange = mipSubRange;
				//vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

				for (UINT i = 1; i < texture9->mLevels; i++) //Changed to match mLevels datatype
				{
					vk::ImageBlit imageBlit;

					// Source
					imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.srcSubresource.layerCount = 1;
					imageBlit.srcSubresource.mipLevel = 0;
					imageBlit.srcOffsets[1].x = int32_t(texture9->mWidth);
					imageBlit.srcOffsets[1].y = int32_t(texture9->mHeight);
					imageBlit.srcOffsets[1].z = 1;

					// Destination
					imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.dstSubresource.layerCount = 1;
					imageBlit.dstSubresource.mipLevel = i;
					imageBlit.dstOffsets[1].x = int32_t(texture9->mWidth >> i);
					imageBlit.dstOffsets[1].y = int32_t(texture9->mHeight >> i);
					imageBlit.dstOffsets[1].z = 1;

					mipSubRange.baseMipLevel = i;

					// Transition current mip level to transfer dest
					imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
					imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
					imageMemoryBarrier.image = texture.mImage;
					imageMemoryBarrier.subresourceRange = mipSubRange;
					commandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

					// Blit from zero level
					commandBuffer.blitImage(texture.mImage, vk::ImageLayout::eTransferSrcOptimal, texture.mImage, vk::ImageLayout::eTransferDstOptimal, 1, &imageBlit, vk::Filter::eLinear);
				}

				commandBuffer.end();

				vk::CommandBuffer commandBuffers[] = { commandBuffer };
				vk::Fence nullFence;
				vk::SubmitInfo submitInfo;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = commandBuffers;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;

				result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkQueueSubmit failed with return code of " << GetResultString((VkResult)result) << std::endl;
					break;
				}

				realDevice->mQueue.waitIdle();

				device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);
				commandBuffer = nullptr;
			}
			break;
			case CubeTexture_GenerateMipSubLevels:
			{
				auto& texture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[workItem->Id]);
				auto& realDevice = texture.mRealDevice;
				CCubeTexture9* texture9 = bit_cast<CCubeTexture9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				vk::Result result;
				vk::PipelineStageFlags sourceStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::PipelineStageFlags destinationStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::CommandBuffer commandBuffer;
				vk::Filter realFilter = ConvertFilter(texture9->mMipFilter);

				vk::CommandBufferAllocateInfo commandBufferInfo;
				commandBufferInfo.commandPool = realDevice->mCommandPool;
				commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
				commandBufferInfo.commandBufferCount = 1;

				result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result);
					break;
				}

				vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
				commandBufferInheritanceInfo.renderPass = nullptr;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = nullptr;
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags = 0;
				//commandBufferInheritanceInfo.pipelineStatistics = 0;

				vk::CommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				result = commandBuffer.begin(&commandBufferBeginInfo);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result) << std::endl;
					break;
				}

				vk::ImageMemoryBarrier imageMemoryBarrier;
				//imageMemoryBarrier.srcAccessMask = 0;
				//imageMemoryBarrier.dstAccessMask = 0;

				vk::ImageSubresourceRange mipSubRange;
				mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
				mipSubRange.levelCount = 1;
				mipSubRange.layerCount = 1;

				/*
				I'm debating whether or not to have the population of the image here. If I don't I'll end up creating another command for that. On the other hand this method should purely populate the other levels as per the spec.
				*/

				// Transition zero mip level to transfer source
				//mipSubRange.baseMipLevel = 0;

				//imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				//imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				//imageMemoryBarrier.image = mImage;
				//imageMemoryBarrier.subresourceRange = mipSubRange;
				//vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

				for (UINT i = 1; i < texture9->mLevels; i++) //Changed to match mLevels datatype
				{
					vk::ImageBlit imageBlit;

					// Source
					imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.srcSubresource.layerCount = 6;
					imageBlit.srcSubresource.mipLevel = 0;
					imageBlit.srcOffsets[1].x = int32_t(texture9->mEdgeLength);
					imageBlit.srcOffsets[1].y = int32_t(texture9->mEdgeLength);
					imageBlit.srcOffsets[1].z = 1;

					// Destination
					imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.dstSubresource.layerCount = 6;
					imageBlit.dstSubresource.mipLevel = i;
					imageBlit.dstOffsets[1].x = int32_t(texture9->mEdgeLength >> i);
					imageBlit.dstOffsets[1].y = int32_t(texture9->mEdgeLength >> i);
					imageBlit.dstOffsets[1].z = 1;

					mipSubRange.baseMipLevel = i;

					// Transition current mip level to transfer dest
					imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
					imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
					imageMemoryBarrier.image = texture.mImage;
					imageMemoryBarrier.subresourceRange = mipSubRange;
					commandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

					// Blit from zero level
					commandBuffer.blitImage(texture.mImage, vk::ImageLayout::eTransferSrcOptimal, texture.mImage, vk::ImageLayout::eTransferDstOptimal, 1, &imageBlit, vk::Filter::eLinear);
				}

				commandBuffer.end();

				vk::CommandBuffer commandBuffers[] = { commandBuffer };
				vk::Fence nullFence;
				vk::SubmitInfo submitInfo;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = commandBuffers;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;

				result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkQueueSubmit failed with return code of " << GetResultString((VkResult)result) << std::endl;
					break;
				}

				realDevice->mQueue.waitIdle();

				device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);
				commandBuffer = nullptr;
			}
			break;
			case VolumeTexture_GenerateMipSubLevels:
			{
				auto& texture = (*commandStreamManager->mRenderManager.mStateManager.mTextures[workItem->Id]);
				auto& realDevice = texture.mRealDevice;
				CVolumeTexture9* texture9 = bit_cast<CVolumeTexture9*>(workItem->Argument1);
				auto& device = realDevice->mDevice;

				vk::Result result;
				vk::PipelineStageFlags sourceStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::PipelineStageFlags destinationStages = vk::PipelineStageFlagBits::eTopOfPipe;
				vk::CommandBuffer commandBuffer;
				vk::Filter realFilter = ConvertFilter(texture9->mMipFilter);

				vk::CommandBufferAllocateInfo commandBufferInfo;
				commandBufferInfo.commandPool = realDevice->mCommandPool;
				commandBufferInfo.level = vk::CommandBufferLevel::ePrimary;
				commandBufferInfo.commandBufferCount = 1;

				result = device.allocateCommandBuffers(&commandBufferInfo, &commandBuffer);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkAllocateCommandBuffers failed with return code of " << GetResultString((VkResult)result) << std::endl;
					break;
				}

				vk::CommandBufferInheritanceInfo commandBufferInheritanceInfo;
				commandBufferInheritanceInfo.renderPass = nullptr;
				commandBufferInheritanceInfo.subpass = 0;
				commandBufferInheritanceInfo.framebuffer = nullptr;
				commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
				//commandBufferInheritanceInfo.queryFlags = 0;
				//commandBufferInheritanceInfo.pipelineStatistics = 0;

				vk::CommandBufferBeginInfo commandBufferBeginInfo;
				commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
				commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

				result = commandBuffer.begin(&commandBufferBeginInfo);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkBeginCommandBuffer failed with return code of " << GetResultString((VkResult)result) << std::endl;
					break;
				}

				vk::ImageMemoryBarrier imageMemoryBarrier;
				//imageMemoryBarrier.srcAccessMask = 0;
				//imageMemoryBarrier.dstAccessMask = 0;

				vk::ImageSubresourceRange mipSubRange;
				mipSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
				mipSubRange.levelCount = 1;
				mipSubRange.layerCount = 1;

				/*
				I'm debating whether or not to have the population of the image here. If I don't I'll end up creating another command for that. On the other hand this method should purely populate the other levels as per the spec.
				*/

				// Transition zero mip level to transfer source
				//mipSubRange.baseMipLevel = 0;

				//imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				//imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				//imageMemoryBarrier.image = mImage;
				//imageMemoryBarrier.subresourceRange = mipSubRange;
				//vkCmdPipelineBarrier(commandBuffer, sourceStages, destinationStages, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

				for (UINT i = 1; i < texture9->mLevels; i++) //Changed to match mLevels datatype
				{
					vk::ImageBlit imageBlit;

					// Source
					imageBlit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.srcSubresource.layerCount = 1;
					imageBlit.srcSubresource.mipLevel = 0;
					imageBlit.srcOffsets[1].x = int32_t(texture9->mWidth);
					imageBlit.srcOffsets[1].y = int32_t(texture9->mHeight);
					imageBlit.srcOffsets[1].z = 1;

					// Destination
					imageBlit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
					imageBlit.dstSubresource.layerCount = 1;
					imageBlit.dstSubresource.mipLevel = i;
					imageBlit.dstOffsets[1].x = int32_t(texture9->mWidth >> i);
					imageBlit.dstOffsets[1].y = int32_t(texture9->mHeight >> i);
					imageBlit.dstOffsets[1].z = 1;

					mipSubRange.baseMipLevel = i;

					// Transition current mip level to transfer dest
					imageMemoryBarrier.oldLayout = vk::ImageLayout::eUndefined;
					imageMemoryBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
					imageMemoryBarrier.image = texture.mImage;
					imageMemoryBarrier.subresourceRange = mipSubRange;
					commandBuffer.pipelineBarrier(sourceStages, destinationStages, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

					// Blit from zero level
					commandBuffer.blitImage(texture.mImage, vk::ImageLayout::eTransferSrcOptimal, texture.mImage, vk::ImageLayout::eTransferDstOptimal, 1, &imageBlit, vk::Filter::eLinear);
				}

				commandBuffer.end();

				vk::CommandBuffer commandBuffers[] = { commandBuffer };
				vk::Fence nullFence;
				vk::SubmitInfo submitInfo;
				submitInfo.waitSemaphoreCount = 0;
				submitInfo.pWaitSemaphores = nullptr;
				submitInfo.pWaitDstStageMask = nullptr;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = commandBuffers;
				submitInfo.signalSemaphoreCount = 0;
				submitInfo.pSignalSemaphores = nullptr;

				result = realDevice->mQueue.submit(1, &submitInfo, nullFence);
				if (result != vk::Result::eSuccess)
				{
					Log(fatal) << "ProcessQueue vkQueueSubmit failed with return code of " << GetResultString((VkResult)result) << std::endl;
					break;
				}

				realDevice->mQueue.waitIdle();

				device.freeCommandBuffers(realDevice->mCommandPool, 1, commandBuffers);
				commandBuffer = nullptr;
			}
			break;
			case Surface_LockRect:
			{
				D3DLOCKED_RECT* pLockedRect = bit_cast<D3DLOCKED_RECT*>(workItem->Argument1);
				RECT* pRect = bit_cast<RECT*>(workItem->Argument2);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument3);

				commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]->Lock(pLockedRect, pRect, Flags);
			}
			break;
			case Surface_UnlockRect:
			{
				commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]->Unlock();
			}
			break;
			case Surface_Flush:
			{
				commandStreamManager->mRenderManager.mStateManager.mSurfaces[workItem->Id]->Flush();
			}
			break;
			case Volume_LockRect:
			{
				D3DLOCKED_BOX* pLockedVolume = bit_cast<D3DLOCKED_BOX*>(workItem->Argument1);
				D3DBOX* pBox = bit_cast<D3DBOX*>(workItem->Argument2);
				DWORD Flags = bit_cast<DWORD>(workItem->Argument3);

				commandStreamManager->mRenderManager.mStateManager.mVolumes[workItem->Id]->LockBox(pLockedVolume, pBox, Flags);
			}
			break;
			case Volume_UnlockRect:
			{
				commandStreamManager->mRenderManager.mStateManager.mVolumes[workItem->Id]->Unlock();
			}
			break;
			case Volume_Flush:
			{
				commandStreamManager->mRenderManager.mStateManager.mVolumes[workItem->Id]->Flush();
			}
			break;
			case Query_Issue:
			{
				auto& realQuery = (*commandStreamManager->mRenderManager.mStateManager.mQueries[workItem->Id]);
				auto& realDevice = realQuery.mRealDevice;

				DWORD dwIssueFlags = bit_cast<DWORD>(workItem->Argument1);

				switch (dwIssueFlags)
				{
				case D3DISSUE_BEGIN:
					realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer].beginQuery(realQuery.mQueryPool, 0, vk::QueryControlFlags());
					break;
				case D3DISSUE_END:
					realDevice->mCommandBuffers[realDevice->mCurrentCommandBuffer].endQuery(realQuery.mQueryPool, 0);
					break;
				default:
					Log(error) << "ProcessQueue unknown query issue type " << dwIssueFlags << std::endl;
					break;
				}

				//(*pViewport) = realDevice.mDeviceState.m9Viewport;
			}
			break;
			case Query_GetData:
			{
				auto& realQuery = (*commandStreamManager->mRenderManager.mStateManager.mQueries[workItem->Id]);
				auto& realDevice = realQuery.mRealDevice;

				void* pData = workItem->Argument1;
				DWORD dwSize = bit_cast<DWORD>(workItem->Argument2);
				DWORD dwGetDataFlags = bit_cast<DWORD>(workItem->Argument3);

				realDevice->mDevice.getQueryPoolResults(realQuery.mQueryPool, (uint32_t)0, (uint32_t)1, (size_t)dwSize, pData, (vk::DeviceSize)4, vk::QueryResultFlags());
			}
			break;
			case Query_GetDataSize:
			{
				Log(warning) << "ProcessQueue assuming 4 byte query result size." << std::endl;
			}
			break;
			default:
			{
				Log(error) << "ProcessQueue unknown work item " << workItem->WorkItemType << std::endl;
			}
			break;
			}
			//}
			//catch (const std::exception& ex)
			//{
			//	Log(warning) << "ProcessQueue - " << workItem->WorkItemType << " " << ex.what()<< std::endl;
			//}
			//catch (...)
			//{
			//	Log(warning) << "ProcessQueue - " << workItem->WorkItemType << std::endl;
			//}

			if (workItem->WillWait)
			{
				SetEvent(workItem->WaitHandle);
			}

			if (workItem->Caller != nullptr)
			{
				workItem->Caller->Release();
				workItem->Caller = nullptr;
			}

			if (!commandStreamManager->mUnusedWorkItems.Push(workItem, count))
			{
				delete workItem;
			}
		}
	}
}