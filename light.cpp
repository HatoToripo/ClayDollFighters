//=============================================================================
//
// ライト処理 [light.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "player.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
#define	DIRECTIONAL_LIGHT_Y		(30.0f)				// 平行光源の高さ

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static BOOL		g_FogEnable = FALSE;

static 	XMFLOAT3 g_pos;
static 	XMFLOAT3 g_dir;
static PLAYER* g_player = GetPlayer();

//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{

	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f);
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// 減衰距離
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ライトのタイプ
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// 並行光源の設定（世界を照らす光）
	g_Light[0].Position = XMFLOAT3( 0.0f, DIRECTIONAL_LIGHT_Y, 0.0f );		// 光の向き
	g_Light[0].Direction = XMFLOAT3(-0.5f, -1.0f, 0.2f);		// 光の向き
	g_Light[0].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );	// 光の色
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// 並行光源
	g_Light[0].Enable = TRUE;									// このライトをON
	SetLight(0, &g_Light[0]);									// これで設定している

	// フォグの初期化（霧の効果）
	g_Fog.FogStart = 100.0f;									// 視点からこの距離離れるとフォグがかかり始める
	g_Fog.FogEnd   = 250.0f;									// ここまで離れるとフォグの色で見えなくなる
	g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );		// フォグの色
	SetFog(&g_Fog);
	SetFogEnable(g_FogEnable);				// 他の場所もチェックする shadow

}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{


}


//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}

LIGHT* GetLight(void)
{
	return &g_Light[0];
}

LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL	GetFogEnable(void)
{
	return(g_FogEnable);
}

void SetLightProjectViewMatrix()
{
	XMFLOAT3 p_pos = { 0.0f,1.0f,0.0f };

	p_pos = g_player->pos;
	g_pos = g_player->pos;

	XMFLOAT3 up = { 0.0f,1.0f,0.0f };


	XMVECTOR  l_dir = XMVector3Normalize(XMLoadFloat3(&g_dir));
	float lightHeight = 500.0f;
	XMVECTOR player_pos = XMLoadFloat3(&p_pos);


	XMVECTOR lightPosition = player_pos - l_dir * lightHeight;


	XMMATRIX lightView = XMMatrixLookAtLH(lightPosition, XMLoadFloat3(&p_pos), XMLoadFloat3(&up));
	XMMATRIX lightProjection = XMMatrixOrthographicLH(1000.0f, 1000.0f, 0.1f, 1000.0f);
	XMMATRIX lightProjView = XMMatrixMultiply(lightView, lightProjection);


	lightProjView = XMMatrixTranspose(lightProjView);
}
