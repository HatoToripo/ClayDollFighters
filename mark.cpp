//=============================================================================
//
// 検知フラグ処理 [mark.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "mark.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(2)				// テクスチャの数

#define	TEXTURE_WIDTH			(10.0f)			// 頂点サイズ
#define	TEXTURE_HEIGHT			(20.0f)			// 頂点サイズ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class MARK
{
public:
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		width;			// 幅
	float		height;			// 高さ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexMark(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static MARK					g_Mark[ENEMY_MAX];	// マークワーク

static int					g_TexNo;			// テクスチャ番号

static char *g_TextureName[] =
{
	"data/TEXTURE/lookFlg0.png",
	"data/TEXTURE/lookFlg1.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMark(void)
{
	MakeVertexMark();

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
		ZeroMemory(&g_Mark[i].material, sizeof(g_Mark[i].material));
		g_Mark[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Mark[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Mark[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Mark[i].width = TEXTURE_WIDTH;
		g_Mark[i].height = TEXTURE_HEIGHT;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMark(void)
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
void UpdateMark(void)
{
	ENEMY* enemy = GetEnemy();
	for(int i = 0; i < ENEMY_MAX; i++)
	{
		// エネミーの頭上に座標を更新
		g_Mark[i].pos = enemy[i].pos;
	}


#ifdef _DEBUG

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMark(void)
{
	ENEMY* enemy = GetEnemy();
	// αテスト設定
	// αテストを有効に
	SetAlphaTestEnable(TRUE);

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == FALSE) continue;
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// ビューマトリックスを取得
		mtxView = XMLoadFloat4x4(&cam->mtxView);


		// 関数使った版
		//mtxWorld = XMMatrixInverse(nullptr, mtxView);
		//mtxWorld.r[3].m128_f32[0] = 0.0f;
		//mtxWorld.r[3].m128_f32[1] = 0.0f;
		//mtxWorld.r[3].m128_f32[2] = 0.0f;


		// 正方行列（直交行列）を転置行列させて逆行列を作ってる版(速い)
		mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
		mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
		mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

		mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
		mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
		mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

		mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
		mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
		mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];


		// スケールを反映
		mtxScl = XMMatrixScaling(g_Mark[i].scl.x, g_Mark[i].scl.y, g_Mark[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Mark[i].pos.x, g_Mark[i].pos.y, g_Mark[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);


		// マテリアル設定
		SetMaterial(g_Mark[i].material);

		// テクスチャ設定
		int texNo = 0;
		if (enemy[i].look == TRUE)	// 検知状態の時はテクスチャを変更
		{
			texNo = 1;
		}

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

		// ポリゴンの描画
		GetDeviceContext()->Draw(4, 0);
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// αテストを無効に
	SetAlphaTestEnable(FALSE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexMark(void)
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
