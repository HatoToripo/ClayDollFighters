

//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

// �}�g���N�X�o�b�t�@
cbuffer WorldBuffer : register( b0 )
{
	matrix World;
}

cbuffer ViewBuffer : register( b1 )
{
	matrix View;
}

cbuffer ProjectionBuffer : register( b2 )
{
	matrix Projection;
}

cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}


struct SHADOWMAP
{
    matrix lightViewProj;

};

cbuffer ShadowMapBuffer : register(b9)
{
    SHADOWMAP Shadowmap;
}

//=============================================================================
// ���_�V�F�[�_
//=============================================================================

void VertexShaderShadow(
						in  float4 inPosition		: POSITION0,
						in  float4 inNormal		    : NORMAL0,
						in  float4 inDiffuse		: COLOR0,
						in  float2 inTexCoord		: TEXCOORD0,

						out float4 outPosition	    : SV_POSITION,
						out float2 outTexCoord : TEXCOORD)
						
{
   
    outPosition = mul(inPosition, mul(World, Shadowmap.lightViewProj));
	
	outTexCoord = inTexCoord;

}

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

Texture2D		g_ShadowMap : register(t1); // �V���h�E�}�b�s���O�p

SamplerComparisonState g_ShadowMapSampler : register(s1);