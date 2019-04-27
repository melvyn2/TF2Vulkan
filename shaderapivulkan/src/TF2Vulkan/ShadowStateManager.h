#pragma once

#include "GraphicsPipeline.h"
#include "VertexFormat.h"

#include <shaderapi/ishaderapi.h>
#include <shaderapi/ishadershadow.h>

#include <unordered_map>

namespace TF2Vulkan
{
	class IShaderShadowInternal : public IShaderShadow
	{
	public:
	};

	enum class ShadowStateID : size_t;
	static constexpr ShadowStateID INVALID_SHADOW_STATE_ID = ShadowStateID(-1);

	class ShadowStateManager : public IShaderShadowInternal
	{
	public:
		void SetDefaultState() override final;

		void DepthFunc(ShaderDepthFunc_t func) override final;
		void EnableDepthWrites(bool enable) override final;
		void EnableDepthTest(bool enable) override final;
		void EnablePolyOffset(PolygonOffsetMode_t mode);

		void EnableStencil(bool enable) override final;
		void StencilFunc(ShaderStencilFunc_t func) override final;
		void StencilPassOp(ShaderStencilOp_t op) override final;
		void StencilFailOp(ShaderStencilOp_t op) override final;
		void StencilDepthFailOp(ShaderStencilOp_t op) override final;
		void StencilReference(int reference) override final;
		void StencilMask(int mask) override final;
		void StencilWriteMask(int mask) override final;

		void EnableColorWrites(bool enable) override final;
		void EnableAlphaWrites(bool enable) override final;

		void EnableBlending(bool enable) override final;
		void BlendFunc(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor) override final;

		void EnableAlphaTest(bool enable) override final;
		void AlphaFunc(ShaderAlphaFunc_t alphaFunc, float alphaRef) override final;

		void PolyMode(ShaderPolyModeFace_t face, ShaderPolyMode_t mode) override final;

		void EnableCulling(bool enable) override final;

		void EnableConstantColor(bool enable) override final;

		void VertexShaderVertexFormat(uint flags, int texCoordCount,
			int* texCoorDimensions, int userDataSize) override final;

		void SetVertexShader(const char* filename, int staticIndex) override final;
		void SetPixelShader(const char* filename, int staticIndex) override final;

		void EnableLighting(bool enable) override final;

		void EnableSpecular(bool enable) override final;

		void EnableSRGBWrite(bool enable) override final;

		void EnableSRGBRead(Sampler_t sampler, bool enable) override final;

		void EnableVertexBlend(bool enable) override final;

		void OverbrightValue(TextureStage_t stage, float value) override final;
		void EnableTexture(Sampler_t sampler, bool enable) override final;
		void EnableTexGen(TextureStage_t stage, bool enable) override final;
		void TexGen(TextureStage_t stage, ShaderTexGenParam_t param) override final;

		void EnableCustomPixelPipe(bool enable) override final;
		void CustomTextureStages(int stageCount) override final;
		void CustomTextureOperation(TextureStage_t stage, ShaderTexChannel_t channel,
			ShaderTexOp_t op, ShaderTexArg_t arg1, ShaderTexArg_t arg2) override final;

		void DrawFlags(uint drawFlags) override final;

		void EnableAlphaPipe(bool enable) override final;
		void EnableConstantAlpha(bool enable) override final;
		void EnableVertexAlpha(bool enable) override final;
		void EnableTextureAlpha(TextureStage_t stage, bool enable) override final;

		void EnableBlendingSeparateAlpha(bool enable) override final;
		void BlendFuncSeparateAlpha(ShaderBlendFactor_t srcFactor, ShaderBlendFactor_t dstFactor) override final;
		void FogMode(ShaderFogMode_t fogMode) override final;

		void SetDiffuseMaterialSource(ShaderMaterialSource_t materialSource) override final;

		void SetMorphFormat(MorphFormat_t format) override final;

		void DisableFogGammaCorrection(bool disable) override final;

		void EnableAlphaToCoverage(bool enable) override final;

		void SetShadowDepthFiltering(Sampler_t stage) override final;

		void BlendOp(ShaderBlendOp_t op) override final;
		void BlendOpSeparateAlpha(ShaderBlendOp_t op) override final;

		ShadowStateID TakeSnapshot();
		bool IsTranslucent(StateSnapshot_t id) const;
		bool IsAlphaTested(StateSnapshot_t id) const;
		bool UsesVertexAndPixelShaders(StateSnapshot_t id) const;
		bool IsDepthWriteEnabled(StateSnapshot_t id) const;

		struct FogParams final
		{
			constexpr FogParams() = default;
			DEFAULT_STRONG_EQUALITY_OPERATOR(FogParams);

			ShaderFogMode_t m_Mode = SHADER_FOGMODE_DISABLED;
		};

		struct ShadowState : PipelineSettings
		{
			constexpr ShadowState() = default;
			DEFAULT_STRONG_EQUALITY_OPERATOR(ShadowState);

			FogParams m_Fog;
		};

		const ShadowState& GetState(StateSnapshot_t id) const;

	protected:
		bool HasStateChanged() const;

		struct ShadowStateHasher final
		{
			size_t operator()(const FogParams& s) const;
			size_t operator()(const ShadowState& s) const;
		};

	private:
		bool m_PipelineDirty = true;
		bool m_FogDirty = true;
		ShadowState m_Settings;

		std::unordered_map<ShadowState, ShadowStateID, ShadowStateHasher> m_StatesToIDs;
		std::vector<const ShadowState*> m_IDsToStates;
	};

	extern ShadowStateManager& g_ShadowStateManager;
}