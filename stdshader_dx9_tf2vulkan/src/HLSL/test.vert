#      X           
 	 SPV_GOOGLE_hlsl_functionality1       GLSL.std.450                      main                      	   
                                                        vertexlit_and_unlit_generic.vert.hlsl    �   X     #line 1 "vertexlit_and_unlit_generic.vert.hlsl"
#line 1 "./common_vs_fxc.hlsli"



#line 1 "./common_fxc.hlsli"



#line 1 "./../../include/stdshader_dx9_tf2vulkan/ShaderShared.h"
#line 13 "./../../include/stdshader_dx9_tf2vulkan/ShaderShared.h"
 static const int _LOCATION_ARRAY_SIZE = 8;

 static const int LOCATIONTYPE_UNIQUE = 0;
 static const int LOCATIONTYPE_ARRAY = 1;
#line 35 "./../../include/stdshader_dx9_tf2vulkan/ShaderShared.h"
 static const int LOCATION_TYPEINDEX_POSITION = 0; static const int LOCATION_POSITION_TYPE = LOCATIONTYPE_ARRAY; static const int LOCATION_POSITION0 = (LOCATION_TYPEINDEX_POSITION) * _LOCATION_ARRAY_SIZE; static const int LOCATION_POSITION1 = LOCATION_POSITION0 + 1; static const int LOCATION_POSITION2 = LOCATION_POSITION0 + 2; static const int LOCATION_POSITION3 = LOCATION_POSITION0 + 3; static const int LOCATION_POSITION4 = LOCATION_POSITION0 + 4; static const int LOCATION_POSITION5 = LOCATION_POSITION0 + 5; static const int LOCATION_POSITION6 = LOCATION_POSITION0 + 6; static const int LOCATION_POSITION7 = LOCATION_POSITION0 + 7;;
 static const int LOCATION_TYPEINDEX_TEXCOORD = 1; static const int LOCATION_TEXCOORD_TYPE = LOCATIONTYPE_ARRAY; static const int LOCATION_TEXCOORD0 = (LOCATION_TYPEINDEX_TEXCOORD) * _LOCATION_ARRAY_SIZE; static const int LOCATION_TEXCOORD1 = LOCATION_TEXCOORD0 + 1; static const int LOCATION_TEXCOORD2 = LOCATION_TEXCOORD0 + 2; static const int LOCATION_TEXCOORD3 = LOCATION_TEXCOORD0 + 3; static const int LOCATION_TEXCOORD4 = LOCATION_TEXCOORD0 + 4; static const int LOCATION_TEXCOORD5 = LOCATION_TEXCOORD0 + 5; static const int LOCATION_TEXCOORD6 = LOCATION_TEXCOORD0 + 6; static const int LOCATION_TEXCOORD7 = LOCATION_TEXCOORD0 + 7;;
 static const int LOCATION_TYPEINDEX_COLOR = 2; static const int LOCATION_COLOR_TYPE = LOCATIONTYPE_ARRAY; static const int LOCATION_COLOR0 = (LOCATION_TYPEINDEX_COLOR) * _LOCATION_ARRAY_SIZE; static const int LOCATION_COLOR1 = LOCATION_COLOR0 + 1; static const int LOCATION_COLOR2 = LOCATION_COLOR0 + 2; static const int LOCATION_COLOR3 = LOCATION_COLOR0 + 3; static const int LOCATION_COLOR4 = LOCATION_COLOR0 + 4; static const int LOCATION_COLOR5 = LOCATION_COLOR0 + 5; static const int LOCATION_COLOR6 = LOCATION_COLOR0 + 6; static const int LOCATION_COLOR7 = LOCATION_COLOR0 + 7;;
 static const int LOCATION_TYPEINDEX_NORMAL = 3; static const int LOCATION_NORMAL_TYPE = LOCATIONTYPE_ARRAY; static const int LOCATION_NORMAL0 = (LOCATION_TYPEINDEX_NORMAL) * _LOCATION_ARRAY_SIZE; static const int LOCATION_NORMAL1 = LOCATION_NORMAL0 + 1; static const int LOCATION_NORMAL2 = LOCATION_NORMAL0 + 2; static const int LOCATION_NORMAL3 = LOCATION_NORMAL0 + 3; static const int LOCATION_NORMAL4 = LOCATION_NORMAL0 + 4; static const int LOCATION_NORMAL5 = LOCATION_NORMAL0 + 5; static const int LOCATION_NORMAL6 = LOCATION_NORMAL0 + 6; static const int LOCATION_NORMAL7 = LOCATION_NORMAL0 + 7;;

 static const int LOCATION_TYPEINDEX_FOG = 4; static const int LOCATION_FOG_TYPE = LOCATIONTYPE_UNIQUE; static const int LOCATION_FOG = (LOCATION_TYPEINDEX_FOG) * _LOCATION_ARRAY_SIZE;;
 static const int LOCATION_TYPEINDEX_BLENDWEIGHT = 5; static const int LOCATION_BLENDWEIGHT_TYPE = LOCATIONTYPE_UNIQUE; static const int LOCATION_BLENDWEIGHT = (LOCATION_TYPEINDEX_BLENDWEIGHT) * _LOCATION_ARRAY_SIZE;;
 static const int LOCATION_TYPEINDEX_BLENDINDICES = 6; static const int LOCATION_BLENDINDICES_TYPE = LOCATIONTYPE_UNIQUE; static const int LOCATION_BLENDINDICES = (LOCATION_TYPEINDEX_BLENDINDICES) * _LOCATION_ARRAY_SIZE;;

 static const int _LOCATION_TYPE_COUNT = 7;
 static const int _LOCATION_COUNT = _LOCATION_TYPE_COUNT * _LOCATION_ARRAY_SIZE;
#line 5 "./common_fxc.hlsli"




[[vk::constant_id(1)]] const bool VERTEXCOLOR = false;
[[vk::constant_id(2)]] const bool CUBEMAP = false;
[[vk::constant_id(3)]] const bool HALFLAMBERT = false;
[[vk::constant_id(4)]] const bool FLASHLIGHT = false;
[[vk::constant_id(5)]] const bool SEAMLESS_BASE = false;
[[vk::constant_id(6)]] const bool SEAMLESS_DETAIL = false;
[[vk::constant_id(7)]] const bool SEPARATE_DETAIL_UVS = false;
[[vk::constant_id(8)]] const bool DECAL = false;
[[vk::constant_id(9)]] const bool DONT_GAMMA_CONVERT_VERTEX_COLOR = false;

[[vk::constant_id(10)]] const bool DYNAMIC_LIGHT = false;
[[vk::constant_id(11)]] const bool STATIC_LIGHT_VERTEX = false;
[[vk::constant_id(12)]] const bool STATIC_LIGHT_LIGHTMAP = false;
[[vk::constant_id(13)]] const int DOWATERFOG = 0;
[[vk::constant_id(14)]] const bool SKINNING = false;
[[vk::constant_id(15)]] const bool LIGHTING_PREVIEW = false;
[[vk::constant_id(16)]] const bool MORPHING = false;






float3 mul3x3(float3 v, float3x3 m)
{
 return mul(v, m);
}
float3 mul4x3(float4 v, float4x3 m)
{
 return mul(v, m);
}

float3 LinearToGamma(const float3 f3linear)
{
 return pow(f3linear, 1.0f / 2.2f);
}

float4 LinearToGamma(const float4 f4linear)
{
 return float4(pow(f4linear.xyz, 1.0f / 2.2f), f4linear.w);
}

float LinearToGamma(const float f1linear)
{
 return pow(f1linear, 1.0f / 2.2f);
}

float3 GammaToLinear(const float3 gamma)
{
 return pow(gamma, 2.2f);
}

float4 GammaToLinear(const float4 gamma)
{
 return float4(pow(gamma.xyz, 2.2f), gamma.w);
}
#line 4 "./common_vs_fxc.hlsli"
#line 20 "./common_vs_fxc.hlsli"
struct LightInfo
{
 float4 color;
 float4 dir;
 float4 pos;
 float4 spotParams;
 float4 atten;
};

cbuffer VertexShaderStandardConstants
{
 float4 cConstants1;
 bool4 g_bLightEnabled;
 int g_nLightCountRegister;
 float4 cEyePosWaterZ;
 float4 cFlexScale;

 float4x4 cModelViewProj;
 float4x4 cViewProj;

 float4 cModelViewProjZ;
 float4 cViewProjZ;

 float4 cFogParams;

 float4x3 cModel[53];
 LightInfo cLightInfo[4];

 float3 cAmbientCubeX[2];
 float3 cAmbientCubeY[2];
 float3 cAmbientCubeZ[2];
};
#line 67 "./common_vs_fxc.hlsli"
void _DecompressShort2Tangent(float2 inputTangent, out float4 outputTangent)
{
 float2 ztSigns = sign(inputTangent);
 float2 xyAbs = abs(inputTangent);
 outputTangent.xy = (xyAbs - 16384.0f) / 16384.0f;
 outputTangent.z = ztSigns.x * sqrt(saturate(1.0f - dot(outputTangent.xy, outputTangent.xy)));
 outputTangent.w = ztSigns.y;
}



void _DecompressShort2Normal(float2 inputNormal, out float3 outputNormal)
{
 float4 result;
 _DecompressShort2Tangent(inputNormal, result);
 outputNormal = result.xyz;
}





void _DecompressUByte4Normal(float4 inputNormal,
 out float3 outputNormal)
{
 float fOne = 1.0f;

 float2 ztSigns = (inputNormal.xy - 128.0f) < 0;
 float2 xyAbs = abs(inputNormal.xy - 128.0f) - ztSigns;
 float2 xySigns = (xyAbs - 64.0f) < 0;
 outputNormal.xy = (abs(xyAbs - 64.0f) - xySigns) / 63.0f;

 outputNormal.z = 1.0f - outputNormal.x - outputNormal.y;
 outputNormal.xyz = normalize(outputNormal.xyz);

 outputNormal.xy *= lerp(fOne.xx, -fOne.xx, xySigns);
 outputNormal.z *= lerp(fOne.x, -fOne.x, ztSigns.x);
}

void DecompressVertex_Normal(float4 inputNormal, out float3 outputNormal)
{
 if (1 == 1)
 {
  if (1 == 0)
  {
   _DecompressShort2Normal(inputNormal.xy, outputNormal);
  }
  else
  {
   _DecompressUByte4Normal(inputNormal, outputNormal);
  }
 }
 else
 {
  outputNormal = inputNormal.xyz;
 }
}

bool ApplyMorph(float3 vPosFlex, inout float3 vPosition)
{

 float3 vPosDelta = vPosFlex.xyz * cFlexScale.x;
 vPosition.xyz += vPosDelta;
 return true;
}

bool ApplyMorph(float3 vPosFlex, float3 vNormalFlex, inout float3 vPosition, inout float3 vNormal)
{

 float3 vPosDelta = vPosFlex.xyz * cFlexScale.x;
 float3 vNormalDelta = vNormalFlex.xyz * cFlexScale.x;
 vPosition.xyz += vPosDelta;
 vNormal += vNormalDelta;
 return true;
}

bool ApplyMorph(float3 vPosFlex, float3 vNormalFlex,
 inout float3 vPosition, inout float3 vNormal, inout float3 vTangent)
{

 float3 vPosDelta = vPosFlex.xyz * cFlexScale.x;
 float3 vNormalDelta = vNormalFlex.xyz * cFlexScale.x;
 vPosition.xyz += vPosDelta;
 vNormal += vNormalDelta;
 vTangent.xyz += vNormalDelta;
 return true;
}

bool ApplyMorph(float4 vPosFlex, float3 vNormalFlex,
 inout float3 vPosition, inout float3 vNormal, inout float3 vTangent, out float flWrinkle)
{

 float3 vPosDelta = vPosFlex.xyz * cFlexScale.x;
 float3 vNormalDelta = vNormalFlex.xyz * cFlexScale.x;
 flWrinkle = vPosFlex.w * cFlexScale.y;
 vPosition.xyz += vPosDelta;
 vNormal += vNormalDelta;
 vTangent.xyz += vNormalDelta;
 return true;
}

float4 SampleMorphDelta(Texture2D vt, SamplerState vtSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect, const float flVertexID, const float flField)
{
 float flColumn = floor(flVertexID / vMorphSubrect.w);

 float4 t;
 t.x = vMorphSubrect.x + vMorphTargetTextureDim.z * flColumn + flField + 0.5f;
 t.y = vMorphSubrect.y + flVertexID - flColumn * vMorphSubrect.w + 0.5f;
 t.xy /= vMorphTargetTextureDim.xy;
 t.z = t.w = 0.f;

 return vt.SampleLevel(vtSampler, t.xy, t.w);
}


void SampleMorphDelta2(Texture2D vt, SamplerState vtSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect, const float flVertexID, out float4 delta1, out float4 delta2)
{
 float flColumn = floor(flVertexID / vMorphSubrect.w);

 float4 t;
 t.x = vMorphSubrect.x + vMorphTargetTextureDim.z * flColumn + 0.5f;
 t.y = vMorphSubrect.y + flVertexID - flColumn * vMorphSubrect.w + 0.5f;
 t.xy /= vMorphTargetTextureDim.xy;
 t.z = t.w = 0.f;

 delta1 = vt.SampleLevel(vtSampler, t.xy, t.w);
 t.x += 1.0f / vMorphTargetTextureDim.x;
 delta2 = vt.SampleLevel(vtSampler, t.xy, t.w);
}

bool ApplyMorph(Texture2D morphTexture, SamplerState morphSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect,
 const float flVertexID, const float3 vMorphTexCoord,
 inout float3 vPosition)
{
 if (MORPHING)
 {
  if (!DECAL)
  {

   float4 vPosDelta = SampleMorphDelta(morphTexture, morphSampler, vMorphTargetTextureDim, vMorphSubrect, flVertexID, 0);
   vPosition += vPosDelta.xyz;
  }
  else
  {
   float4 t = float4(vMorphTexCoord.x, vMorphTexCoord.y, 0.0f, 0.0f);
   float3 vPosDelta = morphTexture.SampleLevel(morphSampler, t.xy, t.w);
   vPosition += vPosDelta.xyz * vMorphTexCoord.z;
  }

  return true;
 }
 else
 {
  return false;
 }
}

bool ApplyMorph(Texture2D morphTexture, SamplerState morphSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect,
 const float flVertexID, const float3 vMorphTexCoord,
 inout float3 vPosition, inout float3 vNormal)
{
 if (MORPHING)
 {
  if (!DECAL)
  {
   float4 vPosDelta, vNormalDelta;
   SampleMorphDelta2(morphTexture, morphSampler, vMorphTargetTextureDim, vMorphSubrect, flVertexID, vPosDelta, vNormalDelta);
   vPosition += vPosDelta.xyz;
   vNormal += vNormalDelta.xyz;
  }
  else
  {
   float4 t = float4(vMorphTexCoord.x, vMorphTexCoord.y, 0.0f, 0.0f);
   float3 vPosDelta = morphTexture.SampleLevel(morphSampler, t.xy, t.w);
   t.x += 1.0f / vMorphTargetTextureDim.x;
   float3 vNormalDelta = morphTexture.SampleLevel(morphSampler, t.xy, t.w);
   vPosition += vPosDelta.xyz * vMorphTexCoord.z;
   vNormal += vNormalDelta.xyz * vMorphTexCoord.z;
  }

  return true;
 }
 else
 {
  return false;
 }
}

bool ApplyMorph(Texture2D morphTexture, SamplerState morphSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect,
 const float flVertexID, const float3 vMorphTexCoord,
 inout float3 vPosition, inout float3 vNormal, inout float3 vTangent)
{
 if (MORPHING)
 {
  if (!DECAL)
  {
   float4 vPosDelta, vNormalDelta;
   SampleMorphDelta2(morphTexture, morphSampler, vMorphTargetTextureDim, vMorphSubrect, flVertexID, vPosDelta, vNormalDelta);
   vPosition += vPosDelta.xyz;
   vNormal += vNormalDelta.xyz;
   vTangent += vNormalDelta.xyz;
  }
  else
  {
   float4 t = float4(vMorphTexCoord.x, vMorphTexCoord.y, 0.0f, 0.0f);
   float3 vPosDelta = morphTexture.SampleLevel(morphSampler, t.xy, t.w);
   t.x += 1.0f / vMorphTargetTextureDim.x;
   float3 vNormalDelta = morphTexture.SampleLevel(morphSampler, t.xy, t.w);
   vPosition += vPosDelta.xyz * vMorphTexCoord.z;
   vNormal += vNormalDelta.xyz * vMorphTexCoord.z;
   vTangent += vNormalDelta.xyz * vMorphTexCoord.z;
  }

  return true;
 }
 else
 {
  return false;
 }
}

bool ApplyMorph(Texture2D morphTexture, SamplerState morphSampler, const float3 vMorphTargetTextureDim, const float4 vMorphSubrect,
 const float flVertexID, const float3 vMorphTexCoord,
 inout float3 vPosition, inout float3 vNormal, inout float3 vTangent, out float flWrinkle)
{
 if (MORPHING)
 {
  if (!DECAL)
  {
   float4 vPosDelta, vNormalDelta;
   SampleMorphDelta2(morphTexture, morphSampler, vMorphTargetTextureDim, vMorphSubrect, flVertexID, vPosDelta, vNormalDelta);
   vPosition += vPosDelta.xyz;
   vNormal += vNormalDelta.xyz;
   vTangent += vNormalDelta.xyz;
   flWrinkle = vPosDelta.w;
  }
  else
  {
   float4 t = float4(vMorphTexCoord.x, vMorphTexCoord.y, 0.0f, 0.0f);
   float4 vPosDelta = morphTexture.SampleLevel(morphSampler, t.xy, t.w);
   t.x += 1.0f / vMorphTargetTextureDim.x;
   float3 vNormalDelta = morphTexture.SampleLevel(morphSampler, t.xy, t.w);

   vPosition += vPosDelta.xyz * vMorphTexCoord.z;
   vNormal += vNormalDelta.xyz * vMorphTexCoord.z;
   vTangent += vNormalDelta.xyz * vMorphTexCoord.z;
   flWrinkle = vPosDelta.w * vMorphTexCoord.z;
  }

  return true;
 }
 else
 {
  flWrinkle = 0.0f;
  return false;
 }
}

float4 DecompressBoneWeights(const float4 weights)
{
 float4 result = weights;

 if (1)
 {





  result += 1;
  result /= 32768;
 }

 return result;
}

void SkinPositionAndNormal(bool bSkinning, const float4 modelPos, const float3 modelNormal,
 const float4 boneWeights, float4 fBoneIndices,
 out float3 worldPos, out float3 worldNormal)
{

 int3 boneIndices = D3DCOLORtoUBYTE4(fBoneIndices);

 if (!bSkinning)
 {
  worldPos = mul4x3(modelPos, cModel[0]);
  worldNormal = mul3x3(modelNormal, (float3x3)cModel[0]);
 }
 else
 {
  float4x3 mat1 = cModel[boneIndices[0]];
  float4x3 mat2 = cModel[boneIndices[1]];
  float4x3 mat3 = cModel[boneIndices[2]];

  float3 weights = DecompressBoneWeights(boneWeights).xyz;
  weights[2] = 1 - (weights[0] + weights[1]);

  float4x3 blendMatrix = mat1 * weights[0] + mat2 * weights[1] + mat3 * weights[2];
  worldPos = mul4x3(modelPos, blendMatrix);
  worldNormal = mul3x3(modelNormal, (float3x3)blendMatrix);
 }
}

float RangeFog(const float3 projPos)
{
 return max(cFogParams.z, (-projPos.z * cFogParams.w + cFogParams.x));
}

float WaterFog(const float3 worldPos, const float3 projPos)
{
 float4 tmp;

 tmp.xy = cEyePosWaterZ.wz - worldPos.z;







 tmp.x = max(0.0f, tmp.x);


 tmp.w = tmp.x / tmp.y;

 tmp.w *= projPos.z;



 return max(cFogParams.z, (-tmp.w * cFogParams.w + cFogParams.y));
}

float CalcFog(const float3 worldPos, const float3 projPos, const int fogType)
{





 if (fogType == 0)
 {
  return RangeFog(projPos);
 }
 else
 {




  return WaterFog(worldPos, projPos);

 }
}

float CalcFog(const float3 worldPos, const float3 projPos, const bool bWaterFog)
{
 float flFog;
 if (!bWaterFog)
 {
  flFog = RangeFog(projPos);
 }
 else
 {




  flFog = WaterFog(worldPos, projPos);

 }

 return flFog;
}

float3 AmbientLight(const float3 worldNormal)
{
 float3 nSquared = worldNormal * worldNormal;
 int3 isNegative = (worldNormal < 0.0);
 float3 linearColor;
 linearColor = nSquared.x * cAmbientCubeX[isNegative.x] +
  nSquared.y * cAmbientCubeY[isNegative.y] +
  nSquared.z * cAmbientCubeZ[isNegative.z];
 return linearColor;
}



float VertexAttenInternal(const float3 worldPos, int lightNum)
{
 float result = 0.0f;


 float3 lightDir = cLightInfo[lightNum].pos - worldPos;


 float lightDistSquared = dot(lightDir, lightDir);


 float ooLightDist = rsqrt(lightDistSquared);


 lightDir *= ooLightDist;

 float3 vDist = dst(lightDistSquared, ooLightDist);

 float flDistanceAtten = 1.0f / dot(cLightInfo[lightNum].atten.xyz, vDist);


 float flCosTheta = dot(cLightInfo[lightNum].dir.xyz, -lightDir);
 float flSpotAtten = (flCosTheta - cLightInfo[lightNum].spotParams.z) * cLightInfo[lightNum].spotParams.w;
 flSpotAtten = max(0.0001f, flSpotAtten);
 flSpotAtten = pow(flSpotAtten, cLightInfo[lightNum].spotParams.x);
 flSpotAtten = saturate(flSpotAtten);


 float flAtten = lerp(flDistanceAtten, flDistanceAtten * flSpotAtten, cLightInfo[lightNum].dir.w);


 result = lerp(flAtten, 1.0f, cLightInfo[lightNum].color.w);

 return result;
}

float CosineTermInternal(const float3 worldPos, const float3 worldNormal, int lightNum, bool bHalfLambert)
{

 float3 lightDir = normalize(cLightInfo[lightNum].pos - worldPos);


 lightDir = lerp(lightDir, -cLightInfo[lightNum].dir, cLightInfo[lightNum].color.w);


 float NDotL = dot(worldNormal, lightDir);

 if (!bHalfLambert)
 {
  NDotL = max(0.0f, NDotL);
 }
 else
 {
  NDotL = NDotL * 0.5 + 0.5;
  NDotL = NDotL * NDotL;
 }
 return NDotL;
}

float3 DoLightInternal(const float3 worldPos, const float3 worldNormal, int lightNum, bool bHalfLambert)
{
 return cLightInfo[lightNum].color*
  CosineTermInternal(worldPos, worldNormal, lightNum, bHalfLambert)*
  VertexAttenInternal(worldPos, lightNum);
}

float3 DoLighting(const float3 worldPos, const float3 worldNormal,
 const float3 staticLightingColor, const bool bStaticLight,
 const bool bDynamicLight, bool bHalfLambert)
{
 float3 linearColor = float3(0.0f, 0.0f, 0.0f);

 if (bStaticLight)
 {
  float3 col = staticLightingColor * 2.0f;
  linearColor += GammaToLinear(col);
 }

 if (bDynamicLight)
 {
  for (int i = 0; i < g_nLightCountRegister.x; i++)
  {
   linearColor += DoLightInternal(worldPos, worldNormal, i, bHalfLambert);
  }
 }

 if (bDynamicLight)
 {
  linearColor += AmbientLight(worldNormal);
 }

 return linearColor;
}
#line 2 "vertexlit_and_unlit_generic.vert.hlsl"

cbuffer VertexShaderCustomConstants
{
 float4 cBaseTexCoordTransform[2];
 float cSeamlessScale;
 float4 cDetailTexCoordTransform[2];

 float3 cMorphTargetTextureDim;
 float4 cMorphSubrect;
};

[[vk::binding(1)]] SamplerState morphSampler;
[[vk::binding(1)]] Texture2D morphTexture;






struct VS_INPUT
{

 float4 vPos : POSITION;
 float4 vNormal : NORMAL;
 float4 vColor : COLOR0;
 float3 vSpecular : COLOR1;

 float4 vBoneWeights : BLENDWEIGHT;
 float4 vBoneIndices : BLENDINDICES;


 float4 vTexCoord0 : TEXCOORD0;
 float4 vTexCoord1 : TEXCOORD1;
 float4 vTexCoord2 : TEXCOORD2;
 float4 vTexCoord3 : TEXCOORD3;


 float3 vPosFlex : POSITION1;
 float3 vNormalFlex : NORMAL1;
 float vVertexID : POSITION2;
};

struct VS_OUTPUT
{
 float4 projPos : POSITION;
 float fog : FOG;

 float3 baseTexCoord : TEXCOORD0;
 float3 detailTexCoord : TEXCOORD1;
 float4 color : TEXCOORD2;

 float3 worldVertToEyeVector : TEXCOORD3;
 float3 worldSpaceNormal : TEXCOORD4;

 float4 vProjPos : TEXCOORD6;
 float4 worldPos_ProjPosZ : TEXCOORD7;

 float3 SeamlessWeights : COLOR0;
 float4 fogFactorW : COLOR1;
};

VS_OUTPUT main(const VS_INPUT v)
{
 VS_OUTPUT o;

 float4 vPosition = v.vPos;
 float3 vNormal = 0;
 if ((!VERTEXCOLOR && (DYNAMIC_LIGHT || STATIC_LIGHT_VERTEX)) ||
  FLASHLIGHT ||
  SEAMLESS_BASE ||
  SEAMLESS_DETAIL ||
  LIGHTING_PREVIEW ||
  (MORPHING && DECAL) ||
  CUBEMAP)
 {
  DecompressVertex_Normal(v.vNormal, vNormal);
 }

 float3 NNormal;
 if (SEAMLESS_BASE || SEAMLESS_DETAIL)
 {
  NNormal = normalize(vNormal);
  o.SeamlessWeights.xyz = NNormal * NNormal;
 }

 if (!MORPHING)
  ApplyMorph(v.vPosFlex, v.vNormalFlex, vPosition.xyz, vNormal);
 else
 {
  ApplyMorph(morphTexture, morphSampler, cMorphTargetTextureDim, cMorphSubrect,
   v.vVertexID, v.vTexCoord2, vPosition.xyz, vNormal);
 }


 float3 worldNormal, worldPos;
 SkinPositionAndNormal(
  (SKINNING),
  vPosition, vNormal,
  v.vBoneWeights, v.vBoneIndices,
  worldPos, worldNormal);

 if (!VERTEXCOLOR)
  worldNormal = normalize(worldNormal);

 if (MORPHING && DECAL)
  worldPos += worldNormal * 0.05f * v.vTexCoord2.z;

 o.worldSpaceNormal = worldNormal;


 float4 vProjPos = mul(float4(worldPos, 1), cViewProj);
 o.projPos = vProjPos;
 vProjPos.z = dot(float4(worldPos, 1), cViewProjZ);

 o.vProjPos = vProjPos;
 o.fogFactorW.w = CalcFog(worldPos, vProjPos, DOWATERFOG);
 o.fog = o.fogFactorW.w;
 o.worldPos_ProjPosZ.xyz = worldPos.xyz;
 o.worldPos_ProjPosZ.w = vProjPos.z;


 if (CUBEMAP)
  o.worldVertToEyeVector.xyz = cEyePosWaterZ.xyz - worldPos;

 if (FLASHLIGHT)
 {
  o.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
 }
 else
 {
  if (VERTEXCOLOR)
  {

   o.color.rgb = (DONT_GAMMA_CONVERT_VERTEX_COLOR) ? v.vColor.rgb : GammaToLinear(v.vColor.rgb);
   o.color.a = v.vColor.a;
  }
  else
  {
   o.color.xyz = DoLighting(worldPos, worldNormal, v.vSpecular, STATIC_LIGHT_VERTEX, DYNAMIC_LIGHT, HALFLAMBERT);
  }
 }

 if (SEAMLESS_BASE)
  o.baseTexCoord.xyz = (cSeamlessScale.x) * v.vPos.xyz;
 else
 {

  o.baseTexCoord.x = dot(v.vTexCoord0, cBaseTexCoordTransform[0]);
  o.baseTexCoord.y = dot(v.vTexCoord0, cBaseTexCoordTransform[1]);
 }

 if (SEAMLESS_DETAIL)
 {


  o.detailTexCoord.xyz = ((cSeamlessScale.x) * cDetailTexCoordTransform[0].x) * v.vPos.xyz;
 }
 else
 {


  o.detailTexCoord.x = dot(v.vTexCoord0, cDetailTexCoordTransform[0]);
  o.detailTexCoord.y = dot(v.vTexCoord0, cDetailTexCoordTransform[1]);
 }

 if (SEPARATE_DETAIL_UVS)
  o.detailTexCoord.xy = v.vTexCoord1.xy;

 if (LIGHTING_PREVIEW)
 {
  float dot = 0.5 + 0.5 * worldNormal * float3(0.7071, 0.7071, 0);
  o.color.xyz = float3(dot, dot, dot);
 }

 return o;
}
       type.VertexShaderStandardConstants           cConstants1         g_bLightEnabled  	       g_nLightCountRegister           cEyePosWaterZ           cFlexScale          cModelViewProj          cViewProj           cModelViewProjZ         cViewProjZ       	   cFogParams       
   cModel          cLightInfo          cAmbientCubeX           cAmbientCubeY           cAmbientCubeZ        LightInfo            color           dir         pos         spotParams          atten    
    VertexShaderStandardConstants        type.VertexShaderCustomConstants     	        cBaseTexCoordTransform          cSeamlessScale   
       cDetailTexCoordTransform     	       cMorphTargetTextureDim          cMorphSubrect    	     VertexShaderCustomConstants   !   type.sampler      "   morphSampler      #   type.2d.image     $   morphTexture         in.var.POSITION      in.var.NORMAL        in.var.COLOR0        in.var.COLOR1        in.var.BLENDWEIGHT       in.var.BLENDINDICES   	   in.var.TEXCOORD0      
   in.var.TEXCOORD1         in.var.TEXCOORD2         in.var.TEXCOORD3         in.var.POSITION1         in.var.NORMAL1       in.var.POSITION2         out.var.POSITION         out.var.FOG      out.var.TEXCOORD0        out.var.TEXCOORD1        out.var.TEXCOORD2        out.var.TEXCOORD3        out.var.TEXCOORD4        out.var.TEXCOORD6        out.var.TEXCOORD7        out.var.COLOR0       out.var.COLOR1       main      %   type.sampled.image  G  &         G  '         G  (         G  )         G  *         G  +         G  ,         G  -         G  .      	   G  /      
   G  0         G  1         G  2         G  3         G  4                POSITION           NORMAL         COLOR0         COLOR1         BLENDWEIGHT        BLENDINDICES      	     TEXCOORD0     
     TEXCOORD1          TEXCOORD2          TEXCOORD3          POSITION1          NORMAL1        POSITION2          POSITION           FOG        TEXCOORD0          TEXCOORD1          TEXCOORD2          TEXCOORD3          TEXCOORD4          TEXCOORD6          TEXCOORD7          COLOR0         COLOR1  G            G           G           G           G           G           G  	         G  
         G           G        	   G        
   G           G           G            G           G           G           G           G           G           G           G           G        	   G        
   G  "   "       G  "   !      G  $   "       G  $   !      G     "       G     !       G      "       G      !      G  5      0   H         #       H        #      H        #       H        #   0   H        #   @   G  6      P   G  7         H         #       H        #      H        #       H        #   0   H        #   @   H        #   P   H              H           H        #   �   H              H           H        #   �   H        #   �   H     	   #   �   H     
   #      H     
         H     
      H        #   �
  H        #   0  H        #   P  H        #   p  G        G  8         H         #       H        #       H        #   0   H        #   P   H        #   `   G          9   1  9   &   1  9   '   1  9   (   1  9   )   1  9   *   1  9   +   1  9   ,   1  9   -   1  9   .   1  9   /   1  9   0     :          2  :   1       1  9   2   1  9   3   1  9   4   +  :   ;      +  :   <       +  :   =      +  :   >      +  :   ?      +  :   @      +  :   A        B       +  B   C         D   B      ,  D   E   C   C   C   +  :   F   	   +  :   G   
   +  :   H      +  :   I      +  B   J   ��L=+  B   K     �?  L   B      ,  L   M   C   C   C   C   +  B   N      ?,  D   O   N   N   N   +  B   P   �5?,  D   Q   P   P   C   +  B   R   � C  S           +  S   T      +  B   U   ��@,  D   V   U   U   U   +  B   W      @+  B   X      C  Y   B      ,  Y   Z   X   X   ,  Y   [   C   C   ,  Y   \   K   K   +  B   ]     �B,  Y   ^   ]   ]   ,  L   _   K   K   K   K     `   :      ,  `   a   =   =   =   ,  `   b   <   <   <   +  :   c      +  :   d      +  B   e   ��8  f   S        g   L      +  S   h   5     i   D        5   i   h   +  S   j           L   L   L   L   L     6      j     7   D   T        L   f   :   L   L   g   g   L   L   L   5   6   7   7   7      k           8   L   T        8   B   8   D   L      l           !      m       !    	 #   B                           n       #      o      L      p      D      q      B      r      L      s      B      t      D     u   !  v   u   4  9   w   �   /   0   4  9   x   �   4   -   4  9   y   �   *   +      z      D      {      L   4  9   |   �   4   -      }      g     ~   9               B     %   #     �   D        �   :         �      i      �      :     �   9      ;  k         ;  l          ;  m   "       ;  n   $       ;  o         ;  o         ;  o         ;  p         ;  o         ;  o         ;  o   	      ;  o   
      ;  o         ;  o         ;  p         ;  p         ;  q         ;  r         ;  s         ;  t         ;  t         ;  r         ;  t         ;  t         ;  r         ;  r         ;  t         ;  r           D   �   +  B   �     ��,  Y   �   �   �   +  B   �   !�<,  Y   �   �   �   +  B   �      8,  L   �   �   �   �   �     L   �     D   �     L   �   .  L   �   6  u          v   �  �   =  L   �      =  L   �      =  L   �      =  D   �      =  L   �      =  L   �      =  L   �   	   =  L   �   
   =  L   �      =  D   �      =  D   �      =  B   �           �     �  9   �   &   �  9   �   �   w   �  9   �   �   )   �  9   �   �   *   �  9   �   �   +   �  9   �   �   3   �  9   �   �   x   �  9   �   �   '   �  �       �  �   �   �   �  �   O  Y   �   �   �          �  Y   �   �   Z   �  �   �   �   [   �  Y   �   �   \   [     Y   �         �   �  Y   �   �   �   �  Y   �   �   ^   �  �   �   �   [   �  Y   �   �   \   [     Y   �         �   �  Y   �   �   �   �  Y   �   �   �   O  D   �   E   �            Q  B   �   �       �  B   �   K   �   Q  B   �   �      �  B   �   �   �   R  D   �   �   �        D   �      E   �     Y   �      .   \   �   �   O  Y   �   �   �          �  Y   �   �   �   O  D   �   �   �            Q  B   �   �         B   �      .   K   �   �   Q  B   �   �      �  B   �   �   �   R  D   �   �   �      �  �   �  �   �  D   �   E   �   �   �        �     �  �       �  y   �   �   �  �     D   �      E   �   �  D   �   �   �   �  �   �  �   �  D   �   �   �   �   �        �     �  9   �   4   �  �       �  �   �   �   �  �        �     =  #   �   $   =  !   �   "   A  z   �       ?   =  D   �   �   A  {   �       @   =  L   �   �   Q  B   �   �       Q  B   �   �      Q  B   �   �      O  D   �   �   �             �  �   �  �   �  �   �       �  �   �  �             �  �       �  4   �   �   �  �   �  �   �  �             �  9   �   -   �  �       �  �   �   �   �  �   P  L   �   �   �   C   C   O  Y   �   �   �          V  %   �   �   �   X  L   �   �   �      C   Q  B   �   �       Q  B   �   �      Q  B   �   �      P  D   �   �   �   �   Q  B   �   �       �  B   �   K   �   �  B   �   �   �   R  L   �   �   �       O  Y   �   �   �          V  %   �   �   �   X  L   �   �   �      C   Q  B   �   �       Q  B   �   �      Q  B   �   �      P  D   �   �   �   �   �  D   �   �   �   �  D   �   �   �   �  D   �   �   �   �  D   �   �   �   �  �   �  �   Q  B   �   �      �  B   �   �   �     B   �         �   Q  B   �   �       Q  B   �   �      �  B   �   �   �   �  B   �   �   �   �  B      �   N   R  L        �       Q  B     �      �  B       �   �  B     �   �   �  B         �  B       N   R  L            O  Y     �   �          O  Y   	             �  Y   
  	    O 	 L       
              O  Y     
  �          V  %     �   �   X  L            C   Q  B     �       �  B     K     Q  B     
      �  B         R  L             O  Y                V  %     �   �   X  L            C   O  D                   �  D     �     O  D                   �  D     �     �  �   �  �   �  D     �   �     �   �  D     �   �     �   �  �   �  �   �  �  �   �  �   �  �   �  D     �   �     �   �  D     �   �     �   O 	 L     �                 �  �   �  �   O  D      �   �             A     !     @   <   =  B   "  !  �  D   #  �   "  �  D   $  �   "  �  D   %     #  �  D   &  �   $  O 	 L   '  �   %              �  �   �  �   �  D   (    �   &  �   �  L   )    �   '  �   O 	 L   *  �   �                �  L   +  *  R   n  �   ,  +  Q  :   -  ,      Q  :   .  ,     Q  :   /  ,     �  9   0  2   �  1      �  0  2  3  �  3  A  �   4     G   -  =  i   5  4  A  �   6     G   .  =  i   7  6  A  �   8     G   /  =  i   9  8  �  L   :  �   _   �  L   ;  :  �   Q  B   <  ;      Q  B   =  ;     �  B   >  <  =  �  B   ?  K   >  �  i   @  5  <  �  i   A  7  =  Q  D   B  @      Q  D   C  A      �  D   D  B  C  Q  D   E  @     Q  D   F  A     �  D   G  E  F  Q  D   H  @     Q  D   I  A     �  D   J  H  I  Q  D   K  @     Q  D   L  A     �  D   M  K  L  �  i   N  9  ?  Q  D   O  N      �  D   P  D  O  Q  D   Q  N     �  D   R  G  Q  Q  D   S  N     �  D   T  J  S  Q  D   U  N     �  D   V  M  U  P  i   W  P  R  T  V  �  D   X  W  )  P  �   Y  P  R  T  �  D   Z  Y  (  �  1  �  2  A  �   [     G   <   =  i   \  [  �  D   ]  \  )  Q  D   ^  \      Q  D   _  \     Q  D   `  \     P  �   a  ^  _  `  �  D   b  a  (  �  1  �  1  �  D   c  X  3  ]  2  �  D   d  Z  3  b  2  �  e      �  �   f  e  �  f    D   g     E   d  �  e  �  e  �  D   h  d  1  g  f       �     �  i      �  |   j  i  �  j  �  D   k  h  J   Q  B   l  �      �  D   m  k  l  �  D   n  c  m  �  i  �  i  �  D   o  c  e  n  j  Q  B   p  o      Q  B   q  o     Q  B   r  o     P  L   s  p  q  r  K   A  }   t     A   =  g   u  t  �  L   v  u  s  A  {   w     ;   =  L   x  w  �  B   y  s  x  R  L   z  y  v     �  {  �  {  �  |  }      �  ~  �  ~  �  9     1   <   �  �      �    �  �  �  �  A  {   �     ?   =  L   �  �  O  Y   �  �  �        P  Y   �  r  r  �  Y   �  �  �  Q  B   �  �        B   �     (   C   �  Q  B   �  �     �  B   �  �  �  �  B   �  �  y  A     �     F   >   =  B   �  �    B   �  �  A     �     F   ?   =  B   �  �  �  B   �  �  �  A     �     F   =   =  B   �  �  �  B   �  �  �    B   �     (   �  �  �  |  �  �  A     �     F   >   =  B   �  �    B   �  y  A     �     F   ?   =  B   �  �  �  B   �  �  �  A     �     F   <   =  B   �  �  �  B   �  �  �    B   �     (   �  �  �  |  �  �  �  �  }  �  {  �  |  �  B   �  �  �  �  �  R  L   �  �  �      O 	 L   �  �   o              R  L   �  y  �          �     �  �      �  '   �  �  �  �  A  {   �     ?   =  L   �  �  O  D   �  �  �            �  D   �  �  o  �  �  �  �  �  D   �  �   |  �  �       �     �  �      �  )   �  �  �  �       �     �  �      �  &   �  �  �  �  �  �      �  0   �  �  �  �  �  D   �  �   W     D   �        �  V   �  �  �  �  �  D   �  E   �  �  �  �  �      �  /   �  �  �  �  �  �  �  �  �  D   �  �  �  �  �  �  :   �  <   �  �  �  A  �   �     >   =  :   �  �  �  9   �  �  �  �  �  �      �  �  �  �  �  �  A  {   �     H   �  <   =  L   �  �  A  {   �     H   �  >   =  L   �  �  Q  B   �  �      Q  B   �  �     Q  B   �  �     P  D   �  �  �  �  �  D   �  �  o    D   �     E   �  A  {   �     H   �  =   =  L   �  �    L   �  �  Q  B   �  �      Q  B   �  �     Q  B   �  �     P  D   �  �  �  �  A     �     H   �  <   ?   =  B   �  �  P  D   �  �  �  �    D   �     .   �  �  �  �  B   �  h  �  �  9   �  (   �  �      �  �  �  �  �  �  �  B   �  �  N   �  B   �  �  N   �  B   �  �  �  �  �  �  �    B   �     (   C   �  �  �  �  �  �  B   �  �  �  �  �  �  L   �  �  �  �  B   �  �  �    B   �         �  �  D   �  �  �  �  B   �  �  �  P  D   �  K   �  �  A  {   �     H   �  @   =  L   �  �  O  D   �  �  �            �  B   �  �  �  �  B   �  K   �  O  D   �  �  �              D   �  �  �  B   �  �  �  A     �     H   �  ?   >   =  B   �  �  �  B   �  �  �  A     �     H   �  ?   ?   =  B   �  �  �  B   �  �  �    B   �     (   e   �  A     �     H   �  ?   <   =  B   �  �    B   �        �  �    B   �     +   �  C   K   �  B   �  �  �  A     �     H   �  =   ?   =  B   �  �    B   �     .   �  �  �    B   �     .   �  K   �  �  L      �  �  Q  B            Q  B           Q  B           P  D           �  D   �  �    �  :   �  �  =   �  �  �  �  �  �  �  �  �  D     �  �  �  �  �        �  /       �    �  D     h  h  �  ~   	  h  E   �  `   
  	  a   b   Q  :     
      A  z        I     =  D       Q  B           �  D         Q  :     
     A  z        c     =  D       Q  B          �  D         �  D         Q  :     
     A  z        d     =  D       Q  B          �  D         �  D         �  D         �    �    �  D       �      O 	 L     �                 �  �  �  �  O  D     �   �               D              V   P  ~   !  .   .   .   �  D   "  !       O 	 L   #  �   "              Q  B   $  �      R  L   %  $  #     �  �  �  �  �  L   &      %  �  �  �  �  �  �  �  �  �  �  L   '  &  �  M   �       �     �  (      �  *   )  *  �  *  A  {   +      <   <   =  L   ,  +  �  B   -  �   ,  R  D   .  -  �       A  {   /      <   =   =  L   0  /  �  B   1  �   0  R  D   2  1  .     �  (  �  )  O  D   3  �   �             A     4      =   =  B   5  4  �  D   6  3  5  �  (  �  (  �  D   7  2  *  6  )       �     �  8      �  +   9  :  �  :  A  {   ;      >   <   =  L   <  ;  �  B   =  �   <  R  D   >  =  �       A  {   ?      >   =   =  L   @  ?  �  B   A  �   @  R  D   B  A  >     �  8  �  9  O  D   C  �   �             A     D      =   =  B   E  D  A     F      >   <   <   =  B   G  F  �  B   H  E  G  �  D   I  C  H  �  8  �  8  �  D   J  B  :  I  9            �  K      �  ,   L  K  �  L  O  D   M  J  �            �  K  �  K  �  D   N  J  8  M  L            �  O      �  3   P  O  �  P  �  D   Q  h  N   �  D   R  Q  Q   �  D   S  O   R  Q  B   T  S      P  D   U  T  T  T  O 	 L   V  '  U              �  O  �  O  �  L   W  '  K  V  P  >     v  >     �  >     7  >     N  >     W  >     �  >     h  >     z  >     �  >     �   >     �  �  8  