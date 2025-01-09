//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : 
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "score.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(2)				// テクスチャの数

#define TEXTURE_LOGO_WIDTH			(100.0f)			// ロゴの縦幅
#define TEXTURE_LOGO_HEIGHT			(50.0f)			// ロゴの横幅

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(10)	// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

#define TEXTURE_GOTITLE_X			((SCREEN_WIDTH - TEXTURE_LOGO_WIDTH) * 0.5)		// gotitleのX座標
#define TEXTURE_GOTITLE_Y			(375.0f)							// gotitleのY座標
#define GOTEITLE_WAIT				(300.0f)					// 描画する待ち時間

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/congratulations.png",
	"data/TEXTURE/setting_gotitle.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static float					g_CountAnim;				// アニメーションカウンタ
static int						g_PatternAnim;				// アニメーションパターン

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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
	g_Pos = { g_w / 2, 50.0f, 0.0f };
	g_CountAnim = 0.0f;
	g_PatternAnim = 0;
	g_TexNo = 0;

	// BGM再生


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	g_CountAnim += 1.0f;
	if (g_CountAnim > ANIM_WAIT)
	{
		g_CountAnim = 0.0f;
		// パターンの切り替え
		g_PatternAnim = (g_PatternAnim + 1) % ANIM_PATTERN_NUM;
	}

	if (GetKeyboardTrigger(DIK_RETURN))
	{// Enter押したら、ステージを切り替える
		SetFade(FADE_OUT, MODE_TITLE);
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_TITLE);
	}


#ifdef _DEBUG	// デバッグ情報を表示する

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// リザルトの背景を描画
	float rx = 0.0f;
	float ry = 0.0f;
	float rw = SCREEN_WIDTH;
	float rh = SCREEN_HEIGHT;

	// アニメーション用
	float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
	float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
	float tx = (float)(g_PatternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
	float ty = (float)(g_PatternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLeftTop(g_VertexBuffer, rx, ry, rw, rh, tx, ty, tw, th);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);

	// リザルトのロゴを描画
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

	rx = TEXTURE_GOTITLE_X;
	ry = TEXTURE_GOTITLE_Y;
	rw = TEXTURE_LOGO_WIDTH;
	rh = TEXTURE_LOGO_HEIGHT;

	tw = 1.0f;
	th = 1.0f;
	tx = 0.0f;
	ty = 0.0f;

	// １枚のポリゴンの頂点とテクスチャ座標を設定
	SetSpriteLTColor(g_VertexBuffer,
		rx, ry, rw, rh,
		tx, ty, tw, th,
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}




