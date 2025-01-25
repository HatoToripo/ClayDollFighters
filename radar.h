//=============================================================================
//
// 検知範囲描画処理 [radar.h]
// Author : 
//
//=============================================================================
#pragma once
class RADAR
{
public:
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	rot;			// 回転
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		width;			// 幅
	float		height;			// 高さ
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRadar(void);
void UninitRadar(void);
void UpdateRadar(void);
void DrawRadar(void);
//void SetSectorData(SECTOR* sector);
void SetRadarFlag(BOOL flag);


