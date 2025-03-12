//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "input.h"
#include "camera.h"
#include "game.h"
#include "player.h"
#include "fade.h"
#include "enemy.h"
#include "shadow.h"
#include "bullet.h"
#include "effect.h"
#include "fade.h"
#include "sound.h"
#include "setting.h"
#include "collision.h"
#include "particle.h"
#include "debugproc.h"
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player_body.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_ARM_L	"data/MODEL/player_arm_l.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_ARM_R	"data/MODEL/player_arm_r.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_HAND_L	"data/MODEL/player_hand_l.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_HAND_R	"data/MODEL/player_hand_r.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_LEG_L	"data/MODEL/player_leg_l.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_LEG_R	"data/MODEL/player_leg_r.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_FOOT_L	"data/MODEL/player_foot_l.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_FOOT_R	"data/MODEL/player_foot_r.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_HEAD	"data/MODEL/player_head.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_SWORD_R	"data/MODEL/player_sword.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_SWORD_B	"data/MODEL/player_sword_back.obj"			// 読み込むモデル名
#define	MODEL_PLAYER_SCABBARD	"data/MODEL/player_scabbard.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(0.5f)							// 移動量
#define	VALUE_DASH			(2.0f)							// 移動量
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)				// 回転量

#define	BARRIER_FRAME		(40)							// 無敵時間
#define	BARRIER_WHITE		(BARRIER_FRAME / 5)				// 無敵時間の点滅間隔

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define RADIAN				(XM_PI / 180)					// ラジアン変換用

#define PLAYER_HEAD_Y		(2.7f)								// 頭のx座標

#define PLAYER_ARM_X		(1.5f)								// 腕のx座標
#define PLAYER_ARM_Y		(2.5f)								// 腕のy座標

#define PLAYER_HAND_X		(0.0f)								// 手のx座標
#define PLAYER_HAND_Y		(-1.5f)								// 手のy座標

#define PLAYER_LEG_X		(1.1f)								// 脚のx座標
#define PLAYER_LEG_Y		(-2.4f)								// 脚のy座標

#define PLAYER_FOOT_X		(0.0f)								// 足のx座標
#define PLAYER_FOOT_Y		(-1.8f)								// 足のy座標

#define PLAYER_SWORD_R_X	(0.0f)								// 剣のx座標
#define PLAYER_SWORD_R_Y	(-2.0f)								// 剣のy座標

#define PLAYER_SWORD_B_X	(0.0f)								// 剣(背中)のx座標
#define PLAYER_SWORD_B_Y	(1.0f)								// 剣(背中)のy座標
#define PLAYER_SWORD_B_Z	(-1.5f)								// 剣(背中)のz座標

#define ANIM_FRAME_STOP		(15.0f)							// 待機アニメーションの間隔
#define ANIM_FRAME_MOVE		(30.0f)							// 移動アニメーションの間隔
#define ANIM_FRAME_DASH		(ANIM_FRAME_MOVE * 0.5f)		// ダッシュアニメーションの間隔

#define ANIM_FRAME_JUMP		(15.0f)								// ジャンプアニメーションの間隔
#define ANIM_FRAME_ATTACK	(10.0f)								// ジャンプアニメーションの間隔

#define PLAYER_JUMP_Y		(50.0f)								// ジャンプ力
#define PLAYER_JUMP_CNT_MAX	(ANIM_FRAME_JUMP * 3.0f - 5.0f)			// ジャンプ全体フレーム
#define PLAYER_ATK_CNT_MAX	(ANIM_FRAME_ATTACK * 3.0f)			// アタック全体フレーム

#define BLEND_FRAME_STOP	(60.0f)							// 待機モーションに遷移するまでの時間
#define BLEND_FRAME_MOVE	(120.0f)							// 移動モーションに遷移するまでの時間
#define BLEND_FRAME_DASH	(120.0f)							// ダッシュモーションに遷移するまでの時間
#define BLEND_FRAME_JUMP	(120.0f)							// ジャンプモーションに遷移するまでの時間
#define BLEND_FRAME_ATTACK	(120.0f)							// 攻撃モーションに遷移するまでの時間

#define ATTACK_WIDTH		(5.0f)							// 攻撃の当たり判定の幅
#define ATTACK_DEPTH		(10.0f)							// 攻撃の当たり判定の奥行き

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player[PLAYER_MAX];						// プレイヤー

static PLAYER		g_Parts[PLAYER_MAX][PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static float		roty = 0.0f;
static float		g_RotDead;						// プレイヤー死亡時にだんだん薄くする

static LIGHT		g_Light;

// プレイヤーの階層アニメーションデータ

// 待機モーションデータ
static INTERPOLATION_DATA stop_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y + 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y - 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y - 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y + 0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },

};

static INTERPOLATION_DATA stop_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },

};

static INTERPOLATION_DATA stop_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
};

static INTERPOLATION_DATA stop_tbl_sword_b[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },

};

static INTERPOLATION_DATA stop_tbl_scabbard[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_STOP },

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
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 40.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(RADIAN * 20.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 30.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 30.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },

};

static INTERPOLATION_DATA move_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 15.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(-RADIAN * 15.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(-RADIAN * 15.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 15.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },

};

static INTERPOLATION_DATA move_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
};

static INTERPOLATION_DATA move_tbl_sword_b[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },

};

static INTERPOLATION_DATA move_tbl_scabbard[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_MOVE },

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
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 80.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 80.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 80.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 80.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },

};

static INTERPOLATION_DATA dash_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },

};

static INTERPOLATION_DATA dash_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
};

static INTERPOLATION_DATA dash_tbl_sword_b[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },

};

static INTERPOLATION_DATA dash_tbl_scabbard[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),		XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_DASH },

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

// ジャンプモーションデータ
static INTERPOLATION_DATA jump_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 100.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 190.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 100.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 190.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 40.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 45.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },

};

static INTERPOLATION_DATA jump_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 90.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 90.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },

};

static INTERPOLATION_DATA jump_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_sword_b[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },

};

static INTERPOLATION_DATA jump_tbl_scabbard[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },

};

static INTERPOLATION_DATA* g_JumpTblAdr[] =
{
	jump_tbl_head,
	jump_tbl_arm_l,
	jump_tbl_arm_r,
	jump_tbl_hand_l,
	jump_tbl_hand_r,
	jump_tbl_leg_l,
	jump_tbl_leg_r,
	jump_tbl_foot_l,
	jump_tbl_foot_r,
	jump_tbl_sword_r,
	jump_tbl_sword_b,
	jump_tbl_scabbard,
};

// アタックモーションデータ
static INTERPOLATION_DATA attack_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f , 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 170.0f, RADIAN * 30.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 25.0f, -RADIAN * 30.0f, RADIAN * 15.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 170.0f, RADIAN * 30.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK},
};

static INTERPOLATION_DATA attack_tbl_hand_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_hand_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 110.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_HAND_X, PLAYER_HAND_Y, 0.0f), XMFLOAT3(-RADIAN * 110.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_leg_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },

};

static INTERPOLATION_DATA attack_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },

};

static INTERPOLATION_DATA attack_tbl_sword_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_SWORD_R_X, PLAYER_SWORD_R_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
};

static INTERPOLATION_DATA attack_tbl_sword_b[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },

};

static INTERPOLATION_DATA attack_tbl_scabbard[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },
	{ XMFLOAT3(PLAYER_SWORD_B_X, PLAYER_SWORD_B_Y, PLAYER_SWORD_B_Z), XMFLOAT3(0.0f, 0.0f, -RADIAN * 30.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_ATTACK },

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
	attack_tbl_sword_b,
	attack_tbl_scabbard,
};

// 選択したアニメーションに移行するフレーム
static float g_AnimTransFrame[PLAYER_ANIM_MAX] =
{
	BLEND_FRAME_STOP,
	BLEND_FRAME_MOVE,
	BLEND_FRAME_DASH,
	BLEND_FRAME_JUMP,
	BLEND_FRAME_ATTACK,
};

static float g_AnimTransFrameCnt[PLAYER_ANIM_MAX];			// アニメーションに移行するフレームのカウンタ

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		g_Player[i].load = TRUE;
		LoadModel(MODEL_PLAYER, &g_Player[i].model);

		g_Player[i].pos = XMFLOAT3(0.0f, PLAYER_OFFSET_Y, 0.0f);
		g_Player[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Player[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Player[i].dir = XMFLOAT2(0.0f, 0.0f);

		g_Player[i].spd = 0.0f;			// 移動スピードクリア
		g_Player[i].hp = 20;				// 体力の初期化
		g_Player[i].gauge = 0;			// 体力の初期化
		g_Player[i].colCnt = 0;			// 体力の初期化

		g_Player[i].attack = FALSE;		// アタックフラグクリア
		g_Player[i].atkVal = 1;			// 攻撃力初期化
		g_Player[i].atkCnt = 0;			// 移動スピードクリア
		g_Player[i].dissolveCnt = 0.0f;	// ディゾルブのカウンタクリア

		g_Player[i].use = TRUE;			// TRUE:生きてる
		g_Player[i].size = PLAYER_SIZE;	// 当たり判定の大きさ
		g_Player[i].jump = FALSE;			// ジャンプフラグクリア
		g_Player[i].jumpCnt = 0;			// ジャンプカウンタクリア

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Player[i].model, &g_Player[i].diffuse[0]);

		// ここでプレイヤー用の影を作成している
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		//g_Player[i].shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ↑
		//        このメンバー変数が生成した影のIndex番号

		// キーを押した時のプレイヤーの向き
		roty = 0.0f;

		// プレイヤー死亡時徐々に薄くする 
		g_RotDead = 0.0f;

		g_Player[i].parent = NULL;			// 本体（親）なのでNULLを入れる


		// 階層アニメーションの初期化
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = FALSE;

			// 位置・回転・スケールの初期設定
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Parts[i][j].parent = &g_Player[i];		// ← ここに親のアドレスを入れる
			//	g_Parts[腕].parent= &g_Player[i];		// 腕だったら親は本体（プレイヤー）
			//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

				// 階層アニメーション用のメンバー変数の初期化
			for (int j = 0; j < PLAYER_ANIM_MAX; j++)
			{
				g_Parts[i][j].time[j] = 0.0f;			// 線形補間用のタイマーをクリア
				g_Parts[i][j].tblNo[j] = 0;			// 再生する行動データテーブルNoをセット
				g_Parts[i][j].tblMax[j] = 0;			// 再生する行動データテーブルのレコード数をセット
			}
			// パーツの読み込みはまだしていない
			g_Parts[i][j].load = FALSE;


			// パーツの初期アニメーションを設定
			g_Parts[i][j].animNum = PLAYER_ANIM_STOP;
		}

		g_Parts[i][PLAYER_PARTS_HEAD].use = TRUE;
		g_Parts[i][PLAYER_PARTS_HEAD].parent = &g_Player[i];	// 親をセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HEAD].load = 1;
		LoadModel(MODEL_PLAYER_HEAD, &g_Parts[i][PLAYER_PARTS_HEAD].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_HEAD].model, &g_Parts[i][PLAYER_PARTS_HEAD].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_ARM_L].use = TRUE;
		g_Parts[i][PLAYER_PARTS_ARM_L].parent = &g_Player[i];	// 親をセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_L].load = 1;
		LoadModel(MODEL_PLAYER_ARM_L, &g_Parts[i][PLAYER_PARTS_ARM_L].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_ARM_L].model, &g_Parts[i][PLAYER_PARTS_ARM_L].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_ARM_R].use = TRUE;
		g_Parts[i][PLAYER_PARTS_ARM_R].parent = &g_Player[i];	// 親をセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_ARM_R].load = 1;
		LoadModel(MODEL_PLAYER_ARM_R, &g_Parts[i][PLAYER_PARTS_ARM_R].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_ARM_R].model, &g_Parts[i][PLAYER_PARTS_ARM_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_HAND_L].use = TRUE;
		g_Parts[i][PLAYER_PARTS_HAND_L].parent = &g_Parts[i][PLAYER_PARTS_ARM_L];	// 親をセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_L].load = 1;
		LoadModel(MODEL_PLAYER_HAND_L, &g_Parts[i][PLAYER_PARTS_HAND_L].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_HAND_L].model, &g_Parts[i][PLAYER_PARTS_HAND_L].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_HAND_R].use = TRUE;
		g_Parts[i][PLAYER_PARTS_HAND_R].parent = &g_Parts[i][PLAYER_PARTS_ARM_R];	// 親をセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_HAND_R].load = 1;
		LoadModel(MODEL_PLAYER_HAND_R, &g_Parts[i][PLAYER_PARTS_HAND_R].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_HAND_R].model, &g_Parts[i][PLAYER_PARTS_HAND_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_LEG_L].use = TRUE;
		g_Parts[i][PLAYER_PARTS_LEG_L].parent = &g_Player[i];	// 親をセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_L].load = 1;
		LoadModel(MODEL_PLAYER_LEG_L, &g_Parts[i][PLAYER_PARTS_LEG_L].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_LEG_L].model, &g_Parts[i][PLAYER_PARTS_LEG_L].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_LEG_R].use = TRUE;
		g_Parts[i][PLAYER_PARTS_LEG_R].parent = &g_Player[i];	// 親をセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_LEG_R].load = 1;
		LoadModel(MODEL_PLAYER_LEG_R, &g_Parts[i][PLAYER_PARTS_LEG_R].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_LEG_R].model, &g_Parts[i][PLAYER_PARTS_LEG_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_FOOT_L].use = TRUE;
		g_Parts[i][PLAYER_PARTS_FOOT_L].parent = &g_Parts[i][PLAYER_PARTS_LEG_L];	// 親をセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_L].load = 1;
		LoadModel(MODEL_PLAYER_FOOT_L, &g_Parts[i][PLAYER_PARTS_FOOT_L].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_FOOT_L].model, &g_Parts[i][PLAYER_PARTS_FOOT_L].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_FOOT_R].use = TRUE;
		g_Parts[i][PLAYER_PARTS_FOOT_R].parent = &g_Parts[i][PLAYER_PARTS_LEG_R];	// 親をセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_FOOT_R].load = 1;
		LoadModel(MODEL_PLAYER_FOOT_R, &g_Parts[i][PLAYER_PARTS_FOOT_R].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_FOOT_R].model, &g_Parts[i][PLAYER_PARTS_FOOT_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_SWORD_R].use = FALSE;
		g_Parts[i][PLAYER_PARTS_SWORD_R].parent = &g_Parts[i][PLAYER_PARTS_HAND_R];	// 親をセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_SWORD_R;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_R].load = 1;
		LoadModel(MODEL_PLAYER_SWORD_R, &g_Parts[i][PLAYER_PARTS_SWORD_R].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_SWORD_R].model, &g_Parts[i][PLAYER_PARTS_SWORD_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_SWORD_B].use = TRUE;
		g_Parts[i][PLAYER_PARTS_SWORD_B].parent = &g_Player[i];	// 親をセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_SWORD_B;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_SWORD_B;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_SWORD_B;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_SWORD_B;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_SWORD_B;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SWORD_B].load = 1;
		LoadModel(MODEL_PLAYER_SWORD_B, &g_Parts[i][PLAYER_PARTS_SWORD_B].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_SWORD_B].model, &g_Parts[i][PLAYER_PARTS_SWORD_B].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_SCABBARD].use = TRUE;
		g_Parts[i][PLAYER_PARTS_SCABBARD].parent = &g_Player[i];	// 親をセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_SCABBARD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_SCABBARD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_SCABBARD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_SCABBARD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_SCABBARD;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][PLAYER_PARTS_SCABBARD].load = 1;
		LoadModel(MODEL_PLAYER_SCABBARD, &g_Parts[i][PLAYER_PARTS_SCABBARD].model);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_SCABBARD].model, &g_Parts[i][PLAYER_PARTS_SCABBARD].diffuse[0]);

		// 最初からパーツをくっつけておく
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[i][j].Animation(PLAYER_ANIM_STOP);
		}

		// クォータニオンの初期化
		XMStoreFloat4(&g_Player[i].Quaternion, XMQuaternionIdentity());

		for (int j = 0; j < PLAYER_ANIM_MAX; j++)
		{
			g_AnimTransFrameCnt[j] = 0.0f;
		}
	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// モデルの解放処理
		if (g_Player[i].load == TRUE)
		{
			UnloadModel(&g_Player[i].model);
			g_Player[i].load = FALSE;
		}

		// パーツの解放処理
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
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
void UpdatePlayer(void)
{
	if (GetMode() == MODE_TITLE)
	{
		// 階層アニメーション
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			// 使われているなら処理する
			// 線形補間の処理
			g_Parts[0][i].Animation(PLAYER_ANIM_MOVE);
		}
	}

	// ゲームモードの処理
	else
	{

		for (int i = 0; i < PLAYER_MAX; i++)
		{


			if (g_Player[i].use == TRUE && GetFade() == FADE_NONE)
			{
				// 無敵時間は色替えをする
				if (g_Player[i].colCnt > 0)
				{
					// 白く光らせる
					if (g_Player[i].colCnt % BARRIER_WHITE == 0)
					{
						for (int j = 0; j < g_Player[i].model.SubsetNum; j++)
						{
							SetModelDiffuse(&g_Player[i].model, j, XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f));
						}

						for (int j = 0; j < PLAYER_PARTS_MAX; j++)
						{
							for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
							{
								SetModelDiffuse(&g_Parts[i][j].model, k, XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f));
							}
						}
					}

					// 元の色
					else
					{
						for (int j = 0; j < g_Player[i].model.SubsetNum; j++)
						{
							SetModelDiffuse(&g_Player[i].model, j, g_Player[i].diffuse[j]);
						}

						for (int j = 0; j < PLAYER_PARTS_MAX; j++)
						{
							for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
							{
								SetModelDiffuse(&g_Parts[i][j].model, k, g_Parts[i][j].diffuse[k]);
							}
						}
					}

					g_Player[i].colCnt++;
					if (g_Player[i].colCnt > BARRIER_FRAME)
					{
						// 無敵時間終了時に元の色に戻す
						g_Player[i].colCnt = 0;
						for (int j = 0; j < g_Player[i].model.SubsetNum; j++)
						{
							SetModelDiffuse(&g_Player[i].model, j, g_Player[i].diffuse[i]);
						}

						for (int j = 0; j < PLAYER_PARTS_MAX; j++)
						{
							for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
							{
								SetModelDiffuse(&g_Parts[i][j].model, k, g_Parts[i][j].diffuse[k]);
							}
						}
					}
				}

				CAMERA* cam = GetCamera();

				g_Player[i].spd *= 0.4f;
				// 移動フラグリセット
				g_Player[i].dir = XMFLOAT2(0.0f, 0.0f);
				// アニメーションを待機モーションにリセット
				int animNum = PLAYER_ANIM_STOP;

				// 移動処理
				if (GetKeyboardPress(DIK_LEFT) || GetKeyboardPress(DIK_A))
				{
					g_Player[i].spd = VALUE_MOVE;
					//g_Player[i].pos.x -= g_Player[i].spd;
					g_Player[i].dir.x = -1.0f;
					//roty = XM_PI / 2;
					animNum = PLAYER_ANIM_MOVE;
				}
				if (GetKeyboardPress(DIK_RIGHT) || GetKeyboardPress(DIK_D))
				{
					g_Player[i].spd = VALUE_MOVE;
					//g_Player[i].pos.x += g_Player[i].spd;
					g_Player[i].dir.x = 1.0f;
					//roty = -XM_PI / 2;
					animNum = PLAYER_ANIM_MOVE;
				}

				if (GetKeyboardPress(DIK_UP) || GetKeyboardPress(DIK_W))
				{
					g_Player[i].spd = VALUE_MOVE;
					//g_Player[i].pos.z += g_Player[i].spd;
					g_Player[i].dir.y = 1.0f;
					//roty = XM_PI;
					animNum = PLAYER_ANIM_MOVE;
				}
				if (GetKeyboardPress(DIK_DOWN) || GetKeyboardPress(DIK_S))
				{
					g_Player[i].spd = VALUE_MOVE;
					//g_Player[i].pos.z -= g_Player[i].spd;
					g_Player[i].dir.y = -1.0f;
					//roty = 0.0f;
					animNum = PLAYER_ANIM_MOVE;
				}

				// ゲームパット操作
				if (IsButtonPressed(0, BUTTON_LEFT))
				{
					g_Player[i].spd = VALUE_MOVE;
					//g_Player[i].pos.x -= g_Player[i].spd;
					g_Player[i].dir.x = XM_PI / 2;
					//roty = XM_PI / 2;
					animNum = PLAYER_ANIM_MOVE;
				}
				if (IsButtonPressed(0, BUTTON_RIGHT))
				{
					g_Player[i].spd = VALUE_MOVE;
					//g_Player[i].pos.x += g_Player[i].spd;
					g_Player[i].dir.x = -XM_PI / 2;
					//roty = -XM_PI / 2;
					animNum = PLAYER_ANIM_MOVE;
				}

				if (IsButtonPressed(0, BUTTON_UP))
				{
					g_Player[i].spd = VALUE_MOVE;
					//g_Player[i].pos.z += g_Player[i].spd;
					g_Player[i].dir.y = XM_PI;
					//roty = XM_PI;
					animNum = PLAYER_ANIM_MOVE;
				}
				if (IsButtonPressed(0, BUTTON_DOWN))
				{
					g_Player[i].spd = VALUE_MOVE;
					//g_Player[i].pos.z -= g_Player[i].spd;
					g_Player[i].dir.y = 0.0f;
					//roty = 0.0f;
					animNum = PLAYER_ANIM_MOVE;
				}

				// ダッシュ処理
				if (GetKeyboardPress(DIK_LSHIFT) || IsButtonPressed(0, BUTTON_X))
				{
					g_Player[i].spd *= VALUE_DASH;

					// 移動キーを押している間はダッシュアニメーションをセット
					if (g_Player[i].spd >= VALUE_MOVE * VALUE_DASH)
					{
						animNum = PLAYER_ANIM_DASH;
					}
				}

				// 設定画面を開く
				if (GetKeyboardTrigger(DIK_P))
				{
					SetSettingFlag();
				}

				else if (IsButtonTriggered(0, BUTTON_SELECT))
				{
					SetSettingFlag();
				}

#ifdef _DEBUG
				if (GetKeyboardPress(DIK_R))
				{
					g_Player[i].pos.z = g_Player[i].pos.x = 0.0f;
					g_Player[i].spd = 0.0f;
					roty = 0.0f;
				}

				//if (GetKeyboardPress(DIK_J))
				//{
				//	g_Player[i].pos.y += g_Player[i].pos.x = 0.0f;
				//	g_Player[i].spd = 0.0f;
				//	roty = 0.0f;
				//}
#endif
				if (g_Player[i].dir.x != 0.0f || g_Player[i].dir.y != 0.0f)
				{
					roty = atan2f(g_Player[i].dir.x, g_Player[i].dir.y) + XM_PI;
				}

				//    // Key入力があったら移動処理する
				if (g_Player[i].spd > 0.0f)
				{
					g_Player[i].rot.y = Turn(roty + cam->rot.y, g_Player[i].rot.y);

					// 入力のあった方向へプレイヤーを向かせて移動させる
					g_Player[i].pos.x -= sinf(roty + cam->rot.y) * g_Player[i].spd;
					g_Player[i].pos.z -= cosf(roty + cam->rot.y) * g_Player[i].spd;
				}

				//{	// 押した方向にプレイヤーを移動させる
				//	// 押した方向にプレイヤーを向かせている所
				//	g_Player[i].rot.y = roty + cam->rot.y;

				//	g_Player[i].pos.x -= sinf(g_Player[i].rot.y) * g_Player[i].spd;
				//	g_Player[i].pos.z -= cosf(g_Player[i].rot.y) * g_Player[i].spd;
				//}
				
				// レイキャストして足元の高さを求める
				XMFLOAT3 HitPosition;		// 交点
				XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
				BOOL ans = RayHitField(g_Player[i].pos, &HitPosition, &Normal);
				if (ans)
				{
					g_Player[i].pos.y = HitPosition.y + PLAYER_OFFSET_Y;
				}
				else
				{
					g_Player[i].pos.y = PLAYER_OFFSET_Y;
					Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}

				//壁の座標を超えないようにする
				if (g_Player[i].pos.x > MAP_RIGHT - g_Player[i].size * 0.5f)
				{
					g_Player[i].pos.x = MAP_RIGHT - g_Player[i].size * 0.5f;
				}
				if (g_Player[i].pos.x < MAP_LEFT + g_Player[i].size * 0.5f)
				{
					g_Player[i].pos.x = MAP_LEFT + g_Player[i].size * 0.5f;
				}
				if (g_Player[i].pos.z > MAP_TOP - g_Player[i].size * 0.5f)
				{
					g_Player[i].pos.z = MAP_TOP - g_Player[i].size * 0.5f;
				}
				if (g_Player[i].pos.z < MAP_DOWN + g_Player[i].size * 0.5f)
				{
					g_Player[i].pos.z = MAP_DOWN + g_Player[i].size * 0.5f;
				}


				// 弾発射処理
				//if (GetKeyboardTrigger(DIK_SPACE))
				//{
				//	SetBullet(g_Player[i].pos, g_Player[i].rot);
				//}

				// 影もプレイヤーの位置に合わせる
				//XMFLOAT3 pos = g_Player[i].pos;
				//pos.y -= (PLAYER_OFFSET_Y - 0.1f);
				//SetPositionShadow(g_Player[i].shadowIdx, pos);

				// ジャンプ処理中
				if (g_Player[i].jump == TRUE)
				{
					float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player[i].jumpCnt;
					float y = PLAYER_JUMP_Y * cosf(XM_PI / 2 + angle);
					//g_Player[i].pos.y -= y;

					g_Player[i].jumpCnt++;
					if (g_Player[i].jumpCnt > PLAYER_JUMP_CNT_MAX)
					{
						g_Player[i].jump = FALSE;
						g_Player[i].jumpCnt = 0;
						g_AnimTransFrameCnt[PLAYER_ANIM_JUMP] = 0.0f;
						for (int j = 0; j < PLAYER_PARTS_MAX; j++)
						{
							g_Parts[i][j].time[PLAYER_ANIM_JUMP] = 0.0f;
						}
					}

				}
				// ジャンプボタン押した？
				else if ((g_Player[i].jump == FALSE) && (GetKeyboardPress(DIK_X)))
				{
					g_Player[i].jump = TRUE;
					g_Player[i].jumpCnt = 0;
				}

				ENEMY* enemy = GetEnemy();
				ENEMY* boss = GetBoss();
				// アタック処理中
				if (g_Player[i].attack == TRUE)
				{
					// アタックアニメーションをセット
					animNum = PLAYER_ANIM_ATTACK;
					g_Player[i].atkCnt++;
					// 剣を抜いたように見えるときだけ手に持つようにセット
					if ((int)g_Parts[i][PLAYER_PARTS_ARM_R].time[PLAYER_ANIM_ATTACK] < 1)
					{
						g_Parts[i][PLAYER_PARTS_SWORD_R].use = FALSE;
						g_Parts[i][PLAYER_PARTS_SWORD_B].use = TRUE;
					}

					else
					{
						g_Parts[i][PLAYER_PARTS_SWORD_R].use = TRUE;
						g_Parts[i][PLAYER_PARTS_SWORD_B].use = FALSE;
						XMFLOAT3 pos = g_Player[i].pos;

						pos.x -= (float)sinf(g_Player[i].rot.y) * ATTACK_DEPTH * 0.5f;
						pos.z -= (float)cosf(g_Player[i].rot.y) * ATTACK_DEPTH * 0.5f;

						for (int j = 0; j < ENEMY_MAX; j++)
						{
							// 生きているエネミーのみ判定
							if (enemy[j].use == TRUE && enemy[j].colCnt == 0)
							{
								if (CollisionBC(pos, enemy[j].pos, ATTACK_DEPTH, enemy[j].size))
								{
									g_Player[i].gauge += enemy[j].DecHP(g_Player[i].atkVal);
								}
							}
						}

						for (int j = 0; j < BOSS_MAX; j++)
						{
							if (boss[j].use == TRUE && boss[j].colCnt == 0)
							{
								if (CollisionBC(pos, boss[j].pos, ATTACK_DEPTH, boss[j].size))
								{
									boss[j].DecHP(g_Player[i].atkVal + g_Player[i].gauge);
								}
							}
						}
					}

					if (g_Player[i].atkCnt >= PLAYER_ATK_CNT_MAX)
					{
						g_Player[i].attack = FALSE;
						g_Player[i].atkCnt = 0;
						g_AnimTransFrameCnt[PLAYER_ANIM_ATTACK] = 0.0f;

						for (int j = 0; j < PLAYER_PARTS_MAX; j++)
						{
							g_Parts[i][j].time[PLAYER_ANIM_ATTACK] = 0.0f;
							g_Parts[i][PLAYER_PARTS_SWORD_R].use = FALSE;
							g_Parts[i][PLAYER_PARTS_SWORD_B].use = TRUE;
						}
					}
				}
				// アタックボタン押した？
				else if ((g_Player[i].attack == FALSE) && (GetKeyboardTrigger(DIK_SPACE)))
				{
					g_Player[i].attack = TRUE;
					g_Player[i].atkCnt = 0;
					animNum = PLAYER_ANIM_ATTACK;
					PlaySound(SOUND_LABEL_SE_slash);
				}

				else if ((g_Player[i].attack == FALSE) && (IsButtonTriggered(0, BUTTON_A)))
				{
					g_Player[i].attack = TRUE;
					g_Player[i].atkCnt = 0;
					animNum = PLAYER_ANIM_ATTACK;
					PlaySound(SOUND_LABEL_SE_slash);
				}

				// 階層アニメーション
				for (int j = 0; j < PLAYER_PARTS_MAX; j++)
				{
					if (i == 0)
					{
						// 使われているなら処理する
						// 線形補間の処理
						if (g_Player[i].jump == TRUE)
						{
							// アタックアニメーションを優先
							if (g_Player[i].attack == TRUE)
							{
								g_Parts[i][j].Animation(animNum, PLAYER_ANIM_JUMP);
							}

							else
							{
								g_Parts[i][j].Animation(PLAYER_ANIM_JUMP, animNum);
							}
						}

						else
						{
							g_Parts[i][j].Animation(animNum, g_Parts[i][j].animNum);
						}
					}
					else
					{
						if (animNum != g_Parts[i][j].animNum)
						{
							g_Parts[i][j].time[animNum] = 0;
							g_Parts[i][j].animNum = animNum;
						}
						if (g_Player[i].jump == TRUE)
						{
							animNum = PLAYER_ANIM_JUMP;
						}
						g_Parts[i][j].Animation(animNum);
					}
				}

				// ポイントライトのテスト
				{
					LIGHT* light = GetLightData(1);
					XMFLOAT3 pos = g_Player[i].pos;
					pos.y += 100.0f;

					light->Position = pos;
					light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
					light->Type = LIGHT_TYPE_POINT;
					light->Enable = TRUE;
					SetLightData(1, light);
				}



				//////////////////////////////////////////////////////////////////////
				// 姿勢制御
				//////////////////////////////////////////////////////////////////////

				XMVECTOR vx, nvx, up;
				XMVECTOR quat;
				float len, angle;


				g_Player[i].UpVector = Normal;
				up = { 0.0f, 1.0f, 0.0f, 0.0f };
				vx = XMVector3Cross(up, XMLoadFloat3(&g_Player[i].UpVector));

				nvx = XMVector3Length(vx);
				XMStoreFloat(&len, nvx);
				nvx = XMVector3Normalize(vx);
				//nvx = vx / len;
				angle = asinf(len);

				//quat = XMQuaternionIdentity();

			//	quat = XMQuaternionRotationAxis(nvx, angle);
				quat = XMQuaternionRotationNormal(nvx, angle);


				quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player[i].Quaternion), quat, 0.05f);
				XMStoreFloat4(&g_Player[i].Quaternion, quat);


#ifdef _DEBUG
				// デバッグ表示
				//PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z, Normal.y);
#endif
			}

			if (g_Player[i].use == FALSE)
			{
				g_RotDead += RADIAN * 5.0f;
				for (int j = 0; j < g_Player[i].model.SubsetNum; j++)
				{
					SetModelDiffuse(&g_Player[i].model, j, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f * (float)cos(g_RotDead)));
				}

				for (int j = 0; j < PLAYER_PARTS_MAX; j++)
				{
					for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
					{
						SetModelDiffuse(&g_Parts[i][j].model, k, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f * (float)cos(g_RotDead)));
					}
				}

				XMFLOAT4 playerDiffuse[MODEL_MAX_MATERIAL];
				GetModelDiffuse(&g_Player[i].model, &playerDiffuse[0]);

				if (playerDiffuse[0].w <= 0.0f)
				{
					SetFade(FADE_OUT, MODE_GAMEOVER);
				}
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (GetViewPortTypeGame() == TYPE_FULL_SCREEN && i == 1) continue;

		if (g_Player[i].use == TRUE || (GetMode() == MODE_TITLE && i == 0))
		{
			if (GetViewPortTypeGame() == TYPE_LEFT_HALF_SCREEN && i != GetPlayerNum()) continue;

			// マテリアル退避用
			//XMFLOAT4 playerDiffuse[MODEL_MAX_MATERIAL];
			//XMFLOAT4 partsDiffuse[PLAYER_PARTS_MAX][MODEL_MAX_MATERIAL];
			// 平坦化行列の影描画
			//{
			//	GetModelDiffuse(&g_Player[i].model, &playerDiffuse[0]);
			//	for (int j = 0; j < g_Player[i].model.SubsetNum; j++)
			//	{
			//		SetModelDiffuse(&g_Player[i].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
			//	}

			//	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			//	// ワールドマトリックスの初期化
			//	mtxWorld = XMMatrixIdentity();

			//	// スケールを反映
			//	mtxScl = XMMatrixScaling(g_Player[i].scl.x * 1.2f, 0.1f, g_Player[i].scl.z * 1.2f);
			//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			//	// 回転を反映
			//	mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
			//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//	// 移動を反映
			//	mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y - PLAYER_OFFSET_Y, g_Player[i].pos.z);
			//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			//	// ワールドマトリックスの設定
			//	SetWorldMatrix(&mtxWorld);

			//	XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);


			//	// モデル描画
			//	DrawModel(&g_Player[i].model);

			//	// 階層アニメーション
			//	for (int j = 0; j < PLAYER_PARTS_MAX; j++)
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
			//		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
			//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//		// 移動を反映
			//		mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
			//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			//		if (g_Parts[i][j].parent != NULL)	// 子供だったら親と結合する
			//		{
			//			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
			//			// ↑
			//			// g_Player[i].mtxWorldを指している
			//		}

			//		XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

			//		// 使われているなら処理する
			//		if (g_Parts[i][j].use == FALSE) continue;

			//		// ワールドマトリックスの設定
			//		SetWorldMatrix(&mtxWorld);

			//		// モデル描画
			//		DrawModel(&g_Parts[i][j].model);

			//	}
			//}

			// モデル描画
			{
				//for (int j = 0; j < g_Player[i].model.SubsetNum; j++)
				//{
				//	SetModelDiffuse(&g_Player[i].model, j, playerDiffuse[j]);
				//}

				// ボス戦に入ったら光らせる
				//if (GetBossFlg() == TRUE)
				{
					SetRimLight(2);
				}
				XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

				// カリング無効
				SetCullingMode(CULL_MODE_NONE);

				// ワールドマトリックスの初期化
				mtxWorld = XMMatrixIdentity();

				// スケールを反映
				mtxScl = XMMatrixScaling(g_Player[i].scl.x, g_Player[i].scl.y, g_Player[i].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// 回転を反映
				mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// クォータニオンを反映
				quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player[i].Quaternion));
				mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

				// 移動を反映
				mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				// ワールドマトリックスの設定
				SetWorldMatrix(&mtxWorld);

				XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);

				// モデル描画
				DrawModel(&g_Player[i].model);


				// 階層アニメーション
				for (int j = 0; j < PLAYER_PARTS_MAX; j++)
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
						// g_Player[i].mtxWorldを指している
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
		}
	}
	SetRimLight(0);
	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER* GetPlayerParts(void)
{
	return &g_Parts[0][0];
}

//=============================================================================
// 回転量調節
//=============================================================================
float NormalizeAngle(float angle)
{

	while (angle > XM_PI) angle -= 2 * XM_PI;
	while (angle < -XM_PI) angle += 2 * XM_PI;
	return angle;
}

//=============================================================================
// プレイヤーの向きを変更
//=============================================================================
float Turn(float target, float current)
{
	target = NormalizeAngle(target);
	current = NormalizeAngle(current);
	float diff = target - current;
	diff = NormalizeAngle(diff);

	return current + diff / 10.0f;
}

//=============================================================================
// アニメーション関数
//=============================================================================
void PLAYER::Animation(int animNum)
{
	INTERPOLATION_DATA* tbl;

	switch (animNum)
	{
	case PLAYER_ANIM_STOP:
		tbl = g_StopTblAdr[tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	case PLAYER_ANIM_MOVE:
		tbl = g_MoveTblAdr[tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	case PLAYER_ANIM_DASH:
		tbl = g_DashTblAdr[tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	case PLAYER_ANIM_JUMP:
		tbl = g_JumpTblAdr[tblNo[animNum]];	// 待機テーブルのアドレスを取得
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
void PLAYER::Animation(int animNum1, int animNum2)
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
			case PLAYER_ANIM_STOP:
				tbl[i] = g_StopTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
				break;
			case PLAYER_ANIM_MOVE:
				tbl[i] = g_MoveTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
				break;
			case PLAYER_ANIM_DASH:
				tbl[i] = g_DashTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
				break;
			case PLAYER_ANIM_JUMP:
				tbl[i] = g_JumpTblAdr[tblNo[AnimNum[i]]];	// 待機テーブルのアドレスを取得
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

//=============================================================================
// モーションブレンド関数
//=============================================================================
void PLAYER::DecHP(int atk)
{
	hp -= atk;
	colCnt++;
	SetEffect(XMFLOAT3(pos.x, pos.y - PLAYER_OFFSET_Y * EFFECT_HEIGHT / 15.0f, pos.z), EFFECT_WIDTH, EFFECT_HEIGHT, EFFECT_HIT);
	PlaySound(SOUND_LABEL_SE_hit);

	if (hp <= 0)
	{
		use = FALSE;
		//ReleaseShadow(shadowIdx);
	}
}


