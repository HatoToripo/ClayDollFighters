//=============================================================================
//
// 検知フラグ処理 [arrow.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "arrow.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(1)				// テクスチャの数

#define	TEXTURE_WIDTH			(5.0f)			// 頂点サイズ
#define	TEXTURE_HEIGHT			(5.0f)			// 頂点サイズ

#define	VALUE_MOVE				(5.0f)			// プレイヤーを中心の円の半径
#define	ARROW_Y					(-4.0f)		// プレイヤーを中心の円の半径
#define RADIAN					(XM_PI / 180.0f)					// ラジアン変換用

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexArrow(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static ARROW					g_Arrow[ENEMY_MAX];	// マークワーク

static int					g_TexNo;			// テクスチャ番号

static char *g_TextureName[] =
{
	"data/TEXTURE/arrow.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitArrow(void)
{
	MakeVertexArrow();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// マークワークの初期化
	for(int i = 0; i < ENEMY_MAX; i++)
	{
		// 検知フラグの初期化
		ZeroMemory(&g_Arrow[i].material, sizeof(g_Arrow[i].material));
		g_Arrow[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Arrow[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Arrow[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Arrow[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Arrow[i].width = TEXTURE_WIDTH;
		g_Arrow[i].height = TEXTURE_HEIGHT;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitArrow(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateArrow(void)
{
	PLAYER* player = GetPlayer();
	if (player->use == TRUE)
	{
		ENEMY* enemy = GetEnemy();
		ENEMY* boss = GetBoss();

		// エネミーの方向にセット
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == FALSE) continue;
			float x = (player->pos.x - enemy[i].pos.x);
			float z = (player->pos.z - enemy[i].pos.z);
			float rad = atan2f(z, x);

			g_Arrow[i].pos.x = player->pos.x - cosf(rad) * VALUE_MOVE;
			g_Arrow[i].pos.y = player->pos.y + ARROW_Y;
			g_Arrow[i].pos.z = player->pos.z - sinf(rad) * VALUE_MOVE;

			g_Arrow[i].rot.y = -rad;
		}

		// ボスの方向にセット
		for (int i = 0; i < BOSS_MAX; i++)
		{
			if (boss[i].use == FALSE) continue;
			float x = (player->pos.x - boss[i].pos.x);
			float z = (player->pos.z - boss[i].pos.z);
			float rad = atan2f(z, x);

			g_Arrow[i].pos.x = player->pos.x - cosf(rad) * VALUE_MOVE;
			g_Arrow[i].pos.y = player->pos.y + ARROW_Y;
			g_Arrow[i].pos.z = player->pos.z - sinf(rad) * VALUE_MOVE;

			g_Arrow[i].rot.y = -rad;
		}
	}

#ifdef _DEBUG

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawArrow(void)
{
	PLAYER* player = GetPlayer();
	if (player->use == TRUE)
	{
		ENEMY* enemy = GetEnemy();
		ENEMY* boss = GetBoss();
		// αテスト設定
		// αテストを有効に
		SetAlphaTestEnable(TRUE);

		// ライティングを無効
		SetLightEnable(FALSE);

		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxTranslate, mtxRot, mtxWorld, mtxView;
		CAMERA* cam = GetCamera();

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == FALSE) continue;
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// スケールを反映
			mtxScl = XMMatrixScaling(g_Arrow[i].scl.x, g_Arrow[i].scl.y, g_Arrow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Arrow[i].rot.x, g_Arrow[i].rot.y, g_Arrow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Arrow[i].pos.x, g_Arrow[i].pos.y, g_Arrow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_Arrow[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}

		// ボスの方向
		for (int i = 0; i < BOSS_MAX; i++)
		{
			if (boss[i].use == FALSE) continue;
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// スケールを反映
			mtxScl = XMMatrixScaling(g_Arrow[i].scl.x, g_Arrow[i].scl.y, g_Arrow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Arrow[i].rot.x, g_Arrow[i].rot.y, g_Arrow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Arrow[i].pos.x, g_Arrow[i].pos.y, g_Arrow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_Arrow[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}

		// カリング設定を戻す
		SetCullingMode(CULL_MODE_BACK);

		// ライティングを有効に
		SetLightEnable(TRUE);

		// αテストを無効に
		SetAlphaTestEnable(FALSE);
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexArrow(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = TEXTURE_WIDTH;
	float fHeight = TEXTURE_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}
