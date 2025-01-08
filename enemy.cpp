//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "player.h"
#include "particle.h"
#include "effect.h"
#include "camera.h"
#include "collision.h"
#include "shadow.h"

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
//#define	MODEL_ENEMY_SWORD_R	"data/MODEL/enemy_sword.obj"			// 読み込むモデル名
//#define	MODEL_ENEMY_SWORD_B	"data/MODEL/enemy_sword_back.obj"			// 読み込むモデル名
//#define	MODEL_ENEMY_SCABBARD	"data/MODEL/enemy_scabbard.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(0.2f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量
#define	VALUE_DISTANCE		(100.0f)					// 動く距離
#define	VALUE_FRAME			(VALUE_DISTANCE / VALUE_MOVE)	// 動く距離

#define	BARRIER_FRAME		(40)						// 無敵時間
#define	BARRIER_WHITE		(BARRIER_FRAME / 5)			// 無敵時間の点滅間隔

#define ENEMY_SHADOW_SIZE	(0.4f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(7.0f)						// エネミーの足元をあわせる

#define RADIAN				(XM_PI / 180.0f)					// ラジアン変換用

#define ANIM_FRAME_STOP		(15.0f)							// 待機アニメーションの間隔
#define ANIM_FRAME_MOVE		(30.0f)							// 移動アニメーションの間隔
#define ANIM_FRAME_DASH		(ANIM_FRAME_MOVE * 0.5f)		// ダッシュアニメーションの間隔

#define ANIM_FRAME_JUMP		(15.0f)							// ジャンプアニメーションの間隔
#define ANIM_FRAME_ATTACK	(60.0f)							// ジャンプアニメーションの間隔

#define ENEMY_HEAD_Y		(2.7f)							// 頭のx座標

#define ENEMY_ARM_X			(1.5f)							// 腕のx座標
#define ENEMY_ARM_Y			(2.5f)							// 腕のy座標

#define ENEMY_HAND_X		(0.0f)							// 手のx座標
#define ENEMY_HAND_Y		(-1.5f)							// 手のy座標

#define ENEMY_LEG_X			(1.1f)							// 脚のx座標
#define ENEMY_LEG_Y			(-2.4f)							// 脚のy座標

#define ENEMY_FOOT_X		(0.0f)							// 足のx座標
#define ENEMY_FOOT_Y		(-1.4f)							// 足のy座標

#define ENEMY_SWORD_R_X		(0.0f)							// 剣のx座標
#define ENEMY_SWORD_R_Y		(-2.0f)							// 剣のy座標

#define ENEMY_SWORD_B_X		(0.0f)							// 剣(背中)のx座標
#define ENEMY_SWORD_B_Y		(1.0f)							// 剣(背中)のy座標
#define ENEMY_SWORD_B_Z		(-1.5f)							// 剣(背中)のz座標

#define BLEND_FRAME_STOP	(60.0f)							// 待機モーションに遷移するまでの時間
#define BLEND_FRAME_MOVE	(90.0f)							// 移動モーションに遷移するまでの時間
#define BLEND_FRAME_DASH	(90.0f)							// ダッシュモーションに遷移するまでの時間
#define BLEND_FRAME_JUMP	(90.0f)							// ジャンプモーションに遷移するまでの時間
#define BLEND_FRAME_ATTACK	(90.0f)							// 攻撃モーションに遷移するまでの時間

#define MOVE_TBL_MAX		(2)								// 移動アニメーションのテーブル数

#define LOOK_CIRCLE			(40.0f)							// プレイヤーを検知する範囲の半径

#define TEXTURE_MAX			(2)				// テクスチャの数
#define TEXTURE_WIDTH		(80.0f)			// テクスチャの横幅
#define TEXTURE_HEIGHT		(80.0f)			// テクスチャの縦幅


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[ENEMY_MAX];				// エネミー

static ENEMY			g_Parts[ENEMY_MAX][ENEMY_PARTS_MAX];				// エネミー

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
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_sword_b[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },

};

static INTERPOLATION_DATA stop_tbl_scabbard[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },

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
	stop_tbl_sword_b,
	stop_tbl_scabbard,
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
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_sword_b[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },

};

static INTERPOLATION_DATA move_tbl_scabbard[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },

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
	move_tbl_sword_b,
	move_tbl_scabbard,
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
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(ENEMY_SWORD_R_X, ENEMY_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_sword_b[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },

};

static INTERPOLATION_DATA dash_tbl_scabbard[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(ENEMY_SWORD_B_X, ENEMY_SWORD_B_Y, ENEMY_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },

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
	dash_tbl_sword_b,
	dash_tbl_scabbard,
};

// 選択したアニメーションに移行するフレーム
static float g_AnimTransFrame[ENEMY_ANIM_MAX] =
{
	BLEND_FRAME_STOP,
	BLEND_FRAME_MOVE,
	BLEND_FRAME_DASH,
};

static float g_AnimTransFrameCnt[ENEMY_ANIM_MAX];			// アニメーションに移行するフレームのカウンタ

static int g_AnimNum[ENEMY_PARTS_MAX];		// 現在のアニメーション番号

static INTERPOLATION_DATA g_MoveTbl0[ENEMY_MAX][MOVE_TBL_MAX] = {	// pos, rot, scl, frame
	{
		{ XMFLOAT3(0.0f, ENEMY_OFFSET_Y, 100.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), VALUE_FRAME },
		{ XMFLOAT3(g_MoveTbl0[0][0].pos.x - VALUE_DISTANCE, ENEMY_OFFSET_Y, g_MoveTbl0[0][0].pos.z), XMFLOAT3(0.0f, RADIAN * 180.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), VALUE_FRAME },
	},

	{
		{ XMFLOAT3(100.0f, ENEMY_OFFSET_Y, 100.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), VALUE_FRAME },
		{ XMFLOAT3(g_MoveTbl0[1][0].pos.x, ENEMY_OFFSET_Y, g_MoveTbl0[1][0].pos.z + VALUE_DISTANCE), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), VALUE_FRAME },
	},

	{
		{ XMFLOAT3(-100.0f, ENEMY_OFFSET_Y, 100.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), VALUE_FRAME },
		{ XMFLOAT3(g_MoveTbl0[2][0].pos.x + VALUE_DISTANCE, ENEMY_OFFSET_Y, g_MoveTbl0[2][0].pos.z), XMFLOAT3(0.0f, RADIAN * 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), VALUE_FRAME },
	},
};

//
//
static INTERPOLATION_DATA* g_MoveTbl0Adr[] =
{
	g_MoveTbl0[0],
	g_MoveTbl0[1],
	g_MoveTbl0[2],
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-100.0f + i * 100.0f, ENEMY_OFFSET_Y, 20.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].hp  = 2;				// 体力初期化
		g_Enemy[i].atk  = 1;			// 攻撃力初期化
		g_Enemy[i].spd  = 0.0f;			// 移動スピードクリア
		g_Enemy[i].colCnt  = 0;			// 無敵時間クリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);
		
		g_Enemy[i].use = TRUE;			// TRUE:生きてる
		g_Enemy[i].parent = NULL;
		for (int j = 0; j < ENEMY_ANIM_MAX; j++)
		{
			// 0番だけ線形補間で動かしてみる
			g_Enemy[i].time[j] = 0.0f;		// 線形補間用のタイマーをクリア
			g_Enemy[i].tblNo[j] = i;		// 再生するアニメデータの先頭アドレスをセット
			g_Enemy[i].tblMax[j] = sizeof(g_MoveTbl0[i]) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		}

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

		}

		g_Parts[i][ENEMY_PARTS_HEAD].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HEAD].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HEAD].load = 1;
		LoadModel(MODEL_ENEMY_HEAD, &g_Parts[i][ENEMY_PARTS_HEAD].model);

		g_Parts[i][ENEMY_PARTS_ARM_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_ARM_L].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_L].load = 1;
		LoadModel(MODEL_ENEMY_ARM_L, &g_Parts[i][ENEMY_PARTS_ARM_L].model);

		g_Parts[i][ENEMY_PARTS_ARM_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_ARM_R].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_ARM_R].load = 1;
		LoadModel(MODEL_ENEMY_ARM_R, &g_Parts[i][ENEMY_PARTS_ARM_R].model);

		g_Parts[i][ENEMY_PARTS_HAND_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HAND_L].parent = &g_Parts[i][ENEMY_PARTS_ARM_L];	// 親をセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_L].load = 1;
		LoadModel(MODEL_ENEMY_HAND_L, &g_Parts[i][ENEMY_PARTS_HAND_L].model);

		g_Parts[i][ENEMY_PARTS_HAND_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HAND_R].parent = &g_Parts[i][ENEMY_PARTS_ARM_R];	// 親をセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_HAND_R].load = 1;
		LoadModel(MODEL_ENEMY_HAND_R, &g_Parts[i][ENEMY_PARTS_HAND_R].model);

		g_Parts[i][ENEMY_PARTS_LEG_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_LEG_L].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_L].load = 1;
		LoadModel(MODEL_ENEMY_LEG_L, &g_Parts[i][ENEMY_PARTS_LEG_L].model);

		g_Parts[i][ENEMY_PARTS_LEG_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_LEG_R].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_LEG_R].load = 1;
		LoadModel(MODEL_ENEMY_LEG_R, &g_Parts[i][ENEMY_PARTS_LEG_R].model);

		g_Parts[i][ENEMY_PARTS_FOOT_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_FOOT_L].parent = &g_Parts[i][ENEMY_PARTS_LEG_L];	// 親をセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_L].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_L, &g_Parts[i][ENEMY_PARTS_FOOT_L].model);

		g_Parts[i][ENEMY_PARTS_FOOT_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_FOOT_R].parent = &g_Parts[i][ENEMY_PARTS_LEG_R];	// 親をセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][ENEMY_PARTS_FOOT_R].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_R, &g_Parts[i][ENEMY_PARTS_FOOT_R].model);

		//g_Parts[i][ENEMY_PARTS_SWORD_R].use = FALSE;
		//g_Parts[i][ENEMY_PARTS_SWORD_R].parent = &g_Parts[i][ENEMY_PARTS_HAND_R];	// 親をセット
		//g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		//g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		//g_Parts[i][ENEMY_PARTS_SWORD_R].load = 1;
		//LoadModel(MODEL_ENEMY_SWORD_R, &g_Parts[i][ENEMY_PARTS_SWORD_R].model);

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
		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{
			// 無敵判定中は色を点滅させる
			if (g_Enemy[i].colCnt > 0)
			{
				if (g_Enemy[i].colCnt % BARRIER_WHITE == 0)
				{
					for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
					{
						SetModelDiffuse(&g_Enemy[i].model, j, XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f));
					}

					for (int j = 0; j < ENEMY_PARTS_MAX; j++)
					{
						for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
						{
							SetModelDiffuse(&g_Parts[i][j].model, k, XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f));
						}
					}
				}

				else
				{
					for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
					{
						SetModelDiffuse(&g_Enemy[i].model, j, g_Enemy[i].diffuse[j]);
					}

					for (int j = 0; j < ENEMY_PARTS_MAX; j++)
					{
						for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
						{
							SetModelDiffuse(&g_Parts[i][j].model, k, g_Enemy[i].diffuse[k]);
						}
					}
				}

				// 無敵判定が終わるタイミングで色をもとに戻す
				g_Enemy[i].colCnt++;
				if (g_Enemy[i].colCnt > BARRIER_FRAME)
				{
					g_Enemy[i].colCnt = 0;
					for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
					{
						SetModelDiffuse(&g_Enemy[i].model, j, g_Enemy[i].diffuse[0]);
					}

					for (int j = 0; j < ENEMY_PARTS_MAX; j++)
					{
						for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
						{
							SetModelDiffuse(&g_Parts[i][j].model, k, g_Enemy[i].diffuse[0]);
						}
					}
				}
			}

			// プレイヤーが検知範囲内にいるかチェック
			float x = (player[0].pos.x - g_Enemy[i].pos.x);
			float z = (player[0].pos.z - g_Enemy[i].pos.z);
			float rad = atan2f(z, x);

			if (CollisionBC(player->pos, g_Enemy[i].pos, player->size, LOOK_CIRCLE))
			{
				g_Enemy[i].look = TRUE;
				g_Enemy[i].rot.y = -(RADIAN * 90.0f + rad);
			}

			else
			{
				// 検知から外れたタイミングで線形補完のポジションを更新
				if (g_Enemy[i].look == TRUE)
				{
					g_Enemy[i].time[0] = 0.0f;
					g_MoveTbl0[i][0].pos.x = g_Enemy[i].pos.x;
					g_MoveTbl0[i][0].pos.z = g_Enemy[i].pos.z;
				}

				g_Enemy[i].look = FALSE;
			}

			if (g_Enemy[i].look == TRUE)
			{
				// プレイヤーをホーミングする動き
				XMFLOAT3 oldPos = g_Enemy[i].pos;
				g_Enemy[i].pos.x += cosf(rad) * VALUE_MOVE;
				g_Enemy[i].pos.z += sinf(rad) * VALUE_MOVE;

				// エネミー同士が重ならないようにする
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					if (i == j || g_Enemy[j].use == FALSE) continue;
					if (CollisionBC(g_Enemy[i].pos, g_Enemy[j].pos, g_Enemy[i].size, g_Enemy[j].size))
					{
						g_Enemy[i].pos = oldPos;
					}
				}
			}

			else
			{
				// 移動処理
				g_Enemy[i].BodyAnimation(i);
			}

			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				g_Parts[i][j].Animation(ENEMY_ANIM_MOVE);
			}

			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}

		// エネミーをだんだん傾ける処理
		else if (g_Enemy[i].use == FALSE && g_Enemy[i].rot.z <= RADIAN * 90.0f)
		{
			g_Enemy[i].rot.z += RADIAN * 5.0f;
			g_Enemy[i].pos.y -= cosf(RADIAN * 5.0f) * ENEMY_OFFSET_Y * 0.03f;
			// 傾きに合わせて色も薄くしていく
			for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
			{
				SetModelDiffuse(&g_Enemy[i].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f * cos(g_Enemy[i].rot.z)));
			}

			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
				{
					SetModelDiffuse(&g_Parts[i][j].model, k, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f * cos(g_Enemy[i].rot.z)));
				}
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
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;


	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	//SetRimLight(1);

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == FALSE && g_Enemy[i].rot.z >= RADIAN * 90.0f) continue;


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


		// モデル描画
		DrawModel(&g_Enemy[i].model);
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
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
		tbl = g_DashTblAdr[tblNo[animNum]];	// 待機テーブルのアドレスを取得
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
			case ENEMY_ANIM_DASH:
				tbl[i] = g_DashTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
				break;
			default:
				tbl[i] = g_DashTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
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

// ボディを線形補完で動かす
void ENEMY::BodyAnimation(int i)
{
	if (tblMax[0] > 0)	// 線形補間を実行する？
	{
		INTERPOLATION_DATA* tbl = g_MoveTbl0Adr[tblNo[i]];	// 待機テーブルのアドレスを取得

		int nowNo = (int)time[0];			// 整数分であるテーブル番号を取り出している
		int maxNo = tblMax[0];				// 登録テーブル数を数えている
		int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている

		XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
		XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
		XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

		XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
		XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
		XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

		float nowTime = time[0] - nowNo;	// 時間部分である少数を取り出している

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
		time[0] += 1.0f / tbl[nowNo].frame;	// 時間を進めている
		if ((int)time[0] >= maxNo)			// 登録テーブル最後まで移動したか？
		{
			time[0] -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
		}
	}
}

int ENEMY::DecHP(int atk)
{
	hp -= atk;
	colCnt++;

	// 体力がなくなったらパーティクルなどをセット
	if (hp <= 0)
	{
		use = false;
		ReleaseShadow(shadowIdx);
		for (int i = 0; i < 10; i++)
		{
			SetParticle(pos, XMFLOAT4(0.8f, 0.7f, 0.2f, 0.85f));
		}
		SetEffect(XMFLOAT3(pos.x, pos.y - ENEMY_OFFSET_Y * EFFECT_HEIGHT / 15.0f, pos.z), EFFECT_WIDTH, EFFECT_HEIGHT, EFECT_DEAD);
		return 1;
	}

	else
	{
		SetEffect(XMFLOAT3(pos.x, pos.y - ENEMY_OFFSET_Y * EFFECT_HEIGHT / 15.0f, pos.z), EFFECT_WIDTH, EFFECT_HEIGHT, EFECT_HIT);
	}
	return 0;
}

//=============================================================================
// アニメーション関数
//=============================================================================
//void EnemyAnimation(int animNum, int i)
//{
//	// 線形補間の処理
//	INTERPOLATION_DATA* tbl;
//
//	switch (animNum)
//	{
//	case ENEMY_ANIM_STOP:
//		tbl = g_StopTblAdr[g_Parts[i].tblNo[animNum]];	// 待機テーブルのアドレスを取得
//		break;
//	case ENEMY_ANIM_MOVE:
//		tbl = g_MoveTblAdr[g_Parts[i].tblNo[animNum]];	// 待機テーブルのアドレスを取得
//		break;
//	default:
//		tbl = g_AttackTblAdr[g_Parts[i].tblNo[animNum]];	// 待機テーブルのアドレスを取得
//		break;
//	}
//
//	int nowNo = (int)g_Parts[i].time[animNum];			// 整数分であるテーブル番号を取り出している
//	int maxNo = g_Parts[i].tblMax[animNum];				// 登録テーブル数を数えている
//	int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
//
//	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
//	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
//	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換
//
//	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
//	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
//	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している
//
//	float nowTime = g_Parts[i].time[animNum] - nowNo;	// 時間部分である少数を取り出している
//
//	Pos = nowPos + Pos * nowTime;								// 現在の移動量を計算している
//	Rot = nowRot + Rot * nowTime;								// 現在の回転量を計算している
//	Scl = nowScl + Scl * nowTime;								// 現在の拡大率を計算している
//
//	// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
//	XMStoreFloat3(&g_Parts[i].pos, Pos);
//
//	// 計算して求めた回転量を現在の移動テーブルに足している
//	XMStoreFloat3(&g_Parts[i].rot, Rot);
//
//	// 計算して求めた拡大率を現在の移動テーブルに足している
//	XMStoreFloat3(&g_Parts[i].scl, Scl);
//
//	// frameを使て時間経過処理をする
//	g_Parts[i].time[animNum] += 1.0f / tbl[nowNo].frame;	// 時間を進めている
//	if ((int)g_Parts[i].time[animNum] >= maxNo)			// 登録テーブル最後まで移動したか？
//	{
//		g_Parts[i].time[animNum] -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
//	}
//}
//
////=============================================================================
//// モーションブレンド関数
////=============================================================================
//void PlayerAnimationBlend(int animNum1, int animNum2, int i)
//{
//	// アニメーション番号が同じだった場合
//	if (animNum1 == animNum2)
//	{
//		PlayerAnimation(animNum1, i);
//		return;
//	}
//
//	// アニメーションが変わったタイミングでカウントをリセットする
//	if (g_AnimTransFrameCnt[animNum1] < g_AnimTransFrameCnt[animNum2])
//	{
//		g_AnimTransFrameCnt[animNum2] = 0.0f;
//	}
//
//	int animNum[2] = { animNum1, animNum2 };
//
//	// 使われているなら処理する
//	if (/*(g_Parts[i].use == TRUE) && */(g_Parts[i].tblMax[animNum1] > 0))
//	{
//		INTERPOLATION_DATA* tbl[2];
//		XMVECTOR Pos[2];
//		XMVECTOR Rot[2];
//		XMVECTOR Scl[2];
//
//		for (int j = 0; j < 2; j++)
//		{	// 線形補間の処理
//			// 1つ目のアニメーションの計算
//			switch (animNum[j])
//			{
//			case ENEMY_ANIM_STOP:
//				tbl[j] = g_StopTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
//				break;
//			case ENEMY_ANIM_MOVE:
//				tbl[j] = g_MoveTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
//				break;
//			case ENEMY_ANIM_DASH:
//				tbl[j] = g_DashTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
//				break;
//			case ENEMY_ANIM_JUMP:
//				tbl[j] = g_JumpTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
//				break;
//			default:
//				tbl[j] = g_AttackTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
//				break;
//			}
//
//			int nowNo = (int)g_Parts[i].time[animNum[j]];			// 整数分であるテーブル番号を取り出している
//			int maxNo = g_Parts[i].tblMax[animNum[j]];				// 登録テーブル数を数えている
//			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
//
//			XMVECTOR nowPos = XMLoadFloat3(&tbl[j][nowNo].pos);	// XMVECTORへ変換
//			XMVECTOR nowRot = XMLoadFloat3(&tbl[j][nowNo].rot);	// XMVECTORへ変換
//			XMVECTOR nowScl = XMLoadFloat3(&tbl[j][nowNo].scl);	// XMVECTORへ変換
//
//			Pos[j] = XMLoadFloat3(&tbl[j][nextNo].pos) - nowPos;	// XYZ移動量を計算している
//			Rot[j] = XMLoadFloat3(&tbl[j][nextNo].rot) - nowRot;	// XYZ回転量を計算している
//			Scl[j] = XMLoadFloat3(&tbl[j][nextNo].scl) - nowScl;	// XYZ拡大率を計算している
//
//			float nowTime = g_Parts[i].time[animNum[j]] - nowNo;	// 時間部分である少数を取り出している
//
//			Pos[j] = nowPos + Pos[j] * nowTime;
//			Rot[j] = nowRot + Rot[j] * nowTime;
//			Scl[j] = nowScl + Scl[j] * nowTime;
//
//			// frameを使て時間経過処理をする
//			g_Parts[i].time[animNum[j]] += 1.0f / tbl[j][nowNo].frame;	// 時間を進めている
//			if ((int)g_Parts[i].time[animNum[j]] >= maxNo)			// 登録テーブル最後まで移動したか？
//			{
//				g_Parts[i].time[animNum[j]] -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
//			}
//		}
//		// 重みづけ
//		float weight = (1.0f / g_AnimTransFrame[animNum1]) * g_AnimTransFrameCnt[animNum1];
//		// モーションブレンドの座標計算
//		XMVECTOR blendPos = Pos[0] * weight + Pos[1] * (1.0f - weight);
//		XMVECTOR blendRot = Rot[0] * weight + Rot[1] * (1.0f - weight);
//		XMVECTOR blendScl = Scl[0] * weight + Scl[1] * (1.0f - weight);
//
//		// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
//		XMStoreFloat3(&g_Parts[i].pos, blendPos);
//
//		// 計算して求めた回転量を現在の移動テーブルに足している
//		XMStoreFloat3(&g_Parts[i].rot, blendRot);
//
//		// 計算して求めた拡大率を現在の移動テーブルに足している
//		XMStoreFloat3(&g_Parts[i].scl, blendScl);
//
//		g_AnimTransFrameCnt[animNum1] += 1.0f;
//		if (g_AnimTransFrameCnt[animNum1] >= g_AnimTransFrame[animNum1])
//		{
//			g_AnimTransFrameCnt[animNum1] = g_AnimTransFrame[animNum1];
//			g_AnimNum[i] = animNum1;
//
//			// アニメーションの移行が完了したタイミングでリセット
//			g_Parts[i].time[animNum2] = 0.0f;
//		}
//	}
//}