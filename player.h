//=============================================================================
//
// モデル処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)					// プレイヤーの数

#define	PLAYER_SIZE		(5.0f)				// 当たり判定の大きさ

enum
{
	PARTS_HEAD,
	PARTS_ARM_L,
	PARTS_ARM_R,
	PARTS_HAND_L,
	PARTS_HAND_R,
	PARTS_LEG_L,
	PARTS_LEG_R,
	PARTS_FOOT_L,
	PARTS_FOOT_R,

	PLAYER_PARTS_MAX
};

enum
{
	ANIM_STOP,
	ANIM_MOVE,
	ANIM_DASH,
	ANIM_JUMP,

	ANIM_MAX
};
//*****************************************************************************
// 構造体定義
//*****************************************************************************
class PLAYER
{
public:
	XMFLOAT4X4			mtxWorld;			// ワールドマトリックス
	XMFLOAT3			pos;				// モデルの位置
	XMFLOAT3			rot;				// モデルの向き(回転)
	XMFLOAT3			scl;				// モデルの大きさ(スケール)

	float				spd;				// 移動スピード
	
	BOOL				load;
	DX11_MODEL			model;				// モデル情報

	int					shadowIdx;			// 影のインデックス番号

	BOOL				use;

	float				size;

	// 階層アニメーション用のメンバー変数
	float				time[ANIM_MAX];				// 線形補間用
	int					tblNo[ANIM_MAX];				// 行動データのテーブル番号
	int					tblMax[ANIM_MAX];				// そのテーブルのデータ数

	BOOL				jump;			// ジャンプフラグ
	int					jumpCnt;		// ジャンプ中のカウント

	// 親は、NULL、子供は親のアドレスを入れる
	PLAYER				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所

};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
void Animation(int animNum, int i);
void AnimationBlend(int animNum1, int animNum2, int i);

