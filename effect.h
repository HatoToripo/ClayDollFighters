//=============================================================================
//
// エフェクト処理 [effect.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	EFFECT_WIDTH			(40.0f)			// 頂点サイズ
#define	EFFECT_HEIGHT			(40.0f)			// 頂点サイズ

enum
{
	EFFECT_HIT,
	EFFECT_DEAD,
	EFFECT_SPONE,

	EFFECT_NUM
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************
class EFFECT
{
public:
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		width;			// 幅
	float		height;		// 高さ
	BOOL		use;			// 使用しているかどうか
	int			texNum;			// テクスチャ番号
	int			texAnim;		// アニメーションカウンタ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEffect(void);
void UninitEffect(void);
void UpdateEffect(void);
void DrawEffect(void);

int SetEffect(XMFLOAT3 pos, float fWidth, float fHeight, int effectNum);


