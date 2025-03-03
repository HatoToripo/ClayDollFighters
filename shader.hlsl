

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

// �}�e���A���o�b�t�@
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte���E�p
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ���C�g�p�o�b�t�@
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte���E�p
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte���E�p
};

// �t�H�O�p�o�b�t�@
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// �����p�o�b�t�@
cbuffer RimLight : register(b6)
{
	int			rim;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

// �f�B�]���u
struct DISSOLVE
{
    float4 edgeColor; // �G�b�W�̐F0
    float Threshold; // �f�B�]���u��臒l
    float edgeWidth; // �G�b�W�̕�
    int Enable;
    float Dummy; //16byte���E�p
};
// �f�B�]���u�p�o�b�t�@
cbuffer DissolveBuffer : register(b8)
{
    DISSOLVE Dissolve;
}

// �V���h�E�}�b�v
struct SHADOWMAP
{
    matrix lightViewProj;

};

// �V���h�E�}�b�v�p�o�b�t�@
cbuffer ShadowMapBuffer : register(b9)
{
    SHADOWMAP Shadowmap;
}


//=============================================================================
// ���_�V�F�[�_
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outWorldPos    : POSITION0,
						  out float4 outLightPos : TEXCOORD1)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

    outLightPos = mul(outWorldPos, Shadowmap.lightViewProj);

	outDiffuse = inDiffuse;
}



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
Texture2D		g_ShadowMap : register(t1); // �V���h�E�}�b�s���O�p
Texture2D		g_NoiseTexture : register(t2); // �f�B�]���u�p�m�C�Y�}�b�v

SamplerState g_SamplerState : register(s0);
SamplerComparisonState g_ShadowMapSampler : register(s1);

//=============================================================================
// �s�N�Z���V�F�[�_
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,
						 in  float4 inLightPos		: TEXCOORD1,

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	if (Material.noTexSampling == 0)
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else
	{
		color = inDiffuse;
	}

	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					tempColor *= att;
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	//�t�H�O
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

	//�����
    if (rim == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
        if ((angle < 0.5f) && (angle > -0.5f))
        //if (angle > -0.3f)
		{
			outDiffuse.r  = 1.0f;
			outDiffuse.bg = 0.0f;			
		}
	}
    else if (rim == 2)
    {
        float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
        if ((angle < 0.5f) && (angle > -0.5f))
        //if (angle > -0.3f)
        {
            outDiffuse.r = 0.0f;
            outDiffuse.bg = 1.0f;
        }
    }

    if (Dissolve.Enable == 1)
    {
        float noiseValue = g_NoiseTexture.Sample(g_SamplerState, inTexCoord).r;
	
	  // �m�C�Y�l��臒l���r���ăf�B�]���u��K�p
        if (noiseValue < Dissolve.Threshold)
        {
        // �G�b�W�̌v�Z
            if (Dissolve.Threshold - noiseValue < Dissolve.edgeWidth)
            {
                outDiffuse = Dissolve.edgeColor; // �G�b�W�̐F
            }
            discard;
        }
    }
	
		// �V���h�E�}�b�v
    if (Light.Enable == 1)
    {
        if (Light.Flags[0].y == 1)
        {
            float shadowFactor = 1.0;
            float2 shadowTexCoord = float2(inLightPos.x, -inLightPos.y) / inLightPos.w * 0.5 + 0.5;
            float currentDepth = inLightPos.z / inLightPos.w;
            currentDepth -= 0.005;

            shadowFactor = g_ShadowMap.SampleCmpLevelZero(g_ShadowMapSampler, shadowTexCoord, currentDepth);

            //if (shadowTexCoord.x < 0 || shadowTexCoord.x > 1 || shadowTexCoord.y < 0 || shadowTexCoord.y > 1)
            //{
            //    shadowFactor = 1.0;
            //}

			//if (shadowFactor == 0)
			//{
				//shadowFactor = 0.2;
			//}

		// PCF �v�Z
            float2 texelSize = 1.0 / 4096.0;
            float2 offsets[9] =
            {
                float2(-1, -1), float2(0, -1), float2(1, -1),
				float2(-1, 0), float2(0, 0), float2(1, 0),
				float2(-1, 1), float2(0, 1), float2(1, 1)
            };

            float pcfRadius = 1.0f;
            float shadow = 0.0;
            for (int i = 0; i < 9; ++i)
            {
				// �T���v�����O
                float2 sampleCoord = shadowTexCoord + offsets[i] * texelSize * pcfRadius;

				// ���ʂ�ǉ�
                shadow += g_ShadowMap.SampleCmpLevelZero(g_ShadowMapSampler, sampleCoord, currentDepth);
            }
			
			// ���ϒl�����߂�
            shadow /= 9.0;
            if (shadow == 0)
            {
                shadow = 0.4;
            }
            else if (shadow > 0 && shadow < 1)
            {
                shadow = 0.4 + shadow;
                if (shadow > 0.9)
                {
                    shadow = 0.9;
                }
            }

            if (shadowTexCoord.x < 0 || shadowTexCoord.x > 1 || shadowTexCoord.y < 0 || shadowTexCoord.y > 1)
            {
                shadow = 1.0;
            }
			// PCF �I��
	
		// outDiffuse.xyz *= shadowFactor;
            outDiffuse.xyz *= shadow;
        }
    }
}
