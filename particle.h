//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************

class PARTICLE
{
public:
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scale;			// スケール
	XMFLOAT3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			width;			// 幅
	float			height;			// 高さ
	BOOL			use;			// 使用しているかどうか

};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

void SetParticle(XMFLOAT3 pos, XMFLOAT4 col);
int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float width, float height);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

