#include "FormatConversion.h"
#include "IStateManagerDynamic.h"
#include "ShaderDevice.h"
#include "ShadowStateManager.h"
#include "TF2Vulkan/TextureData.h"
#include "VulkanFactories.h"
#include "VulkanMesh.h"

#include <TF2Vulkan/Util/DirtyVar.h>
#include <TF2Vulkan/Util/interface.h>
#include <TF2Vulkan/Util/ImageManip.h>
#include <TF2Vulkan/Util/std_algorithm.h>
#include <TF2Vulkan/Util/std_string.h>
#include <TF2Vulkan/Util/std_utility.h>

#include <Color.h>
#include <materialsystem/imesh.h>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <unordered_map>

using namespace TF2Vulkan;

#define LOG_FUNC_TEX_NAME(texHandle, texName) \
	LOG_FUNC_MSG(texName)

#define LOG_FUNC_TEX(texHandle) \
	LOG_FUNC_TEX_NAME((texHandle), (m_Textures.at(texHandle).GetDebugName()))

namespace
{
	union SamplerSettingsDynamic
	{
		constexpr SamplerSettingsDynamic() :
			m_MinFilter(SHADER_TEXFILTERMODE_LINEAR_MIPMAP_LINEAR),
			m_MagFilter(SHADER_TEXFILTERMODE_LINEAR_MIPMAP_LINEAR),

			m_WrapS(SHADER_TEXWRAPMODE_REPEAT),
			m_WrapT(SHADER_TEXWRAPMODE_REPEAT),
			m_WrapU(SHADER_TEXWRAPMODE_REPEAT)
		{
		}

		constexpr bool operator==(const SamplerSettingsDynamic& s) const
		{
			return m_BackingValue == s.m_BackingValue;
		}

		struct
		{
			ShaderTexFilterMode_t m_MinFilter : 3;
			ShaderTexFilterMode_t m_MagFilter : 3;

			ShaderTexWrapMode_t m_WrapS : 2;
			ShaderTexWrapMode_t m_WrapT : 2;
			ShaderTexWrapMode_t m_WrapU : 2;
		};
		int m_BackingValue;
	};
	static_assert(sizeof(SamplerSettingsDynamic::m_BackingValue) == sizeof(SamplerSettingsDynamic));
}

template<> struct ::std::hash<SamplerSettingsDynamic>
{
	size_t operator()(const SamplerSettingsDynamic& s) const
	{
		return Util::hash_value(s.m_BackingValue);
	}
};

namespace
{
	class ShaderAPI final : public IShaderAPI_StateManagerDynamic
	{
	public:
		int GetViewports(ShaderViewport_t* viewports, int max) const override { NOT_IMPLEMENTED_FUNC(); }

		double CurrentTime() const override { NOT_IMPLEMENTED_FUNC(); }

		void GetLightmapDimensions(int* w, int* h) override { NOT_IMPLEMENTED_FUNC(); }

		void LoadMatrix(float* m) override { NOT_IMPLEMENTED_FUNC(); }
		void MultMatrix(float* m) override { NOT_IMPLEMENTED_FUNC(); }
		void MultMatrixLocal(float* m) override { NOT_IMPLEMENTED_FUNC(); }
		void GetMatrix(MaterialMatrixMode_t matrixMode, float* dst) override { NOT_IMPLEMENTED_FUNC(); }
		void LoadCameraToWorld() override { NOT_IMPLEMENTED_FUNC(); }
		void Ortho(double left, double right, double bottom, double top, double zNear, double zFar) override { NOT_IMPLEMENTED_FUNC(); }
		void PerspectiveX(double fovX, double aspect, double zNear, double zFar) override { NOT_IMPLEMENTED_FUNC(); }
		void PerspectiveOffCenterX(double fovX, double aspect, double zNear, double zFar,
			double bottom, double top, double left, double right) override { NOT_IMPLEMENTED_FUNC(); }
		void PickMatrix(int x, int y, int width, int height) override { NOT_IMPLEMENTED_FUNC(); }
		void Rotate(float angle, float x, float y, float z) override { NOT_IMPLEMENTED_FUNC(); }
		void Translate(float x, float y, float z) override { NOT_IMPLEMENTED_FUNC(); }
		void Scale(float x, float y, float z) override { NOT_IMPLEMENTED_FUNC(); }
		void ScaleXY(float x, float y) override { NOT_IMPLEMENTED_FUNC(); }

		void Color3f(float r, float g, float b) override { NOT_IMPLEMENTED_FUNC(); }
		void Color3fv(const float* rgb) override { NOT_IMPLEMENTED_FUNC(); }
		void Color4f(float r, float g, float b, float a) override { NOT_IMPLEMENTED_FUNC(); }
		void Color4fv(const float* rgba) override { NOT_IMPLEMENTED_FUNC(); }

		void Color3ub(uint8_t r, uint8_t g, uint8_t b) override { NOT_IMPLEMENTED_FUNC(); }
		void Color3ubv(const uint8_t* rgb) override { NOT_IMPLEMENTED_FUNC(); }
		void Color4ub(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override { NOT_IMPLEMENTED_FUNC(); }
		void Color4ubv(const uint8_t* rgba) override { NOT_IMPLEMENTED_FUNC(); }

		void SetVertexShaderConstant(int var, const float* vec, int numConst, bool force) override { NOT_IMPLEMENTED_FUNC(); }
		void SetBooleanVertexShaderConstant(int var, const BOOL* vec, int numBools, bool force) override { NOT_IMPLEMENTED_FUNC(); }
		void SetIntegerVertexShaderConstant(int var, const int* vec, int numIntVecs, bool force) override { NOT_IMPLEMENTED_FUNC(); }
		void SetPixelShaderConstant(int var, const float* vec, int numConst, bool force) override { NOT_IMPLEMENTED_FUNC(); }
		void SetBooleanPixelShaderConstant(int var, const BOOL* vec, int numBools, bool force) override { NOT_IMPLEMENTED_FUNC(); }
		void SetIntegerPixelShaderConstant(int var, const int* vec, int numIntVecs, bool force) override { NOT_IMPLEMENTED_FUNC(); }

		void GetWorldSpaceCameraPosition(float* pos) const override { NOT_IMPLEMENTED_FUNC(); }

		int GetCurrentLightCombo() const override { NOT_IMPLEMENTED_FUNC(); }

		void SetTextureTransformDimension(TextureStage_t stage, int dimension, bool projected) override { NOT_IMPLEMENTED_FUNC(); }
		void DisableTextureTransform(TextureStage_t stage) override { NOT_IMPLEMENTED_FUNC(); }
		void SetBumpEnvMatrix(TextureStage_t stage, float m00, float m01, float m10, float m11) override { NOT_IMPLEMENTED_FUNC(); }

		void SetVertexShaderIndex(int index) override { NOT_IMPLEMENTED_FUNC(); }
		void SetPixelShaderIndex(int index) override { NOT_IMPLEMENTED_FUNC(); }

		int GetMaxLights() const override { NOT_IMPLEMENTED_FUNC(); }
		const LightDesc_t& GetLight(int lightNum) const override { NOT_IMPLEMENTED_FUNC(); }

		void SetVertexShaderStateAmbientLightCube() override { NOT_IMPLEMENTED_FUNC(); }
		void SetPixelShaderStateAmbientLightCube(int pshReg, bool forceToBlack) override { NOT_IMPLEMENTED_FUNC(); }
		void CommitPixelShaderLighting(int pshReg) override { NOT_IMPLEMENTED_FUNC(); }

		CMeshBuilder* GetVertexModifyBuilder() override { NOT_IMPLEMENTED_FUNC(); }
		bool InFlashlightMode() const override { NOT_IMPLEMENTED_FUNC(); }
		bool InEditorMode() const override { NOT_IMPLEMENTED_FUNC(); }

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

		void TexMinFilter(ShaderAPITextureHandle_t tex, ShaderTexFilterMode_t mode) override;
		void TexMagFilter(ShaderAPITextureHandle_t tex, ShaderTexFilterMode_t mode) override;
		void TexWrap(ShaderAPITextureHandle_t tex, ShaderTexCoordComponent_t coord, ShaderTexWrapMode_t wrapMode) override;

		void CopyRenderTargetToTexture(ShaderAPITextureHandle_t texHandle) override { NOT_IMPLEMENTED_FUNC(); }
		void CopyRenderTargetToTextureEx(ShaderAPITextureHandle_t texHandle, int renderTargetID,
			const Rect_t* srcRect, const Rect_t* dstRect) override { NOT_IMPLEMENTED_FUNC(); }
		void CopyTextureToRenderTargetEx(int renderTargetID, ShaderAPITextureHandle_t texHandle,
			const Rect_t* srcRect, const Rect_t* dstRect) override { NOT_IMPLEMENTED_FUNC(); }
		void CopyRenderTargetToScratchTexture(ShaderAPITextureHandle_t srcRT,
			ShaderAPITextureHandle_t dstTex, const Rect_t* srcRect, const Rect_t* dstRect) override { NOT_IMPLEMENTED_FUNC(); }

		void FlushBufferedPrimitives() override;

		IMesh* GetDynamicMesh(IMaterial* material, int hwSkinBoneCount, bool buffered,
			IMesh* vertexOverride, IMesh* indexOverride) override;
		IMesh* GetDynamicMeshEx(IMaterial* material, VertexFormat_t vertexFormat, int hwSkinBoneCount,
			bool buffered, IMesh* vertexOverride, IMesh* indexOverride) override;

		bool IsTranslucent(StateSnapshot_t id) const override;
		bool IsAlphaTested(StateSnapshot_t id) const override;
		bool UsesVertexAndPixelShaders(StateSnapshot_t id) const override;
		bool IsDepthWriteEnabled(StateSnapshot_t id) const override;

		VertexFormat_t ComputeVertexFormat(int snapshotCount, StateSnapshot_t* ids) const override;
		VertexFormat_t ComputeVertexUsage(int snapshotCount, StateSnapshot_t* ids) const override;

		void BeginPass(StateSnapshot_t snapshot) override { NOT_IMPLEMENTED_FUNC(); }
		void RenderPass(int passID, int passCount) override { NOT_IMPLEMENTED_FUNC(); }
		bool IsInPass() const override { NOT_IMPLEMENTED_FUNC(); }
		void BeginFrame() override { NOT_IMPLEMENTED_FUNC(); }
		bool IsInFrame() const override { NOT_IMPLEMENTED_FUNC(); }
		void EndFrame() override { NOT_IMPLEMENTED_FUNC(); }

		void FlushHardware() override { NOT_IMPLEMENTED_FUNC(); }
		void ForceHardwareSync() override { NOT_IMPLEMENTED_FUNC(); }

		void SetNumBoneWeights(int boneCount) override { NOT_IMPLEMENTED_FUNC(); }

		void SetLight(int light, const LightDesc_t& desc) override { NOT_IMPLEMENTED_FUNC(); }
		void SetLightingOrigin(Vector lightingOrigin) override { NOT_IMPLEMENTED_FUNC(); }
		void SetAmbientLight(float r, float g, float b) override { NOT_IMPLEMENTED_FUNC(); }
		void SetAmbientLightCube(Vector4D cube[6]) override { NOT_IMPLEMENTED_FUNC(); }

		void ShadeMode(ShaderShadeMode_t mode) override { NOT_IMPLEMENTED_FUNC(); }

		void CullMode(MaterialCullMode_t mode) override { NOT_IMPLEMENTED_FUNC(); }

		void ForceDepthFuncEquals(bool enable) override { NOT_IMPLEMENTED_FUNC(); }

		void OverrideDepthEnable(bool enable, bool depthEnable) override { NOT_IMPLEMENTED_FUNC(); }

		void SetHeightClipZ(float z) override { NOT_IMPLEMENTED_FUNC(); }
		void SetHeightClipMode(MaterialHeightClipMode_t mode) override { NOT_IMPLEMENTED_FUNC(); }

		void SetClipPlane(int index, const float* plane) override { NOT_IMPLEMENTED_FUNC(); }
		void EnableClipPlane(int index, bool enable) override { NOT_IMPLEMENTED_FUNC(); }

		void SetSkinningMatrices() override { NOT_IMPLEMENTED_FUNC(); }

		ImageFormat GetNearestSupportedFormat(ImageFormat fmt, bool filteringRequired) const override;
		ImageFormat GetNearestRenderTargetFormat(ImageFormat fmt) const override;

		bool DoRenderTargetsNeedSeparateDepthBuffer() const override;

		IShaderAPITexture& CreateTexture(std::string&& dbgName, const vk::ImageCreateInfo& imgCI) override;
		ShaderAPITextureHandle_t CreateTexture(int width, int height, int depth, ImageFormat dstImgFormat,
			int mipLevelCount, int copyCount, int flags, const char* dbgName, const char* texGroupName) override;
		void CreateTextures(ShaderAPITextureHandle_t* handles, int count, int width, int height, int depth,
			ImageFormat dstImgFormat, int mipLevelCount, int copyCount, int flags, const char* dbgName,
			const char* texGroupName) override;
		void DeleteTexture(ShaderAPITextureHandle_t tex) override;
		bool IsTexture(ShaderAPITextureHandle_t tex) override;
		bool IsTextureResident(ShaderAPITextureHandle_t tex) override { NOT_IMPLEMENTED_FUNC(); }

		ShaderAPITextureHandle_t CreateDepthTexture( ImageFormat rtFormat, int width,
			int height, const char* dbgName, bool texture) override { NOT_IMPLEMENTED_FUNC(); }

		void TexImageFromVTF(ShaderAPITextureHandle_t texHandle, IVTFTexture* vtf, int ivtfFrame) override;

		bool UpdateTexture(ShaderAPITextureHandle_t texHandle, const TextureData* data,
			size_t count) override;

		bool TexLock(int level, int cubeFaceID, int xOffset, int yOffset,
			int width, int height, CPixelWriter& writer) override { NOT_IMPLEMENTED_FUNC(); }
		void TexUnlock() override { NOT_IMPLEMENTED_FUNC(); }

		void TexSetPriority(int priority) override { NOT_IMPLEMENTED_FUNC(); }

		void BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle) override { NOT_IMPLEMENTED_FUNC(); }

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

		void FogStart(float start) override { NOT_IMPLEMENTED_FUNC(); }
		void FogEnd(float end) override { NOT_IMPLEMENTED_FUNC(); }
		void SetFogZ(float fogZ) override { NOT_IMPLEMENTED_FUNC(); }
		void SceneFogColor3ub(unsigned char r, unsigned char g, unsigned char b) override { NOT_IMPLEMENTED_FUNC(); }
		void GetSceneFogColor(unsigned char* rgb) override { NOT_IMPLEMENTED_FUNC(); }
		void SceneFogMode(MaterialFogMode_t mode) override { NOT_IMPLEMENTED_FUNC(); }
		void GetFogDistances(float* start, float* end, float* fogZ) override { NOT_IMPLEMENTED_FUNC(); }
		void FogMaxDensity(float maxDensity) override { NOT_IMPLEMENTED_FUNC(); }
		MaterialFogMode_t GetSceneFogMode() override { NOT_IMPLEMENTED_FUNC(); }
		void SetPixelShaderFogParams(int reg) override { NOT_IMPLEMENTED_FUNC(); }
		MaterialFogMode_t GetCurrentFogType() const override { NOT_IMPLEMENTED_FUNC(); }

		bool CanDownloadTextures() const override;

		void ResetRenderState(bool fullReset) override;

		int GetCurrentDynamicVBSize() override { NOT_IMPLEMENTED_FUNC(); }
		void DestroyVertexBuffers(bool exitingLevel) override { NOT_IMPLEMENTED_FUNC(); }

		void EvictManagedResources() override;

		void SyncToken(const char* token) override { NOT_IMPLEMENTED_FUNC(); }

		void SetStandardVertexShaderConstants(float overbright) override;

		ShaderAPIOcclusionQuery_t CreateOcclusionQueryObject() override { NOT_IMPLEMENTED_FUNC(); }
		void DestroyOcclusionQueryObject(ShaderAPIOcclusionQuery_t query) override { NOT_IMPLEMENTED_FUNC(); }
		void BeginOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t query) override { NOT_IMPLEMENTED_FUNC(); }
		void EndOcclusionQueryDrawing(ShaderAPIOcclusionQuery_t query) override { NOT_IMPLEMENTED_FUNC(); }
		int OcclusionQuery_GetNumPixelsRendered(ShaderAPIOcclusionQuery_t query, bool flush) override { NOT_IMPLEMENTED_FUNC(); }

		const FlashlightState_t& GetFlashlightState(VMatrix& worldToTexture) const override { NOT_IMPLEMENTED_FUNC(); }
		const FlashlightState_t& GetFlashlightStateEx(VMatrix& worldToTexture,
			ITexture** flashlightDepthTex) const override { NOT_IMPLEMENTED_FUNC(); }
		void SetFlashlightState(const FlashlightState_t& state, const VMatrix& worldToTexture) override { NOT_IMPLEMENTED_FUNC(); }
		void SetFlashlightStateEx(const FlashlightState_t& state, const VMatrix& worldToTexture,
			ITexture* flashlightDepthTex) override { NOT_IMPLEMENTED_FUNC(); }

		void ClearVertexAndPixelShaderRefCounts() override { NOT_IMPLEMENTED_FUNC(); }
		void PurgeUnusedVertexAndPixelShaders() override { NOT_IMPLEMENTED_FUNC(); }

		void DXSupportLevelChanged() override;

		void EnableUserClipTransformOverride(bool enable) override { NOT_IMPLEMENTED_FUNC(); }
		void UserClipTransform(const VMatrix& worldToView) override { NOT_IMPLEMENTED_FUNC(); }

		MorphFormat_t ComputeMorphFormat(int snapshots, StateSnapshot_t* ids) const override;

		void HandleDeviceLost() override { NOT_IMPLEMENTED_FUNC(); }

		void EnableLinearColorSpaceFrameBuffer(bool enable) override { NOT_IMPLEMENTED_FUNC_NOBREAK(); }

		void SetFloatRenderingParameter(RenderParamFloat_t param, float value) override { NOT_IMPLEMENTED_FUNC(); }
		void SetIntRenderingParameter(RenderParamInt_t param, int value) override { NOT_IMPLEMENTED_FUNC(); }
		void SetVectorRenderingParameter(RenderParamVector_t param, const Vector& value) override { NOT_IMPLEMENTED_FUNC(); }

		float GetFloatRenderingParameter(RenderParamFloat_t param) const override { NOT_IMPLEMENTED_FUNC(); }
		int GetIntRenderingParameter(RenderParamInt_t param) const override { NOT_IMPLEMENTED_FUNC(); }
		Vector GetVectorRenderingParameter(RenderParamVector_t param) const override { NOT_IMPLEMENTED_FUNC(); }

		void SetFastClipPlane(const float* plane) override { NOT_IMPLEMENTED_FUNC(); }
		void EnableFastClip(bool enable) override { NOT_IMPLEMENTED_FUNC(); }

		void GetMaxToRender(IMesh* mesh, bool maxUntilFlush, int* maxVerts, int* maxIndices) override { NOT_IMPLEMENTED_FUNC(); }

		int GetMaxVerticesToRender(IMaterial* material) override { NOT_IMPLEMENTED_FUNC(); }
		int GetMaxIndicesToRender() override { NOT_IMPLEMENTED_FUNC(); }

		void SetStencilEnable(bool enabled) override { NOT_IMPLEMENTED_FUNC(); }
		void SetStencilFailOperation(StencilOperation_t op) override { NOT_IMPLEMENTED_FUNC(); }
		void SetStencilZFailOperation(StencilOperation_t op) override { NOT_IMPLEMENTED_FUNC(); }
		void SetStencilPassOperation(StencilOperation_t op) override { NOT_IMPLEMENTED_FUNC(); }
		void SetStencilCompareFunction(StencilComparisonFunction_t fn) override { NOT_IMPLEMENTED_FUNC(); }
		void SetStencilReferenceValue(int ref) override { NOT_IMPLEMENTED_FUNC(); }
		void SetStencilTestMask(uint32 mask) override { NOT_IMPLEMENTED_FUNC(); }
		void SetStencilWriteMask(uint32 mask) override { NOT_IMPLEMENTED_FUNC(); }
		void ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax, int value) override { NOT_IMPLEMENTED_FUNC(); }

		void DisableAllLocalLights() override { NOT_IMPLEMENTED_FUNC(); }
		int CompareSnapshots(StateSnapshot_t lhs, StateSnapshot_t rhs) override { NOT_IMPLEMENTED_FUNC(); }

		IMesh* GetFlexMesh() override { NOT_IMPLEMENTED_FUNC(); }

		bool SupportsMSAAMode(int msaaMode) override { NOT_IMPLEMENTED_FUNC(); }

		bool OwnGPUResources(bool enable) override { NOT_IMPLEMENTED_FUNC(); }

		void BeginPIXEvent(unsigned long color, const char* szName) override { NOT_IMPLEMENTED_FUNC(); }
		void EndPIXEvent() override { NOT_IMPLEMENTED_FUNC(); }
		void SetPIXMarker(unsigned long color, const char* szName) override { NOT_IMPLEMENTED_FUNC(); }

		void EnableAlphaToCoverage() override { NOT_IMPLEMENTED_FUNC(); }
		void DisableAlphaToCoverage() override { NOT_IMPLEMENTED_FUNC(); }

		void ComputeVertexDescription(unsigned char* buffer, VertexFormat_t fmt, MeshDesc_t& desc) const override { NOT_IMPLEMENTED_FUNC(); }

		bool SupportsShadowDepthTextures() override { NOT_IMPLEMENTED_FUNC(); }

		void SetDisallowAccess(bool disallowed) override { NOT_IMPLEMENTED_FUNC(); }
		void EnableShaderShaderMutex(bool enabled) override { NOT_IMPLEMENTED_FUNC(); }
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

		void SetScissorRect(const int left, const int top, const int right, const int bottom,
			const bool enableScissor) override { NOT_IMPLEMENTED_FUNC(); }

		bool SupportsCSAAMode(int numSamples, int qualityLevel) override { NOT_IMPLEMENTED_FUNC(); }

		void InvalidateDelayedShaderConstants() override { NOT_IMPLEMENTED_FUNC(); }

		float GammaToLinear_HardwareSpecific(float gamma) const override;
		float LinearToGamma_HardwareSpecific(float linear) const override;

		void SetLinearToGammaConversionTextures(ShaderAPITextureHandle_t srgbWriteEnabledTex,
			ShaderAPITextureHandle_t identityTex) override;

		ImageFormat GetNullTextureFormat() override { NOT_IMPLEMENTED_FUNC(); }

		void BindVertexTexture(VertexTextureSampler_t sampler, ShaderAPITextureHandle_t tex) override { NOT_IMPLEMENTED_FUNC(); }

		void EnableHWMorphing(bool enabled) override { NOT_IMPLEMENTED_FUNC(); }

		void SetFlexWeights(int firstWeight, int count, const MorphWeight_t* weights) override { NOT_IMPLEMENTED_FUNC(); }

		void AcquireThreadOwnership() override { NOT_IMPLEMENTED_FUNC(); }
		void ReleaseThreadOwnership() override { NOT_IMPLEMENTED_FUNC(); }

		bool SupportsNormalMapCompression() const override { NOT_IMPLEMENTED_FUNC(); }

		void EnableBuffer2FramesAhead(bool enable) override { NOT_IMPLEMENTED_FUNC(); }

		void SetDepthFeatheringPixelShaderConstant(int constant, float depthBlendScale) override { NOT_IMPLEMENTED_FUNC(); }

		void PrintfVA(char* fmt, va_list vargs) override { NOT_IMPLEMENTED_FUNC(); }
		void Printf(PRINTF_FORMAT_STRING const char* fmt, ...) override { NOT_IMPLEMENTED_FUNC(); }
		float Knob(char* knobName, float* setValue) override { NOT_IMPLEMENTED_FUNC(); }

		void OverrideAlphaWriteEnable(bool enable, bool alphaWriteEnable) override { NOT_IMPLEMENTED_FUNC(); }
		void OverrideColorWriteEnable(bool overrideEnable, bool colorWriteEnable) override { NOT_IMPLEMENTED_FUNC(); }

		void LockRect(void** outBits, int* outPitch, ShaderAPITextureHandle_t tex, int mipLevel,
			int x, int y, int w, int h, bool write, bool read) override { NOT_IMPLEMENTED_FUNC(); }
		void UnlockRect(ShaderAPITextureHandle_t tex, int mipLevel) override { NOT_IMPLEMENTED_FUNC(); }

		void BindStandardTexture(Sampler_t sampler, StandardTextureId_t id) override { NOT_IMPLEMENTED_FUNC(); }
		void BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id) override { NOT_IMPLEMENTED_FUNC(); }

		ITexture* GetRenderTargetEx(int renderTargetID) override { NOT_IMPLEMENTED_FUNC(); }

		void SetToneMappingScaleLinear(const Vector& scale) override { NOT_IMPLEMENTED_FUNC_NOBREAK(); }
		const Vector& GetToneMappingScaleLinear() const override { NOT_IMPLEMENTED_FUNC(); }
		float GetLightMapScaleFactor() const override { NOT_IMPLEMENTED_FUNC(); }

		void LoadBoneMatrix(int boneIndex, const float* m) override { NOT_IMPLEMENTED_FUNC(); }

		void GetDXLevelDefaults(uint& dxLevelMax, uint& dxLevelRecommended) override { NOT_IMPLEMENTED_FUNC(); }

		float GetAmbientLightCubeLuminance() override { NOT_IMPLEMENTED_FUNC(); }

		void GetDX9LightState(LightState_t* state) const override { NOT_IMPLEMENTED_FUNC(); }
		int GetPixelFogCombo() override { NOT_IMPLEMENTED_FUNC(); }

		bool IsHWMorphingEnabled() const override { NOT_IMPLEMENTED_FUNC(); }

		void GetStandardTextureDimensions(int* width, int* height, StandardTextureId_t id) override { NOT_IMPLEMENTED_FUNC(); }

		bool ShouldWriteDepthToDestAlpha() const override { NOT_IMPLEMENTED_FUNC(); }

		void PushDeformation(const DeformationBase_t* deformation) override { NOT_IMPLEMENTED_FUNC(); }
		void PopDeformation() override { NOT_IMPLEMENTED_FUNC(); }
		int GetNumActiveDeformations() const override { NOT_IMPLEMENTED_FUNC(); }

		int GetPackedDeformationInformation(int maxOfUnderstoodDeformations, float* constantValuesOut,
			int bufSize, int maxDeformations, int* numDefsOut) const override { NOT_IMPLEMENTED_FUNC(); }

		void MarkUnusedVertexFields(unsigned int flags, int texCoordCount, bool* unusedTexCoords) override { NOT_IMPLEMENTED_FUNC(); }

		void ExecuteCommandBuffer(uint8* cmdBuf) override { NOT_IMPLEMENTED_FUNC(); }

		void SetStandardTextureHandle(StandardTextureId_t id, ShaderAPITextureHandle_t tex) override;
		void GetCurrentColorCorrection(ShaderColorCorrectionInfo_t* info) override { NOT_IMPLEMENTED_FUNC(); }

		void SetPSNearAndFarZ(int psRegister) override { NOT_IMPLEMENTED_FUNC(); }

		void TexLodClamp(int something) override;
		void TexLodBias(float bias) override;

		void CopyTextureToTexture(int something1, int something2) override { NOT_IMPLEMENTED_FUNC(); }

		const IShaderAPITexture& GetTexture(ShaderAPITextureHandle_t texID) const override;

	private:
		mutable std::recursive_mutex m_ShaderLock;

		std::atomic<ShaderAPITextureHandle_t> m_NextTextureHandle = 1;

		struct ShaderTexture : IShaderAPITexture
		{
			ShaderTexture(std::string&& debugName, ShaderAPITextureHandle_t handle,
				const vk::ImageCreateInfo& ci, vma::AllocatedImage&& img);

			std::string m_DebugName;
			vk::ImageCreateInfo m_CreateInfo;
			vma::AllocatedImage m_Image;
			ShaderAPITextureHandle_t m_Handle;
			std::unordered_map<vk::ImageViewCreateInfo, vk::UniqueImageView> m_ImageViews;

			SamplerSettingsDynamic m_SamplerSettings;

			std::string_view GetDebugName() const override { return m_DebugName; }
			const vk::Image& GetImage() const override { return m_Image.GetImage(); }
			const vk::ImageCreateInfo& GetImageCreateInfo() const override { return m_CreateInfo; }
			const vk::ImageView& FindOrCreateView(const vk::ImageViewCreateInfo& createInfo) override;
			ShaderAPITextureHandle_t GetHandle() const override { return m_Handle; }
		};
		std::unordered_map<ShaderAPITextureHandle_t, ShaderTexture> m_Textures;

		std::unordered_map<VertexFormat, VulkanMesh> m_DynamicMeshes;

		std::array<ShaderAPITextureHandle_t, TEXTURE_MAX_STD_TEXTURES> m_StdTextures;
		ShaderAPITextureHandle_t m_L2GConvTex_SRGBWriteEnabled = INVALID_SHADERAPI_TEXTURE_HANDLE;
		ShaderAPITextureHandle_t m_L2GConvTex_Identity = INVALID_SHADERAPI_TEXTURE_HANDLE;
	};
}

static ShaderAPI s_ShaderAPI;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(ShaderAPI, IShaderAPI, SHADERAPI_INTERFACE_VERSION, s_ShaderAPI);

IShaderAPI_StateManagerDynamic& TF2Vulkan::g_StateManagerDynamic = s_ShaderAPI;
IShaderAPIInternal& TF2Vulkan::g_ShaderAPIInternal = s_ShaderAPI;

void ShaderAPI::ClearBuffers(bool clearColor, bool clearDepth, bool clearStencil, int rtWidth, int rtHeight)
{
	LOG_FUNC();

	auto& queue = g_ShaderDevice.GetGraphicsQueue();
	auto cmdBuf = queue.CreateCmdBufferAndBegin();
	vk::ClearAttachment att;

	if (clearColor)
	{
		att.aspectMask |= vk::ImageAspectFlagBits::eColor;
		Util::algorithm::copy(GetDynamicState().m_ClearColor, att.clearValue.color.float32);
		att.colorAttachment = 0;
	}

	if (clearDepth)
	{
		att.aspectMask |= vk::ImageAspectFlagBits::eDepth;
		att.clearValue.depthStencil.depth = 0;
	}

	if (clearStencil)
	{
		att.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		att.clearValue.depthStencil.stencil = 0;
	}

	vk::ClearRect rect;
	Util::SafeConvert(rtWidth, rect.rect.extent.width);
	Util::SafeConvert(rtHeight, rect.rect.extent.height);

	g_ShadowStateManager.ApplyCurrentState(*cmdBuf);
	cmdBuf->clearAttachments(att, rect);

	cmdBuf->endRenderPass();
	queue.EndAndSubmit(cmdBuf.get());
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

	auto snapshot = g_ShadowStateManager.TakeSnapshot();
	return Util::SafeConvert<StateSnapshot_t>(snapshot);
}

void ShaderAPI::TexMinFilter(ShaderAPITextureHandle_t texHandle, ShaderTexFilterMode_t mode)
{
	LOG_FUNC();
	auto& tex = m_Textures.at(texHandle);
	tex.m_SamplerSettings.m_MinFilter = mode;
}

void ShaderAPI::TexMagFilter(ShaderAPITextureHandle_t texHandle, ShaderTexFilterMode_t mode)
{
	LOG_FUNC();
	auto& tex = m_Textures.at(texHandle);
	tex.m_SamplerSettings.m_MagFilter = mode;
}

void ShaderAPI::TexWrap(ShaderAPITextureHandle_t texHandle, ShaderTexCoordComponent_t coord, ShaderTexWrapMode_t wrapMode)
{
	LOG_FUNC();
	auto& tex = m_Textures.at(texHandle);

	auto& ss = tex.m_SamplerSettings;
	switch (coord)
	{
	case SHADER_TEXCOORD_S:
		ss.m_WrapS = wrapMode;
		break;
	case SHADER_TEXCOORD_T:
		ss.m_WrapT = wrapMode;
		break;
	case SHADER_TEXCOORD_U:
		ss.m_WrapU = wrapMode;
		break;

	default:
		Warning(TF2VULKAN_PREFIX "Invalid wrapMode %i\n", int(wrapMode));
	}
}

void ShaderAPI::FlushBufferedPrimitives()
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}

IMesh* ShaderAPI::GetDynamicMesh(IMaterial* material, int hwSkinBoneCount,
	bool buffered, IMesh* vertexOverride, IMesh* indexOverride)
{
	LOG_FUNC();
	return GetDynamicMeshEx(material, material->GetVertexFormat(), hwSkinBoneCount, buffered, vertexOverride, indexOverride);
	const VertexFormat fmt(material->GetVertexFormat());
}

IMesh* ShaderAPI::GetDynamicMeshEx(IMaterial* material, VertexFormat_t vertexFormat,
	int hwSkinBoneCount, bool buffered, IMesh* vertexOverride, IMesh* indexOverride)
{
	LOG_FUNC();

	const VertexFormat fmt(vertexFormat);

	if (auto found = m_DynamicMeshes.find(fmt); found != m_DynamicMeshes.end())
		return &found->second;

	return &m_DynamicMeshes.emplace(fmt, VulkanMesh(fmt)).first->second;
}

bool ShaderAPI::IsTranslucent(StateSnapshot_t id) const
{
	LOG_FUNC();
	return g_ShadowStateManager.IsTranslucent(id);
}

bool ShaderAPI::IsAlphaTested(StateSnapshot_t id) const
{
	LOG_FUNC();
	return g_ShadowStateManager.IsAlphaTested(id);
}

bool ShaderAPI::UsesVertexAndPixelShaders(StateSnapshot_t id) const
{
	LOG_FUNC();
	return g_ShadowStateManager.UsesVertexAndPixelShaders(id);
}

bool ShaderAPI::IsDepthWriteEnabled(StateSnapshot_t id) const
{
	LOG_FUNC();
	return g_ShadowStateManager.IsDepthWriteEnabled(id);
}

VertexFormat_t ShaderAPI::ComputeVertexFormat(int snapshotCount, StateSnapshot_t* ids) const
{
	LOG_FUNC();

	assert(snapshotCount > 0);
	assert(ids);
	if (snapshotCount <= 0 || !ids)
		return VERTEX_FORMAT_UNKNOWN;

	const auto& vtxFmt0 = g_ShadowStateManager.GetState(ids[0]).m_VSVertexFormat;
	VertexCompressionType_t compression = CompressionType(vtxFmt0);
	uint_fast8_t userDataSize = UserDataSize(vtxFmt0);
	uint_fast8_t boneCount = NumBoneWeights(vtxFmt0);
	VertexFormatFlags flags = VertexFormatFlags(VertexFlags(vtxFmt0));
	uint_fast8_t texCoordSize[VERTEX_MAX_TEXTURE_COORDINATES];
	for (size_t i = 0; i < std::size(texCoordSize); i++)
		texCoordSize[i] = TexCoordSize(i, vtxFmt0);

	for (int i = 1; i < snapshotCount; i++)
	{
		const auto& fmt = g_ShadowStateManager.GetState(*ids).m_VSVertexFormat;

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

	// Untested;
	assert(boneCount == 0);
	assert(userDataSize == 0);

	VertexFormat_t retVal =
		VertexFormat_t(flags) |
		VERTEX_BONEWEIGHT(boneCount) |
		VERTEX_USERDATA_SIZE(userDataSize);

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
	return PromoteToHardware(fmt, FormatUsage::ImmutableTexture, filteringRequired);
}

ImageFormat ShaderAPI::GetNearestRenderTargetFormat(ImageFormat fmt) const
{
	LOG_FUNC();
	return PromoteToHardware(fmt, FormatUsage::RenderTarget, true);
}

bool ShaderAPI::DoRenderTargetsNeedSeparateDepthBuffer() const
{
	LOG_FUNC();
	return false;
}

IShaderAPITexture& ShaderAPI::CreateTexture(std::string&& dbgName, const vk::ImageCreateInfo& imgCI)
{
	const auto handle = m_NextTextureHandle++;
	LOG_FUNC_TEX_NAME(handle, dbgName);

	vma::AllocationCreateInfo allocCreateInfo;
	allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	auto createdImg = g_ShaderDevice.GetVulkanAllocator().createImageUnique(imgCI, allocCreateInfo);

	dbgName = Util::string::concat("[", handle, "] ", std::move(dbgName));
	g_ShaderDevice.SetDebugName(createdImg.GetImage(), Util::string::concat("Texture: ", dbgName).c_str());

	return m_Textures.emplace(handle, ShaderTexture{ std::move(dbgName), handle, imgCI, std::move(createdImg) }).first->second;
}

ShaderAPITextureHandle_t ShaderAPI::CreateTexture(int width, int height, int depth,
	ImageFormat dstImgFormat, int mipLevelCount, int copyCount, int flags,
	const char* dbgName, const char* texGroupName)
{
	LOG_FUNC();
	ENSURE(width > 0);
	ENSURE(height > 0);
	ENSURE(depth > 0);
	ENSURE(mipLevelCount > 0);

	vk::ImageCreateInfo createInfo;

	if (height == 1 && depth == 1)
		createInfo.imageType = vk::ImageType::e1D;
	else if (depth == 1)
		createInfo.imageType = vk::ImageType::e2D;
	else
		createInfo.imageType = vk::ImageType::e3D;

	Util::SafeConvert(width, createInfo.extent.width);
	Util::SafeConvert(height, createInfo.extent.height);
	Util::SafeConvert(depth, createInfo.extent.depth);
	createInfo.arrayLayers = 1; // No support for texture arrays in stock valve materialsystem
	Util::SafeConvert(mipLevelCount, createInfo.mipLevels);
	createInfo.format = ConvertImageFormat(dstImgFormat);
	createInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment;

	return CreateTexture(dbgName, createInfo).GetHandle();
}

void ShaderAPI::CreateTextures(ShaderAPITextureHandle_t* handles, int count, int width, int height, int depth, ImageFormat dstImgFormat, int mipLevelCount, int copyCount, int flags, const char* dbgName, const char* texGroupName)
{
	LOG_FUNC();

	for (int i = 0; i < count; i++)
	{
		handles[i] = CreateTexture(width, height, depth, dstImgFormat,
			mipLevelCount, copyCount, flags, dbgName, texGroupName);
	}
}

const vk::ImageView& ShaderAPI::ShaderTexture::FindOrCreateView(const vk::ImageViewCreateInfo& viewCreateInfo)
{
	auto& imgViews = m_ImageViews;

	if (auto found = imgViews.find(viewCreateInfo); found != imgViews.end())
		return found->second.get();

	// Not found, create now
	const auto& result = (imgViews[viewCreateInfo] = g_ShaderDevice.GetVulkanDevice().createImageViewUnique(viewCreateInfo));
	g_ShaderDevice.SetDebugName(result, Util::string::concat("ImageView: ", m_DebugName).c_str());

	return result.get();
}

const IShaderAPITexture& ShaderAPI::GetTexture(ShaderAPITextureHandle_t texID) const
{
	if (texID == 0)
	{
		assert(m_Textures.find(texID) == m_Textures.end());
		return g_ShaderDevice.GetBackBufferColorTexture();
	}

	return m_Textures.at(size_t(texID));
}

void ShaderAPI::DeleteTexture(ShaderAPITextureHandle_t tex)
{
	LOG_FUNC_TEX(tex);

	auto& realTex = m_Textures.at(tex);

	// Update debug name
	{
		const auto dbgName = realTex.GetDebugName();

		char buf[128];
		sprintf_s(buf, "[DELETED] Texture: %.*s", PRINTF_SV(dbgName));
		g_ShaderDevice.SetDebugName(realTex.m_Image.GetImage(), buf);

		for (auto& iv : realTex.m_ImageViews)
		{
			sprintf_s(buf, "[DELETED] ImageView: %.*s", PRINTF_SV(dbgName));
			g_ShaderDevice.SetDebugName(iv.second, buf);
		}
	}

	m_Textures.erase(tex);
}

bool ShaderAPI::IsTexture(ShaderAPITextureHandle_t tex)
{
	LOG_FUNC_TEX(tex);

	bool found = m_Textures.find(tex) != m_Textures.end();
	assert(found);
	return found;
}

void ShaderAPI::TexImageFromVTF(ShaderAPITextureHandle_t texHandle, IVTFTexture* vtf, int frameIndex)
{
	LOG_FUNC_TEX(texHandle);

	const auto mipCount = vtf->MipCount();
	ENSURE(mipCount > 0);

	const auto faceCount = vtf->FaceCount();
	ENSURE(faceCount > 0);

	const auto arraySize = vtf->MipCount() * vtf->FaceCount();
	auto* texDatas = (TextureData*)stackalloc(arraySize * sizeof(TextureData));

	for (int mip = 0; mip < mipCount; mip++)
	{
		int width, height, depth;
		vtf->ComputeMipLevelDimensions(mip, &width, &height, &depth);

		const int mipSize = vtf->ComputeMipSize(mip);
		const int stride = vtf->RowSizeInBytes(mip);

		for (int face = 0; face < faceCount; face++)
		{
			TextureData& texData = texDatas[mip * faceCount + face];
			texData = {};
			texData.m_Format = vtf->Format();

			Util::SafeConvert(width, texData.m_Width);
			Util::SafeConvert(height, texData.m_Height);
			Util::SafeConvert(depth, texData.m_Depth);
			texData.m_Data = vtf->ImageData(frameIndex, face, mip);
			Util::SafeConvert(mipSize, texData.m_DataLength);
			Util::SafeConvert(stride, texData.m_Stride);

			Util::SafeConvert(mip, texData.m_MipLevel);
			texData.m_CubeFace = CubeMapFaceIndex_t(face);

			texData.Validate();
		}
	}

	UpdateTexture(texHandle, texDatas, arraySize);
}

static void TransitionImageLayout(const vk::Image& image, const vk::Format& format,
	const vk::ImageLayout& oldLayout, const vk::ImageLayout& newLayout,
	vk::CommandBuffer cmdBuf, uint32_t mipLevel)
{
	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.image = image;

	auto& srr = barrier.subresourceRange;
	srr.aspectMask = vk::ImageAspectFlagBits::eColor;
	srr.baseMipLevel = mipLevel;
	srr.levelCount = 1;
	srr.baseArrayLayer = 0;
	srr.layerCount = 1;

	vk::PipelineStageFlags srcStageMask;
	vk::PipelineStageFlags dstStageMask;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
		dstStageMask = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		srcStageMask = vk::PipelineStageFlagBits::eTransfer;
		dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
	{
		throw VulkanException("Unsupported layout transition", EXCEPTION_DATA());
	}

	cmdBuf.pipelineBarrier(
		srcStageMask,
		dstStageMask,
		vk::DependencyFlags{}, {}, {}, barrier);
}

static void CopyBufferToImage(const vk::Buffer& buf, const vk::Image& img, uint32_t width, uint32_t height,
	vk::CommandBuffer cmdBuf)
{
	vk::BufferImageCopy copy;

	copy.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	copy.imageSubresource.layerCount = 1;
	copy.imageExtent = { width, height, 1 };

	cmdBuf.copyBufferToImage(buf, img, vk::ImageLayout::eTransferDstOptimal, copy);
}

bool ShaderAPI::UpdateTexture(ShaderAPITextureHandle_t texHandle, const TextureData* data, size_t count)
{
	LOG_FUNC_TEX(texHandle);

	auto& tex = m_Textures.at(texHandle);

	auto& device = g_ShaderDevice.GetVulkanDevice();
	auto& alloc = g_ShaderDevice.GetVulkanAllocator();
	auto& queue = g_ShaderDevice.GetGraphicsQueue();
	//const auto memReqs = device.getImageMemoryRequirements(tex.m_Image.m_Image.get());

	// Prepare the staging buffer
	vma::AllocatedBuffer stagingBuf;
	{
		size_t totalSize = 0;
		for (size_t i = 0; i < count; i++)
		{
			totalSize += data[i].m_DataLength;
		}

		// Allocate staging buffer
		stagingBuf = Factories::BufferFactory{}
			.SetSize(totalSize)
			.SetUsage(vk::BufferUsageFlagBits::eTransferSrc)
			.SetMemoryRequiredFlags(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
			.SetDebugName(Util::string::concat(tex.m_DebugName, ": UpdateTexture() staging buffer"))
			.Create();

		// Copy the data into the staging buffer
		size_t currentOffset = 0;
		for (size_t i = 0; i < count; i++)
		{
			stagingBuf.GetAllocation().map().Write(data[i].m_Data, data[i].m_DataLength, currentOffset);
			currentOffset += data[i].m_DataLength;
		}
	}

	// Copy staging buffer into destination texture
	{
		auto cmdBuffer = queue.CreateCmdBufferAndBegin();

		TransitionImageLayout(tex.m_Image.GetImage(), tex.m_CreateInfo.format,
			vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, cmdBuffer.get(), 0);

		CopyBufferToImage(stagingBuf.GetBuffer(), tex.m_Image.GetImage(), tex.m_CreateInfo.extent.width,
			tex.m_CreateInfo.extent.height, cmdBuffer.get());

		TransitionImageLayout(tex.m_Image.GetImage(), tex.m_CreateInfo.format,
			vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, cmdBuffer.get(), 0);

		queue.EndAndSubmit(cmdBuffer.get());
	}

	return true;
}

void ShaderAPI::SetRenderTarget(ShaderAPITextureHandle_t colTexHandle, ShaderAPITextureHandle_t depthTexHandle)
{
	LOG_FUNC();
	return SetRenderTargetEx(0, colTexHandle, depthTexHandle);
}

void ShaderAPI::SetRenderTargetEx(int renderTargetID, ShaderAPITextureHandle_t colTex, ShaderAPITextureHandle_t depthTex)
{
	LOG_FUNC();
	return g_ShadowStateManager.SetRenderTargetEx(renderTargetID, colTex, depthTex);
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
	return true; // Why wouldn't we be able to?
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
		fmt |= g_ShadowStateManager.GetState(ids[i]).m_VSMorphFormat;

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

void ShaderAPI::SetStandardTextureHandle(StandardTextureId_t id, ShaderAPITextureHandle_t tex)
{
	LOG_FUNC();
	m_StdTextures.at(id) = tex;
}

void ShaderAPI::TexLodClamp(int something)
{
	LOG_FUNC();

	if (something != 0)
		NOT_IMPLEMENTED_FUNC();
}

void ShaderAPI::TexLodBias(float bias)
{
	LOG_FUNC();

	if (bias != 0)
		NOT_IMPLEMENTED_FUNC();
}

ShaderAPI::ShaderTexture::ShaderTexture(std::string&& debugName, ShaderAPITextureHandle_t handle,
	const vk::ImageCreateInfo& ci, vma::AllocatedImage&& img) :
	m_DebugName(std::move(debugName)), m_Handle(handle), m_CreateInfo(ci), m_Image(std::move(img))
{
}
