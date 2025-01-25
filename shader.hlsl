

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// マトリクスバッファ
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

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte境界用
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
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// 縁取り用バッファ
cbuffer RimLight : register(b6)
{
	int			rim;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

// ディゾルブ
struct DISSOLVE
{
    float4 edgeColor; // エッジの色0
    float Threshold; // ディゾルブの閾値
    float edgeWidth; // エッジの幅
    int Enable;
    int Dummy; //16byte境界用
};
// ディゾルブ用バッファ
cbuffer DissolveBuffer : register(b8)
{
    DISSOLVE Dissolve;
}

//struct SECTOR
//{
//    float startAngle;
//    float endAngle;
//    int flag;
//    int Dummy; //16byte境界用
//};

//cbuffer SectorBuffer : register(b8)
//{
//    SECTOR Sector;
//};


//=============================================================================
// 頂点シェーダ
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outWorldPos    : POSITION0)
{
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	outPosition = mul(inPosition, wvp);

	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

	outDiffuse = inDiffuse;
}



//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
Texture2D		noiseTexture : register(t1); // ノイズマップ
SamplerState	g_SamplerState : register( s0 );


//=============================================================================
// ピクセルシェーダ
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,

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

	//フォグ
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

	//縁取り
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
        float noiseValue = noiseTexture.Sample(g_SamplerState, inTexCoord).r;
	
	  // ノイズ値と閾値を比較してディゾルブを適用
        if (noiseValue < Dissolve.Threshold)
        {
        // エッジの計算
            if (Dissolve.Threshold - noiseValue < Dissolve.edgeWidth)
            {
                outDiffuse = Dissolve.edgeColor; // エッジの色
            }
            discard;
        }
    }
	//if (Sector.flag == true)
 //   {
	//	// UV座標系の中心からのベクトル
 //       float2 dir = inTexCoord - float2(0.5f, 0.5f);
 //       float dist = length(dir);
		
	//	if (dist > 0.5f)
 //       {
 //           outDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
 //       }
		
	//	// 角度計算
 //       float angle = atan2(dir.y, dir.x);
		
	//	// 扇形の範囲内チェック
 //       if (angle >= 0.0f && angle <= 1.0f)
 //       {
 //           outDiffuse = inDiffuse;
 //       }
		
 //       else
 //       {
 //           outDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
 //       }
 //   }
}
