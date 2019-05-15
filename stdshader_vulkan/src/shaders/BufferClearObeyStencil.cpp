#include "BaseShaderNext.h"
#include "ParamGroups.h"

#include <TF2Vulkan/IShaderNextInstanceMgr.h>
#include <TF2Vulkan/Util/Macros.h>

using namespace TF2Vulkan;
using namespace TF2Vulkan::Shaders;

inline namespace BufferClearObeyStencil
{
	struct Params
	{
		NSHADER_PARAM(CLEARCOLOR, SHADER_PARAM_TYPE_INTEGER, "1", "activates clearing of color");
		NSHADER_PARAM(CLEARALPHA, SHADER_PARAM_TYPE_INTEGER, "-1", "activates clearing of alpha. -1 == copy CLEARCOLOR setting");
		NSHADER_PARAM(CLEARDEPTH, SHADER_PARAM_TYPE_INTEGER, "1", "activates clearing of depth");
	};

	class Shader final : public ShaderNext<Shader, Params, SHADER_NOT_EDITABLE>
	{
	public:
		const char* GetName() const override { LOG_FUNC(); return "BufferClearObeyStencil"; }

		// Inherited via ShaderNext
		void OnInitShaderInstance(IMaterialVar** ppParams, IShaderInit* params,
			const char* pMaterialName) override;
		void OnDrawElements(const OnDrawElementsParams& params) override;

		void InitShader(IShaderNextInstanceMgr& instanceMgr) override;

	private:
		IShaderGroup* m_BufferClearObeyStencilVS = nullptr;
		IShaderGroup* m_BufferClearObeyStencilPS = nullptr;
	};

	struct SpecConstBuffer
	{
		bool32 USESCOLOR;
	};
	struct SpecConstLayoutInfo
	{
		SPEC_CONST_BUF_ENTRY(SpecConstBuffer, USESCOLOR);
	};

	struct Context : CBasePerMaterialContextData
	{
		SpecConstBuffer m_BaseBuffer;
	};
}

static const Shader::InstanceRegister s_Shader;

void Shader::InitShader(IShaderNextInstanceMgr& instanceMgr)
{
	m_BufferClearObeyStencilVS = &instanceMgr.FindOrCreateShaderGroup(
		ShaderType::Vertex, "bufferclearobeystencil_vs");
	m_BufferClearObeyStencilPS = &instanceMgr.FindOrCreateShaderGroup(
		ShaderType::Pixel, "bufferclearobeystencil_ps");
}

void Shader::OnInitShaderInstance(IMaterialVar** params,
	IShaderInit* pShaderInit, const char* pMaterialName)
{
	LOG_FUNC();

	InitIntParam(CLEARALPHA, params, -1);
}

void Shader::OnDrawElements(const OnDrawElementsParams& params)
{
	LOG_FUNC();

	auto& shadow = params.shadow;
	auto& dynamic = params.dynamic;

	const bool bEnableColorWrites = params[CLEARCOLOR]->GetBoolValue();
	const bool bEnableAlphaWrites = (params[CLEARALPHA]->GetIntValue() >= 0) ? params[CLEARALPHA]->GetBoolValue() : bEnableColorWrites;

	if (shadow)
	{
		shadow->DepthFunc(SHADER_DEPTHFUNC_ALWAYS);
		shadow->EnableDepthWrites(params[CLEARDEPTH]->GetBoolValue());
		shadow->EnableColorWrites(bEnableColorWrites);
		shadow->EnableAlphaWrites(bEnableAlphaWrites);

		shadow->VertexShaderVertexFormat(VERTEX_POSITION | VERTEX_COLOR, 1);

		shadow->SetVertexShader(m_BufferClearObeyStencilVS);
		shadow->SetPixelShader(m_BufferClearObeyStencilPS);

		/*shadow->SetPixelShader("bufferclearobeystencil_ps",
			{
				{ "USESCOLOR", bEnableColorWrites || bEnableAlphaWrites },
			});*/
	}

	if (dynamic)
	{
		NOT_IMPLEMENTED_FUNC();
		//dynamic->SetPixelShaderIndex
	}

	Draw();
}
