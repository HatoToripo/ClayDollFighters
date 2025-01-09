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
#define ENEMY_MAX		(10)					// エネミーの数
#define BOSS_MAX		(1)						// ボスの数

#define	ENEMY_SIZE		(4.0f)				// 当たり判定の大きさ
#define	BOSS_SPONE_TIME	(180)				// ボス出現後カメラに映るフレーム
#define ENEMY_OFFSET_Y		(7.0f)						// エネミーの足元をあわせる

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
	ENEMY_PARTS_SWORD_R,
	//ENEMY_PARTS_SWORD_B,
	//ENEMY_PARTS_SCABBARD,

	ENEMY_PARTS_MAX
};

enum
{
	ENEMY_ANIM_STOP,
	ENEMY_ANIM_MOVE,
	ENEMY_ANIM_ATTACK,

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

	int					hp;					// エネミーの体力
	BOOL				attack;				// アタックフラグ
	int					atkVal;				// 攻撃力
	int					atkCnt;				// アタック中のカウント
	float				spd;				// 移動スピード
	int					moveCnt;			// 移動する間隔
	BOOL				moveFlg;			// 移動フラグ

	int					colCnt;				// 無敵時間
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
	void Animation(int animNum1, int animNum2);
	int DecHP(int atk);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
ENEMY *GetEnemyParts(void);
ENEMY *GetBoss(void);
void ColCheck(XMFLOAT3 pos, ENEMY& enemy, ENEMY parts[ENEMY_PARTS_MAX]);
BOOL GetBossFlg(void);
void AttackCheck(ENEMY& enemy, ENEMY parts[ENEMY_PARTS_MAX]);
void SetBoss(void);
int GetBossSponeCnt(void);
void AddBossSponeCnt(void);
void ReleaseEnemy(void);
