#include "common_vs_fxc.hlsli"
#include "bufferclearobeystencil.common.hlsli"

struct VS_INPUT
{
	float4 vPos    : POSITION;
	float4 vColor  : COLOR;
};

struct VS_OUTPUT
{
	float4 vProjPos  : SV_Position;
	float4 vColor    : COLOR;
};

VS_OUTPUT main(const VS_INPUT v)
{
	VS_OUTPUT o;

	o.vProjPos.xyz = v.vPos.xyz;
	o.vProjPos.w = 1.0f;

	if (USESCOLOR)
		o.vColor = v.vColor;

	return o;
}
