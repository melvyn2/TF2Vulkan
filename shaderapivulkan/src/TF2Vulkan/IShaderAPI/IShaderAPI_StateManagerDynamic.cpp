#include "stdafx.h"
#include "IShaderAPI_StateManagerDynamic.h"
#include "interface/IMaterialInternal.h"
#include "interface/ITextureInternal.h"
#include "interface/internal/IShaderInternal.h"
#include "interface/internal/IStateManagerStatic.h"
#include "TF2Vulkan/shaders/VulkanShaderManager.h"

#include <TF2Vulkan/Util/Color.h>
#include <TF2Vulkan/Util/DirtyVar.h>
#include <TF2Vulkan/Util/platform.h>

#include <materialsystem/IShader.h>
#include <materialsystem/imaterialvar.h>

#undef min
#undef max

using namespace TF2Vulkan;

void IShaderAPI_StateManagerDynamic::SetAnisotropicLevel(int anisoLevel)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_AnisotropicLevel, anisoLevel, m_Dirty);
}

MaterialFogMode_t IShaderAPI_StateManagerDynamic::GetCurrentFogType() const
{
	LOG_FUNC();
	return GetFogParams().m_Mode;
}

const LogicalFogParams& IShaderAPI_StateManagerDynamic::GetFogParams() const
{
	LOG_FUNC();
	return m_State.m_FogParams;
}

void IShaderAPI_StateManagerDynamic::SetLight(int light, const LightDesc_t& desc)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_Lights, light, desc, m_Dirty);
}

size_t IShaderAPI_StateManagerDynamic::GetLights(LightDesc_t* lights, size_t maxLights) const
{
	LOG_FUNC();
	assert(maxLights >= m_State.m_Lights.size());
	maxLights = std::max(maxLights, m_State.m_Lights.size());

	size_t actualLights = 0;
	for (size_t i = 0; i < maxLights; i++)
	{
		if (m_State.m_Lights[i].m_Type == MATERIAL_LIGHT_DISABLE)
			continue;

		lights[actualLights] = m_State.m_Lights[i];
		actualLights++;
	}

	return actualLights;
}

void IShaderAPI_StateManagerDynamic::SetLightingOrigin(Vector lightingOrigin)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_LightingOrigin, lightingOrigin, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetAmbientLight(float r, float g, float b)
{
	LOG_FUNC();

	const Vector4D val(r, g, b, 1);
	for (uint_fast8_t i = 0; i < m_State.m_LightAmbientCube.size(); i++)
		Util::SetDirtyVar(m_State.m_LightAmbientCube, i, val, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetAmbientLightCube(Vector4D cube[6])
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_LightAmbientCube,
		*reinterpret_cast<const std::array<Vector4D, 6>*>(cube), m_Dirty);
}

void IShaderAPI_StateManagerDynamic::GetAmbientLightCube(Shaders::AmbientLightCube& cube) const
{
	LOG_FUNC();

	cube.x[0].SetFrom(m_State.m_LightAmbientCube[0].Base());
	cube.x[1].SetFrom(m_State.m_LightAmbientCube[1].Base());
	cube.y[0].SetFrom(m_State.m_LightAmbientCube[2].Base());
	cube.y[1].SetFrom(m_State.m_LightAmbientCube[3].Base());
	cube.z[0].SetFrom(m_State.m_LightAmbientCube[4].Base());
	cube.z[1].SetFrom(m_State.m_LightAmbientCube[5].Base());
}

void IShaderAPI_StateManagerDynamic::CullMode(MaterialCullMode_t mode)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_CullMode, mode, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::OverrideDepthEnable(bool enable, bool depthEnable)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}

void IShaderAPI_StateManagerDynamic::ForceDepthFuncEquals(bool enable)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_ForceDepthFuncEquals, enable, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::EnableLinearColorSpaceFrameBuffer(bool enable)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_FBLinear, enable, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetFullScreenTextureHandle(ShaderAPITextureHandle_t tex)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_FullScreenTexture, tex, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetNumBoneWeights(int boneCount)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_BoneCount, boneCount, m_Dirty); // TODO: <# bone weights> == <bone count>?
}

void IShaderAPI_StateManagerDynamic::ClearColor4ub(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	LOG_FUNC();

	float fValues[4] =
	{
		r / 255.0f,
		g / 255.0f,
		b / 255.0f,
		a / 255.0f,
	};

	for (uint_fast8_t i = 0; i < 4; i++)
		Util::SetDirtyVar(m_State.m_ClearColor, i, fValues[i], m_Dirty);
}

void IShaderAPI_StateManagerDynamic::ClearColor3ub(uint8_t r, uint8_t g, uint8_t b)
{
	LOG_FUNC();
	return ClearColor4ub(r, g, b, 255);
}

void IShaderAPI_StateManagerDynamic::Bind(IMaterial* material)
{
	LOG_FUNC();

	TF2VULKAN_PIX_MARKER("Bind %s: %s", material->GetShaderName(), material->GetName());

	if constexpr (false)
	{
		const auto numParams = Util::SafeConvert<size_t>(assert_cast<IMaterialInternal*>(material)->GetShader()->GetNumParams());
		const auto params = material->GetShaderParams();

		char buf[32768];
		size_t base = sprintf_s(buf, "Bind %s: %s\n", material->GetShaderName(), material->GetName());
		for (size_t i = 0; i < numParams && base < std::size(buf); i++)
			base += sprintf_s(buf + base, std::size(buf) - base, "\t%s: %s\n", params[i]->GetName(), params[i]->GetStringValue());

		Plat_DebugString(buf);
	}

	Util::SetDirtyVar(m_State.m_BoundMaterial, material, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::BindTexture(Sampler_t sampler, ShaderAPITextureHandle_t textureHandle)
{
	LOG_FUNC();
	return BindTextures(&textureHandle, &textureHandle + 1, true, sampler - SHADER_SAMPLER0);
}

IShaderAPI_StateManagerDynamic::IShaderAPI_StateManagerDynamic()
{
	//for (auto& stack : m_MatrixStacks)
	//	stack.emplace().Identity();
}

void IShaderAPI_StateManagerDynamic::SetViewports(int count, const ShaderViewport_t* viewports)
{
	LOG_FUNC();

	m_State.m_Viewports.clear();
	for (int i = 0; i < count; i++)
		m_State.m_Viewports.push_back(viewports[i]);

	m_Dirty = true;
}

MaterialFogMode_t IShaderAPI_StateManagerDynamic::GetSceneFogMode()
{
	LOG_FUNC();
	return m_State.m_FogParams.m_Mode;
}

void IShaderAPI_StateManagerDynamic::GetFogDistances(float* start, float* end, float* fogZ)
{
	LOG_FUNC();

	auto& fog = m_State.m_FogParams;

	if (start)
		*start = fog.m_Start;

	if (end)
		*end = fog.m_End;

	if (fogZ)
		*fogZ = fog.m_Z;
}

void IShaderAPI_StateManagerDynamic::FogMaxDensity(float maxDensity)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_FogParams.m_MaxDensity, maxDensity, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::GetSceneFogColor(unsigned char* rgb)
{
	LOG_FUNC();
	if (rgb)
	{
		auto& fog = m_State.m_FogParams;
		rgb[0] = fog.m_ColorR;
		rgb[1] = fog.m_ColorG;
		rgb[2] = fog.m_ColorB;
	}
}

void IShaderAPI_StateManagerDynamic::SceneFogMode(MaterialFogMode_t mode)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_FogParams.m_Mode, mode, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::GetDX9LightState(LightState_t * state) const
{
	LOG_FUNC();

	if (state)
		*state = m_State.m_LightState;
}

void IShaderAPI_StateManagerDynamic::DisableAllLocalLights()
{
	LOG_FUNC();

	for (auto& light : m_State.m_Lights)
		Util::SetDirtyVar(light.m_Type, MATERIAL_LIGHT_DISABLE, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::GetWorldSpaceCameraPosition(float* pos) const
{
	assert(uintptr_t(pos) % alignof(Vector) == 0); // Alignment must match
#pragma warning(suppress : 4996)
	return GetWorldSpaceCameraPosition(*reinterpret_cast<Vector*>(pos));
}

void IShaderAPI_StateManagerDynamic::GetWorldSpaceCameraPosition(Vector& pos) const
{
	LOG_FUNC();
	pos[0] = m_State.m_WorldSpaceCameraPosition.x;
	pos[1] = m_State.m_WorldSpaceCameraPosition.y;
	pos[2] = m_State.m_WorldSpaceCameraPosition.z;
}

void IShaderAPI_StateManagerDynamic::SetToneMappingScaleLinear(const Vector & scale)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_TonemappingScale, scale, m_Dirty);
}

const Vector& IShaderAPI_StateManagerDynamic::GetToneMappingScaleLinear() const
{
	LOG_FUNC();
	return m_State.m_TonemappingScale;
}

float IShaderAPI_StateManagerDynamic::GetLightMapScaleFactor() const
{
	LOG_FUNC();
	return 1; // TODO
}

void IShaderAPI_StateManagerDynamic::EnableUserClipTransformOverride(bool enable)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_UserClipTransformOverrideEnabled, enable, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::UserClipTransform(const VMatrix& worldToView)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_UserClipTransformOverride, worldToView, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetSkinningMatrices()
{
	LOG_FUNC();
	// Just let shaders handle all uniform buffers (for now?)
}

void IShaderAPI_StateManagerDynamic::SetShaderInstance(ShaderType type, const IShaderInstance* instance)
{
	assert(instance->GetGroup().GetShaderType() == type);
	switch (type)
	{
	case ShaderType::Pixel:
		Util::SetDirtyVar(m_State.m_PSShader, assert_cast<const IShaderInstanceInternal*>(instance), m_Dirty);
		break;
	case ShaderType::Vertex:
		Util::SetDirtyVar(m_State.m_VSShader, assert_cast<const IShaderInstanceInternal*>(instance), m_Dirty);
		break;

	default:
		assert(!"Unknown shader type");
		break;
	}
}

void IShaderAPI_StateManagerDynamic::BindUniformBuffer(const BufferPoolEntry& buf, UniformBufferIndex index)
{
	if (index == UniformBufferIndex::Invalid)
		return; // Nothing to do

	Util::SetDirtyVar(m_State.m_UniformBuffers, index, buf, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::BindSamplers(const SamplerSettings* begin, const SamplerSettings* end, bool merge, uint32_t first)
{
	LOG_FUNC();
	const size_t count = end - begin;

	if (!merge)
	{
		if (m_State.m_BoundSamplers.size() != count)
		{
			m_Dirty = true;
			m_State.m_BoundSamplers.resize(count);
		}

		for (size_t i = 0; i < count; i++)
			Util::SetDirtyVar(m_State.m_BoundSamplers[i], begin[i], m_Dirty);
	}
	else
	{
		NOT_IMPLEMENTED_FUNC();
	}
}

void IShaderAPI_StateManagerDynamic::BindTextures(
	const ITexture* const* begin, const ITexture* const* end, bool merge, uint32_t first)
{
	LOG_FUNC();
	const size_t count = end - begin;

	auto handles = (ShaderAPITextureHandle_t*)_alloca(sizeof(ShaderAPITextureHandle_t) * count);

	auto test0 = &ITextureInternal::SetErrorTexture;
#pragma warning(suppress : 4996)
	auto test1 = static_cast<void(ITextureInternal::*)(Sampler_t)>(&ITextureInternal::Bind);
	auto test2 = &ITextureInternal::GetTextureHandle;

	for (size_t i = 0; i < count; i++)
	{
		auto texInternal = assert_cast<const ITextureInternal*>(begin[i]);
		handles[i] = texInternal->GetTextureHandle(0);
	}

	return BindTextures(handles, handles + count, merge, first);
}

void IShaderAPI_StateManagerDynamic::BindTextures(
	const ShaderAPITextureHandle_t* begin, const ShaderAPITextureHandle_t* end, bool merge, uint32_t first)
{
	LOG_FUNC();
	const size_t count = end - begin;

	if (!merge)
	{
		if (m_State.m_BoundTextures.size() != count)
		{
			m_Dirty = true;
			m_State.m_BoundTextures.resize(count);
		}

		for (size_t i = 0; i < count; i++)
		{
			auto texDbgName = begin[i] ? GetTexture(begin[i]).GetDebugName() : "<NULL>";
			TF2VULKAN_PIX_MARKER("BindTexture %.*s @ g_Textures2D[%zu]", PRINTF_SV(texDbgName), i);
			Util::SetDirtyVar(m_State.m_BoundTextures[i], begin[i], m_Dirty);
		}
	}
	else
	{
		NOT_IMPLEMENTED_FUNC();
	}
}

void IShaderAPI_StateManagerDynamic::SetOverbright(float overbright)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
	//Util::SetDirtyVar(m_State.m_SCOverbright, overbright, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetDefaultState()
{
	LOG_FUNC();
	//m_State = {};
	m_Dirty = true;
}

void IShaderAPI_StateManagerDynamic::LoadBoneMatrices(Shaders::VSModelMatrices& bones) const
{
	bones.m_Model[0] = GetMatrix(MATERIAL_MODEL).Transpose().As3x4();
	if (m_State.m_BoneCount > 1)
	{
		memcpy_s(bones.m_Model.data() + 1, sizeof(bones.m_Model) - sizeof(bones.m_Model[0]),
			m_State.m_BoneMatrices.m_Model.data() + 1, sizeof(bones.m_Model[0])* (m_State.m_BoneCount - 1));
	}
}

int IShaderAPI_StateManagerDynamic::GetCurrentNumBones() const
{
	LOG_FUNC();
	return m_State.m_BoneCount;
}

void IShaderAPI_StateManagerDynamic::MatrixMode(MaterialMatrixMode_t mode)
{
	LOG_FUNC();

	ENSURE(mode >= 0);
	ENSURE(mode < NUM_MATRIX_MODES);

	m_MatrixMode = mode;
}

void IShaderAPI_StateManagerDynamic::GetMatrix(MaterialMatrixMode_t mode, float* dst)
{
	LOG_FUNC();
	return GetMatrix(mode, *reinterpret_cast<VMatrix*>(dst));
}

void IShaderAPI_StateManagerDynamic::GetMatrix(MaterialMatrixMode_t mode, VMatrix & dst) const
{
	LOG_FUNC();
	auto& stack = m_MatrixStacks.at(mode);

	if (!stack.empty())
		dst = stack.top();
	else
		dst.Identity();
}

static const VMatrix MAT4_IDENTITY = []
{
	VMatrix temp;
	temp.Identity();
	return temp;
}();

const VMatrix& IShaderAPI_StateManagerDynamic::GetMatrix(MaterialMatrixMode_t matrixMode) const
{
	auto& stack = m_MatrixStacks.at(matrixMode);

	if (!stack.empty())
		return stack.top();
	else
		return MAT4_IDENTITY;
}

void IShaderAPI_StateManagerDynamic::PushMatrix()
{
	LOG_FUNC();

	auto& stack = m_MatrixStacks.at(m_MatrixMode);

	if (!stack.empty())
		stack.push(stack.top());
	else
		stack.emplace().Identity();
}

void IShaderAPI_StateManagerDynamic::PopMatrix()
{
	LOG_FUNC();

	auto& stack = m_MatrixStacks.at(m_MatrixMode);

	if (!stack.empty())
		stack.pop();
}

void IShaderAPI_StateManagerDynamic::LoadIdentity()
{
	LOG_FUNC();

	m_MatrixStacks.at(m_MatrixMode).top().Identity();
	m_Dirty = true;
}

void IShaderAPI_StateManagerDynamic::LoadMatrix(float* m)
{
	LOG_FUNC();
	return LoadMatrix(*reinterpret_cast<const VMatrix*>(m));
}

void IShaderAPI_StateManagerDynamic::LoadMatrix(const VMatrix & m)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_MatrixStacks.at(m_MatrixMode).top(), m, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::MultMatrix(float* m)
{
	LOG_FUNC();
	return MultMatrix(*reinterpret_cast<const VMatrix*>(m));
}

void IShaderAPI_StateManagerDynamic::MultMatrixLocal(float* m)
{
	LOG_FUNC();
	return MultMatrixLocal(*reinterpret_cast<const VMatrix*>(m));
}

bool IShaderAPI_StateManagerDynamic::InFlashlightMode() const
{
	LOG_FUNC();
	return m_State.m_InFlashlightMode;
}

void IShaderAPI_StateManagerDynamic::SetFloatRenderingParameter(RenderParamFloat_t param, float value)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_RenderParamsFloat, param, value, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetIntRenderingParameter(RenderParamInt_t param, int value)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_RenderParamsInt, param, value, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetVectorRenderingParameter(RenderParamVector_t param, const Vector & value)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_RenderParamsVector, param, value, m_Dirty);
}

float IShaderAPI_StateManagerDynamic::GetFloatRenderingParameter(RenderParamFloat_t param) const
{
	LOG_FUNC();
	return m_State.m_RenderParamsFloat.at(param);
}

int IShaderAPI_StateManagerDynamic::GetIntRenderingParameter(RenderParamInt_t param) const
{
	LOG_FUNC();
	return m_State.m_RenderParamsInt.at(param);
}

Vector IShaderAPI_StateManagerDynamic::GetVectorRenderingParameter(RenderParamVector_t param) const
{
	LOG_FUNC();
	return m_State.m_RenderParamsVector.at(param);
}

void IShaderAPI_StateManagerDynamic::SetStencilEnable(bool enabled)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_StencilEnable, enabled, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetStencilFailOperation(StencilOperation_t op)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_StencilFailOp, op, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetStencilZFailOperation(StencilOperation_t op)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_StencilDepthFailOp, op, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetStencilPassOperation(StencilOperation_t op)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_StencilPassOp, op, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetStencilCompareFunction(StencilComparisonFunction_t fn)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_StencilCompareFunc, fn, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetStencilReferenceValue(int ref)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_StencilRef, ref, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetStencilTestMask(uint32 mask)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_StencilTestMask, mask, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetStencilWriteMask(uint32 mask)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_StencilWriteMask, mask, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetScissorRect(const int left, const int top,
	const int right, const int bottom, const bool enableScissor)
{
	LOG_FUNC();

	Util::SetDirtyVar(m_State.m_ScissorEnable, enableScissor, m_Dirty);
	if (enableScissor)
	{
		Util::SetDirtyVar(m_State.m_ScissorX, left, m_Dirty);
		Util::SetDirtyVar(m_State.m_ScissorY, top, m_Dirty);
		Util::SetDirtyVar(m_State.m_ScissorWidth, right - left, m_Dirty);
		Util::SetDirtyVar(m_State.m_ScissorHeight, bottom - top, m_Dirty);
	}
}

void IShaderAPI_StateManagerDynamic::SetClipPlane(int index, const float* plane)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}

void IShaderAPI_StateManagerDynamic::EnableClipPlane(int index, bool enable)
{
	NOT_IMPLEMENTED_FUNC_NOBREAK();
}

void IShaderAPI_StateManagerDynamic::FogStart(float start)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_FogParams.m_Start, start, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::FogEnd(float end)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_FogParams.m_End, end, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SetFogZ(float fogZ)
{
	LOG_FUNC();
	Util::SetDirtyVar(m_State.m_FogParams.m_Z, fogZ, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::SceneFogColor3ub(unsigned char r, unsigned char g, unsigned char b)
{
	LOG_FUNC();
	auto& fog = m_State.m_FogParams;
	Util::SetDirtyVar(fog.m_ColorR, r, m_Dirty);
	Util::SetDirtyVar(fog.m_ColorG, g, m_Dirty);
	Util::SetDirtyVar(fog.m_ColorB, b, m_Dirty);
}

void IShaderAPI_StateManagerDynamic::LoadBoneMatrix(int boneIndex, const float* m)
{
	LOG_FUNC();
	return LoadBoneMatrix(Util::SafeConvert<uint32_t>(boneIndex), *reinterpret_cast<const matrix3x4_t*>(m));
}

void IShaderAPI_StateManagerDynamic::LoadBoneMatrix(uint32_t boneIndex, const matrix3x4_t& matrix)
{
	LOG_FUNC();

	if (boneIndex >= m_State.m_BoneCount)
	{
		Util::SafeConvert(boneIndex + 1, m_State.m_BoneCount);
		m_Dirty = true;
	}

	Util::SetDirtyVar(m_State.m_BoneMatrices[boneIndex], matrix, m_Dirty);

	if (boneIndex == 0)
	{
		MatrixMode(MATERIAL_MODEL);
		LoadMatrix(VMatrix(matrix).Transpose());
	}
}
