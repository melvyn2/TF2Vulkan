#pragma once

#include "TF2Vulkan/LogicalState.h"
#include "IShaderAPI_MeshManager.h"

#include <TF2Vulkan/IShaderDynamicNext.h>
#include <TF2Vulkan/Util/InPlaceVector.h>

#include <Color.h>
#include <shaderapi/ishaderapi.h>
#include <shaderapi/ishaderdynamic.h>

#include <stack>

namespace TF2Vulkan
{
	class IShaderAPI_StateManagerDynamic : public IShaderAPI_MeshManager, public IShaderDynamicNext
	{
		using BaseClass = IShaderAPI_TextureManager;

	public:
		IShaderAPI_StateManagerDynamic();

		void SetViewports(int count, const ShaderViewport_t* viewports) override final;
		int GetViewports(ShaderViewport_t* viewports, int maxViewports) const override final { NOT_IMPLEMENTED_FUNC(); }

		double CurrentTime() const override final { NOT_IMPLEMENTED_FUNC(); }

		void GetLightmapDimensions(int* w, int* h) override final { NOT_IMPLEMENTED_FUNC(); }

		void MatrixMode(MaterialMatrixMode_t matrixMode) override final;
		void PushMatrix() override final;
		void PopMatrix() override final;
		[[deprecated]] void LoadMatrix(float* m) override final;
		void LoadMatrix(const VMatrix& m);
		[[deprecated]] void MultMatrix(float* m) override final;
		void MultMatrix(const VMatrix& m) { NOT_IMPLEMENTED_FUNC(); }
		[[deprecated]] void MultMatrixLocal(float* m) override final;
		void MultMatrixLocal(const VMatrix& m) { NOT_IMPLEMENTED_FUNC(); }
		[[deprecated]] void GetMatrix(MaterialMatrixMode_t matrixMode, float* dst) override final;
		void GetMatrix(MaterialMatrixMode_t matrixMode, VMatrix& dst) const override final;
		const VMatrix& GetMatrix(MaterialMatrixMode_t matrixMode) const;
		void LoadIdentity() override final;
		void LoadCameraToWorld() override final { NOT_IMPLEMENTED_FUNC(); }
		void Ortho(double left, double right, double bottom, double top, double zNear, double zFar) override final { NOT_IMPLEMENTED_FUNC(); }
		void PerspectiveX(double fovx, double aspect, double zNear, double zFar) override final { NOT_IMPLEMENTED_FUNC(); }
		void PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) override final { NOT_IMPLEMENTED_FUNC(); }
		void PickMatrix(int x, int y, int width, int height) override final { NOT_IMPLEMENTED_FUNC(); }
		void Rotate(float angle, float x, float y, float z) override final { NOT_IMPLEMENTED_FUNC(); }
		void Translate(float x, float y, float z) override final { NOT_IMPLEMENTED_FUNC(); }
		void Scale(float x, float y, float z) override final { NOT_IMPLEMENTED_FUNC(); }
		void ScaleXY(float x, float y) override final { NOT_IMPLEMENTED_FUNC(); }

		void Color3f(float r, float g, float b) override final { NOT_IMPLEMENTED_FUNC(); }
		void Color3fv(float const* pColor) override final { NOT_IMPLEMENTED_FUNC(); }
		void Color4f(float r, float g, float b, float a) override final { NOT_IMPLEMENTED_FUNC(); }
		void Color4fv(float const* pColor) override final { NOT_IMPLEMENTED_FUNC(); }

		void Color3ub(unsigned char r, unsigned char g, unsigned char b) override final { NOT_IMPLEMENTED_FUNC(); }
		void Color3ubv(unsigned char const* pColor) override final { NOT_IMPLEMENTED_FUNC(); }
		void Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) override final { NOT_IMPLEMENTED_FUNC(); }
		void Color4ubv(unsigned char const* pColor) override final { NOT_IMPLEMENTED_FUNC(); }

		void ClearColor3ub(uint8_t r, uint8_t g, uint8_t b) override final;
		void ClearColor4ub(uint8_t r, uint8_t g, uint8_t b, uint8_t a) override final;

		void SetAnisotropicLevel(int anisoLevel) override final;
		void SetFullScreenTextureHandle(ShaderAPITextureHandle_t tex) override final;
		void SetDefaultState() override final;
		void LoadBoneMatrices(Shaders::VSModelMatrices& bones) const override final;
		int GetCurrentNumBones() const override final;
		void SetNumBoneWeights(int boneCount) override final;
		void Bind(IMaterial* material) override final;

		bool InFlashlightMode() const override final;
		void BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle) override final;

		void SetVertexShaderConstant(int var, const float* vec, int numConst, bool force = false) override final { NOT_IMPLEMENTED_FUNC(); }
		void SetBooleanVertexShaderConstant(int var, const BOOL* vec, int numBools, bool force = false) override final { NOT_IMPLEMENTED_FUNC(); }
		void SetIntegerVertexShaderConstant(int var, const int* vec, int numIntVecs, bool force = false) override final { NOT_IMPLEMENTED_FUNC(); }
		void SetPixelShaderConstant(int var, const float* vec, int numConst, bool force = false) override final { NOT_IMPLEMENTED_FUNC(); }
		void SetBooleanPixelShaderConstant(int var, const BOOL* vec, int numBools, bool force = false) override final { NOT_IMPLEMENTED_FUNC(); }
		void SetIntegerPixelShaderConstant(int var, const int* vec, int numIntVecs, bool force = false) override final { NOT_IMPLEMENTED_FUNC(); }

		void SetFloatRenderingParameter(RenderParamFloat_t param, float value) override final;
		void SetIntRenderingParameter(RenderParamInt_t param, int value) override final;
		void SetVectorRenderingParameter(RenderParamVector_t param, const Vector& value) override final;
		float GetFloatRenderingParameter(RenderParamFloat_t param) const override final;
		int GetIntRenderingParameter(RenderParamInt_t param) const override final;
		Vector GetVectorRenderingParameter(RenderParamVector_t param) const override final;

		void SetStencilEnable(bool enabled) override final;
		void SetStencilFailOperation(StencilOperation_t op) override final;
		void SetStencilZFailOperation(StencilOperation_t op) override final;
		void SetStencilPassOperation(StencilOperation_t op) override final;
		void SetStencilCompareFunction(StencilComparisonFunction_t fn) override final;
		void SetStencilReferenceValue(int ref) override final;
		void SetStencilTestMask(uint32 mask) override final;
		void SetStencilWriteMask(uint32 mask) override final;

		void SetScissorRect(const int left, const int top, const int right, const int bottom,
			const bool enableScissor) override final;

		void SetClipPlane(int index, const float* plane) override final;
		void EnableClipPlane(int index, bool enable) override final;

		void FogStart(float start) override final;
		void FogEnd(float end) override final;
		void SetFogZ(float fogZ) override final;
		void SceneFogColor3ub(unsigned char r, unsigned char g, unsigned char b) override final;
		void SceneFogMode(MaterialFogMode_t mode) override final;
		MaterialFogMode_t GetSceneFogMode() override final;
		void GetFogDistances(float* start, float* end, float* fogZ) override final;
		void FogMaxDensity(float maxDensity) override final;
		void GetSceneFogColor(unsigned char* rgb) override final;
		void SetPixelShaderFogParams(int reg) override final { NOT_IMPLEMENTED_FUNC(); }
		MaterialFogMode_t GetCurrentFogType() const override final;
		int GetPixelFogCombo() override final { NOT_IMPLEMENTED_FUNC_NOBREAK(); return 0; }
		const LogicalFogParams& GetFogParams() const override final;

		void SetLight(int light, const LightDesc_t& desc) override final;
		size_t GetLights(LightDesc_t* lights, size_t maxLights) const override final;
		void SetLightingOrigin(Vector lightingOrigin) override final;
		void SetAmbientLight(float r, float g, float b) override final;
		void SetAmbientLightCube(Vector4D cube[6]) override final;
		void GetAmbientLightCube(Shaders::AmbientLightCube& cube) const override final;
		void SetPixelShaderStateAmbientLightCube(int pshReg, bool forceToBlack) override final { NOT_IMPLEMENTED_FUNC(); }
		void CommitPixelShaderLighting(int pshReg) override final { NOT_IMPLEMENTED_FUNC(); }
		void GetDX9LightState(LightState_t* state) const override final;
		void DisableAllLocalLights() override final;

		void CullMode(MaterialCullMode_t mode) override final;

		[[deprecated]] void GetWorldSpaceCameraPosition(float* pos) const override final;
		[[deprecated]] void GetWorldSpaceCameraPosition(Vector& pos) const override final;

		void SetVertexShaderIndex(int index) override final { NOT_IMPLEMENTED_FUNC(); }
		void SetPixelShaderIndex(int index) override final { NOT_IMPLEMENTED_FUNC(); }

		void SetDepthFeatheringPixelShaderConstant(int constant, float depthBlendScale) override final { NOT_IMPLEMENTED_FUNC(); }

		void OverrideDepthEnable(bool enable, bool depthEnable) override final;
		void ForceDepthFuncEquals(bool enable) override final;

		void EnableLinearColorSpaceFrameBuffer(bool enable) override final;

		void SetToneMappingScaleLinear(const Vector& scale) override final;
		const Vector& GetToneMappingScaleLinear(void) const override final;
		float GetLightMapScaleFactor() const override final;

		void EnableUserClipTransformOverride(bool enable) override final;
		void UserClipTransform(const VMatrix& worldToView) override final;

		void SetSkinningMatrices() override final;

		// IShaderDynamicNext
		using ShaderAPITextureRange = ResourceRange<ShaderAPITextureHandle_t>;
		void SetShaderInstance(ShaderType type, const IShaderInstance* instance) override final;
		void BindUniformBuffer(const BufferPoolEntry& buf, UniformBufferIndex index) override final;
		void BindSamplerRanges(const SamplerRange* begin, const SamplerRange* end, bool merge) override final;
		void BindTextureRanges(const TextureRange* begin, const TextureRange* end, bool merge) override final;
		void BindTextureRanges(const ShaderAPITextureRange* begin, const ShaderAPITextureRange* end, bool merge);
		void BindStandardTexture(Sampler_t sampler, StandardTextureId_t id) override final;
		void BindStandardVertexTexture(VertexTextureSampler_t sampler, StandardTextureId_t id) override final { NOT_IMPLEMENTED_FUNC(); }

		// Helpers
		void SetOverbright(float overbright);

		void LoadBoneMatrix(uint32_t boneIndex, const matrix3x4_t& m);

		const LogicalDynamicState& GetDynamicState() const { return m_State; }

	private:
		[[deprecated]] void LoadBoneMatrix(int boneIndex, const float* m) override final;

		MaterialMatrixMode_t m_MatrixMode = {};
		using MatrixStack = std::stack<VMatrix, std::vector<VMatrix>>;
		std::array<MatrixStack, NUM_MATRIX_MODES> m_MatrixStacks;

		LogicalDynamicState m_State;
		bool m_Dirty = true;

		IBufferPool* m_VSMatricesBufPool = nullptr;
	};

	extern IShaderAPI_StateManagerDynamic& g_StateManagerDynamic;
}
