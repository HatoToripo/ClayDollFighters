//=============================================================================
//
// 検知範囲描画処理 [radar.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "radar.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(1)				// テクスチャの数

#define SECTOR_DIVIDE			(100)			// 扇形の分割数
#define	TEXTURE_WIDTH			(LOOK_CIRCLE / SECTOR_DIVIDE * 2)			// 頂点サイズ
#define	TEXTURE_HEIGHT			(LOOK_CIRCLE)			// 頂点サイズ
#define RADIAN					(XM_PI / 180.0f)					// ラジアン変換用
#define	VALUE_MOVE				(10.0f)			// エネミーを中心の円の半径
#define VALUE_ROTATE			(LOOK_CIRCLE_RAD / SECTOR_DIVIDE)

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexRadar(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static RADAR			g_Radar[ENEMY_MAX];	// 検知範囲ワーク
//static SECTOR			g_Sector[ENEMY_MAX];	// 検知範囲ワーク

static int					g_TexNo;			// テクスチャ番号

static BOOL					g_RadarFlg;		// レーダーフラグ

static char *g_TextureName[] =
{
	"data/TEXTURE/rectangle.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRadar(void)
{
	MakeVertexRadar();

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
		ZeroMemory(&g_Radar[i].material, sizeof(g_Radar[i].material));
		g_Radar[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Radar[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Radar[i].rot = XMFLOAT3(RADIAN * 90.0f, 0.0f, RADIAN * 90.0f);
		g_Radar[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Radar[i].width = TEXTURE_WIDTH;
		g_Radar[i].height = TEXTURE_HEIGHT;

		//g_Sector[i].startAngle = 0.0f;
		//g_Sector[i].endAngle = 0.0f;
		//g_Sector[i].flag = false;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitRadar(void)
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
void UpdateRadar(void)
{
	if (GetKeyboardTrigger(DIK_J))
	{
		if (g_RadarFlg == TRUE)
		{
			g_RadarFlg = FALSE;
		}
		else
		{
			g_RadarFlg = TRUE;
		}
	}


	ENEMY* enemy = GetEnemy();
	for(int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == FALSE) continue;
		// エネミーの頭上に座標を更新
		g_Radar[i].pos = enemy[i].pos;
		g_Radar[i].pos.y = 0.1f;
		// エネミーの前にセット
		g_Radar[i].rot.y = enemy[i].rot.y - RADIAN * 90.0f - LOOK_CIRCLE_RAD * 0.5f;
		
		//// 描画する扇形の角度をセット
		//g_Sector[i].startAngle = g_Radar[i].rot.y - LOOK_CIRCLE_RAD * 0.5f;
		//g_Sector[i].endAngle = LOOK_CIRCLE_RAD;
	}
#ifdef _DEBUG

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawRadar(void)
{
	if (g_RadarFlg == TRUE)
	{
		ENEMY* enemy = GetEnemy();
		// αテスト設定
	// αテストを有効に
		SetAlphaTestEnable(TRUE);

		// ライティングを無効
		SetLightEnable(FALSE);

		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		// 扇形フラグ有効
		//SetSectorFlag(TRUE);

		XMMATRIX mtxScl, mtxTranslate, mtxRot, mtxWorld;

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == FALSE) continue;
			for (int j = 0; j < SECTOR_DIVIDE; j++)
			{
				// ワールドマトリックスの初期化
				mtxWorld = XMMatrixIdentity();

				// スケールを反映
				mtxScl = XMMatrixScaling(g_Radar[i].scl.x, g_Radar[i].scl.y, g_Radar[i].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// 回転を反映
				mtxRot = XMMatrixRotationRollPitchYaw(g_Radar[i].rot.x, g_Radar[i].rot.y + VALUE_ROTATE * j, g_Radar[i].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// 移動を反映
				mtxTranslate = XMMatrixTranslation(g_Radar[i].pos.x, g_Radar[i].pos.y, g_Radar[i].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				// ワールドマトリックスの設定
				SetWorldMatrix(&mtxWorld);

				// マテリアル設定
				SetMaterial(g_Radar[i].material);

				//SetSectorData(&g_Sector[i]);

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

				// ポリゴンの描画
				GetDeviceContext()->Draw(4, 0);
			}

		}

		// 扇形フラグ無効
		//SetSectorFlag(FALSE);

		// ライティングを有効に
		SetLightEnable(TRUE);

		// αテストを無効に
		SetAlphaTestEnable(FALSE);
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexRadar(void)
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

//=============================================================================
// 扇形の設定
//=============================================================================
//void SetSectorData(SECTOR* sector)
//{
//	SetSector(sector);
//}

void SetRadarFlag(BOOL flag)
{
	g_RadarFlg = flag;
}