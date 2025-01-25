//=============================================================================
//
// チュートリアル画面処理 [tutorial.cpp]
// Author : 
//
//=============================================================================
#include "tutorial.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(3)				// テクスチャの数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static BOOL			g_TutorialFlg = TRUE;
static int			tutorialNum;
static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/fade_black.png",
	"data/TEXTURE/tutorial_keyboard.png",
	"data/TEXTURE/tutorial_gamepad.png",
};

static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;
	tutorialNum = 0;			// チュートリアル画像の番号初期化
	// BGM再生


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTutorial(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		tutorialNum++;
		if (tutorialNum >= CONTROLLER_MAX)
		{
			g_TutorialFlg = FALSE;
			PLAYER* parts = GetPlayerParts();
			for (int i = 0; i < PLAYER_PARTS_MAX; i++)
			{
				parts[i].time[PLAYER_ANIM_MOVE] = 0.0f;
			}
		}
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		tutorialNum++;
		if (tutorialNum >= CONTROLLER_MAX)
		{
			g_TutorialFlg = FALSE;
			PLAYER* parts = GetPlayerParts();
			for (int i = 0; i < PLAYER_PARTS_MAX; i++)
			{
				parts[i].time[PLAYER_ANIM_MOVE] = 0.0f;
			}
		}
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		tutorialNum++;
		if (tutorialNum >= CONTROLLER_MAX)
		{
			g_TutorialFlg = FALSE;
			PLAYER* parts = GetPlayerParts();
			for (int i = 0; i < PLAYER_PARTS_MAX; i++)
			{
				parts[i].time[PLAYER_ANIM_MOVE] = 0.0f;
			}
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	SetMaterial(material);

	// リザルトの背景を描画
	float rx = g_Pos.x;
	float ry = g_Pos.y;
	float rw = g_w;
	float rh = g_h;

	// アニメーション用
	float tw = 1.0f;	// テクスチャの幅
	float th = 1.0f;	// テクスチャの高さ
	float tx = 0.0f;	// テクスチャの左上X座標
	float ty = 0.0f;	// テクスチャの左上Y座標

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLeftTop(g_VertexBuffer, rx, ry, rw, rh, tx, ty, tw, th);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// リザルトのロゴを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1 + tutorialNum]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		rx, ry, rw, rh,
		tx, ty, tw, th,
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

BOOL GetTutorialFlg(void)
{
	return g_TutorialFlg;
}

void SetTutorialFlg(void)
{
	g_TutorialFlg = TRUE;
}