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
#include "player.h"
#include "shadow.h"
#include "bullet.h"
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

#define	VALUE_MOVE			(0.5f)							// 移動量
#define	VALUE_DASH			(2.0f)							// 移動量
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)				// 回転量

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる
#define RADIAN				(XM_PI / 180)					// ラジアン変換用

#define PLAYER_HEAD_Y		(2.7f)								// 頭のx座標

#define PLAYER_ARM_X		(1.5f)								// 腕のx座標
#define PLAYER_ARM_Y		(2.5f)								// 腕のy座標

#define PLAYER_HAND_X		(0.0f)								// 腕のx座標
#define PLAYER_HAND_Y		(-1.5f)								// 腕のy座標

#define PLAYER_LEG_X		(1.1f)								// 腕のx座標
#define PLAYER_LEG_Y		(-2.4f)								// 腕のy座標

#define PLAYER_FOOT_X		(0.0f)								// 腕のx座標
#define PLAYER_FOOT_Y		(-1.4f)								// 腕のy座標

#define PLAYER_JUMP_Y		(50.0f)								// ジャンプ力
#define PLAYER_JUMP_CNT_MAX	(28.0f)								// ジャンプ全体フレーム

#define ANIM_FRAME_STOP		(15.0f)							// 待機アニメーションの間隔
#define ANIM_FRAME_MOVE		(30.0f)							// 移動アニメーションの間隔
#define ANIM_FRAME_DASH		(ANIM_FRAME_MOVE * 0.5f)		// ダッシュアニメーションの間隔

#define ANIM_FRAME_JUMP		(15.0f)								// ジャンプアニメーションの間隔

#define BLEND_FRAME_STOP	(60.0f)							// 待機モーションに遷移するまでの時間
#define BLEND_FRAME_MOVE	(90.0f)							// 移動モーションに遷移するまでの時間
#define BLEND_FRAME_DASH	(90.0f)							// ダッシュモーションに遷移するまでの時間
#define BLEND_FRAME_JUMP	(90.0f)							// ジャンプモーションに遷移するまでの時間


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER		g_Player;						// プレイヤー

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// プレイヤーのパーツ用

static float		roty = 0.0f;

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
};

// ジャンプモーションデータ
static INTERPOLATION_DATA jump_tbl_head[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(0.0f, PLAYER_HEAD_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_arm_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(-RADIAN * 140.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_arm_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 30.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_ARM_X, PLAYER_ARM_Y, 0.0f), XMFLOAT3(RADIAN * 50.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
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
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_leg_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(-RADIAN * 90.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_LEG_X, PLAYER_LEG_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },

};

static INTERPOLATION_DATA jump_tbl_foot_l[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(-PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
};

static INTERPOLATION_DATA jump_tbl_foot_r[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 20.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP - 5.0f },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(RADIAN * 90.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },
	{ XMFLOAT3(PLAYER_FOOT_X, PLAYER_FOOT_Y, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),	XMFLOAT3(1.0f, 1.0f, 1.0f), ANIM_FRAME_JUMP },

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
};

// 選択したアニメーションに移行するフレーム
static float g_AnimTransFrame[ANIM_MAX] =
{
	BLEND_FRAME_STOP,
	BLEND_FRAME_MOVE,
	BLEND_FRAME_DASH,
	BLEND_FRAME_JUMP
};

static float g_AnimTransFrameCnt[ANIM_MAX];			// アニメーションに移行するフレームのカウンタ

static int g_AnimNum[PLAYER_PARTS_MAX];		// 現在のアニメーション番号

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player.load = TRUE;
	LoadModel(MODEL_PLAYER, &g_Player.model);

	g_Player.pos = XMFLOAT3(-10.0f, 0.0f - 50.0f, -50.0f);
	g_Player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.spd = 0.0f;			// 移動スピードクリア

	g_Player.use = TRUE;			// TRUE:生きてる
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ
	g_Player.jump = FALSE;
	g_Player.jumpCnt = 0;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号

	// キーを押した時のプレイヤーの向き
	roty = 0.0f;

	g_Player.parent = NULL;			// 本体（親）なのでNULLを入れる


	// 階層アニメーションの初期化
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// 親子関係
		g_Parts[i].parent = &g_Player;		// ← ここに親のアドレスを入れる
		//	g_Parts[腕].parent= &g_Player;		// 腕だったら親は本体（プレイヤー）
		//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

			// 階層アニメーション用のメンバー変数の初期化
		for (int j = 0; j < ANIM_MAX; j++)
		{
			g_Parts[i].time[j] = 0.0f;			// 線形補間用のタイマーをクリア
			g_Parts[i].tblNo[j] = 0;			// 再生する行動データテーブルNoをセット
			g_Parts[i].tblMax[j] = 0;			// 再生する行動データテーブルのレコード数をセット
		}
		// パーツの読み込みはまだしていない
		g_Parts[i].load = FALSE;


		// パーツの初期アニメーションを設定
		g_AnimNum[i] = ANIM_STOP;

	}

	g_Parts[PARTS_HEAD].use = TRUE;
	g_Parts[PARTS_HEAD].parent = &g_Player;	// 親をセット
	g_Parts[PARTS_HEAD].tblNo[ANIM_STOP] = PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HEAD].tblMax[ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HEAD].tblNo[ANIM_MOVE] = PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HEAD].tblMax[ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HEAD].tblNo[ANIM_DASH] = PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HEAD].tblMax[ANIM_DASH] = sizeof(dash_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HEAD].tblNo[ANIM_JUMP] = PARTS_HEAD;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HEAD].tblMax[ANIM_JUMP] = sizeof(jump_tbl_head) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HEAD].load = 1;
	LoadModel(MODEL_PLAYER_HEAD, &g_Parts[PARTS_HEAD].model);

	g_Parts[PARTS_ARM_L].use = TRUE;
	g_Parts[PARTS_ARM_L].parent = &g_Player;	// 親をセット
	g_Parts[PARTS_ARM_L].tblNo[ANIM_STOP] = PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_ARM_L].tblMax[ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_ARM_L].tblNo[ANIM_MOVE] = PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_ARM_L].tblMax[ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_ARM_L].tblNo[ANIM_DASH] = PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_ARM_L].tblMax[ANIM_DASH] = sizeof(dash_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_ARM_L].tblNo[ANIM_JUMP] = PARTS_ARM_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_ARM_L].tblMax[ANIM_JUMP] = sizeof(jump_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_ARM_L].load = 1;
	LoadModel(MODEL_PLAYER_ARM_L, &g_Parts[PARTS_ARM_L].model);

	g_Parts[PARTS_ARM_R].use = TRUE;
	g_Parts[PARTS_ARM_R].parent = &g_Player;	// 親をセット
	g_Parts[PARTS_ARM_R].tblNo[ANIM_STOP] = PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_ARM_R].tblMax[ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_ARM_R].tblNo[ANIM_MOVE] = PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_ARM_R].tblMax[ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_ARM_R].tblNo[ANIM_DASH] = PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_ARM_R].tblMax[ANIM_DASH] = sizeof(dash_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_ARM_R].tblNo[ANIM_JUMP] = PARTS_ARM_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_ARM_R].tblMax[ANIM_JUMP] = sizeof(jump_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_ARM_R].load = 1;
	LoadModel(MODEL_PLAYER_ARM_R, &g_Parts[PARTS_ARM_R].model);

	g_Parts[PARTS_HAND_L].use = TRUE;
	g_Parts[PARTS_HAND_L].parent = &g_Parts[PARTS_ARM_L];	// 親をセット
	g_Parts[PARTS_HAND_L].tblNo[ANIM_STOP] = PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HAND_L].tblMax[ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HAND_L].tblNo[ANIM_MOVE] = PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HAND_L].tblMax[ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HAND_L].tblNo[ANIM_DASH] = PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HAND_L].tblMax[ANIM_DASH] = sizeof(dash_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HAND_L].tblNo[ANIM_JUMP] = PARTS_HAND_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HAND_L].tblMax[ANIM_JUMP] = sizeof(jump_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HAND_L].load = 1;
	LoadModel(MODEL_PLAYER_HAND_L, &g_Parts[PARTS_HAND_L].model);

	g_Parts[PARTS_HAND_R].use = TRUE;
	g_Parts[PARTS_HAND_R].parent = &g_Parts[PARTS_ARM_R];	// 親をセット
	g_Parts[PARTS_HAND_R].tblNo[ANIM_STOP] = PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HAND_R].tblMax[ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HAND_R].tblNo[ANIM_MOVE] = PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HAND_R].tblMax[ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HAND_R].tblNo[ANIM_DASH] = PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HAND_R].tblMax[ANIM_DASH] = sizeof(dash_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HAND_R].tblNo[ANIM_JUMP] = PARTS_HAND_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_HAND_R].tblMax[ANIM_JUMP] = sizeof(jump_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_HAND_R].load = 1;
	LoadModel(MODEL_PLAYER_HAND_R, &g_Parts[PARTS_HAND_R].model);

	g_Parts[PARTS_LEG_L].use = TRUE;
	g_Parts[PARTS_LEG_L].parent = &g_Player;	// 親をセット
	g_Parts[PARTS_LEG_L].tblNo[ANIM_STOP] = PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_LEG_L].tblMax[ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_LEG_L].tblNo[ANIM_MOVE] = PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_LEG_L].tblMax[ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_LEG_L].tblNo[ANIM_DASH] = PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_LEG_L].tblMax[ANIM_DASH] = sizeof(dash_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_LEG_L].tblNo[ANIM_JUMP] = PARTS_LEG_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_LEG_L].tblMax[ANIM_JUMP] = sizeof(jump_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_LEG_L].load = 1;
	LoadModel(MODEL_PLAYER_LEG_L, &g_Parts[PARTS_LEG_L].model);

	g_Parts[PARTS_LEG_R].use = TRUE;
	g_Parts[PARTS_LEG_R].parent = &g_Player;	// 親をセット
	g_Parts[PARTS_LEG_R].tblNo[ANIM_STOP] = PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_LEG_R].tblMax[ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_LEG_R].tblNo[ANIM_MOVE] = PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_LEG_R].tblMax[ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_LEG_R].tblNo[ANIM_DASH] = PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_LEG_R].tblMax[ANIM_DASH] = sizeof(dash_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_LEG_R].tblNo[ANIM_JUMP] = PARTS_LEG_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_LEG_R].tblMax[ANIM_JUMP] = sizeof(jump_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_LEG_R].load = 1;
	LoadModel(MODEL_PLAYER_LEG_R, &g_Parts[PARTS_LEG_R].model);

	g_Parts[PARTS_FOOT_L].use = TRUE;
	g_Parts[PARTS_FOOT_L].parent = &g_Parts[PARTS_LEG_L];	// 親をセット
	g_Parts[PARTS_FOOT_L].tblNo[ANIM_STOP] = PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_FOOT_L].tblMax[ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_FOOT_L].tblNo[ANIM_MOVE] = PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_FOOT_L].tblMax[ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_FOOT_L].tblNo[ANIM_DASH] = PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_FOOT_L].tblMax[ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_FOOT_L].tblNo[ANIM_JUMP] = PARTS_FOOT_L;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_FOOT_L].tblMax[ANIM_JUMP] = sizeof(jump_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_FOOT_L].load = 1;
	LoadModel(MODEL_PLAYER_FOOT_L, &g_Parts[PARTS_FOOT_L].model);

	g_Parts[PARTS_FOOT_R].use = TRUE;
	g_Parts[PARTS_FOOT_R].parent = &g_Parts[PARTS_LEG_R];	// 親をセット
	g_Parts[PARTS_FOOT_R].tblNo[ANIM_STOP] = PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_FOOT_R].tblMax[ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_FOOT_R].tblNo[ANIM_MOVE] = PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_FOOT_R].tblMax[ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_FOOT_R].tblNo[ANIM_DASH] = PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_FOOT_R].tblMax[ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_FOOT_R].tblNo[ANIM_JUMP] = PARTS_FOOT_R;			// 再生するアニメデータの先頭アドレスをセット
	g_Parts[PARTS_FOOT_R].tblMax[ANIM_JUMP] = sizeof(jump_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
	g_Parts[PARTS_FOOT_R].load = 1;
	LoadModel(MODEL_PLAYER_FOOT_R, &g_Parts[PARTS_FOOT_R].model);
	// クォータニオンの初期化
	XMStoreFloat4(&g_Player.Quaternion, XMQuaternionIdentity());

	for (int i = 0; i < ANIM_MAX; i++)
	{
		g_AnimTransFrameCnt[i] = 0.0f;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load == TRUE)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	// パーツの解放処理
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// パーツの解放処理
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}



}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();

	g_Player.spd *= 0.4f;

	// アニメーションを待機モーションにリセット
	int animNum = ANIM_STOP;

	// 移動処理
	if (GetKeyboardPress(DIK_LEFT))
	{
		g_Player.spd = VALUE_MOVE;
		//g_Player.pos.x -= g_Player.spd;
		roty = XM_PI / 2;
		animNum = ANIM_MOVE;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		g_Player.spd = VALUE_MOVE;
		//g_Player.pos.x += g_Player.spd;
		roty = -XM_PI / 2;
		animNum = ANIM_MOVE;
	}
	if (GetKeyboardPress(DIK_UP))
	{
		g_Player.spd = VALUE_MOVE;
		//g_Player.pos.z += g_Player.spd;
		roty = XM_PI;
		animNum = ANIM_MOVE;
	}
	if (GetKeyboardPress(DIK_DOWN))
	{
		g_Player.spd = VALUE_MOVE;
		//g_Player.pos.z -= g_Player.spd;
		roty = 0.0f;
		animNum = ANIM_MOVE;
	}

	if (GetKeyboardPress(DIK_LSHIFT))
	{
		g_Player.spd *= VALUE_DASH;
		animNum = ANIM_DASH;
	}

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.spd = 0.0f;
		roty = 0.0f;
	}
#endif

	if (GetKeyboardPress(DIK_J))
	{
		g_Player.pos.y += g_Player.pos.x = 0.0f;
		g_Player.spd = 0.0f;
		roty = 0.0f;
	}

	{	// 押した方向にプレイヤーを移動させる
		// 押した方向にプレイヤーを向かせている所
		g_Player.rot.y = roty + cam->rot.y;

		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}


	// レイキャストして足元の高さを求める
	XMFLOAT3 HitPosition;		// 交点
	XMFLOAT3 Normal;			// ぶつかったポリゴンの法線ベクトル（向き）
	BOOL ans = RayHitField(g_Player.pos, &HitPosition, &Normal);
	if (ans)
	{
		g_Player.pos.y = HitPosition.y + PLAYER_OFFSET_Y;
	}
	else
	{
		g_Player.pos.y = PLAYER_OFFSET_Y;
		Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	}


	// 弾発射処理
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		SetBullet(g_Player.pos, g_Player.rot);
	}

	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// ジャンプ処理中
	if (g_Player.jump == TRUE)
	{
		float angle = (XM_PI / PLAYER_JUMP_CNT_MAX) * g_Player.jumpCnt;
		float y = PLAYER_JUMP_Y * cosf(XM_PI / 2 + angle);
		//g_Player.pos.y -= y;

		g_Player.jumpCnt++;
		if (g_Player.jumpCnt > PLAYER_JUMP_CNT_MAX)
		{
			g_Player.jump = FALSE;
			g_Player.jumpCnt = 0;
			g_AnimTransFrameCnt[ANIM_JUMP] = 0.0f;
			for (int i = 0; i < PLAYER_PARTS_MAX; i++)
			{
				g_Parts[i].time[ANIM_JUMP] = 0.0f;
			}
		}

	}
	// ジャンプボタン押した？
	else if ((g_Player.jump == FALSE) && (GetKeyboardTrigger(DIK_X)))
	{
		g_Player.jump = TRUE;
		g_Player.jumpCnt = 0;
	}

	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// 使われているなら処理する
		// 線形補間の処理
		if (g_Player.jump == TRUE)
		{
			AnimationBlend(ANIM_JUMP, animNum, i);
		}

		else
		{
			AnimationBlend(animNum, g_AnimNum[i], i);
		}
	}

	// ポイントライトのテスト
	{
		LIGHT* light = GetLightData(1);
		XMFLOAT3 pos = g_Player.pos;
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


	g_Player.UpVector = Normal;
	up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vx = XMVector3Cross(up, XMLoadFloat3(&g_Player.UpVector));

	nvx = XMVector3Length(vx);
	XMStoreFloat(&len, nvx);
	nvx = XMVector3Normalize(vx);
	//nvx = vx / len;
	angle = asinf(len);

	//quat = XMQuaternionIdentity();

//	quat = XMQuaternionRotationAxis(nvx, angle);
	quat = XMQuaternionRotationNormal(nvx, angle);


	quat = XMQuaternionSlerp(XMLoadFloat4(&g_Player.Quaternion), quat, 0.05f);
	XMStoreFloat4(&g_Player.Quaternion, quat);


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, Normal.y);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// クォータニオンを反映
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// 縁取りの設定
	//SetRimLight(1);

	// モデル描画
	DrawModel(&g_Player.model);

	SetRimLight(0);

	// 階層アニメーション
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ↑
			// g_Player.mtxWorldを指している
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// 使われているなら処理する
		if (g_Parts[i].use == FALSE) continue;

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// モデル描画
		DrawModel(&g_Parts[i].model);

	}

	//SetRimLight(0);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}

//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER* GetPlayerParts(void)
{
	return &g_Parts[0];
}

void Animation(int animNum, int i)
{
	// 線形補間の処理
	INTERPOLATION_DATA* tbl;

	switch (animNum)
	{
	case ANIM_STOP:
		tbl = g_StopTblAdr[g_Parts[i].tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	case ANIM_MOVE:
		tbl = g_MoveTblAdr[g_Parts[i].tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	case ANIM_DASH:
		tbl = g_DashTblAdr[g_Parts[i].tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	default:
		tbl = g_JumpTblAdr[g_Parts[i].tblNo[animNum]];	// 待機テーブルのアドレスを取得
		break;
	}

	int nowNo = (int)g_Parts[i].time[animNum];			// 整数分であるテーブル番号を取り出している
	int maxNo = g_Parts[i].tblMax[animNum];				// 登録テーブル数を数えている
	int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている

	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

	float nowTime = g_Parts[i].time[animNum] - nowNo;	// 時間部分である少数を取り出している

	Pos = nowPos + Pos * nowTime;								// 現在の移動量を計算している
	Rot = nowRot + Rot * nowTime;								// 現在の回転量を計算している
	Scl = nowScl + Scl * nowTime;								// 現在の拡大率を計算している

	// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
	XMStoreFloat3(&g_Parts[i].pos, Pos);

	// 計算して求めた回転量を現在の移動テーブルに足している
	XMStoreFloat3(&g_Parts[i].rot, Rot);

	// 計算して求めた拡大率を現在の移動テーブルに足している
	XMStoreFloat3(&g_Parts[i].scl, Scl);

	// frameを使て時間経過処理をする
	g_Parts[i].time[animNum] += 1.0f / tbl[nowNo].frame;	// 時間を進めている
	if ((int)g_Parts[i].time[animNum] >= maxNo)			// 登録テーブル最後まで移動したか？
	{
		g_Parts[i].time[animNum] -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
	}
}

//=============================================================================
// モーションブレンド関数
//=============================================================================
void AnimationBlend(int animNum1, int animNum2, int i)
{
	// アニメーション番号が同じだった場合
	if (animNum1 == animNum2)
	{
		Animation(animNum1, i);
		return;
	}

	// アニメーションが変わったタイミングでカウントをリセットする
	if (g_AnimTransFrameCnt[animNum1] < g_AnimTransFrameCnt[animNum2])
	{
		g_AnimTransFrameCnt[animNum2] = 0.0f;
	}

	int animNum[2] = { animNum1, animNum2 };

	// 使われているなら処理する
	if ((g_Parts[i].use == TRUE) && (g_Parts[i].tblMax[animNum1] > 0))
	{
		INTERPOLATION_DATA* tbl[2];
		XMVECTOR Pos[2];
		XMVECTOR Rot[2];
		XMVECTOR Scl[2];

		for (int j = 0; j < 2; j++)
		{	// 線形補間の処理
			// 1つ目のアニメーションの計算
			switch (animNum[j])
			{
			case ANIM_STOP:
				tbl[j] = g_StopTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
				break;
			case ANIM_MOVE:
				tbl[j] = g_MoveTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
				break;
			case ANIM_DASH:
				tbl[j] = g_DashTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
				break;
			default:
				tbl[j] = g_JumpTblAdr[g_Parts[i].tblNo[animNum[j]]];	// 待機テーブルのアドレスを取得
				break;
			}

			int nowNo = (int)g_Parts[i].time[animNum[j]];			// 整数分であるテーブル番号を取り出している
			int maxNo = g_Parts[i].tblMax[animNum[j]];				// 登録テーブル数を数えている
			int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている

			XMVECTOR nowPos = XMLoadFloat3(&tbl[j][nowNo].pos);	// XMVECTORへ変換
			XMVECTOR nowRot = XMLoadFloat3(&tbl[j][nowNo].rot);	// XMVECTORへ変換
			XMVECTOR nowScl = XMLoadFloat3(&tbl[j][nowNo].scl);	// XMVECTORへ変換

			Pos[j] = XMLoadFloat3(&tbl[j][nextNo].pos) - nowPos;	// XYZ移動量を計算している
			Rot[j] = XMLoadFloat3(&tbl[j][nextNo].rot) - nowRot;	// XYZ回転量を計算している
			Scl[j] = XMLoadFloat3(&tbl[j][nextNo].scl) - nowScl;	// XYZ拡大率を計算している

			float nowTime = g_Parts[i].time[animNum[j]] - nowNo;	// 時間部分である少数を取り出している

			Pos[j] = nowPos + Pos[j] * nowTime;
			Rot[j] = nowRot + Rot[j] * nowTime;
			Scl[j] = nowScl + Scl[j] * nowTime;

			// frameを使て時間経過処理をする
			g_Parts[i].time[animNum[j]] += 1.0f / tbl[j][nowNo].frame;	// 時間を進めている
			if ((int)g_Parts[i].time[animNum[j]] >= maxNo)			// 登録テーブル最後まで移動したか？
			{
				g_Parts[i].time[animNum[j]] -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
			}
		}
		// 重みづけ
		float weight = (1.0f / g_AnimTransFrame[animNum1]) * g_AnimTransFrameCnt[animNum1];
		// モーションブレンドの座標計算
		XMVECTOR blendPos = Pos[0] * weight + Pos[1] * (1.0f - weight);
		XMVECTOR blendRot = Rot[0] * weight + Rot[1] * (1.0f - weight);
		XMVECTOR blendScl = Scl[0] * weight + Scl[1] * (1.0f - weight);

		// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
		XMStoreFloat3(&g_Parts[i].pos, blendPos);

		// 計算して求めた回転量を現在の移動テーブルに足している
		XMStoreFloat3(&g_Parts[i].rot, blendRot);

		// 計算して求めた拡大率を現在の移動テーブルに足している
		XMStoreFloat3(&g_Parts[i].scl, blendScl);

		g_AnimTransFrameCnt[animNum1] += 1.0f;
		if (g_AnimTransFrameCnt[animNum1] >= g_AnimTransFrame[animNum1])
		{
			g_AnimTransFrameCnt[animNum1] = g_AnimTransFrame[animNum1];
			g_AnimNum[i] = animNum1;
			g_Parts[i].time[animNum2] = 0.0f;
		}
	}
}