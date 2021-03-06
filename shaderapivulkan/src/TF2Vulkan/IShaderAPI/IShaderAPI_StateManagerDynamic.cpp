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
#include <TF2Vulkan/Util/std_iterator.h>

#include <materialsystem/IShader.h>
#include <materialsystem/imaterialvar.h>
#include <shaderapi/ishaderutil.h>

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

	assert(sampler >= SHADER_SAMPLER0);
	const ShaderAPITextureRange range
	{
		.m_Resources = &textureHandle,
		.m_Count = 1,
		.m_Offset = size_t(sampler - SHADER_SAMPLER0),
	};

	return BindTextureRanges(&range, &range + 1, true);
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

template<typename T, typename TOnBindFunc = void*>
static void BindRanges(const IShaderDynamicNext::ResourceRange<T>* begin, const IShaderDynamicNext::ResourceRange<T>* end,
	bool merge, Util::InPlaceVector<T, MAX_SHADER_RESOURCE_BINDINGS>& bindsVec, bool& dirty, const T& defaultVal,
	const TOnBindFunc& onBindFunc = nullptr)
{
	LOG_FUNC();

	uint32_t assignedCount = 0;
	uint32_t unassignedSlots = std::numeric_limits<uint32_t>::max();
	static_assert(MAX_SHADER_RESOURCE_BINDINGS <= sizeof(unassignedSlots) * CHAR_BIT);

	for (const auto& range : Util::iterator::IterRangeWrapper(begin, end))
	{
		for (size_t i = 0; i < range.m_Count; i++)
		{
			const uint32_t bindIndex = range.m_Offset + i;

			if (bindIndex >= assignedCount)
				assignedCount = bindIndex + 1;

			if (range.m_Resources)
			{
				if (bindsVec.size() <= bindIndex)
					bindsVec.resize(bindIndex + 1);

				if constexpr (!std::is_same_v<TOnBindFunc, void*>)
					onBindFunc(bindIndex, range.m_Resources[i]);

				Util::SetDirtyVar(bindsVec.at(bindIndex), range.m_Resources[i], dirty);
			}
			else
			{
				assert(unassignedSlots & (1 << bindIndex)); // Check for overlapping ranges
			}

			unassignedSlots &= ~(1 << bindIndex);
		}
	}

	if (!merge)
	{
		// Shrink to fit
		if (bindsVec.size() != assignedCount)
		{
			dirty = true;
			bindsVec.resize(assignedCount);
		}

		for (size_t i = 0; i < assignedCount; i++)
		{
			if (unassignedSlots & (1 << i))
				bindsVec[i] = defaultVal;
		}
	}
}

void IShaderAPI_StateManagerDynamic::BindSamplerRanges(const SamplerRange* begin, const SamplerRange* end, bool merge)
{
	LOG_FUNC();
	return BindRanges(begin, end, merge, m_State.m_BoundSamplers, m_Dirty, SamplerSettings{});
}

void IShaderAPI_StateManagerDynamic::BindTextureRanges(
	const ShaderAPITextureRange* begin, const ShaderAPITextureRange* end, bool merge)
{
	LOG_FUNC();
	return BindRanges(begin, end, merge, m_State.m_BoundTextures, m_Dirty, INVALID_SHADERAPI_TEXTURE_HANDLE,
		[&](uint32_t index, ShaderAPITextureHandle_t texID)
		{
			auto texDbgName = texID ? GetTexture(texID).GetDebugName() : "<NULL>";
			TF2VULKAN_PIX_MARKER("BindTexture%s %.*s @ g_Textures2D[%zu]",
				merge ? "(merge)" : "", PRINTF_SV(texDbgName), index);
		});
}

void IShaderAPI_StateManagerDynamic::BindStandardTexture(Sampler_t sampler, StandardTextureId_t id)
{
	LOG_FUNC();

	if (auto handle = GetStdTextureHandle(id); handle != INVALID_SHADERAPI_TEXTURE_HANDLE)
	{
		BindTexture(sampler, GetStdTextureHandle(id));
	}
	else
	{
		g_ShaderUtil->BindStandardTexture(sampler, id);
	}
}

void IShaderAPI_StateManagerDynamic::BindTextureRanges(
	const TextureRange* begin, const TextureRange* end, bool merge)
{
	LOG_FUNC();
	const size_t rangesCount = end - begin;

	auto outputRanges = (ShaderAPITextureRange*)_alloca(sizeof(ShaderAPITextureRange) * rangesCount);

	size_t texCount = 0;
	// Count the number of ITexture* pointers inside all of the ranges
	for (auto& inputRange : Util::iterator::IterRangeWrapper{ begin, end })
	{
		if (inputRange.m_Resources)
			texCount += inputRange.m_Count;
	}

	// Stack allocate a single buffer for all of the ShaderAPITextureHandle_t inside our output ranges
	auto texturePointers = (ShaderAPITextureHandle_t*)_alloca(sizeof(ShaderAPITextureHandle_t) * texCount);
	texCount = 0;

	// Create all of our output ranges
	for (size_t rangeIndex = 0; rangeIndex < rangesCount; rangeIndex++)
	{
		const auto& inputRange = begin[rangeIndex];
		auto& outputRange = outputRanges[rangeIndex];
		outputRange.m_Count = inputRange.m_Count;
		outputRange.m_Offset = inputRange.m_Offset;

		if (inputRange.m_Resources)
		{
			auto* outHandles = &texturePointers[texCount];
			outputRange.m_Resources = outHandles;
			for (size_t i = 0; i < inputRange.m_Count; i++)
			{
				auto texInternal = assert_cast<const ITextureInternal*>(inputRange.m_Resources[i]);
				outHandles[i] = texInternal->GetTextureHandle(0);
			}
		}
		else
		{
			outputRange.m_Resources = nullptr;
		}
	}

	return BindTextureRanges(outputRanges, outputRanges + rangesCount, merge);
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
