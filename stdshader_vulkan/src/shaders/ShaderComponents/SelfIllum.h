#pragma once

#include "BaseShaderComponent.h"
#include <shaderlib/cshader.h>

namespace TF2Vulkan{ namespace Shaders{ namespace Components
{
	struct SelfIllum
	{
		struct SpecConstBuf
		{
			bool32 SELFILLUM;
			bool32 SELFILLUMFRESNEL;
		};
		template<typename T> struct SpecConstLayout
		{
			SPEC_CONST_BUF_ENTRY(T, SELFILLUM);
			SPEC_CONST_BUF_ENTRY(T, SELFILLUMFRESNEL);
		};

		struct UniformBuf
		{
			float1 m_SelfIllumEnvMapMaskAlpha;
			float1 m_SelfIllumFresnel;
			float1 m_SelfIllumFresnelMin;
			float1 m_SelfIllumFresnelMax;
			float1 m_SelfIllumFresnelExp;
		};

		struct Params
		{
			NSHADER_PARAM(SELFILLUMTINT, SHADER_PARAM_TYPE_COLOR, "[1 1 1]", "Self-illumination tint");
			NSHADER_PARAM(SELFILLUM_ENVMAPMASK_ALPHA, SHADER_PARAM_TYPE_FLOAT, "0.0", "defines that self illum value comes from env map mask alpha");
			NSHADER_PARAM(SELFILLUMFRESNEL, SHADER_PARAM_TYPE_BOOL, "0", "Self illum fresnel");
			NSHADER_PARAM(SELFILLUMFRESNELMINMAXEXP, SHADER_PARAM_TYPE_VEC3, "[0 1 1]", "Self illum fresnel min, max, exp");
			NSHADER_PARAM(SELFILLUMMASK, SHADER_PARAM_TYPE_TEXTURE, "shadertest/BaseTexture", "If we bind a texture here, it overrides base alpha (if any) for self illum");

		private:
			template<typename... TGroups> friend class ShaderComponents;

			void InitParamGroup(IMaterialVar** params) const
			{
				if (!IS_FLAG_SET(MATERIAL_VAR_SELFILLUM))
					params[SELFILLUMFRESNEL]->SetBoolValue(false);
			}

			void PreDraw(IMaterialVar** params, UniformBuf* uniformBuf, SpecConstBuf* specConstBuf) const
			{
				if (specConstBuf->SELFILLUM = IS_FLAG_SET(MATERIAL_VAR_SELFILLUM))
				{
					uniformBuf->m_SelfIllumEnvMapMaskAlpha = params[SELFILLUM_ENVMAPMASK_ALPHA]->GetFloatValue();

					if (specConstBuf->SELFILLUMFRESNEL = params[SELFILLUMFRESNEL]->GetBoolValue())
					{
						float settings[3];
						params[SELFILLUMFRESNELMINMAXEXP]->GetVecValue(settings, std::size(settings));
						uniformBuf->m_SelfIllumFresnelMin = settings[0];
						uniformBuf->m_SelfIllumFresnelMax = settings[1];
						uniformBuf->m_SelfIllumFresnelExp = settings[2];
					}
				}
			}
		};
	};
} } }
