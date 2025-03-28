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
#define PLAYER_MAX	(2)					// プレイヤーの数

#define	PLAYER_SIZE		(3.0f)				// 当たり判定の大きさ
#define	PLAYER_HP_MAX	(10)				// プレイヤーの体力の最大値
#define PLAYER_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる

enum
{
	PLAYER_PARTS_HEAD,
	PLAYER_PARTS_ARM_L,
	PLAYER_PARTS_ARM_R,
	PLAYER_PARTS_HAND_L,
	PLAYER_PARTS_HAND_R,
	PLAYER_PARTS_LEG_L,
	PLAYER_PARTS_LEG_R,
	PLAYER_PARTS_FOOT_L,
	PLAYER_PARTS_FOOT_R,
	PLAYER_PARTS_SWORD_R,
	PLAYER_PARTS_SWORD_B,
	PLAYER_PARTS_SCABBARD,

	PLAYER_PARTS_MAX
};

enum
{
	PLAYER_ANIM_STOP,
	PLAYER_ANIM_MOVE,
	PLAYER_ANIM_DASH,
	PLAYER_ANIM_JUMP,
	PLAYER_ANIM_ATTACK,

	PLAYER_ANIM_MAX
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

	XMFLOAT2			dir;				// プレイヤーの方向

	int					hp;					// 体力
	int					gauge;				// ゲージ
	float				spd;				// 移動スピード
	int					colCnt;				// 無敵時間

	BOOL				attack;				// アタックフラグ
	int					atkVal;				// 攻撃力
	int					atkCnt;				// アタック中のカウント

	BOOL				load;
	DX11_MODEL			model;				// モデル情報
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// モデルの色
	float				dissolveCnt;		// ディゾルブのカウント

	int					shadowIdx;			// 影のインデックス番号

	BOOL				use;

	float				size;

	// 階層アニメーション用のメンバー変数
	float				time[PLAYER_ANIM_MAX];				// 線形補間用
	int					tblNo[PLAYER_ANIM_MAX];				// 行動データのテーブル番号
	int					tblMax[PLAYER_ANIM_MAX];			// そのテーブルのデータ数
	int					animNum;							// 現在のアニメーション番号

	BOOL				jump;			// ジャンプフラグ
	int					jumpCnt;		// ジャンプ中のカウント

	// 親は、NULL、子供は親のアドレスを入れる
	PLAYER				*parent;			// 自分が親ならNULL、自分が子供なら親のplayerアドレス

	// クォータニオン
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// 自分が立っている所

	void Animation(int animNum);
	void Animation(int animNum1, int animNum2);
	void DecHP(int atk);
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
PLAYER* GetPlayerParts(void);
float NormalizeAngle(float angle);
float Turn(float target, float current);

//void PlayerAnimation(int animNum, int i);
//void PlayerAnimationBlend(int animNum1, int animNum2, int i);

