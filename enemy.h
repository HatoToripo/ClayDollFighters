//=============================================================================
//
// エネミーモデル処理 [enemy.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_MAX		(3)					// エネミーの数

#define	ENEMY_SIZE		(4.0f)				// 当たり判定の大きさ

enum
{
	ENEMY_PARTS_HEAD,
	ENEMY_PARTS_ARM_L,
	ENEMY_PARTS_ARM_R,
	ENEMY_PARTS_HAND_L,
	ENEMY_PARTS_HAND_R,
	ENEMY_PARTS_LEG_L,
	ENEMY_PARTS_LEG_R,
	ENEMY_PARTS_FOOT_L,
	ENEMY_PARTS_FOOT_R,
	//ENEMY_PARTS_SWORD_R,
	//ENEMY_PARTS_SWORD_B,
	//ENEMY_PARTS_SCABBARD,

	ENEMY_PARTS_MAX
};

enum
{
	ENEMY_ANIM_STOP,
	ENEMY_ANIM_MOVE,
	ENEMY_ANIM_DASH,
	//ENEMY_ANIM_JUMP,
	//ENEMY_ANIM_ATTACK,

	ENEMY_ANIM_MAX
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************
class ENEMY
{
public:
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float				spd;				// 移動スピード
	float				size;				// 当たり判定の大きさ
	int					shadowIdx;			// 影のインデックス番号
	BOOL				look;				// プレイヤーを発見した状態

	// 階層アニメーション用のメンバー変数
	float				time[ENEMY_ANIM_MAX];				// 線形補間用
	int					tblNo[ENEMY_ANIM_MAX];				// 行動データのテーブル番号
	int					tblMax[ENEMY_ANIM_MAX];				// そのテーブルのデータ数
	int					animNum;							// 現在のアニメーション番号

	// 親は、NULL、子供は親のアドレスを入れる
	ENEMY* parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// アニメーション関数
	void Animation(int animNum);
	void BodyAnimation(int i);
	void Animation(int animNum1, int animNum2);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

HRESULT MakeVertexLookFlg(void);
