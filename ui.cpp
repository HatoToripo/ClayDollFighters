//=============================================================================
//
// UI表示処理 [ui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "ui.h"
#include "player.h"
#include "enemy.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(16)	// HPサイズ
#define TEXTURE_HEIGHT				(32)	// 

#define GAUGE_WIDTH					(32.0f * PLAYER_HP_MAX)	// ゲージサイズ
#define GAUGE_HEIGHT				(32)	// 
#define TEXTURE_MAX					(4)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)	// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/heart.png",
	"data/TEXTURE/bar_white.png",
	"data/TEXTURE/bar_blue.png",
	"data/TEXTURE/pipo-hikarimono002.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_Hp;					// スコア

static BOOL						g_Load = FALSE;
static int						g_AnimNum;					// エフェクトのアニメーション
static float					g_CountAnim;				// アニメーションカウンタ

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUI(void)
{
	ID3D11Device *pDevice = GetDevice();

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


	// プレイヤーの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 10.0f, 20.0f, 0.0f };
	g_TexNo = 0;

	g_Hp = PLAYER_HP_MAX;	// 体力の初期化

	g_CountAnim = 0.0f;
	g_AnimNum = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void)
{
	PLAYER* player = GetPlayer();
	g_Hp = player->hp;
	g_TexNo = 0;
	g_AnimNum = (g_AnimNum + 1) % 10;
	g_CountAnim += 1.0f;
	if (g_CountAnim > ANIM_WAIT)
	{
		g_CountAnim = 0.0f;
		// パターンの切り替え
		g_AnimNum = (g_AnimNum + 1) % ANIM_PATTERN_NUM;
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUI(void)
{
	PLAYER* player = GetPlayer();

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

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// 桁数分処理する
	for (int i = 0; i < g_Hp; i++)
	{
		// 今回表示する桁の数字
		float x = (float)((i % 2));

		// HPの位置やテクスチャー座標を反映
		float px = g_Pos.x + g_w*i;	// HPの表示位置X
		float py = g_Pos.y;			// HPの表示位置Y
		float pw = g_w;				// HPの表示幅
		float ph = g_h;				// HPの表示高さ

		float tw = 0.5f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = x * tw;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	g_TexNo++;

	// ゲージの描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

		float px = g_Pos.x;	// ゲージの表示位置X
		float py = g_Pos.y + TEXTURE_HEIGHT;			// ゲージの表示位置Y
		float pw = GAUGE_WIDTH;				// ゲージの表示幅
		float ph = g_h;				// ゲージの表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		g_TexNo++;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		pw = pw * 1.0f / ENEMY_MAX * player->gauge;				// ゲージの表示幅

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		g_TexNo++;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
		px += pw * 0.5f;
		py -= 5.0f;
		pw = 20.0f;
		ph += 20.0f;
		tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		tx = (float)(g_AnimNum % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
		ty = (float)(g_AnimNum / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}
