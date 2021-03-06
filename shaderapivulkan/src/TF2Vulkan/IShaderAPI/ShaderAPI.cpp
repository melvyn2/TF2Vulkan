#include "TF2Vulkan/FormatInfo.h"
#include "IShaderAPI_StateManagerDynamic.h"
#include "TF2Vulkan/SamplerSettings.h"
#include "interface/internal/IShaderDeviceInternal.h"
#include "interface/internal/IStateManagerStatic.h"
#include "TF2Vulkan/IStateManagerVulkan.h"
#include "TF2Vulkan/TextureData.h"
#include "TF2Vulkan/VulkanFactories.h"
#include "TF2Vulkan/meshes/VulkanMesh.h"
#include "TF2Vulkan/VulkanUtil.h"

#include <TF2Vulkan/Util/Color.h>
#include <TF2Vulkan/Util/DirtyVar.h>
#include <TF2Vulkan/Util/ImageManip.h>
#include <TF2Vulkan/Util/InPlaceVector.h>
#include <TF2Vulkan/Util/interface.h>
#include <TF2Vulkan/Util/std_algorithm.h>
#include <TF2Vulkan/Util/std_string.h>
#include <TF2Vulkan/Util/std_utility.h>

#include <Color.h>
#include <materialsystem/imesh.h>
#include <shaderapi/commandbuffer.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <unordered_map>

using namespace TF2Vulkan;

#undef min
#undef max

namespace
{
	class ShaderAPI final : public IShaderAPI_StateManagerDynamic
	{
	public:
		int GetCurrentLightCombo() const override { NOT_IMPLEMENTED_FUNC(); }

		void SetTextureTransformDimension(TextureStage_t stage, int dimension, bool projected) override { NOT_IMPLEMENTED_FUNC(); }
		void DisableTextureTransform(TextureStage_t stage) override { NOT_IMPLEMENTED_FUNC(); }
		void SetBumpEnvMatrix(TextureStage_t stage, float m00, float m01, float m10, float m11) override { NOT_IMPLEMENTED_FUNC(); }

		int GetMaxLights() const override { NOT_IMPLEMENTED_FUNC(); }
		const LightDesc_t& GetLight(int lightNum) const override { NOT_IMPLEMENTED_FUNC(); }

		void SetVertexShaderStateAmbientLightCube() override;

		bool InEditorMode() const override;

		MorphFormat_t GetBoundMorphFormat() override { NOT_IMPLEMENTED_FUNC(); }

		void ClearBuffersObeyStencil(bool clearColor, bool clearDepth) override { NOT_IMPLEMENTED_FUNC(); }
		void ClearBuffersObeyStencilEx(bool clearColor, bool clearAlpha, bool clearDepth) override { NOT_IMPLEMENTED_FUNC(); }
		void ClearBuffers(bool clearColor, bool clearDepth, bool clearStencil, int rtWidth, int rtHeight) override;

		void BindVertexShader(VertexShaderHandle_t vtxShader) override { NOT_IMPLEMENTED_FUNC(); }
		void BindGeometryShader(GeometryShaderHandle_t geoShader) override { NOT_IMPLEMENTED_FUNC(); }
		void BindPixelShader(PixelShaderHandle_t pixShader) override { NOT_IMPLEMENTED_FUNC(); }

		void SetRasterState(const ShaderRasterState_t& state) override { NOT_IMPLEMENTED_FUNC(); }

		bool SetMode(void* hwnd, int adapter, const ShaderDeviceInfo_t& info) override;

		void ChangeVideoMode(const ShaderDeviceInfo_t& info) override;

		StateSnapshot_t TakeSnapshot() override;

		void CopyRenderTargetToTexture(ShaderAPITextureHandle_t texHandle) override;
		void CopyRenderTargetToTextureEx(ShaderAPITextureHandle_t texHandle, int renderTargetID,
			const Rect_t* srcRect, const Rect_t* dstRect) override;
		void CopyTextureToRenderTargetEx(int renderTargetID, ShaderAPITextureHandle_t texHandle,
			const Rect_t* srcRect, const Rect_t* dstRect) override;
		void CopyRenderTargetToScratchTexture(ShaderAPITextureHandle_t srcRT,
			ShaderAPITextureHandle_t dstTex, const Rect_t* srcRect, const Rect_t* dstRect) override;

		void FlushBufferedPrimitives() override;

		bool IsTranslucent(StateSnapshot_t id) const override;
		bool IsAlphaTested(StateSnapshot_t id) const override;
		bool UsesVertexAndPixelShaders(StateSnapshot_t id) const override;
		bool IsDepthWriteEnabled(StateSnapshot_t id) const override;

		VertexFormat_t ComputeVertexFormat(int snapshotCount, StateSnapshot_t* ids) const override;
		VertexFormat_t ComputeVertexUsage(int snapshotCount, StateSnapshot_t* ids) const override;

		void BeginPass(StateSnapshot_t snapshot) override;
		void RenderPass(int passID, int passCount) override;
		bool IsInPass() const override { NOT_IMPLEMENTED_FUNC(); }
		void BeginFrame() override;
		bool IsInFrame() const override;
		void EndFrame() override;

		void FlushHardware() override { NOT_IMPLEMENTED_FUNC(); }
		void ForceHardwareSync() override;

		void ShadeMode(ShaderShadeMode_t mode) override { NOT_IMPLEMENTED_FUNC(); }

		void SetHeightClipZ(float z) override { NOT_IMPLEMENTED_FUNC(); }
		void SetHeightClipMode(MaterialHeightClipMode_t mode) override { NOT_IMPLEMENTED_FUNC(); }

		ImageFormat GetNearestSupportedFormat(ImageFormat fmt, bool filteringRequired) const override;
		ImageFormat GetNearestRenderTargetFormat(ImageFormat fmt) const override;

		bool DoRenderTargetsNeedSeparateDepthBuffer() const override;

		void SetRenderTarget(ShaderAPITextureHandle_t colTexHandle,
			ShaderAPITextureHandle_t depthTexHandle) override;
		void SetRenderTargetEx(int renderTargetID, ShaderAPITextureHandle_t colTex,
			ShaderAPITextureHandle_t depthTex) override;

		void ReadPixels(int x, int y, int width, int height, unsigned char* data,
			ImageFormat dstFormat) override { NOT_IMPLEMENTED_FUNC(); }
		void ReadPixels(Rect_t* srcRect, Rect_t* dstRect, unsigned char* data,
			ImageFormat dstFormat, int dstStride) override { NOT_IMPLEMENTED_FUNC(); }

		int SelectionMode(bool selectionMode) override { NOT_IMPLEMENTED_FUNC(); }
		void SelectionBuffer(unsigned int* buf, int size) override { NOT_IMPLEMENTED_FUNC(); }
		void ClearSelectionNames() override { NOT_IMPLEMENTED_FUNC(); }
		void LoadSelectionName(int name) override { NOT_IMPLEMENTED_FUNC(); }
		void PushSelectionName(int name) override { NOT_IMPLEMENTED_FUNC(); }
		void PopSelectionName() override { NOT_IMPLEMENTED_FUNC(); }

		void ClearSnapshots() override;

		bool CanDownloadTextures() const override;

		void ResetRenderState(bool fullReset) override;

		void EvictManagedResources() override;

		void SyncToken(const char* token) override { NOT_IMPLEMENTED_FUNC(); }

		void SetStandardVertexShaderConstants(float overbright) override;

		ShaderAPIOcclusionQuery_t CreateOcclusionQueryObject() override;
		void DestroyOcclusionQueryObject(ShaderAPIOcclusionQuery_t query) override;
		void BeginOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t query) override;
		void EndOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t query) override;
		int OcclusionQuery_GetNumPixelsRendered(ShaderAPIOcclusionQuery_t query, bool flush) override;

		const FlashlightState_t& GetFlashlightState(VMatrix& worldToTexture) const override { NOT_IMPLEMENTED_FUNC(); }
		const FlashlightState_t& GetFlashlightStateEx(VMatrix& worldToTexture,
			ITexture** flashlightDepthTex) const override { NOT_IMPLEMENTED_FUNC(); }
		void SetFlashlightState(const FlashlightState_t& state, const VMatrix& worldToTexture) override { NOT_IMPLEMENTED_FUNC(); }
		void SetFlashlightStateEx(const FlashlightState_t& state, const VMatrix& worldToTexture,
			ITexture* flashlightDepthTex) override { NOT_IMPLEMENTED_FUNC(); }

		void ClearVertexAndPixelShaderRefCounts() override { NOT_IMPLEMENTED_FUNC_NOBREAK(); }
		void PurgeUnusedVertexAndPixelShaders() override { NOT_IMPLEMENTED_FUNC_NOBREAK(); }

		void DXSupportLevelChanged() override;

		MorphFormat_t ComputeMorphFormat(int snapshots, StateSnapshot_t* ids) const override;

		void HandleDeviceLost() override { NOT_IMPLEMENTED_FUNC(); }

		void SetFastClipPlane(const float* plane) override { NOT_IMPLEMENTED_FUNC(); }
		void EnableFastClip(bool enable) override { NOT_IMPLEMENTED_FUNC(); }

		void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value) override { NOT_IMPLEMENTED_FUNC(); }

		int CompareSnapshots(StateSnapshot_t lhs, StateSnapshot_t rhs) override;

		bool SupportsMSAAMode(int msaaMode) override { NOT_IMPLEMENTED_FUNC(); }

		bool OwnGPUResources(bool enable) override { NOT_IMPLEMENTED_FUNC(); }

		void BeginPIXEvent(unsigned long color, const char* szName) override;
		void EndPIXEvent() override;
		void SetPIXMarker(const Color& color, const char* szName) override;

		void EnableAlphaToCoverage() override { NOT_IMPLEMENTED_FUNC(); }
		void DisableAlphaToCoverage() override { NOT_IMPLEMENTED_FUNC(); }

		bool SupportsShadowDepthTextures() override { NOT_IMPLEMENTED_FUNC(); }

		void SetDisallowAccess(bool disallowed) override { NOT_IMPLEMENTED_FUNC_NOBREAK(); }
		void EnableShaderShaderMutex(bool enabled) override;
		void ShaderLock() override;
		void ShaderUnlock() override;

		ImageFormat GetShadowDepthTextureFormat() override { NOT_IMPLEMENTED_FUNC(); }

		bool SupportsFetch4() override { NOT_IMPLEMENTED_FUNC(); }
		void SetShadowDepthBiasFactors(float slopeScaleDepthBias, float shadowDepthBias) override { NOT_IMPLEMENTED_FUNC(); }

		void BindVertexBuffer(int streamID, IVertexBuffer* vtxBuf, int byteOffset, int firstVertex, int vtxCount,
			VertexFormat_t fmt, int repetitions) override { NOT_IMPLEMENTED_FUNC(); }
		void BindIndexBuffer(IIndexBuffer* indexBuf, int offsetInBytes) override { NOT_IMPLEMENTED_FUNC(); }
		void Draw(MaterialPrimitiveType_t type, int firstIndex, int indexCount) override { NOT_IMPLEMENTED_FUNC(); }

		void PerformFullScreenStencilOperation() override { NOT_IMPLEMENTED_FUNC(); }

		bool SupportsCSAAMode(int numSamples, int qualityLevel) override { NOT_IMPLEMENTED_FUNC(); }

		void InvalidateDelayedShaderConstants() override;

		float GammaToLinear_HardwareSpecific(float gamma) const override;
		float LinearToGamma_HardwareSpecific(float linear) const override;

		void SetLinearToGammaConversionTextures(ShaderAPITextureHandle_t srgbWriteEnabledTex,
			ShaderAPITextureHandle_t identityTex) override;

		ImageFormat GetNullTextureFormat() override { NOT_IMPLEMENTED_FUNC(); }

		void BindVertexTexture(VertexTextureSampler_t sampler, ShaderAPITextureHandle_t tex) override { NOT_IMPLEMENTED_FUNC(); }

		void EnableHWMorphing(bool enabled) override { NOT_IMPLEMENTED_FUNC(); }

		void SetFlexWeights(int firstWeight, int count, const MorphWeight_t* weights) override { NOT_IMPLEMENTED_FUNC(); }

		void AcquireThreadOwnership() override { NOT_IMPLEMENTED_FUNC_NOBREAK(); }
		void ReleaseThreadOwnership() override { NOT_IMPLEMENTED_FUNC_NOBREAK(); }

		bool SupportsNormalMapCompression() const override { NOT_IMPLEMENTED_FUNC(); }

		void EnableBuffer2FramesAhead(bool enable) override { LOG_FUNC(); } // Does nothing

		void PrintfVA(char* fmt, va_list vargs) override { NOT_IMPLEMENTED_FUNC(); }
		void Printf(PRINTF_FORMAT_STRING const char* fmt, ...) override { NOT_IMPLEMENTED_FUNC(); }
		float Knob(char* knobName, float* setValue) override { NOT_IMPLEMENTED_FUNC(); }

		void OverrideAlphaWriteEnable(bool enable, bool alphaWriteEnable) override { NOT_IMPLEMENTED_FUNC(); }
		void OverrideColorWriteEnable(bool overrideEnable, bool colorWriteEnable) override { NOT_IMPLEMENTED_FUNC(); }

		ITexture* GetRenderTargetEx(int renderTargetID) override;

		void GetDXLevelDefaults(uint& dxLevelMax, uint& dxLevelRecommended) override { NOT_IMPLEMENTED_FUNC(); }

		float GetAmbientLightCubeLuminance() override { NOT_IMPLEMENTED_FUNC(); }

		bool IsHWMorphingEnabled() const override;

		bool ShouldWriteDepthToDestAlpha() const override;

		void PushDeformation(const DeformationBase_t* deformation) override { NOT_IMPLEMENTED_FUNC(); }
		void PopDeformation() override { NOT_IMPLEMENTED_FUNC(); }
		int GetNumActiveDeformations() const override { NOT_IMPLEMENTED_FUNC(); }

		int GetPackedDeformationInformation(int maxOfUnderstoodDeformations, float* constantValuesOut,
			int bufSize, int maxDeformations, int* numDefsOut) const override { NOT_IMPLEMENTED_FUNC(); }

		void MarkUnusedVertexFields(unsigned int flags, int texCoordCount, bool* unusedTexCoords) override;

		void ExecuteCommandBuffer(uint8* cmdBuf) override;

		void GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* info) override { NOT_IMPLEMENTED_FUNC(); }

		void SetPSNearAndFarZ(int psRegister) override { NOT_IMPLEMENTED_FUNC(); }

		void CopyTextureToTexture(int something1, int something2) override { NOT_IMPLEMENTED_FUNC(); }

	private:
		mutable std::recursive_mutex m_ShaderLock;

		bool m_IsInFrame = false;
		ShaderAPITextureHandle_t m_L2GConvTex_SRGBWriteEnabled = INVALID_SHADERAPI_TEXTURE_HANDLE;
		ShaderAPITextureHandle_t m_L2GConvTex_Identity = INVALID_SHADERAPI_TEXTURE_HANDLE;
	};
}

static ShaderAPI s_ShaderAPI;
IShaderAPI* ::g_ShaderAPI = &s_ShaderAPI;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(ShaderAPI, IShaderAPI, SHADERAPI_INTERFACE_VERSION, s_ShaderAPI);
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(ShaderAPI, IShaderDynamicNext, SHADERDYNAMICNEXT_INTERFACE_VERSION, s_ShaderAPI);
//EXPOSE_SINGLE_INTERFACE_GLOBALVAR(ShaderAPI, IShaderDynamicAPI, SHADERDYNAMIC_INTERFACE_VERSION, s_ShaderAPI);

IShaderAPI_StateManagerDynamic& TF2Vulkan::g_StateManagerDynamic = s_ShaderAPI;
IShaderAPIInternal& TF2Vulkan::g_ShaderAPIInternal = s_ShaderAPI;
IShaderAPI_TextureManager& TF2Vulkan::g_TextureManager = s_ShaderAPI;
IShaderAPI_MeshManager& TF2Vulkan::g_MeshManager = s_ShaderAPI;

void ShaderAPI::ClearBuffers(bool clearColor, bool clearDepth, bool clearStencil, int rtWidth, int rtHeight)
{
	LOG_FUNC();
	return; // TODO: Fix renderdoc crash when this is enabled
	if (!g_StateManagerStatic.IsAnyRenderTargetBound())
		return;

	auto& cmdBuf = g_ShaderDevice.GetPrimaryCmdBuf();

	auto pixScope = cmdBuf.DebugRegionBegin(PIX_COLOR_CLEAR, "ShaderAPI::ClearBuffers(%s, %s, %s, %i, %i)",
		PRINTF_BOOL(clearColor), PRINTF_BOOL(clearDepth), PRINTF_BOOL(clearStencil),
		rtWidth, rtHeight);

	const auto curSnapshot = g_StateManagerStatic.TakeSnapshot();
	NOT_IMPLEMENTED_FUNC();//g_StateManagerStatic.ApplyState(curSnapshot, cmdBuf);
	const auto& curState = g_StateManagerStatic.GetState(curSnapshot);

	Util::InPlaceVector<vk::ClearAttachment, 2> atts;
	vk::ClearRect rects[2];
	{
		Util::SafeConvert(rtWidth, rects[0].rect.extent.width);
		Util::SafeConvert(rtHeight, rects[0].rect.extent.height);
		rects[0].layerCount = 1;
		rects[1] = rects[0];
	}

	if (clearColor && curState.m_OMColorRTs[0] >= 0)
	{
		auto& att = atts.emplace_back();
		att.aspectMask |= vk::ImageAspectFlagBits::eColor;
		Util::algorithm::copy(GetDynamicState().m_ClearColor, att.clearValue.color.float32);
		att.colorAttachment = 0;
	}

	if ((clearDepth || clearStencil) && curState.m_OMDepthRT >= 0)
	{
		auto& att = atts.emplace_back();

		if (clearColor)
			att = {};

		if (clearDepth)
			att.aspectMask |= vk::ImageAspectFlagBits::eDepth;

		if (clearStencil)
			att.aspectMask |= vk::ImageAspectFlagBits::eStencil;

		att.clearValue.depthStencil.depth = 1;
		att.clearValue.depthStencil.stencil = 0;
	}

	cmdBuf.clearAttachments(atts.size(), atts.data(), atts.size(), rects);
}

bool ShaderAPI::SetMode(void* hwnd, int adapter, const ShaderDeviceInfo_t& info)
{
	LOG_FUNC();
	return g_ShaderDevice.SetMode(hwnd, adapter, info);
}

void ShaderAPI::ChangeVideoMode(const ShaderDeviceInfo_t& info)
{
	// TODO
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}

StateSnapshot_t ShaderAPI::TakeSnapshot()
{
	LOG_FUNC();

	auto snapshot = g_StateManagerStatic.TakeSnapshot();
	return Util::SafeConvert<StateSnapshot_t>(snapshot);
}

void ShaderAPI::FlushBufferedPrimitives()
{
	//if (g_ShaderDevice.IsPrimaryCmdBufReady())
	//	TF2VULKAN_PIX_MARKER(__FUNCTION__);
}

void ShaderAPI::SetPIXMarker(const Color& color, const char* name)
{
	LOG_FUNC();
	g_ShaderDevice.GetPrimaryCmdBuf().InsertDebugLabel(color, name);
}

void ShaderAPI::BeginPIXEvent(unsigned long color, const char* name)
{
	LOG_FUNC();
	Color c;
	c.m_RawColor = color;

	auto& cmdBuf = g_ShaderDevice.GetPrimaryCmdBuf();
	const auto marker = cmdBuf.InitDebugUtilsLabel(name, c);
	g_ShaderDevice.GetPrimaryCmdBuf().beginDebugUtilsLabelEXT(marker);
}

void ShaderAPI::EndPIXEvent()
{
	LOG_FUNC();
	g_ShaderDevice.GetPrimaryCmdBuf().endDebugUtilsLabelEXT();
}

bool ShaderAPI::IsTranslucent(StateSnapshot_t id) const
{
	LOG_FUNC();
	return g_StateManagerStatic.IsTranslucent(id);
}

bool ShaderAPI::IsAlphaTested(StateSnapshot_t id) const
{
	LOG_FUNC();
	return g_StateManagerStatic.IsAlphaTested(id);
}

bool ShaderAPI::UsesVertexAndPixelShaders(StateSnapshot_t id) const
{
	LOG_FUNC();
	return g_StateManagerStatic.UsesVertexAndPixelShaders(id);
}

bool ShaderAPI::IsDepthWriteEnabled(StateSnapshot_t id) const
{
	LOG_FUNC();
	return g_StateManagerStatic.IsDepthWriteEnabled(id);
}

VertexFormat_t ShaderAPI::ComputeVertexFormat(int snapshotCount, StateSnapshot_t* ids) const
{
	LOG_FUNC();

	assert(snapshotCount > 0);
	assert(ids);
	if (snapshotCount <= 0 || !ids)
		return VERTEX_FORMAT_UNKNOWN;

	const auto& vtxFmt0 = g_StateManagerStatic.GetState(ids[0]).m_VSVertexFormat;
	VertexCompressionType_t compression = CompressionType(vtxFmt0);
	uint_fast8_t userDataSize = UserDataSize(vtxFmt0);
	uint_fast8_t boneCount = NumBoneWeights(vtxFmt0);
	VertexFormatFlags flags = VertexFormatFlags(VertexFlags(vtxFmt0));
	uint_fast8_t texCoordSize[VERTEX_MAX_TEXTURE_COORDINATES];
	for (size_t i = 0; i < std::size(texCoordSize); i++)
		texCoordSize[i] = TexCoordSize(i, vtxFmt0);

	for (int i = 1; i < snapshotCount; i++)
	{
		const auto& fmt = g_StateManagerStatic.GetState(*ids).m_VSVertexFormat;

		if (auto thisComp = CompressionType(fmt); thisComp != compression)
		{
			Warning(TF2VULKAN_PREFIX "Encountered a material with two passes that specify different vertex compression types!\n");
			assert(false);
			compression = VERTEX_COMPRESSION_NONE;
		}

		if (auto thisBoneCount = NumBoneWeights(fmt); thisBoneCount != boneCount)
		{
			Warning(TF2VULKAN_PREFIX "Encountered a material with two passes that use different numbers of bones!\n");
			assert(false);
		}

		if (auto thisUserDataSize = UserDataSize(fmt); thisUserDataSize != userDataSize)
		{
			Warning(TF2VULKAN_PREFIX "Encountered a material with two passes that use different user data sizes!\n");
			assert(false);
		}

		for (size_t t = 0; t < std::size(texCoordSize); t++)
		{
			auto& oldTCD = texCoordSize[t];
			if (auto thisTexCoordDim = TexCoordSize(t, fmt); thisTexCoordDim != oldTCD)
			{
				if (oldTCD != 0)
					Warning(TF2VULKAN_PREFIX "Encountered a material with two passes that use different texture coord sizes!\n");
				//assert(false);

				if (thisTexCoordDim > oldTCD)
					oldTCD = thisTexCoordDim;
			}
		}
	}

	// Untested
	assert(boneCount == 0);

	VertexFormat_t retVal =
		VertexFormat_t(flags) |
		VERTEX_BONEWEIGHT(boneCount) |
		VERTEX_USERDATA_SIZE(userDataSize);

	[[maybe_unused]] const VertexFormat dbgTest(retVal);

	assert(userDataSize >= 0 && userDataSize <= 4); // Untested

	for (size_t i = 0; i < std::size(texCoordSize); i++)
		retVal |= VERTEX_TEXCOORD_SIZE(i, texCoordSize[i]);

	return retVal;
}

VertexFormat_t ShaderAPI::ComputeVertexUsage(int snapshotCount, StateSnapshot_t* ids) const
{
	LOG_FUNC();
	return ComputeVertexFormat(snapshotCount, ids);
}

ImageFormat ShaderAPI::GetNearestSupportedFormat(ImageFormat fmt, bool filteringRequired) const
{
	LOG_FUNC();
	return FormatInfo::PromoteToHardware(fmt, FormatUsage::ImmutableTexture, filteringRequired);
}

ImageFormat ShaderAPI::GetNearestRenderTargetFormat(ImageFormat fmt) const
{
	LOG_FUNC();
	return FormatInfo::PromoteToHardware(fmt, FormatUsage::RenderTarget, true);
}

bool ShaderAPI::DoRenderTargetsNeedSeparateDepthBuffer() const
{
	LOG_FUNC();
	return false;
}

vk::ImageView ShaderAPI::ShaderTexture::FindOrCreateView(const vk::ImageViewCreateInfo& viewCreateInfo)
{
	LOG_FUNC();
	auto& imgViews = m_ImageViews;

	if (auto found = imgViews.find(viewCreateInfo); found != imgViews.end())
		return found->second.get();

	// Not found, create now
	auto [device, lock] = g_ShaderDevice.GetVulkanDevice().locked();
	const auto& result = (imgViews[viewCreateInfo] = device.createImageViewUnique(viewCreateInfo));
	g_ShaderDevice.SetDebugName(result, Util::string::concat("ImageView: ", m_DebugName).c_str());

	return result.get();
}

void ShaderAPI::SetRenderTarget(ShaderAPITextureHandle_t colTexHandle, ShaderAPITextureHandle_t depthTexHandle)
{
	LOG_FUNC();
	return SetRenderTargetEx(0, colTexHandle, depthTexHandle);
}

void ShaderAPI::SetRenderTargetEx(int renderTargetID, ShaderAPITextureHandle_t colTex, ShaderAPITextureHandle_t depthTex)
{
	LOG_FUNC();
	return g_StateManagerStatic.SetRenderTargetEx(renderTargetID, colTex, depthTex);
}

void ShaderAPI::ClearSnapshots()
{
	LOG_FUNC();
	// We never want to clear "snapshots" (graphics pipelines) on vulkan

	FlushBufferedPrimitives(); // This is a side effect of this function
}

bool ShaderAPI::CanDownloadTextures() const
{
	LOG_FUNC();
	return true; // TODO: Why wouldn't we be able to?
}

void ShaderAPI::ResetRenderState(bool fullReset)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}

void ShaderAPI::EvictManagedResources()
{
	LOG_FUNC();
	// Nothing to do here (for now...)
}

void ShaderAPI::SetStandardVertexShaderConstants(float overbright)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
	g_StateManagerDynamic.SetOverbright(overbright);
}

void ShaderAPI::DXSupportLevelChanged()
{
	LOG_FUNC();
	// We don't care
}

MorphFormat_t ShaderAPI::ComputeMorphFormat(int snapshots, StateSnapshot_t* ids) const
{
	MorphFormat_t fmt = {};

	for (int i = 0; i < snapshots; i++)
		fmt |= g_StateManagerStatic.GetState(ids[i]).m_VSMorphFormat;

	return fmt;
}

void ShaderAPI::ShaderLock()
{
	LOG_FUNC();
	m_ShaderLock.lock();
}

void ShaderAPI::ShaderUnlock()
{
	LOG_FUNC();
	m_ShaderLock.unlock();
}

float ShaderAPI::GammaToLinear_HardwareSpecific(float gamma) const
{
	LOG_FUNC();
	return std::pow(gamma, 2.2f);
}

float ShaderAPI::LinearToGamma_HardwareSpecific(float linear) const
{
	LOG_FUNC();
	return std::pow(linear, 1.0f / 2.2f);
}

void ShaderAPI::SetLinearToGammaConversionTextures(ShaderAPITextureHandle_t srgbWriteEnabledTex, ShaderAPITextureHandle_t identityTex)
{
	LOG_FUNC();
	m_L2GConvTex_SRGBWriteEnabled = srgbWriteEnabledTex;
	m_L2GConvTex_Identity = identityTex;
}

bool ShaderAPI::InEditorMode() const
{
	LOG_FUNC();
	return false; // Not sure if this will ever be true for TF2Vulkan?
}

void ShaderAPI::BeginPass(StateSnapshot_t snapshot)
{
	g_StateManagerStatic.SetState(snapshot);
}

void ShaderAPI::RenderPass(int passID, int passCount)
{
	LOG_FUNC();

	// Everything should be single-pass in vulkan due to unrestricted shader instruction count
	assert(passID == 0);
	assert(passCount == 1);

	auto& cmdBuf = g_ShaderDevice.GetPrimaryCmdBuf();

	g_StateManagerVulkan.ApplyState(
		g_StateManagerStatic.GetState(g_StateManagerStatic.TakeSnapshot()),
		g_StateManagerDynamic.GetDynamicState(),
		*g_StateManagerDynamic.GetActiveMesh().m_Mesh,
		cmdBuf);

	auto& activeMesh = GetActiveMesh();
	activeMesh.m_Mesh->DrawInternal(cmdBuf, activeMesh.m_FirstIndex, activeMesh.m_IndexCount);
}

bool ShaderAPI::ShouldWriteDepthToDestAlpha() const
{
	LOG_FUNC();
	return GetIntRenderingParameter(INT_RENDERPARM_WRITE_DEPTH_TO_DESTALPHA);
}

bool ShaderAPI::IsHWMorphingEnabled() const
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
	//return true; // Always enabled for vulkan
	return false; // Disabled (for now) to reduce complexity
}

void ShaderAPI::MarkUnusedVertexFields(unsigned int flags, int texCoordCount, bool* unusedTexCoords)
{
	assert(flags == 0); // TODO: What flags go here?
	NOT_IMPLEMENTED_FUNC();
}

ITexture* ShaderAPI::GetRenderTargetEx(int renderTargetID)
{
	assert(renderTargetID == 0);
	return &g_ShaderDevice.GetBackBufferColorTexture();
}

namespace
{
	struct BaseCommandBufferCmd
	{
		CommandBufferCommand_t m_Command;
	};

	union CommandBufferCmd
	{
		CommandBufferCommand_t m_Command;

		struct : BaseCommandBufferCmd
		{

		} m_End, m_SetAmbientCubeDynamicStateVertexShader;

		struct : BaseCommandBufferCmd
		{
			uint8_t* m_Address;
		} m_Jump;

		struct : BaseCommandBufferCmd
		{
			uint8_t* m_Address;
		} m_JumpSubroutine;

		struct : BaseCommandBufferCmd
		{
			int m_FirstRegister;
			int m_NumRegisters;
			float m_Values[4];
		} m_SetVertexShaderFloatConstant, m_SetPixelShaderFloatConstant;

		struct : BaseCommandBufferCmd
		{
			int m_DestRegister;
		} m_SetPixelShaderFogParams,
			m_SetPixelShaderStateAmbientLightCube,
			m_CommitPixelShaderLighting,
			m_StoreEyePosInPSConst;

		struct : BaseCommandBufferCmd
		{
			Sampler_t m_Sampler;
			ShaderAPITextureHandle_t m_Texture;
		} m_BindShaderAPITextureHandle;

		struct : BaseCommandBufferCmd
		{
			int m_Index;
		} m_SetPSIndex, m_SetVSIndex;

		struct : BaseCommandBufferCmd
		{
			int m_Register;
			float m_BlendScale;
		} m_SetDepthFeatheringConst;

		struct : BaseCommandBufferCmd
		{
			Sampler_t m_Sampler;
			StandardTextureId_t m_StdTexture;
		} m_BindStdTexture;
	};
}

#define TOSTRING_CASE(enumVal) case enumVal : return #enumVal;

static inline const char* ToString(CommandBufferCommand_t cmd)
{
	switch (cmd)
	{
	default:
		assert(!"Unknown CommandBufferCommand_t");
		return "UNKNOWN";

		TOSTRING_CASE(CBCMD_END);
		TOSTRING_CASE(CBCMD_JUMP);
		TOSTRING_CASE(CBCMD_JSR);
		TOSTRING_CASE(CBCMD_SET_PIXEL_SHADER_FLOAT_CONST);
		TOSTRING_CASE(CBCMD_SET_VERTEX_SHADER_FLOAT_CONST);
		TOSTRING_CASE(CBCMD_SET_VERTEX_SHADER_FLOAT_CONST_REF);
		TOSTRING_CASE(CBCMD_SETPIXELSHADERFOGPARAMS);
		TOSTRING_CASE(CBCMD_STORE_EYE_POS_IN_PSCONST);
		TOSTRING_CASE(CBCMD_COMMITPIXELSHADERLIGHTING);
		TOSTRING_CASE(CBCMD_SETPIXELSHADERSTATEAMBIENTLIGHTCUBE);
		TOSTRING_CASE(CBCMD_SETAMBIENTCUBEDYNAMICSTATEVERTEXSHADER);
		TOSTRING_CASE(CBCMD_SET_DEPTH_FEATHERING_CONST);
		TOSTRING_CASE(CBCMD_BIND_STANDARD_TEXTURE);
		TOSTRING_CASE(CBCMD_BIND_SHADERAPI_TEXTURE_HANDLE);
		TOSTRING_CASE(CBCMD_SET_PSHINDEX);
		TOSTRING_CASE(CBCMD_SET_VSHINDEX);
		TOSTRING_CASE(CBCMD_SET_VERTEX_SHADER_FLASHLIGHT_STATE);
		TOSTRING_CASE(CBCMD_SET_PIXEL_SHADER_FLASHLIGHT_STATE);
		TOSTRING_CASE(CBCMD_SET_PIXEL_SHADER_UBERLIGHT_STATE);
		TOSTRING_CASE(CBCMD_SET_VERTEX_SHADER_NEARZFARZ_STATE);
	}
}

static void PrintCommandBufferCommand(CommandBufferCommand_t cmd)
{
	char buf[512];
	sprintf_s(buf, "%s\t%s\n", TF2VULKAN_PREFIX, ToString(cmd));
	OutputDebugStringA(buf);
}

void ShaderAPI::ExecuteCommandBuffer(uint8* cmdBuf)
{
	LOG_FUNC();
	const auto cmdBufBegin = cmdBuf;

	auto& realCmdBuf = g_ShaderDevice.GetPrimaryCmdBuf();

	const auto& cmdBufTyped = *reinterpret_cast<CommandBufferCmd**>(&cmdBuf);
	while (cmdBufTyped->m_Command != CBCMD_END)
	{
		//PrintCommandBufferCommand(cmdBufTyped->m_Command);

		switch (cmdBufTyped->m_Command)
		{
		default:
			assert(!"Unknown CommandBufferCommand_t");
			return;

		case CBCMD_JSR:
		{
			ExecuteCommandBuffer(cmdBufTyped->m_JumpSubroutine.m_Address);
			cmdBuf += sizeof(cmdBufTyped->m_JumpSubroutine);
			break;
		}

		case CBCMD_SETPIXELSHADERFOGPARAMS:
		{
			SetPixelShaderFogParams(cmdBufTyped->m_SetPixelShaderFogParams.m_DestRegister);
			cmdBuf += sizeof(cmdBufTyped->m_SetPixelShaderFogParams);
			break;
		}

		case CBCMD_BIND_SHADERAPI_TEXTURE_HANDLE:
		{
			const auto& cmd = cmdBufTyped->m_BindShaderAPITextureHandle;
			BindTexture(cmd.m_Sampler, cmd.m_Texture);
			cmdBuf += sizeof(cmd);
			break;
		}

		case CBCMD_SET_VERTEX_SHADER_FLOAT_CONST:
		{
			const auto& cmd = cmdBufTyped->m_SetVertexShaderFloatConstant;
			SetVertexShaderConstant(cmd.m_FirstRegister, cmd.m_Values, cmd.m_NumRegisters);
			cmdBuf += sizeof(cmd) - sizeof(cmd.m_Values) + (sizeof(float) * 4 * cmd.m_NumRegisters);
			break;
		}

		case CBCMD_SET_PIXEL_SHADER_FLOAT_CONST:
		{
			const auto& cmd = cmdBufTyped->m_SetPixelShaderFloatConstant;
			SetPixelShaderConstant(cmd.m_FirstRegister, cmd.m_Values, cmd.m_NumRegisters);
			cmdBuf += sizeof(cmd) - sizeof(cmd.m_Values) + (sizeof(float) * 4 * cmd.m_NumRegisters);
			break;
		}

		case CBCMD_SETAMBIENTCUBEDYNAMICSTATEVERTEXSHADER:
		{
			const auto& cmd = cmdBufTyped->m_SetAmbientCubeDynamicStateVertexShader;
			SetVertexShaderStateAmbientLightCube();
			cmdBuf += sizeof(cmd);
			break;
		}

		case CBCMD_SET_VSHINDEX:
		{
			const auto& cmd = cmdBufTyped->m_SetPSIndex;
			SetVertexShaderIndex(cmd.m_Index);
			cmdBuf += sizeof(cmd);
			break;
		}

		case CBCMD_SET_PSHINDEX:
		{
			const auto& cmd = cmdBufTyped->m_SetPSIndex;
			SetPixelShaderIndex(cmd.m_Index);
			cmdBuf += sizeof(cmd);
			break;
		}

		case CBCMD_SET_DEPTH_FEATHERING_CONST:
		{
			const auto& cmd = cmdBufTyped->m_SetDepthFeatheringConst;
			SetDepthFeatheringPixelShaderConstant(cmd.m_Register, cmd.m_BlendScale);
			cmdBuf += sizeof(cmd);
			break;
		}

		case CBCMD_BIND_STANDARD_TEXTURE:
		{
			const auto& cmd = cmdBufTyped->m_BindStdTexture;
			BindStandardTexture(cmd.m_Sampler, cmd.m_StdTexture);
			cmdBuf += sizeof(cmd);
			break;
		}

		case CBCMD_SETPIXELSHADERSTATEAMBIENTLIGHTCUBE:
		{
			const auto& cmd = cmdBufTyped->m_SetPixelShaderStateAmbientLightCube;
			SetPixelShaderStateAmbientLightCube(cmd.m_DestRegister, false); // TODO: force to black???
			cmdBuf += sizeof(cmd);
			break;
		}

		case CBCMD_COMMITPIXELSHADERLIGHTING:
		{
			const auto& cmd = cmdBufTyped->m_CommitPixelShaderLighting;
			CommitPixelShaderLighting(cmd.m_DestRegister);
			cmdBuf += sizeof(cmd);
			break;
		}

		case CBCMD_STORE_EYE_POS_IN_PSCONST:
		{
			const auto& cmd = cmdBufTyped->m_StoreEyePosInPSConst;
			SetPixelShaderConstant(cmd.m_DestRegister, nullptr, 1);
			cmdBuf += sizeof(cmd);
			break;
		}
		}
	}
}

void ShaderAPI::SetVertexShaderStateAmbientLightCube()
{
	NOT_IMPLEMENTED_FUNC();
	// TODO: Update the vertex shader uniform buffer with the ambient light cube
	// TODO: This probably belongs in IShaderAPI_StateManagerDynamic
}

void ShaderAPI::ForceHardwareSync()
{
	LOG_FUNC_ANYTHREAD();
	auto [graphicsQueue, lock] = g_ShaderDevice.GetGraphicsQueue().locked();
	graphicsQueue->GetQueue().waitIdle();
}

void ShaderAPI::BeginFrame()
{
	LOG_FUNC();
	assert(!m_IsInFrame);
	m_IsInFrame = true;
}

bool ShaderAPI::IsInFrame() const
{
	return m_IsInFrame;
}

void ShaderAPI::EndFrame()
{
	LOG_FUNC();
	assert(m_IsInFrame);
	m_IsInFrame = false;
}

void ShaderAPI::InvalidateDelayedShaderConstants()
{
	// We don't use this functionality (dynamic uniform buffers are used instead)

	//LOG_FUNC();
	// ^^^ Commented out because InvalidateDelayedShaderConstants() gets spammed
}

void ShaderAPI::CopyRenderTargetToScratchTexture(ShaderAPITextureHandle_t srcRT,
	ShaderAPITextureHandle_t dstTexID, const Rect_t* srcRect, const Rect_t* dstRect)
{
	LOG_FUNC();
	TF2VULKAN_PIX_MARKER(__FUNCTION__);

	auto& srcTex = GetTexture(srcRT);
	auto& dstTex = GetTexture(dstTexID);

	auto& cmdBuf = g_ShaderDevice.GetPrimaryCmdBuf();

	vk::ImageBlit blit;
	blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	blit.srcSubresource.baseArrayLayer = 0;
	blit.srcSubresource.mipLevel = 0;
	blit.srcSubresource.layerCount = 1;
	blit.dstSubresource = blit.srcSubresource;
	blit.srcOffsets[0].z = blit.dstOffsets[0].z = 0;
	blit.srcOffsets[1].z = blit.dstOffsets[1].z = 1;

	const auto CopyRectToOffsets = [](const IVulkanTexture & tex, const Rect_t * rect, vk::Offset3D & offs0, vk::Offset3D & offs1)
	{
		offs0.z = 0;
		offs1.z = 1;

		if (rect)
		{
			offs0.x = rect->x;
			offs0.y = rect->y;
			offs1.x = rect->x + rect->width;
			offs1.y = rect->y + rect->height;
		}
		else
		{
			offs0.x = offs0.y = 0;
			tex.GetSize(offs1.x, offs1.y);
		}
	};

	CopyRectToOffsets(srcTex, srcRect, blit.srcOffsets[0], blit.srcOffsets[1]);
	CopyRectToOffsets(dstTex, dstRect, blit.dstOffsets[0], blit.dstOffsets[1]);

	auto barrierFactory = Factories::ImageMemoryBarrierFactory{}
		.SetProducerStage(vk::PipelineStageFlagBits::eColorAttachmentOutput, true, true)
		.SetConsumerStage(vk::PipelineStageFlagBits::eTransfer, true, true);

	barrierFactory.SetImage(srcTex)
		.SetOldLayout(srcTex.GetDefaultLayout())
		.SetNewLayout(vk::ImageLayout::eTransferSrcOptimal)
		.Submit(cmdBuf);

	barrierFactory.SetImage(dstTex)
		.SetOldLayout(dstTex.GetDefaultLayout())
		.SetNewLayout(vk::ImageLayout::eTransferDstOptimal)
		.Submit(cmdBuf);

	if ((blit.srcOffsets[1] - blit.srcOffsets[0]) == (blit.dstOffsets[1] - blit.dstOffsets[0]))
	{
		const vk::ImageCopy copy(
			blit.srcSubresource, blit.srcOffsets[0],
			blit.dstSubresource, blit.dstOffsets[0],
			ToExtent(blit.srcOffsets[1] - blit.srcOffsets[0]));

		// We can just copy
		// TODO: Ensure that formats are compatible
		cmdBuf.copyImage(srcTex.GetImage(), vk::ImageLayout::eTransferSrcOptimal,
			dstTex.GetImage(), vk::ImageLayout::eTransferDstOptimal,
			copy);
	}
	else
	{
		// Need to blit
		cmdBuf.blitImage(srcTex.GetImage(), vk::ImageLayout::eColorAttachmentOptimal,
			dstTex.GetImage(), vk::ImageLayout::eTransferDstOptimal,
			blit, vk::Filter::eLinear);
	}

	barrierFactory
		.SetProducerStage(vk::PipelineStageFlagBits::eTransfer, true, true)
		.SetConsumerStage(vk::PipelineStageFlagBits::eAllGraphics, true, true);

	barrierFactory.SetImage(srcTex)
		.SetOldLayout(vk::ImageLayout::eTransferSrcOptimal)
		.SetNewLayout(srcTex.GetDefaultLayout())
		.Submit(cmdBuf);

	barrierFactory.SetImage(dstTex)
		.SetOldLayout(vk::ImageLayout::eTransferDstOptimal)
		.SetNewLayout(dstTex.GetDefaultLayout())
		.Submit(cmdBuf);
}

int ShaderAPI::CompareSnapshots(StateSnapshot_t lhs, StateSnapshot_t rhs)
{
	LOG_FUNC();

	const auto& stateLHS = g_StateManagerStatic.GetState(lhs);
	const auto& stateRHS = g_StateManagerStatic.GetState(rhs);

	const auto result = stateLHS <=> stateRHS;

	if (std::is_lt(result))
		return -1;
	else if (std::is_gt(result))
		return 1;
	else
		return 0;
}

ShaderAPIOcclusionQuery_t ShaderAPI::CreateOcclusionQueryObject()
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
	return ShaderAPIOcclusionQuery_t(0);
}
void ShaderAPI::DestroyOcclusionQueryObject(ShaderAPIOcclusionQuery_t query)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}
void ShaderAPI::BeginOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t query)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}
void ShaderAPI::EndOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t query)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}
int ShaderAPI::OcclusionQuery_GetNumPixelsRendered(ShaderAPIOcclusionQuery_t query, bool flush)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
	return 0;
}

void ShaderAPI::CopyRenderTargetToTextureEx(ShaderAPITextureHandle_t texHandle, int renderTargetID,
	const Rect_t* srcRect, const Rect_t* dstRect)
{
	LOG_FUNC();
	TF2VULKAN_PIX_MARKER(__FUNCTION__ "(): NOT IMPLEMENTED");
}

void ShaderAPI::CopyTextureToRenderTargetEx(int renderTargetID, ShaderAPITextureHandle_t texHandle,
	const Rect_t* srcRect, const Rect_t* dstRect)
{
	LOG_FUNC();
	TF2VULKAN_PIX_MARKER(__FUNCTION__ "(): NOT IMPLEMENTED");
}

void ShaderAPI::CopyRenderTargetToTexture(ShaderAPITextureHandle_t texID)
{
	LOG_FUNC();
	return CopyRenderTargetToTextureEx(texID, 0, nullptr, nullptr);
}

void ShaderAPI::EnableShaderShaderMutex(bool enabled)
{
	LOG_FUNC(); // We always use thread-safe code with vulkan
}
