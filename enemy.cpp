//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "debugproc.h"
#include "enemy.h"
#include "player.h"
#include "particle.h"
#include "effect.h"
#include "camera.h"
#include "fade.h"
#include "collision.h"
#include "shadow.h"
#include <time.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY		"data/MODEL/enemy_body.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_ARM_L	"data/MODEL/enemy_arm_l.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_ARM_R	"data/MODEL/enemy_arm_r.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_HAND_L	"data/MODEL/enemy_hand_l.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_HAND_R	"data/MODEL/enemy_hand_r.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_LEG_L	"data/MODEL/enemy_leg_l.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_LEG_R	"data/MODEL/enemy_leg_r.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_FOOT_L	"data/MODEL/enemy_foot_l.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_FOOT_R	"data/MODEL/enemy_foot_r.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_HEAD	"data/MODEL/enemy_head.obj"			// 読み込むモデル名
#define	MODEL_ENEMY_SWORD_R	"data/MODEL/enemy_sword.obj"			// 読み込むモデル名
//#define	MODEL_ENEMY_SWORD_B	"data/MODEL/enemy_sword_back.obj"			// 読み込むモデル名
//#define	MODEL_ENEMY_SCABBARD	"data/MODEL/enemy_scabbard.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(0.2f)						// 移動量
#define	VALUE_BACK			(1.5f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量
#define	VALUE_DISTANCE		(100.0f)					// 動く距離
#define	VALUE_FRAME			(VALUE_DISTANCE / VALUE_MOVE)	// 動く距離

#define	BARRIER_FRAME		(40)						// 無敵時間
#define	BARRIER_WHITE		(BARRIER_FRAME / 5)			// 無敵時間の点滅間隔

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ

#define RADIAN				(XM_PI / 180.0f)					// ラジアン変換用

#define ANIM_FRAME_STOP		(15.0f)							// 待機アニメーションの間隔
#define ANIM_FRAME_MOVE		(30.0f)							// 移動アニメーションの間隔
#define ANIM_FRAME_DASH		(ANIM_FRAME_MOVE * 0.5f)		// ダッシュアニメーションの間隔

#define ANIM_FRAME_JUMP		(15.0f)							// ジャンプアニメーションの間隔
#define ANIM_FRAME_ATTACK	(15.0f)							// ジャンプアニメーションの間隔

#define ENEMY_HEAD_Y		(2.7f)							// 頭のx座標

#define ENEMY_ARM_X			(1.5f)							// 腕のx座標
#define ENEMY_ARM_Y			(2.5f)							// 腕のy座標

#define ENEMY_HAND_X		(0.0f)							// 手のx座標
#define ENEMY_HAND_Y		(-1.5f)							// 手のy座標

#define ENEMY_LEG_X			(1.1f)							// 脚のx座標
#define ENEMY_LEG_Y			(-2.4f)							// 脚のy座標

#define ENEMY_FOOT_X		(0.0f)							// 足のx座標
#define ENEMY_FOOT_Y		(-1.8f)							// 足のy座標

#define ENEMY_SWORD_R_X		(0.0f)							// 剣のx座標
#define ENEMY_SWORD_R_Y		(-2.0f)							// 剣のy座標

#define ENEMY_SWORD_B_X		(0.0f)							// 剣(背中)のx座標
#define ENEMY_SWORD_B_Y		(1.0f)							// 剣(背中)のy座標
#define ENEMY_SWORD_B_Z		(-1.5f)							// 剣(背中)のz座標

#define BLEND_FRAME_STOP	(60.0f)							// 待機モーションに遷移するまでの時間
#define BLEND_FRAME_MOVE	(90.0f)							// 移動モーションに遷移するまでの時間
#define BLEND_FRAME_DASH	(90.0f)							// ダッシュモーションに遷移するまでの時間
#define BLEND_FRAME_ATTACK	(30.0f)							// 攻撃モーションに遷移するまでの時間

#define MOVE_TBL_MAX		(2)								// 移動アニメーションのテーブル数
#define MOVE_INTERVAL		(60 * 3)						// 次の角度を計算するタイミング
#define ROT_INTERVAL		(5)						// 次の角度を計算するタイミング

#define TEXTURE_MAX			(2)				// テクスチャの数
#define TEXTURE_WIDTH		(80.0f)			// テクスチャの横幅
#define TEXTURE_HEIGHT		(80.0f)			// テクスチャの縦幅

#define ATTACK_WIDTH		(5.0f)							// 攻撃の当たり判定の幅
#define ATTACK_DEPTH		(6.0f)							// 攻撃の当たり判定の奥行き
#define ENEMY_ATK_CNT_MAX	(ANIM_FRAME_ATTACK * 3.0f)			// アタック全体フレーム

#define BOSS_SIZE_VALUE			(2.0f)			// ボスのサイズ倍率

#define DISSOLVE_VALUE			(2.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[ENEMY_MAX];				// エネミー

static ENEMY			g_Parts[ENEMY_MAX][ENEMY_PARTS_MAX];				// エネミー

static ENEMY			g_Boss[BOSS_MAX];				// ボス

static ENEMY			g_BossParts[BOSS_MAX][ENEMY_PARTS_MAX];				// ボスのパーツ

static DISSOLVE			g_EnemyDissolve[ENEMY_MAX];				// エネミーのディゾルブ
static DISSOLVE			g_EnemyPartsDissolve[ENEMY_MAX][ENEMY_PARTS_MAX];				// エネミーのパーツのディゾルブ

static BOOL				g_BossFlg;						// ボス戦突入フラグ
static int				g_BossSponeCnt;						// ボス出現カウンタ
int g_Enemy_load = 0;

static INTERPOLATION_DATA stop_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, ENEMY_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(0.0f, ENEMY_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y + 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y - 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y - 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y + 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },

};

static INTERPOLATION_DATA stop_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },

};

static INTERPOLATION_DATA stop_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};



static INTERPOLATION_DATA* g_StopTblAdr[] =
{
	stop_tbl_head,
	stop_tbl_arm_l,
	stop_tbl_arm_r,
	stop_tbl_hand_l,
	stop_tbl_hand_r,
	stop_tbl_leg_l,
	stop_tbl_leg_r,
	stop_tbl_foot_l,
	stop_tbl_foot_r,
	stop_tbl_sword_r,
};

// 歩きアニメデータ
static INTERPOLATION_DATA move_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, ENEMY_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(0.0f, ENEMY_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 40.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(RADIAN * 20.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 30.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 30.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },

};

static INTERPOLATION_DATA move_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 15.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(-RADIAN * 15.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(-RADIAN * 15.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 15.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },

};

static INTERPOLATION_DATA move_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f,  0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f,  0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};


static INTERPOLATION_DATA* g_MoveTblAdr[] =
{
	move_tbl_head,
	move_tbl_arm_l,
	move_tbl_arm_r,
	move_tbl_hand_l,
	move_tbl_hand_r,
	move_tbl_leg_l,
	move_tbl_leg_r,
	move_tbl_foot_l,
	move_tbl_foot_r,
	move_tbl_sword_r,
};

// ダッシュアニメデータ
static INTERPOLATION_DATA dash_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, ENEMY_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(0.0f, ENEMY_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 80.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 80.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 80.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 80.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },

};

static INTERPOLATION_DATA dash_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },

};

static INTERPOLATION_DATA dash_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};


static INTERPOLATION_DATA* g_DashTblAdr[] =
{
	dash_tbl_head,
	dash_tbl_arm_l,
	dash_tbl_arm_r,
	dash_tbl_hand_l,
	dash_tbl_hand_r,
	dash_tbl_leg_l,
	dash_tbl_leg_r,
	dash_tbl_foot_l,
	dash_tbl_foot_r,
	dash_tbl_sword_r,
};

// アタックモーションデータ
static INTERPOLATION_DATA attack_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, ENEMY_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(0.0f, ENEMY_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f , 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_ARM_X, ENEMY_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_HAND_X, ENEMY_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_LEG_X, ENEMY_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },

};

static INTERPOLATION_DATA attack_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_FOOT_X, ENEMY_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },

};

static INTERPOLATION_DATA attack_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(-RADIAN * 15.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA* g_AttackTblAdr[] =
{
	attack_tbl_head,
	attack_tbl_arm_l,
	attack_tbl_arm_r,
	attack_tbl_hand_l,
	attack_tbl_hand_r,
	attack_tbl_leg_l,
	attack_tbl_leg_r,
	attack_tbl_foot_l,
	attack_tbl_foot_r,
	attack_tbl_sword_r,
};


// 選択したアニメーションに移行するフレーム
static float g_AnimTransFrame[ENEMY_ANIM_MAX] =
{
	BLEND_FRAME_STOP,
	BLEND_FRAME_MOVE,
	BLEND_FRAME_ATTACK,
};

static float g_AnimTransFrameCnt[ENEMY_ANIM_MAX];			// アニメーションに移行するフレームのカウンタ

static int g_AnimNum[ENEMY_PARTS_MAX];		// 現在のアニメーション番号

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	srand((unsigned int)time(NULL));

	// ボス戦フラグをリセット
	g_BossFlg = FALSE;
	g_BossSponeCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-140.0f + i * 15.0f + rand() % 100, ENEMY_OFFSET_Y, -140.0f + i * 15.0f + rand() % 100);
		g_Enemy[i].rot = XMFLOAT3(0.0f, rand() % 360 * RADIAN, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].hp  = 3;				// 体力初期化
		g_Enemy[i].attack = FALSE;		// アタックフラグクリア
		g_Enemy[i].atkVal = 1;			// 攻撃力初期化
		g_Enemy[i].atkCnt = 0;			// アタックカウンタ初期化
		g_Enemy[i].spd  = 0.0f;			// 移動スピードクリア
		g_Enemy[i].moveCnt  = 0;		// 移動間隔クリア
		g_Enemy[i].moveFlg  = TRUE;	// 移動フラグクリア
		g_Enemy[i].colCnt  = 0;			// 無敵時間クリア
		g_Enemy[i].dissolveCnt = 0.0f;			// 無敵時間クリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		//XMFLOAT3 pos = g_Enemy[i].pos;
		//pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		//g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);
		
		g_Enemy[i].use = TRUE;			// TRUE:生きてる
		g_Enemy[i].parent = NULL;

		g_EnemyDissolve[i].edgeColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = FALSE;

			// 位置・回転・スケールの初期設定
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Parts[i][j].parent = &g_Enemy[i];		// ← ここに親のアドレスを入れる

				// 階層アニメーション用のメンバー変数の初期化
			for (int k = 0; k < ENEMY_ANIM_MAX; k++)
			{
				g_Parts[i][j].time[k] = 0.0f;			// 線形補間用のタイマーをクリア
				g_Parts[i][j].tblNo[k] = 0;			// 再生する行動データテーブルNoをセット
				g_Parts[i][j].tblMax[k] = 0;			// 再生する行動データテーブルのレコード数をセット
			}
			// パーツの読み込みはまだしていない
			g_Parts[i][j].load = FALSE;


			// パーツの初期アニメーションを設定
			g_AnimNum[i] = ENEMY_ANIM_STOP;

			g_EnemyPartsDissolve[i][j].edgeColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		g_Parts[i][ENEMY_PARTS_HEAD].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HEAD].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HEAD].load = 1;
		LoadModel(MODEL_ENEMY_HEAD, &g_Parts[i][ENEMY_PARTS_HEAD].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_HEAD].model, &g_Parts[i][ENEMY_PARTS_HEAD].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_ARM_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_ARM_L].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_L].load = 1;
		LoadModel(MODEL_ENEMY_ARM_L, &g_Parts[i][ENEMY_PARTS_ARM_L].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_ARM_L].model, &g_Parts[i][ENEMY_PARTS_ARM_L].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_ARM_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_ARM_R].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_R].load = 1;
		LoadModel(MODEL_ENEMY_ARM_R, &g_Parts[i][ENEMY_PARTS_ARM_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_ARM_R].model, &g_Parts[i][ENEMY_PARTS_ARM_R].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_HAND_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HAND_L].parent = &g_Parts[i][ENEMY_PARTS_ARM_L];	// 親をセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_L].load = 1;
		LoadModel(MODEL_ENEMY_HAND_L, &g_Parts[i][ENEMY_PARTS_HAND_L].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_HAND_L].model, &g_Parts[i][ENEMY_PARTS_HAND_L].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_HAND_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HAND_R].parent = &g_Parts[i][ENEMY_PARTS_ARM_R];	// 親をセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_R].load = 1;
		LoadModel(MODEL_ENEMY_HAND_R, &g_Parts[i][ENEMY_PARTS_HAND_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_HAND_R].model, &g_Parts[i][ENEMY_PARTS_HAND_R].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_LEG_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_LEG_L].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_L].load = 1;
		LoadModel(MODEL_ENEMY_LEG_L, &g_Parts[i][ENEMY_PARTS_LEG_L].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_LEG_L].model, &g_Parts[i][ENEMY_PARTS_LEG_L].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_LEG_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_LEG_R].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_R].load = 1;
		LoadModel(MODEL_ENEMY_LEG_R, &g_Parts[i][ENEMY_PARTS_LEG_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_LEG_R].model, &g_Parts[i][ENEMY_PARTS_LEG_R].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_FOOT_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_FOOT_L].parent = &g_Parts[i][ENEMY_PARTS_LEG_L];	// 親をセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_L, &g_Parts[i][ENEMY_PARTS_FOOT_L].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_FOOT_L].model, &g_Parts[i][ENEMY_PARTS_FOOT_L].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_FOOT_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_FOOT_R].parent = &g_Parts[i][ENEMY_PARTS_LEG_R];	// 親をセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_R, &g_Parts[i][ENEMY_PARTS_FOOT_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_FOOT_R].model, &g_Parts[i][ENEMY_PARTS_FOOT_R].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_SWORD_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_SWORD_R].parent = &g_Parts[i][ENEMY_PARTS_HAND_R];	// 親をセット
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_SWORD_R].load = 1;
		LoadModel(MODEL_ENEMY_SWORD_R, &g_Parts[i][ENEMY_PARTS_SWORD_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_SWORD_R].model, &g_Parts[i][ENEMY_PARTS_SWORD_R].diffuse[0]);

		//g_Parts[i][ENEMY_PARTS_SWORD_B].use = TRUE;
		//g_Parts[i][ENEMY_PARTS_SWORD_B].parent = &g_Enemy[i];	// 親をセット
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SWORD_B;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SWORD_B;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_SWORD_B;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SWORD_B].load = 1;
		//LoadModel(MODEL_ENEMY_SWORD_B, &g_Parts[i][ENEMY_PARTS_SWORD_B].model);

		//g_Parts[i][ENEMY_PARTS_SCABBARD].use = TRUE;
		//g_Parts[i][ENEMY_PARTS_SCABBARD].parent = &g_Enemy[i];	// 親をセット
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SCABBARD;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SCABBARD;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_SCABBARD;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SCABBARD].load = 1;
		//LoadModel(MODEL_ENEMY_SCABBARD, &g_Parts[i][ENEMY_PARTS_SCABBARD].model);

		// 最初からパーツをくっつけておく
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[i][j].Animation(ENEMY_ANIM_STOP);
		}
	}

	// ボスの初期化
	for (int i = 0; i < BOSS_MAX; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Boss[i].model);
		g_Boss[i].load = TRUE;

		g_Boss[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y * BOSS_SIZE_VALUE, 0.0f);
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(1.0f * BOSS_SIZE_VALUE, 1.0f * BOSS_SIZE_VALUE, 1.0f * BOSS_SIZE_VALUE);

		g_Boss[i].hp = 60;				// 体力初期化
		g_Boss[i].attack = FALSE;		// アタックフラグクリア
		g_Boss[i].atkVal = 2;			// 攻撃力初期化
		g_Boss[i].atkCnt = 0;			// アタックカウンタ初期化
		g_Boss[i].spd = 0.0f;			// 移動スピードクリア
		g_Boss[i].moveCnt = 0;			// 移動間隔クリア
		g_Boss[i].moveFlg = TRUE;		// 移動フラグクリア
		g_Boss[i].colCnt = 0;			// 無敵時間クリア
		g_Boss[i].size = ENEMY_SIZE * BOSS_SIZE_VALUE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Boss[i].model, &g_Boss[i].diffuse[0]);

		g_Boss[i].use = FALSE;		// TRUE:生きてる
		g_Boss[i].parent = NULL;

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_BossParts[i][j].use = FALSE;

			// 位置・回転・スケールの初期設定
			g_BossParts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_BossParts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_BossParts[i][j].scl = XMFLOAT3(1.0f * BOSS_SIZE_VALUE, 1.0f * BOSS_SIZE_VALUE, 1.0f * BOSS_SIZE_VALUE);

			// 親子関係
			g_BossParts[i][j].parent = &g_Boss[i];		// ← ここに親のアドレスを入れる

			// 階層アニメーション用のメンバー変数の初期化
			for (int k = 0; k < ENEMY_ANIM_MAX; k++)
			{
				g_BossParts[i][j].time[k] = 0.0f;			// 線形補間用のタイマーをクリア
				g_BossParts[i][j].tblNo[k] = 0;			// 再生する行動データテーブルNoをセット
				g_BossParts[i][j].tblMax[k] = 0;			// 再生する行動データテーブルのレコード数をセット
			}
			// パーツの読み込みはまだしていない
			g_BossParts[i][j].load = FALSE;


			// パーツの初期アニメーションを設定
			g_AnimNum[i] = ENEMY_ANIM_STOP;

		}

		g_BossParts[i][ENEMY_PARTS_HEAD].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_HEAD].parent = &g_Boss[i];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HEAD].load = 1;
		LoadModel(MODEL_ENEMY_HEAD, &g_BossParts[i][ENEMY_PARTS_HEAD].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_HEAD].model, &g_BossParts[i][ENEMY_PARTS_HEAD].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_ARM_L].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_ARM_L].parent = &g_Boss[i];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_ARM_L].load = 1;
		LoadModel(MODEL_ENEMY_ARM_L, &g_BossParts[i][ENEMY_PARTS_ARM_L].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_ARM_L].model, &g_BossParts[i][ENEMY_PARTS_ARM_L].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_ARM_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_ARM_R].parent = &g_Boss[i];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_ARM_R].load = 1;
		LoadModel(MODEL_ENEMY_ARM_R, &g_BossParts[i][ENEMY_PARTS_ARM_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_ARM_R].model, &g_BossParts[i][ENEMY_PARTS_ARM_R].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_HAND_L].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_HAND_L].parent = &g_BossParts[i][ENEMY_PARTS_ARM_L];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HAND_L].load = 1;
		LoadModel(MODEL_ENEMY_HAND_L, &g_BossParts[i][ENEMY_PARTS_HAND_L].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_HAND_L].model, &g_BossParts[i][ENEMY_PARTS_HAND_L].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_HAND_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_HAND_R].parent = &g_BossParts[i][ENEMY_PARTS_ARM_R];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_HAND_R].load = 1;
		LoadModel(MODEL_ENEMY_HAND_R, &g_BossParts[i][ENEMY_PARTS_HAND_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_HAND_R].model, &g_BossParts[i][ENEMY_PARTS_HAND_R].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_LEG_L].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_LEG_L].parent = &g_Boss[i];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_LEG_L].load = 1;
		LoadModel(MODEL_ENEMY_LEG_L, &g_BossParts[i][ENEMY_PARTS_LEG_L].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_LEG_L].model, &g_BossParts[i][ENEMY_PARTS_LEG_L].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_LEG_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_LEG_R].parent = &g_Boss[i];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_LEG_R].load = 1;
		LoadModel(MODEL_ENEMY_LEG_R, &g_BossParts[i][ENEMY_PARTS_LEG_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_LEG_R].model, &g_BossParts[i][ENEMY_PARTS_LEG_R].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_FOOT_L].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_FOOT_L].parent = &g_BossParts[i][ENEMY_PARTS_LEG_L];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_FOOT_L].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_L, &g_BossParts[i][ENEMY_PARTS_FOOT_L].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_FOOT_L].model, &g_BossParts[i][ENEMY_PARTS_FOOT_L].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_FOOT_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_FOOT_R].parent = &g_BossParts[i][ENEMY_PARTS_LEG_R];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_FOOT_R].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_R, &g_BossParts[i][ENEMY_PARTS_FOOT_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_FOOT_R].model, &g_BossParts[i][ENEMY_PARTS_FOOT_R].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_SWORD_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_SWORD_R].parent = &g_BossParts[i][ENEMY_PARTS_HAND_R];	// 親をセット
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_BossParts[i][ENEMY_PARTS_SWORD_R].load = 1;
		LoadModel(MODEL_ENEMY_SWORD_R, &g_BossParts[i][ENEMY_PARTS_SWORD_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_SWORD_R].model, &g_BossParts[i][ENEMY_PARTS_SWORD_R].diffuse[0]);
	}


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;
		}

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			if (g_Parts[i][j].load == TRUE)
			{
				// パーツの解放処理
				UnloadModel(&g_Parts[i][j].model);
				g_Parts[i][j].load = FALSE;
			}
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	PLAYER* player = GetPlayer();
	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE && GetFade() == FADE_NONE)		// このエネミーが使われている？
		{
			// アニメーションを歩きモーションにリセット
			int animNum = ENEMY_ANIM_STOP;

			// 無敵判定中は色を点滅させる
			ColCheck(player->pos, g_Enemy[i], g_Parts[i]);

			// プレイヤーが検知範囲内にいるかチェック
			float x = (player->pos.x - g_Enemy[i].pos.x);
			float z = (player->pos.z - g_Enemy[i].pos.z);
			float rad = atan2f(z, x);

			XMFLOAT3 pos = g_Enemy[i].pos;
			//pos.x -= (float)sinf(g_Enemy[i].rot.y) * LOOK_CIRCLE;
			//pos.z -= (float)cosf(g_Enemy[i].rot.y) * LOOK_CIRCLE;

			//if (CollisionBC(player->pos, pos, player->size, LOOK_CIRCLE))

			// プレイヤーが検知範囲内にいるか
			XMFLOAT3 dir = XMFLOAT3((float)cosf(-(g_Enemy[i].rot.y + RADIAN * 90.0f)), 0.0f, (float)sinf(-(g_Enemy[i].rot.y + LOOK_CIRCLE_RAD)));
			if (CollisionSector(player->pos, g_Enemy[i].pos, dir, LOOK_CIRCLE, LOOK_CIRCLE_RAD))
			{
				g_Enemy[i].look = TRUE;
				g_Enemy[i].rot.y = -(RADIAN * 90.0f + rad);
			}

			else
			{
				// 無敵時間中は検知状態のまま
				if (g_Enemy[i].colCnt == 0) g_Enemy[i].look = FALSE;
			}

			// 攻撃モーション中は移動を止める
			if (g_Enemy[i].attack == TRUE)
			{
				// アタックアニメーションをセット
				animNum = ENEMY_ANIM_ATTACK;
				AttackCheck(g_Enemy[i], g_Parts[i]);
			}

			// 攻撃中でないとき
			else
			{
				// プレイヤーを検知している場合
				if (g_Enemy[i].look == TRUE)
				{
					// プレイヤーをホーミングする動き
					XMFLOAT3 oldPos = g_Enemy[i].pos;
					g_Enemy[i].pos.x -= sinf(g_Enemy[i].rot.y) * VALUE_MOVE;
					g_Enemy[i].pos.z -= cosf(g_Enemy[i].rot.y) * VALUE_MOVE;

					// オブジェクト同士が重ならないようにする

					if (CollisionBC(g_Enemy[i].pos, player->pos, g_Enemy[i].size, player->size))
					{
						g_Enemy[i].pos = oldPos;
					}

					for (int j = 0; j < ENEMY_MAX; j++)
					{
						if (i == j || g_Enemy[j].use == FALSE) continue;
						if (CollisionBC(g_Enemy[i].pos, g_Enemy[j].pos, g_Enemy[i].size, g_Enemy[j].size))
						{
							g_Enemy[i].pos = oldPos;
						}
					}
				
					XMFLOAT3 pos = g_Enemy[i].pos;

					pos.x -= sinf(g_Enemy[i].rot.y) * ATTACK_DEPTH * 0.5f;
					pos.z -= cosf(g_Enemy[i].rot.y) * ATTACK_DEPTH * 0.5f;
					animNum = ENEMY_ANIM_MOVE;

					// 攻撃範囲内チェック
					if ((g_Enemy[i].attack == FALSE) && (CollisionBC(pos, player->pos, ATTACK_DEPTH - 1.0f, player->size)) && player->colCnt == 0)
					{
						g_Enemy[i].attack = TRUE;
						g_Enemy[i].atkCnt = 0;
						animNum = ENEMY_ANIM_ATTACK;
						PlaySound(SOUND_LABEL_SE_shot);
					}
				
				}

				// プレイヤーを検知していない場合
				else
				{
					// 移動処理
					g_Enemy[i].moveCnt = (g_Enemy[i].moveCnt + 1) % MOVE_INTERVAL;
					if (g_Enemy[i].moveFlg == TRUE)
					{
						g_Enemy[i].pos.x -= sinf(g_Enemy[i].rot.y) * VALUE_MOVE;
						g_Enemy[i].pos.z -= cosf(g_Enemy[i].rot.y) * VALUE_MOVE;
						// 壁の座標を超えないように調整
						if (g_Enemy[i].pos.x > MAP_RIGHT - g_Enemy[i].size * 0.5f)
						{
							g_Enemy[i].pos.x = MAP_RIGHT - g_Enemy[i].size * 0.5f;
						}
						if (g_Enemy[i].pos.x < MAP_LEFT + g_Enemy[i].size * 0.5f)
						{
							g_Enemy[i].pos.x = MAP_LEFT + g_Enemy[i].size * 0.5f;
						}
						if (g_Enemy[i].pos.z > MAP_TOP - g_Enemy[i].size * 0.5f)
						{
							g_Enemy[i].pos.z = MAP_TOP - g_Enemy[i].size * 0.5f;
						}
						if (g_Enemy[i].pos.z < MAP_DOWN + g_Enemy[i].size * 0.5f)
						{
							g_Enemy[i].pos.z = MAP_DOWN + g_Enemy[i].size * 0.5f;
						}
						animNum = ENEMY_ANIM_MOVE;
					}

					// ランダムでその場にとどまる
					if (g_Enemy[i].moveCnt == 0)
					{
						float rot = 0.0f;
						switch (rand() % 5)
						{
						case 1:
						case 2:
						case 3:
							rot = rand() % 360 * RADIAN;
							g_Enemy[i].rot.y = -(RADIAN * 90.0f + rot);
							g_Enemy[i].moveFlg = TRUE;
							animNum = ENEMY_ANIM_MOVE;
							break;
						default:
							animNum = ENEMY_ANIM_STOP;
							g_Enemy[i].moveFlg = FALSE;
							break;
						}
					}
				}
			}

			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				g_Parts[i][j].Animation(animNum);
			}

			// 影もプレイヤーの位置に合わせる
			pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	
		// エネミーのディゾルブ処理
		else if (g_Enemy[i].use == FALSE && g_Enemy[i].dissolveCnt <= RADIAN * 90.0f)
		{
			g_Enemy[i].dissolveCnt += RADIAN * 1.0f;
			//g_Enemy[i].pos.y -= cosf(RADIAN * 5.0f) * ENEMY_OFFSET_Y * 0.03f;
			// 傾きに合わせて色も薄くしていく
			//for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
			//{
			//	SetModelDiffuse(&g_Enemy[i].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f * (float)cos(g_Enemy[i].rot.z)));
			//}

			//for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			//{
			//	for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
			//	{
			//		SetModelDiffuse(&g_Parts[i][j].model, k, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f * (float)cos(g_Enemy[i].rot.z)));
			//	}
			//}
		}
	}

	// ボスの処理
	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use == TRUE && GetFade() == FADE_NONE)		// このボスが使われている？
		{
			int animNum;
			ColCheck(player->pos, g_Boss[i], g_BossParts[i]);
			// 攻撃モーション中は移動を止める
			if (g_Boss[i].attack == TRUE)
			{
				// アタックアニメーションをセット
				animNum = ENEMY_ANIM_ATTACK;
				AttackCheck(g_Boss[i], g_BossParts[i]);
			}

			else
			{

				float x = (player->pos.x - g_Boss[i].pos.x);
				float z = (player->pos.z - g_Boss[i].pos.z);
				float rad = atan2f(z, x);
				g_Boss[i].rot.y = -(RADIAN * 90.0f + rad);
				XMFLOAT3 oldPos = g_Boss[i].pos;
				// オブジェクト同士が重ならないようにする
				g_Boss[i].pos.x += cosf(rad) * VALUE_MOVE * BOSS_SIZE_VALUE;
				g_Boss[i].pos.z += sinf(rad) * VALUE_MOVE * BOSS_SIZE_VALUE;

				if (CollisionBC(g_Boss[i].pos, player->pos, g_Boss[i].size, player->size))
				{
					g_Boss[i].pos = oldPos;
				}

				animNum = ENEMY_ANIM_MOVE;

				// 影を足元に合わせる
				{
					XMFLOAT3 pos = g_Boss[i].pos;
					pos.y -= (ENEMY_OFFSET_Y - 0.1f);
					SetPositionShadow(g_Boss[i].shadowIdx, pos);
				}

				XMFLOAT3 pos = g_Boss[i].pos;

				pos.x -= (float)sinf(g_Boss[i].rot.y) * ATTACK_DEPTH * 0.5f;
				pos.y /= BOSS_SIZE_VALUE;
				pos.z -= (float)cosf(g_Boss[i].rot.y) * ATTACK_DEPTH * 0.5f;

				// 攻撃範囲内チェック
				if ((g_Boss[i].attack == FALSE) && (CollisionBC(pos, player->pos, (ATTACK_DEPTH - 1.0f), player->size)) && player->colCnt == 0)
				{
					g_Boss[i].attack = TRUE;
					g_Boss[i].atkCnt = 0;
					animNum = ENEMY_ANIM_ATTACK;
					PlaySound(SOUND_LABEL_SE_shot);
				}
			}

			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				g_BossParts[i][j].Animation(animNum);
			}
		}
	}

#ifdef _DEBUG

	if (GetKeyboardTrigger(DIK_P))
	{
		// モデルの色を変更できるよ！半透明にもできるよ。
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
		}
	}

	if (GetKeyboardTrigger(DIK_L))
	{
		// モデルの色を元に戻している
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
		}
	}
#endif


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// モデルの色退避
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == FALSE && g_Enemy[i].dissolveCnt >= RADIAN * 90.0f) continue;
		if (g_Enemy[i].use == FALSE && GetViewPortType() == TYPE_SHADOWMAP) continue;
		//XMFLOAT4 enemyDiffuse[MODEL_MAX_MATERIAL];
		//XMFLOAT4 partsDiffuse[ENEMY_PARTS_MAX][MODEL_MAX_MATERIAL];

		//GetModelDiffuse(&g_Enemy[i].model, &enemyDiffuse[0]);
		// 平坦化行列の影描画
		//{
		//	for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
		//	{
		//		SetModelDiffuse(&g_Enemy[i].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
		//	}

		//	// ワールドマトリックスの初期化
		//	mtxWorld = XMMatrixIdentity();

		//	// スケールを反映
		//	mtxScl = XMMatrixScaling(g_Enemy[i].scl.x * 1.2f, 0.1f, g_Enemy[i].scl.z * 1.2f);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		//	// 回転を反映
		//	mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//	// 移動を反映
		//	mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y - ENEMY_OFFSET_Y, g_Enemy[i].pos.z);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		//	// ワールドマトリックスの設定
		//	SetWorldMatrix(&mtxWorld);

		//	XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		//	// モデル描画
		//	DrawModel(&g_Enemy[i].model);
		//	for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		//	{
		//		GetModelDiffuse(&g_Parts[i][j].model, &partsDiffuse[j][0]);
		//		for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
		//		{
		//			SetModelDiffuse(&g_Parts[i][j].model, k, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
		//		}

		//		// ワールドマトリックスの初期化
		//		mtxWorld = XMMatrixIdentity();

		//		// スケールを反映
		//		mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x * 1.2f, 0.1f, g_Parts[i][j].scl.z * 1.2f);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		//		// 回転を反映
		//		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, 0.0f, g_Parts[i][j].rot.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//		// 移動を反映
		//		mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		//		if (g_Parts[i][j].parent != NULL)	// 子供だったら親と結合する
		//		{
		//			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
		//			// ↑
		//			// g_Player.mtxWorldを指している
		//		}

		//		XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

		//		// 使われているなら処理する
		//		if (g_Parts[i][j].use == FALSE) continue;

		//		// ワールドマトリックスの設定
		//		SetWorldMatrix(&mtxWorld);

		//		if (g_Enemy[i].use == FALSE)
		//		{
		//			SetDissolve(&g_EnemyDissolve[i]);
		//			SetDissolveBuffer(TRUE, sinf(g_Enemy[i].dissolveCnt));
		//		}

		//		// モデル描画
		//		DrawModel(&g_Parts[i][j].model);
		//	}

		//}

		// モデル描画
		{
			//for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
			//{
			//	SetModelDiffuse(&g_Enemy[i].model, j, enemyDiffuse[j]);
			//}

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

			if (g_Enemy[i].use == FALSE)
			{
				SetDissolve(&g_EnemyDissolve[i]);
				SetDissolveBuffer(TRUE, sinf(g_Enemy[i].dissolveCnt));
			}

			// モデル描画
			DrawModel(&g_Enemy[i].model);
			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				//for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
				//{
				//	SetModelDiffuse(&g_Parts[i][j].model, k, partsDiffuse[j][k]);
				//}
				// ワールドマトリックスの初期化
				mtxWorld = XMMatrixIdentity();

				// スケールを反映
				mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// 回転を反映
				mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// 移動を反映
				mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				if (g_Parts[i][j].parent != NULL)	// 子供だったら親と結合する
				{
					mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
					// ↑
					// g_Player.mtxWorldを指している
				}

				XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

				// 使われているなら処理する
				if (g_Parts[i][j].use == FALSE) continue;

				// ワールドマトリックスの設定
				SetWorldMatrix(&mtxWorld);

				// モデル描画
				DrawModel(&g_Parts[i][j].model);
			}

		}
		SetDissolveBuffer(FALSE, 0.0f);
	}

	// ボス描画
	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use == FALSE) continue;
		//XMFLOAT4 bossDiffuse[MODEL_MAX_MATERIAL];
		//XMFLOAT4 bossPartsDiffuse[ENEMY_PARTS_MAX][MODEL_MAX_MATERIAL];

		// 平坦化行列の影描画
		//{
		//	GetModelDiffuse(&g_Boss[i].model, &bossDiffuse[0]);
		//	for (int j = 0; j < g_Boss[i].model.SubsetNum; j++)
		//	{
		//		SetModelDiffuse(&g_Boss[i].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
		//	}

		//	// ワールドマトリックスの初期化
		//	mtxWorld = XMMatrixIdentity();

		//	// スケールを反映
		//	mtxScl = XMMatrixScaling(g_Boss[i].scl.x * 1.2f, g_Boss[i].scl.y * 0.1f, g_Boss[i].scl.z * 1.2f);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		//	// 回転を反映
		//	mtxRot = XMMatrixRotationRollPitchYaw(g_Boss[i].rot.x, g_Boss[i].rot.y + XM_PI, g_Boss[i].rot.z);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//	// 移動を反映
		//	mtxTranslate = XMMatrixTranslation(g_Boss[i].pos.x, g_Boss[i].pos.y - ENEMY_OFFSET_Y * BOSS_SIZE_VALUE, g_Boss[i].pos.z);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		//	// ワールドマトリックスの設定
		//	SetWorldMatrix(&mtxWorld);

		//	XMStoreFloat4x4(&g_Boss[i].mtxWorld, mtxWorld);

		//	// モデル描画
		//	DrawModel(&g_Boss[i].model);
		//	for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		//	{
		//		GetModelDiffuse(&g_BossParts[i][j].model, &bossPartsDiffuse[j][0]);

		//		for (int k = 0; k < g_BossParts[i][j].model.SubsetNum; k++)
		//		{
		//			SetModelDiffuse(&g_BossParts[i][j].model, k, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
		//		}

		//		// ワールドマトリックスの初期化
		//		mtxWorld = XMMatrixIdentity();

		//		// スケールを反映
		//		mtxScl = XMMatrixScaling(g_BossParts[i][j].scl.x, g_BossParts[i][j].scl.y, g_BossParts[i][j].scl.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		//		// 回転を反映
		//		mtxRot = XMMatrixRotationRollPitchYaw(g_BossParts[i][j].rot.x, 0.0f, g_BossParts[i][j].rot.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//		// 移動を反映
		//		mtxTranslate = XMMatrixTranslation(g_BossParts[i][j].pos.x, g_BossParts[i][j].pos.y, g_BossParts[i][j].pos.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		//		if (g_BossParts[i][j].parent != NULL)	// 子供だったら親と結合する
		//		{
		//			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_BossParts[i][j].parent->mtxWorld));
		//			// ↑
		//			// g_Player.mtxWorldを指している
		//		}

		//		XMStoreFloat4x4(&g_BossParts[i][j].mtxWorld, mtxWorld);

		//		// 使われているなら処理する
		//		if (g_BossParts[i][j].use == FALSE) continue;

		//		// ワールドマトリックスの設定
		//		SetWorldMatrix(&mtxWorld);

		//		// モデル描画
		//		DrawModel(&g_BossParts[i][j].model);

		//	}
		//}

		// モデル描画
		SetRimLight(1);
		{
			//for (int j = 0; j < g_Boss[i].model.SubsetNum; j++)
			//{
			//	SetModelDiffuse(&g_Boss[i].model, j, bossDiffuse[j]);
			//}

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Boss[i].scl.x, g_Boss[i].scl.y, g_Boss[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Boss[i].rot.x, g_Boss[i].rot.y + XM_PI, g_Boss[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Boss[i].mtxWorld, mtxWorld);


			// モデル描画
			DrawModel(&g_Boss[i].model);
			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				//for (int k = 0; k < g_BossParts[i][j].model.SubsetNum; k++)
				//{
				//	SetModelDiffuse(&g_BossParts[i][j].model, k, bossPartsDiffuse[j][k]);
				//}
				// ワールドマトリックスの初期化
				mtxWorld = XMMatrixIdentity();

				// スケールを反映
				mtxScl = XMMatrixScaling(g_BossParts[i][j].scl.x, g_BossParts[i][j].scl.y, g_BossParts[i][j].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// 回転を反映
				mtxRot = XMMatrixRotationRollPitchYaw(g_BossParts[i][j].rot.x, g_BossParts[i][j].rot.y, g_BossParts[i][j].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// 移動を反映
				mtxTranslate = XMMatrixTranslation(g_BossParts[i][j].pos.x, g_BossParts[i][j].pos.y, g_BossParts[i][j].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				if (g_BossParts[i][j].parent != NULL)	// 子供だったら親と結合する
				{
					mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_BossParts[i][j].parent->mtxWorld));
					// ↑
					// g_Player.mtxWorldを指している
				}

				XMStoreFloat4x4(&g_BossParts[i][j].mtxWorld, mtxWorld);

				// 使われているなら処理する
				if (g_BossParts[i][j].use == FALSE) continue;

				// ワールドマトリックスの設定
				SetWorldMatrix(&mtxWorld);

				// モデル描画
				DrawModel(&g_BossParts[i][j].model);

			}
		}
	}
	SetRimLight(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}


//=============================================================================
// ボスの取得
//=============================================================================
ENEMY *GetBoss()
{
	return &g_Boss[0];
}

//=============================================================================
// アニメーション関数
//=============================================================================
void ENEMY::Animation(int animNum)
{
	INTERPOLATION_DATA* tbl;

	switch (animNum)
	{
	case ENEMY_ANIM_STOP:
		tbl = g_StopTblAdr[tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	case ENEMY_ANIM_MOVE:
		tbl = g_MoveTblAdr[tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	default:
		tbl = g_AttackTblAdr[tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	}

	int nowNo = (int)time[animNum];			// 整数分であるテーブル番号を取り出している
	int maxNo = tblMax[animNum];				// 登録テーブル数を数えている
	int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている

	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

	float nowTime = time[animNum] - nowNo;	// 時間部分である少数を取り出している

	Pos = nowPos + Pos * nowTime;								// 現在の移動量を計算している
	Rot = nowRot + Rot * nowTime;								// 現在の回転量を計算している
	Scl = nowScl + Scl * nowTime;								// 現在の拡大率を計算している

	// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
	XMStoreFloat3(&pos, Pos);

	// 計算して求めた回転量を現在の移動テーブルに足している
	XMStoreFloat3(&rot, Rot);

	// 計算して求めた拡大率を現在の移動テーブルに足している
	XMStoreFloat3(&scl, Scl);

	// frameを使て時間経過処理をする
	time[animNum] += 1.0f / tbl[nowNo].frame;	// 時間を進めている
	if ((int)time[animNum] >= maxNo)			// 登録テーブル最後まで移動したか？
	{
		time[animNum] -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
	}

}

//=============================================================================
// モーションブレンド関数
//=============================================================================
void ENEMY::Animation(int animNum1, int animNum2)
{
	// アニメーション番号が同じだった場合
	if (animNum1 == animNum2)
	{
		Animation(animNum1);
		return;
	}

	// アニメーションが変わったタイミングでカウントをリセットする
	if (g_AnimTransFrameCnt[animNum1] < g_AnimTransFrameCnt[animNum2])
	{
		g_AnimTransFrameCnt[animNum2] = 0.0f;
	}

	int AnimNum[2] = { animNum1, animNum2 };

	// 使われているなら処理する
	if ((tblMax[animNum1] > 0))
	{
		INTERPOLATION_DATA* tbl[2];
		XMVECTOR Pos[2];
		XMVECTOR Rot[2];
		XMVECTOR Scl[2];

		for (int i = 0; i < 2; i++)
		{	// 線形補間の処理
			// 1つ目のアニメーションの計算
			switch (AnimNum[i])
			{
			case ENEMY_ANIM_STOP:
				tbl[i] = g_StopTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
				break;
			case ENEMY_ANIM_MOVE:
				tbl[i] = g_MoveTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
				break;
			default:
				tbl[i] = g_AttackTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
				break;
			}

			int nowNo = (int)time[AnimNum[i]];			// 整数分であるテーブル番号を取り出している
			int maxNo = tblMax[AnimNum[i]];				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている

			XMVECTOR nowPos = XMLoadFloat3(&tbl[i][nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[i][nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[i][nowNo].scl);	// XMVECTORへ変換

			Pos[i] = XMLoadFloat3(&tbl[i][nextNo].pos) - nowPos;	// XYZ移動量を計算している
			Rot[i] = XMLoadFloat3(&tbl[i][nextNo].rot) - nowRot;	// XYZ回転量を計算している
			Scl[i] = XMLoadFloat3(&tbl[i][nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = time[AnimNum[i]] - nowNo;	// 時間部分である少数を取り出している

			Pos[i] = nowPos + Pos[i] * nowTime;
			Rot[i] = nowRot + Rot[i] * nowTime;
			Scl[i] = nowScl + Scl[i] * nowTime;

			// frameを使て時間経過処理をする
			time[AnimNum[i]] += 1.0f / tbl[i][nowNo].frame;	// 時間を進めている
			if ((int)time[AnimNum[i]] >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				time[AnimNum[i]] -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
			}
		}
		// 重みづけ
		float weight = (1.0f / g_AnimTransFrame[animNum1]) * g_AnimTransFrameCnt[animNum1];
		// モーションブレンドの座標計算
		XMVECTOR blendPos = Pos[0] * weight + Pos[1] * (1.0f - weight);
		XMVECTOR blendRot = Rot[0] * weight + Rot[1] * (1.0f - weight);
		XMVECTOR blendScl = Scl[0] * weight + Scl[1] * (1.0f - weight);

		// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
		XMStoreFloat3(&pos, blendPos);

		// 計算して求めた回転量を現在の移動テーブルに足している
		XMStoreFloat3(&rot, blendRot);

		// 計算して求めた拡大率を現在の移動テーブルに足している
		XMStoreFloat3(&scl, blendScl);

		g_AnimTransFrameCnt[animNum1] += 1.0f;
		if (g_AnimTransFrameCnt[animNum1] >= g_AnimTransFrame[animNum1])
		{
			g_AnimTransFrameCnt[animNum1] = g_AnimTransFrame[animNum1];
			animNum = animNum1;

			// アニメーションの移行が完了したタイミングでリセット
			time[animNum2] = 0.0f;
		}
	}
}

int ENEMY::DecHP(int atk)
{
	hp -= atk;
	PLAYER* player = GetPlayer();
	float x = (player->pos.x - pos.x);
	float z = (player->pos.z - pos.z);
	float rad = atan2f(z, x);
	pos.x -= cosf(rad) * VALUE_BACK;
	pos.z -= sinf(rad) * VALUE_BACK;

	// 体力がなくなったらパーティクルなどをセット
	if (hp <= 0)
	{
		use = false;
		//ReleaseShadow(shadowIdx);
		for (int i = 0; i < 30; i++)
		{
			SetParticle(pos, XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f));
		}
		SetEffect(XMFLOAT3(pos.x, pos.y - ENEMY_OFFSET_Y * EFFECT_HEIGHT / 15.0f, pos.z), EFFECT_WIDTH, EFFECT_HEIGHT, EFFECT_DEAD);
		PlaySound(SOUND_LABEL_SE_enemy_death);
		return 1;
	}

	else
	{
		SetEffect(XMFLOAT3(pos.x, pos.y - ENEMY_OFFSET_Y * EFFECT_HEIGHT / 15.0f, pos.z), EFFECT_WIDTH, EFFECT_HEIGHT, EFFECT_HIT);
		PlaySound(SOUND_LABEL_SE_hit);
	}
	colCnt++;

	return 0;
}
// ボスをスポーン
void SetBoss(void)
{
	for (int i = 0; i < BOSS_MAX; i++)
	{
		g_Boss[i].use = TRUE;
		g_BossFlg = TRUE;
		g_Boss[i].look = TRUE;
		g_BossSponeCnt++;

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_BossParts[i][j].use = TRUE;
		}
	}
}

// 無敵判定チェック
void ColCheck(XMFLOAT3 pos, ENEMY &enemy, ENEMY parts[ENEMY_PARTS_MAX])
{
	if (enemy.colCnt > 0)
	{
		// 検知状態にする
		enemy.look = TRUE;

		// プレイヤーの方向を向かせる
		float x = (pos.x - enemy.pos.x);
		float z = (pos.z - enemy.pos.z);
		float rad = atan2f(z, x);
		enemy.rot.y = -(RADIAN * 90.0f + rad);

		if (enemy.colCnt % BARRIER_WHITE == 0)
		{
			for (int i = 0; i < enemy.model.SubsetNum; i++)
			{
				SetModelDiffuse(&enemy.model, i, XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f));
			}

			for (int i = 0; i < ENEMY_PARTS_MAX; i++)
			{
				for (int j = 0; j < parts[i].model.SubsetNum; j++)
				{
					SetModelDiffuse(&parts[i].model, j, XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f));
				}
			}
		}

		else
		{
			for (int i = 0; i < enemy.model.SubsetNum; i++)
			{
				SetModelDiffuse(&enemy.model, i, enemy.diffuse[i]);
			}

			for (int i = 0; i < ENEMY_PARTS_MAX; i++)
			{
				for (int j = 0; j < parts[i].model.SubsetNum; j++)
				{
					SetModelDiffuse(&parts[i].model, j, parts[i].diffuse[j]);
				}
			}
		}

		// 無敵判定が終わるタイミングで色をもとに戻す
		enemy.colCnt++;
		if (enemy.colCnt > BARRIER_FRAME)
		{
			enemy.colCnt = 0;
			enemy.look = TRUE;
			for (int i = 0; i < enemy.model.SubsetNum; i++)
			{
				SetModelDiffuse(&enemy.model, i, enemy.diffuse[i]);
			}

			for (int i = 0; i < ENEMY_PARTS_MAX; i++)
			{
				for (int j = 0; j < parts[i].model.SubsetNum; j++)
				{
					SetModelDiffuse(&parts[i].model, j, parts[i].diffuse[j]);
				}
			}
		}
	}
}

BOOL GetBossFlg(void)
{
	return g_BossFlg;
}
//=============================================================================
// アタック関数
//=============================================================================
void AttackCheck(ENEMY &enemy, ENEMY parts[ENEMY_PARTS_MAX])
{
	PLAYER* player = GetPlayer();

	enemy.atkCnt++;
	XMFLOAT3 pos = enemy.pos;

	pos.x -= (float)sinf(enemy.rot.y) * ATTACK_DEPTH * 0.5f;
	pos.z -= (float)cosf(enemy.rot.y) * ATTACK_DEPTH * 0.5f;

	// 武器を振っているときのみ判定
	if ((int)parts[ENEMY_PARTS_SWORD_R].time[ENEMY_ANIM_ATTACK] > 1)
	{
		if (player->use == TRUE && player->colCnt == 0)
		{
			if (CollisionBC(pos, player->pos, ATTACK_DEPTH * enemy.scl.y, player->size))
			{
				player->DecHP(enemy.atkVal);
			}
		}
	}

	if (enemy.atkCnt >= ENEMY_ATK_CNT_MAX)
	{
		enemy.attack = FALSE;
		enemy.atkCnt = 0;
		g_AnimTransFrameCnt[ENEMY_ANIM_ATTACK] = 0.0f;

		for (int i = 0; i < ENEMY_ANIM_ATTACK; i++)
		{
			parts[i].time[ENEMY_ANIM_ATTACK] = 0.0f;
		}
	}
}

void ReleaseEnemy(void)
{
	// ボス出現時にエネミーはすべて消す
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use = FALSE;
		//ReleaseShadow(g_Enemy[i].shadowIdx);
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = TRUE;
		}
	}
}

int GetBossSponeCnt(void)
{
	return g_BossSponeCnt;
}

void AddBossSponeCnt(void)
{
	g_BossFlg = TRUE;
	g_BossSponeCnt++;
}