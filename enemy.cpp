//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
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
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY		"data/MODEL/enemy_body.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_ARM_L	"data/MODEL/enemy_arm_l.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_ARM_R	"data/MODEL/enemy_arm_r.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_HAND_L	"data/MODEL/enemy_hand_l.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_HAND_R	"data/MODEL/enemy_hand_r.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_LEG_L	"data/MODEL/enemy_leg_l.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_LEG_R	"data/MODEL/enemy_leg_r.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_FOOT_L	"data/MODEL/enemy_foot_l.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_FOOT_R	"data/MODEL/enemy_foot_r.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_HEAD	"data/MODEL/enemy_head.obj"			// �ǂݍ��ރ��f����
#define	MODEL_ENEMY_SWORD_R	"data/MODEL/enemy_sword.obj"			// �ǂݍ��ރ��f����
//#define	MODEL_ENEMY_SWORD_B	"data/MODEL/enemy_sword_back.obj"			// �ǂݍ��ރ��f����
//#define	MODEL_ENEMY_SCABBARD	"data/MODEL/enemy_scabbard.obj"			// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(0.2f)						// �ړ���
#define	VALUE_BACK			(1.5f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��
#define	VALUE_DISTANCE		(100.0f)					// ��������
#define	VALUE_FRAME			(VALUE_DISTANCE / VALUE_MOVE)	// ��������

#define	BARRIER_FRAME		(40)						// ���G����
#define	BARRIER_WHITE		(BARRIER_FRAME / 5)			// ���G���Ԃ̓_�ŊԊu

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��

#define RADIAN				(XM_PI / 180.0f)					// ���W�A���ϊ��p

#define ANIM_FRAME_STOP		(15.0f)							// �ҋ@�A�j���[�V�����̊Ԋu
#define ANIM_FRAME_MOVE		(30.0f)							// �ړ��A�j���[�V�����̊Ԋu
#define ANIM_FRAME_DASH		(ANIM_FRAME_MOVE * 0.5f)		// �_�b�V���A�j���[�V�����̊Ԋu

#define ANIM_FRAME_JUMP		(15.0f)							// �W�����v�A�j���[�V�����̊Ԋu
#define ANIM_FRAME_ATTACK	(15.0f)							// �W�����v�A�j���[�V�����̊Ԋu

#define ENEMY_HEAD_Y		(2.7f)							// ����x���W

#define ENEMY_ARM_X			(1.5f)							// �r��x���W
#define ENEMY_ARM_Y			(2.5f)							// �r��y���W

#define ENEMY_HAND_X		(0.0f)							// ���x���W
#define ENEMY_HAND_Y		(-1.5f)							// ���y���W

#define ENEMY_LEG_X			(1.1f)							// �r��x���W
#define ENEMY_LEG_Y			(-2.4f)							// �r��y���W

#define ENEMY_FOOT_X		(0.0f)							// ����x���W
#define ENEMY_FOOT_Y		(-1.8f)							// ����y���W

#define ENEMY_SWORD_R_X		(0.0f)							// ����x���W
#define ENEMY_SWORD_R_Y		(-2.0f)							// ����y���W

#define ENEMY_SWORD_B_X		(0.0f)							// ��(�w��)��x���W
#define ENEMY_SWORD_B_Y		(1.0f)							// ��(�w��)��y���W
#define ENEMY_SWORD_B_Z		(-1.5f)							// ��(�w��)��z���W

#define BLEND_FRAME_STOP	(60.0f)							// �ҋ@���[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_MOVE	(90.0f)							// �ړ����[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_DASH	(90.0f)							// �_�b�V�����[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_ATTACK	(30.0f)							// �U�����[�V�����ɑJ�ڂ���܂ł̎���

#define MOVE_TBL_MAX		(2)								// �ړ��A�j���[�V�����̃e�[�u����
#define MOVE_INTERVAL		(60 * 3)						// ���̊p�x���v�Z����^�C�~���O
#define ROT_INTERVAL		(5)						// ���̊p�x���v�Z����^�C�~���O

#define TEXTURE_MAX			(2)				// �e�N�X�`���̐�
#define TEXTURE_WIDTH		(80.0f)			// �e�N�X�`���̉���
#define TEXTURE_HEIGHT		(80.0f)			// �e�N�X�`���̏c��

#define ATTACK_WIDTH		(5.0f)							// �U���̓����蔻��̕�
#define ATTACK_DEPTH		(6.0f)							// �U���̓����蔻��̉��s��
#define ENEMY_ATK_CNT_MAX	(ANIM_FRAME_ATTACK * 3.0f)			// �A�^�b�N�S�̃t���[��

#define BOSS_SIZE_VALUE			(2.0f)			// �{�X�̃T�C�Y�{��

#define DISSOLVE_VALUE			(2.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[ENEMY_MAX];				// �G�l�~�[

static ENEMY			g_Parts[ENEMY_MAX][ENEMY_PARTS_MAX];				// �G�l�~�[

static ENEMY			g_Boss[BOSS_MAX];				// �{�X

static ENEMY			g_BossParts[BOSS_MAX][ENEMY_PARTS_MAX];				// �{�X�̃p�[�c

static DISSOLVE			g_EnemyDissolve[ENEMY_MAX];				// �G�l�~�[�̃f�B�]���u
static DISSOLVE			g_EnemyPartsDissolve[ENEMY_MAX][ENEMY_PARTS_MAX];				// �G�l�~�[�̃p�[�c�̃f�B�]���u

static BOOL				g_BossFlg;						// �{�X��˓��t���O
static int				g_BossSponeCnt;						// �{�X�o���J�E���^
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

// �����A�j���f�[�^
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

// �_�b�V���A�j���f�[�^
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

// �A�^�b�N���[�V�����f�[�^
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


// �I�������A�j���[�V�����Ɉڍs����t���[��
static float g_AnimTransFrame[ENEMY_ANIM_MAX] =
{
	BLEND_FRAME_STOP,
	BLEND_FRAME_MOVE,
	BLEND_FRAME_ATTACK,
};

static float g_AnimTransFrameCnt[ENEMY_ANIM_MAX];			// �A�j���[�V�����Ɉڍs����t���[���̃J�E���^

static int g_AnimNum[ENEMY_PARTS_MAX];		// ���݂̃A�j���[�V�����ԍ�

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	srand((unsigned int)time(NULL));

	// �{�X��t���O�����Z�b�g
	g_BossFlg = FALSE;
	g_BossSponeCnt = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(-140.0f + i * 15.0f + rand() % 100, ENEMY_OFFSET_Y, -140.0f + i * 15.0f + rand() % 100);
		g_Enemy[i].rot = XMFLOAT3(0.0f, rand() % 360 * RADIAN, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Enemy[i].hp  = 3;				// �̗͏�����
		g_Enemy[i].attack = FALSE;		// �A�^�b�N�t���O�N���A
		g_Enemy[i].atkVal = 1;			// �U���͏�����
		g_Enemy[i].atkCnt = 0;			// �A�^�b�N�J�E���^������
		g_Enemy[i].spd  = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].moveCnt  = 0;		// �ړ��Ԋu�N���A
		g_Enemy[i].moveFlg  = TRUE;	// �ړ��t���O�N���A
		g_Enemy[i].colCnt  = 0;			// ���G���ԃN���A
		g_Enemy[i].dissolveCnt = 0.0f;			// ���G���ԃN���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		//XMFLOAT3 pos = g_Enemy[i].pos;
		//pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		//g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);
		
		g_Enemy[i].use = TRUE;			// TRUE:�����Ă�
		g_Enemy[i].parent = NULL;

		g_EnemyDissolve[i].edgeColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = FALSE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// �e�q�֌W
			g_Parts[i][j].parent = &g_Enemy[i];		// �� �����ɐe�̃A�h���X������

				// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			for (int k = 0; k < ENEMY_ANIM_MAX; k++)
			{
				g_Parts[i][j].time[k] = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
				g_Parts[i][j].tblNo[k] = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
				g_Parts[i][j].tblMax[k] = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
			}
			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_Parts[i][j].load = FALSE;


			// �p�[�c�̏����A�j���[�V������ݒ�
			g_AnimNum[i] = ENEMY_ANIM_STOP;

			g_EnemyPartsDissolve[i][j].edgeColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		g_Parts[i][ENEMY_PARTS_HEAD].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HEAD].parent = &g_Enemy[i];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HEAD].load = 1;
		LoadModel(MODEL_ENEMY_HEAD, &g_Parts[i][ENEMY_PARTS_HEAD].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_HEAD].model, &g_Parts[i][ENEMY_PARTS_HEAD].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_ARM_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_ARM_L].parent = &g_Enemy[i];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_L].load = 1;
		LoadModel(MODEL_ENEMY_ARM_L, &g_Parts[i][ENEMY_PARTS_ARM_L].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_ARM_L].model, &g_Parts[i][ENEMY_PARTS_ARM_L].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_ARM_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_ARM_R].parent = &g_Enemy[i];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_ARM_R].load = 1;
		LoadModel(MODEL_ENEMY_ARM_R, &g_Parts[i][ENEMY_PARTS_ARM_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_ARM_R].model, &g_Parts[i][ENEMY_PARTS_ARM_R].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_HAND_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HAND_L].parent = &g_Parts[i][ENEMY_PARTS_ARM_L];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_L].load = 1;
		LoadModel(MODEL_ENEMY_HAND_L, &g_Parts[i][ENEMY_PARTS_HAND_L].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_HAND_L].model, &g_Parts[i][ENEMY_PARTS_HAND_L].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_HAND_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_HAND_R].parent = &g_Parts[i][ENEMY_PARTS_ARM_R];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_HAND_R].load = 1;
		LoadModel(MODEL_ENEMY_HAND_R, &g_Parts[i][ENEMY_PARTS_HAND_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_HAND_R].model, &g_Parts[i][ENEMY_PARTS_HAND_R].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_LEG_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_LEG_L].parent = &g_Enemy[i];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_L].load = 1;
		LoadModel(MODEL_ENEMY_LEG_L, &g_Parts[i][ENEMY_PARTS_LEG_L].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_LEG_L].model, &g_Parts[i][ENEMY_PARTS_LEG_L].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_LEG_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_LEG_R].parent = &g_Enemy[i];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_LEG_R].load = 1;
		LoadModel(MODEL_ENEMY_LEG_R, &g_Parts[i][ENEMY_PARTS_LEG_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_LEG_R].model, &g_Parts[i][ENEMY_PARTS_LEG_R].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_FOOT_L].use = TRUE;
		g_Parts[i][ENEMY_PARTS_FOOT_L].parent = &g_Parts[i][ENEMY_PARTS_LEG_L];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_L].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_L, &g_Parts[i][ENEMY_PARTS_FOOT_L].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_FOOT_L].model, &g_Parts[i][ENEMY_PARTS_FOOT_L].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_FOOT_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_FOOT_R].parent = &g_Parts[i][ENEMY_PARTS_LEG_R];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_FOOT_R].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_R, &g_Parts[i][ENEMY_PARTS_FOOT_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_FOOT_R].model, &g_Parts[i][ENEMY_PARTS_FOOT_R].diffuse[0]);

		g_Parts[i][ENEMY_PARTS_SWORD_R].use = TRUE;
		g_Parts[i][ENEMY_PARTS_SWORD_R].parent = &g_Parts[i][ENEMY_PARTS_HAND_R];	// �e���Z�b�g
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][ENEMY_PARTS_SWORD_R].load = 1;
		LoadModel(MODEL_ENEMY_SWORD_R, &g_Parts[i][ENEMY_PARTS_SWORD_R].model);
		GetModelDiffuse(&g_Parts[i][ENEMY_PARTS_SWORD_R].model, &g_Parts[i][ENEMY_PARTS_SWORD_R].diffuse[0]);

		//g_Parts[i][ENEMY_PARTS_SWORD_B].use = TRUE;
		//g_Parts[i][ENEMY_PARTS_SWORD_B].parent = &g_Enemy[i];	// �e���Z�b�g
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SWORD_B;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SWORD_B;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_SWORD_B;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[i][ENEMY_PARTS_SWORD_B].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		//g_Parts[i][ENEMY_PARTS_SWORD_B].load = 1;
		//LoadModel(MODEL_ENEMY_SWORD_B, &g_Parts[i][ENEMY_PARTS_SWORD_B].model);

		//g_Parts[i][ENEMY_PARTS_SCABBARD].use = TRUE;
		//g_Parts[i][ENEMY_PARTS_SCABBARD].parent = &g_Enemy[i];	// �e���Z�b�g
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SCABBARD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SCABBARD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblNo[ENEMY_ANIM_DASH] = ENEMY_PARTS_SCABBARD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Parts[i][ENEMY_PARTS_SCABBARD].tblMax[ENEMY_ANIM_DASH] = sizeof(dash_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		//g_Parts[i][ENEMY_PARTS_SCABBARD].load = 1;
		//LoadModel(MODEL_ENEMY_SCABBARD, &g_Parts[i][ENEMY_PARTS_SCABBARD].model);

		// �ŏ�����p�[�c���������Ă���
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[i][j].Animation(ENEMY_ANIM_STOP);
		}
	}

	// �{�X�̏�����
	for (int i = 0; i < BOSS_MAX; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Boss[i].model);
		g_Boss[i].load = TRUE;

		g_Boss[i].pos = XMFLOAT3(0.0f, ENEMY_OFFSET_Y * BOSS_SIZE_VALUE, 0.0f);
		g_Boss[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Boss[i].scl = XMFLOAT3(1.0f * BOSS_SIZE_VALUE, 1.0f * BOSS_SIZE_VALUE, 1.0f * BOSS_SIZE_VALUE);

		g_Boss[i].hp = 60;				// �̗͏�����
		g_Boss[i].attack = FALSE;		// �A�^�b�N�t���O�N���A
		g_Boss[i].atkVal = 2;			// �U���͏�����
		g_Boss[i].atkCnt = 0;			// �A�^�b�N�J�E���^������
		g_Boss[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Boss[i].moveCnt = 0;			// �ړ��Ԋu�N���A
		g_Boss[i].moveFlg = TRUE;		// �ړ��t���O�N���A
		g_Boss[i].colCnt = 0;			// ���G���ԃN���A
		g_Boss[i].size = ENEMY_SIZE * BOSS_SIZE_VALUE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Boss[i].model, &g_Boss[i].diffuse[0]);

		g_Boss[i].use = FALSE;		// TRUE:�����Ă�
		g_Boss[i].parent = NULL;

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_BossParts[i][j].use = FALSE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_BossParts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_BossParts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_BossParts[i][j].scl = XMFLOAT3(1.0f * BOSS_SIZE_VALUE, 1.0f * BOSS_SIZE_VALUE, 1.0f * BOSS_SIZE_VALUE);

			// �e�q�֌W
			g_BossParts[i][j].parent = &g_Boss[i];		// �� �����ɐe�̃A�h���X������

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			for (int k = 0; k < ENEMY_ANIM_MAX; k++)
			{
				g_BossParts[i][j].time[k] = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
				g_BossParts[i][j].tblNo[k] = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
				g_BossParts[i][j].tblMax[k] = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
			}
			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_BossParts[i][j].load = FALSE;


			// �p�[�c�̏����A�j���[�V������ݒ�
			g_AnimNum[i] = ENEMY_ANIM_STOP;

		}

		g_BossParts[i][ENEMY_PARTS_HEAD].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_HEAD].parent = &g_Boss[i];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HEAD].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HEAD].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HEAD].load = 1;
		LoadModel(MODEL_ENEMY_HEAD, &g_BossParts[i][ENEMY_PARTS_HEAD].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_HEAD].model, &g_BossParts[i][ENEMY_PARTS_HEAD].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_ARM_L].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_ARM_L].parent = &g_Boss[i];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_L].load = 1;
		LoadModel(MODEL_ENEMY_ARM_L, &g_BossParts[i][ENEMY_PARTS_ARM_L].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_ARM_L].model, &g_BossParts[i][ENEMY_PARTS_ARM_L].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_ARM_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_ARM_R].parent = &g_Boss[i];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_ARM_R].load = 1;
		LoadModel(MODEL_ENEMY_ARM_R, &g_BossParts[i][ENEMY_PARTS_ARM_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_ARM_R].model, &g_BossParts[i][ENEMY_PARTS_ARM_R].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_HAND_L].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_HAND_L].parent = &g_BossParts[i][ENEMY_PARTS_ARM_L];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_L].load = 1;
		LoadModel(MODEL_ENEMY_HAND_L, &g_BossParts[i][ENEMY_PARTS_HAND_L].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_HAND_L].model, &g_BossParts[i][ENEMY_PARTS_HAND_L].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_HAND_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_HAND_R].parent = &g_BossParts[i][ENEMY_PARTS_ARM_R];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_HAND_R].load = 1;
		LoadModel(MODEL_ENEMY_HAND_R, &g_BossParts[i][ENEMY_PARTS_HAND_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_HAND_R].model, &g_BossParts[i][ENEMY_PARTS_HAND_R].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_LEG_L].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_LEG_L].parent = &g_Boss[i];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_L].load = 1;
		LoadModel(MODEL_ENEMY_LEG_L, &g_BossParts[i][ENEMY_PARTS_LEG_L].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_LEG_L].model, &g_BossParts[i][ENEMY_PARTS_LEG_L].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_LEG_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_LEG_R].parent = &g_Boss[i];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_LEG_R].load = 1;
		LoadModel(MODEL_ENEMY_LEG_R, &g_BossParts[i][ENEMY_PARTS_LEG_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_LEG_R].model, &g_BossParts[i][ENEMY_PARTS_LEG_R].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_FOOT_L].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_FOOT_L].parent = &g_BossParts[i][ENEMY_PARTS_LEG_L];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_L].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_L].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_L, &g_BossParts[i][ENEMY_PARTS_FOOT_L].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_FOOT_L].model, &g_BossParts[i][ENEMY_PARTS_FOOT_L].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_FOOT_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_FOOT_R].parent = &g_BossParts[i][ENEMY_PARTS_LEG_R];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_FOOT_R].load = 1;
		LoadModel(MODEL_ENEMY_FOOT_R, &g_BossParts[i][ENEMY_PARTS_FOOT_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_FOOT_R].model, &g_BossParts[i][ENEMY_PARTS_FOOT_R].diffuse[0]);

		g_BossParts[i][ENEMY_PARTS_SWORD_R].use = FALSE;
		g_BossParts[i][ENEMY_PARTS_SWORD_R].parent = &g_BossParts[i][ENEMY_PARTS_HAND_R];	// �e���Z�b�g
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_STOP] = ENEMY_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_STOP] = sizeof(stop_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_MOVE] = ENEMY_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_MOVE] = sizeof(move_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblNo[ENEMY_ANIM_ATTACK] = ENEMY_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_BossParts[i][ENEMY_PARTS_SWORD_R].tblMax[ENEMY_ANIM_ATTACK] = sizeof(attack_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_BossParts[i][ENEMY_PARTS_SWORD_R].load = 1;
		LoadModel(MODEL_ENEMY_SWORD_R, &g_BossParts[i][ENEMY_PARTS_SWORD_R].model);
		GetModelDiffuse(&g_BossParts[i][ENEMY_PARTS_SWORD_R].model, &g_BossParts[i][ENEMY_PARTS_SWORD_R].diffuse[0]);
	}


	return S_OK;
}

//=============================================================================
// �I������
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
				// �p�[�c�̉������
				UnloadModel(&g_Parts[i][j].model);
				g_Parts[i][j].load = FALSE;
			}
		}
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	PLAYER* player = GetPlayer();
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE && GetFade() == FADE_NONE)		// ���̃G�l�~�[���g���Ă���H
		{
			// �A�j���[�V������������[�V�����Ƀ��Z�b�g
			int animNum = ENEMY_ANIM_STOP;

			// ���G���蒆�͐F��_�ł�����
			ColCheck(player->pos, g_Enemy[i], g_Parts[i]);

			// �v���C���[�����m�͈͓��ɂ��邩�`�F�b�N
			float x = (player->pos.x - g_Enemy[i].pos.x);
			float z = (player->pos.z - g_Enemy[i].pos.z);
			float rad = atan2f(z, x);

			XMFLOAT3 pos = g_Enemy[i].pos;
			//pos.x -= (float)sinf(g_Enemy[i].rot.y) * LOOK_CIRCLE;
			//pos.z -= (float)cosf(g_Enemy[i].rot.y) * LOOK_CIRCLE;

			//if (CollisionBC(player->pos, pos, player->size, LOOK_CIRCLE))

			// �v���C���[�����m�͈͓��ɂ��邩
			XMFLOAT3 dir = XMFLOAT3((float)cosf(-(g_Enemy[i].rot.y + RADIAN * 90.0f)), 0.0f, (float)sinf(-(g_Enemy[i].rot.y + LOOK_CIRCLE_RAD)));
			if (CollisionSector(player->pos, g_Enemy[i].pos, dir, LOOK_CIRCLE, LOOK_CIRCLE_RAD))
			{
				g_Enemy[i].look = TRUE;
				g_Enemy[i].rot.y = -(RADIAN * 90.0f + rad);
			}

			else
			{
				// ���G���Ԓ��͌��m��Ԃ̂܂�
				if (g_Enemy[i].colCnt == 0) g_Enemy[i].look = FALSE;
			}

			// �U�����[�V�������͈ړ����~�߂�
			if (g_Enemy[i].attack == TRUE)
			{
				// �A�^�b�N�A�j���[�V�������Z�b�g
				animNum = ENEMY_ANIM_ATTACK;
				AttackCheck(g_Enemy[i], g_Parts[i]);
			}

			// �U�����łȂ��Ƃ�
			else
			{
				// �v���C���[�����m���Ă���ꍇ
				if (g_Enemy[i].look == TRUE)
				{
					// �v���C���[���z�[�~���O���铮��
					XMFLOAT3 oldPos = g_Enemy[i].pos;
					g_Enemy[i].pos.x -= sinf(g_Enemy[i].rot.y) * VALUE_MOVE;
					g_Enemy[i].pos.z -= cosf(g_Enemy[i].rot.y) * VALUE_MOVE;

					// �I�u�W�F�N�g���m���d�Ȃ�Ȃ��悤�ɂ���

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

					// �U���͈͓��`�F�b�N
					if ((g_Enemy[i].attack == FALSE) && (CollisionBC(pos, player->pos, ATTACK_DEPTH - 1.0f, player->size)) && player->colCnt == 0)
					{
						g_Enemy[i].attack = TRUE;
						g_Enemy[i].atkCnt = 0;
						animNum = ENEMY_ANIM_ATTACK;
						PlaySound(SOUND_LABEL_SE_shot);
					}
				
				}

				// �v���C���[�����m���Ă��Ȃ��ꍇ
				else
				{
					// �ړ�����
					g_Enemy[i].moveCnt = (g_Enemy[i].moveCnt + 1) % MOVE_INTERVAL;
					if (g_Enemy[i].moveFlg == TRUE)
					{
						g_Enemy[i].pos.x -= sinf(g_Enemy[i].rot.y) * VALUE_MOVE;
						g_Enemy[i].pos.z -= cosf(g_Enemy[i].rot.y) * VALUE_MOVE;
						// �ǂ̍��W�𒴂��Ȃ��悤�ɒ���
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

					// �����_���ł��̏�ɂƂǂ܂�
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

			// �e���v���C���[�̈ʒu�ɍ��킹��
			pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	
		// �G�l�~�[�̃f�B�]���u����
		else if (g_Enemy[i].use == FALSE && g_Enemy[i].dissolveCnt <= RADIAN * 90.0f)
		{
			g_Enemy[i].dissolveCnt += RADIAN * 1.0f;
			//g_Enemy[i].pos.y -= cosf(RADIAN * 5.0f) * ENEMY_OFFSET_Y * 0.03f;
			// �X���ɍ��킹�ĐF���������Ă���
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

	// �{�X�̏���
	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use == TRUE && GetFade() == FADE_NONE)		// ���̃{�X���g���Ă���H
		{
			int animNum;
			ColCheck(player->pos, g_Boss[i], g_BossParts[i]);
			// �U�����[�V�������͈ړ����~�߂�
			if (g_Boss[i].attack == TRUE)
			{
				// �A�^�b�N�A�j���[�V�������Z�b�g
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
				// �I�u�W�F�N�g���m���d�Ȃ�Ȃ��悤�ɂ���
				g_Boss[i].pos.x += cosf(rad) * VALUE_MOVE * BOSS_SIZE_VALUE;
				g_Boss[i].pos.z += sinf(rad) * VALUE_MOVE * BOSS_SIZE_VALUE;

				if (CollisionBC(g_Boss[i].pos, player->pos, g_Boss[i].size, player->size))
				{
					g_Boss[i].pos = oldPos;
				}

				animNum = ENEMY_ANIM_MOVE;

				// �e�𑫌��ɍ��킹��
				{
					XMFLOAT3 pos = g_Boss[i].pos;
					pos.y -= (ENEMY_OFFSET_Y - 0.1f);
					SetPositionShadow(g_Boss[i].shadowIdx, pos);
				}

				XMFLOAT3 pos = g_Boss[i].pos;

				pos.x -= (float)sinf(g_Boss[i].rot.y) * ATTACK_DEPTH * 0.5f;
				pos.y /= BOSS_SIZE_VALUE;
				pos.z -= (float)cosf(g_Boss[i].rot.y) * ATTACK_DEPTH * 0.5f;

				// �U���͈͓��`�F�b�N
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
		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f));
		}
	}

	if (GetKeyboardTrigger(DIK_L))
	{
		// ���f���̐F�����ɖ߂��Ă���
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, g_Enemy[0].diffuse[j]);
		}
	}
#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���f���̐F�ޔ�
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == FALSE && g_Enemy[i].dissolveCnt >= RADIAN * 90.0f) continue;
		if (g_Enemy[i].use == FALSE && GetViewPortType() == TYPE_SHADOWMAP) continue;
		//XMFLOAT4 enemyDiffuse[MODEL_MAX_MATERIAL];
		//XMFLOAT4 partsDiffuse[ENEMY_PARTS_MAX][MODEL_MAX_MATERIAL];

		//GetModelDiffuse(&g_Enemy[i].model, &enemyDiffuse[0]);
		// ���R���s��̉e�`��
		//{
		//	for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
		//	{
		//		SetModelDiffuse(&g_Enemy[i].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
		//	}

		//	// ���[���h�}�g���b�N�X�̏�����
		//	mtxWorld = XMMatrixIdentity();

		//	// �X�P�[���𔽉f
		//	mtxScl = XMMatrixScaling(g_Enemy[i].scl.x * 1.2f, 0.1f, g_Enemy[i].scl.z * 1.2f);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		//	// ��]�𔽉f
		//	mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//	// �ړ��𔽉f
		//	mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y - ENEMY_OFFSET_Y, g_Enemy[i].pos.z);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		//	// ���[���h�}�g���b�N�X�̐ݒ�
		//	SetWorldMatrix(&mtxWorld);

		//	XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);


		//	// ���f���`��
		//	DrawModel(&g_Enemy[i].model);
		//	for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		//	{
		//		GetModelDiffuse(&g_Parts[i][j].model, &partsDiffuse[j][0]);
		//		for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
		//		{
		//			SetModelDiffuse(&g_Parts[i][j].model, k, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
		//		}

		//		// ���[���h�}�g���b�N�X�̏�����
		//		mtxWorld = XMMatrixIdentity();

		//		// �X�P�[���𔽉f
		//		mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x * 1.2f, 0.1f, g_Parts[i][j].scl.z * 1.2f);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		//		// ��]�𔽉f
		//		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, 0.0f, g_Parts[i][j].rot.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//		// �ړ��𔽉f
		//		mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		//		if (g_Parts[i][j].parent != NULL)	// �q����������e�ƌ�������
		//		{
		//			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
		//			// ��
		//			// g_Player.mtxWorld���w���Ă���
		//		}

		//		XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

		//		// �g���Ă���Ȃ珈������
		//		if (g_Parts[i][j].use == FALSE) continue;

		//		// ���[���h�}�g���b�N�X�̐ݒ�
		//		SetWorldMatrix(&mtxWorld);

		//		if (g_Enemy[i].use == FALSE)
		//		{
		//			SetDissolve(&g_EnemyDissolve[i]);
		//			SetDissolveBuffer(TRUE, sinf(g_Enemy[i].dissolveCnt));
		//		}

		//		// ���f���`��
		//		DrawModel(&g_Parts[i][j].model);
		//	}

		//}

		// ���f���`��
		{
			//for (int j = 0; j < g_Enemy[i].model.SubsetNum; j++)
			//{
			//	SetModelDiffuse(&g_Enemy[i].model, j, enemyDiffuse[j]);
			//}

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

			if (g_Enemy[i].use == FALSE)
			{
				SetDissolve(&g_EnemyDissolve[i]);
				SetDissolveBuffer(TRUE, sinf(g_Enemy[i].dissolveCnt));
			}

			// ���f���`��
			DrawModel(&g_Enemy[i].model);
			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				//for (int k = 0; k < g_Parts[i][j].model.SubsetNum; k++)
				//{
				//	SetModelDiffuse(&g_Parts[i][j].model, k, partsDiffuse[j][k]);
				//}
				// ���[���h�}�g���b�N�X�̏�����
				mtxWorld = XMMatrixIdentity();

				// �X�P�[���𔽉f
				mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// ��]�𔽉f
				mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				if (g_Parts[i][j].parent != NULL)	// �q����������e�ƌ�������
				{
					mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
					// ��
					// g_Player.mtxWorld���w���Ă���
				}

				XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

				// �g���Ă���Ȃ珈������
				if (g_Parts[i][j].use == FALSE) continue;

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&mtxWorld);

				// ���f���`��
				DrawModel(&g_Parts[i][j].model);
			}

		}
		SetDissolveBuffer(FALSE, 0.0f);
	}

	// �{�X�`��
	for (int i = 0; i < BOSS_MAX; i++)
	{
		if (g_Boss[i].use == FALSE) continue;
		//XMFLOAT4 bossDiffuse[MODEL_MAX_MATERIAL];
		//XMFLOAT4 bossPartsDiffuse[ENEMY_PARTS_MAX][MODEL_MAX_MATERIAL];

		// ���R���s��̉e�`��
		//{
		//	GetModelDiffuse(&g_Boss[i].model, &bossDiffuse[0]);
		//	for (int j = 0; j < g_Boss[i].model.SubsetNum; j++)
		//	{
		//		SetModelDiffuse(&g_Boss[i].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
		//	}

		//	// ���[���h�}�g���b�N�X�̏�����
		//	mtxWorld = XMMatrixIdentity();

		//	// �X�P�[���𔽉f
		//	mtxScl = XMMatrixScaling(g_Boss[i].scl.x * 1.2f, g_Boss[i].scl.y * 0.1f, g_Boss[i].scl.z * 1.2f);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		//	// ��]�𔽉f
		//	mtxRot = XMMatrixRotationRollPitchYaw(g_Boss[i].rot.x, g_Boss[i].rot.y + XM_PI, g_Boss[i].rot.z);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//	// �ړ��𔽉f
		//	mtxTranslate = XMMatrixTranslation(g_Boss[i].pos.x, g_Boss[i].pos.y - ENEMY_OFFSET_Y * BOSS_SIZE_VALUE, g_Boss[i].pos.z);
		//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		//	// ���[���h�}�g���b�N�X�̐ݒ�
		//	SetWorldMatrix(&mtxWorld);

		//	XMStoreFloat4x4(&g_Boss[i].mtxWorld, mtxWorld);

		//	// ���f���`��
		//	DrawModel(&g_Boss[i].model);
		//	for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		//	{
		//		GetModelDiffuse(&g_BossParts[i][j].model, &bossPartsDiffuse[j][0]);

		//		for (int k = 0; k < g_BossParts[i][j].model.SubsetNum; k++)
		//		{
		//			SetModelDiffuse(&g_BossParts[i][j].model, k, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
		//		}

		//		// ���[���h�}�g���b�N�X�̏�����
		//		mtxWorld = XMMatrixIdentity();

		//		// �X�P�[���𔽉f
		//		mtxScl = XMMatrixScaling(g_BossParts[i][j].scl.x, g_BossParts[i][j].scl.y, g_BossParts[i][j].scl.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		//		// ��]�𔽉f
		//		mtxRot = XMMatrixRotationRollPitchYaw(g_BossParts[i][j].rot.x, 0.0f, g_BossParts[i][j].rot.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		//		// �ړ��𔽉f
		//		mtxTranslate = XMMatrixTranslation(g_BossParts[i][j].pos.x, g_BossParts[i][j].pos.y, g_BossParts[i][j].pos.z);
		//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		//		if (g_BossParts[i][j].parent != NULL)	// �q����������e�ƌ�������
		//		{
		//			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_BossParts[i][j].parent->mtxWorld));
		//			// ��
		//			// g_Player.mtxWorld���w���Ă���
		//		}

		//		XMStoreFloat4x4(&g_BossParts[i][j].mtxWorld, mtxWorld);

		//		// �g���Ă���Ȃ珈������
		//		if (g_BossParts[i][j].use == FALSE) continue;

		//		// ���[���h�}�g���b�N�X�̐ݒ�
		//		SetWorldMatrix(&mtxWorld);

		//		// ���f���`��
		//		DrawModel(&g_BossParts[i][j].model);

		//	}
		//}

		// ���f���`��
		SetRimLight(1);
		{
			//for (int j = 0; j < g_Boss[i].model.SubsetNum; j++)
			//{
			//	SetModelDiffuse(&g_Boss[i].model, j, bossDiffuse[j]);
			//}

			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Boss[i].scl.x, g_Boss[i].scl.y, g_Boss[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Boss[i].rot.x, g_Boss[i].rot.y + XM_PI, g_Boss[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Boss[i].pos.x, g_Boss[i].pos.y, g_Boss[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Boss[i].mtxWorld, mtxWorld);


			// ���f���`��
			DrawModel(&g_Boss[i].model);
			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				//for (int k = 0; k < g_BossParts[i][j].model.SubsetNum; k++)
				//{
				//	SetModelDiffuse(&g_BossParts[i][j].model, k, bossPartsDiffuse[j][k]);
				//}
				// ���[���h�}�g���b�N�X�̏�����
				mtxWorld = XMMatrixIdentity();

				// �X�P�[���𔽉f
				mtxScl = XMMatrixScaling(g_BossParts[i][j].scl.x, g_BossParts[i][j].scl.y, g_BossParts[i][j].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// ��]�𔽉f
				mtxRot = XMMatrixRotationRollPitchYaw(g_BossParts[i][j].rot.x, g_BossParts[i][j].rot.y, g_BossParts[i][j].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(g_BossParts[i][j].pos.x, g_BossParts[i][j].pos.y, g_BossParts[i][j].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				if (g_BossParts[i][j].parent != NULL)	// �q����������e�ƌ�������
				{
					mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_BossParts[i][j].parent->mtxWorld));
					// ��
					// g_Player.mtxWorld���w���Ă���
				}

				XMStoreFloat4x4(&g_BossParts[i][j].mtxWorld, mtxWorld);

				// �g���Ă���Ȃ珈������
				if (g_BossParts[i][j].use == FALSE) continue;

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&mtxWorld);

				// ���f���`��
				DrawModel(&g_BossParts[i][j].model);

			}
		}
	}
	SetRimLight(0);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetEnemy()
{
	return &g_Enemy[0];
}


//=============================================================================
// �{�X�̎擾
//=============================================================================
ENEMY *GetBoss()
{
	return &g_Boss[0];
}

//=============================================================================
// �A�j���[�V�����֐�
//=============================================================================
void ENEMY::Animation(int animNum)
{
	INTERPOLATION_DATA* tbl;

	switch (animNum)
	{
	case ENEMY_ANIM_STOP:
		tbl = g_StopTblAdr[tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	case ENEMY_ANIM_MOVE:
		tbl = g_MoveTblAdr[tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	default:
		tbl = g_AttackTblAdr[tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	}

	int nowNo = (int)time[animNum];			// �������ł���e�[�u���ԍ������o���Ă���
	int maxNo = tblMax[animNum];				// �o�^�e�[�u�����𐔂��Ă���
	int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���

	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

	float nowTime = time[animNum] - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

	Pos = nowPos + Pos * nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
	Rot = nowRot + Rot * nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
	Scl = nowScl + Scl * nowTime;								// ���݂̊g�嗦���v�Z���Ă���

	// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
	XMStoreFloat3(&pos, Pos);

	// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
	XMStoreFloat3(&rot, Rot);

	// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
	XMStoreFloat3(&scl, Scl);

	// frame���g�Ď��Ԍo�ߏ���������
	time[animNum] += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
	if ((int)time[animNum] >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
	{
		time[animNum] -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
	}

}

//=============================================================================
// ���[�V�����u�����h�֐�
//=============================================================================
void ENEMY::Animation(int animNum1, int animNum2)
{
	// �A�j���[�V�����ԍ��������������ꍇ
	if (animNum1 == animNum2)
	{
		Animation(animNum1);
		return;
	}

	// �A�j���[�V�������ς�����^�C�~���O�ŃJ�E���g�����Z�b�g����
	if (g_AnimTransFrameCnt[animNum1] < g_AnimTransFrameCnt[animNum2])
	{
		g_AnimTransFrameCnt[animNum2] = 0.0f;
	}

	int AnimNum[2] = { animNum1, animNum2 };

	// �g���Ă���Ȃ珈������
	if ((tblMax[animNum1] > 0))
	{
		INTERPOLATION_DATA* tbl[2];
		XMVECTOR Pos[2];
		XMVECTOR Rot[2];
		XMVECTOR Scl[2];

		for (int i = 0; i < 2; i++)
		{	// ���`��Ԃ̏���
			// 1�ڂ̃A�j���[�V�����̌v�Z
			switch (AnimNum[i])
			{
			case ENEMY_ANIM_STOP:
				tbl[i] = g_StopTblAdr[tblNo[AnimNum[i]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			case ENEMY_ANIM_MOVE:
				tbl[i] = g_MoveTblAdr[tblNo[AnimNum[i]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			default:
				tbl[i] = g_AttackTblAdr[tblNo[AnimNum[i]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			}

			int nowNo = (int)time[AnimNum[i]];			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = tblMax[AnimNum[i]];				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���

			XMVECTOR nowPos = XMLoadFloat3(&tbl[i][nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[i][nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[i][nowNo].scl);	// XMVECTOR�֕ϊ�

			Pos[i] = XMLoadFloat3(&tbl[i][nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			Rot[i] = XMLoadFloat3(&tbl[i][nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			Scl[i] = XMLoadFloat3(&tbl[i][nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = time[AnimNum[i]] - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos[i] = nowPos + Pos[i] * nowTime;
			Rot[i] = nowRot + Rot[i] * nowTime;
			Scl[i] = nowScl + Scl[i] * nowTime;

			// frame���g�Ď��Ԍo�ߏ���������
			time[AnimNum[i]] += 1.0f / tbl[i][nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)time[AnimNum[i]] >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				time[AnimNum[i]] -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
			}
		}
		// �d�݂Â�
		float weight = (1.0f / g_AnimTransFrame[animNum1]) * g_AnimTransFrameCnt[animNum1];
		// ���[�V�����u�����h�̍��W�v�Z
		XMVECTOR blendPos = Pos[0] * weight + Pos[1] * (1.0f - weight);
		XMVECTOR blendRot = Rot[0] * weight + Rot[1] * (1.0f - weight);
		XMVECTOR blendScl = Scl[0] * weight + Scl[1] * (1.0f - weight);

		// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
		XMStoreFloat3(&pos, blendPos);

		// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
		XMStoreFloat3(&rot, blendRot);

		// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
		XMStoreFloat3(&scl, blendScl);

		g_AnimTransFrameCnt[animNum1] += 1.0f;
		if (g_AnimTransFrameCnt[animNum1] >= g_AnimTransFrame[animNum1])
		{
			g_AnimTransFrameCnt[animNum1] = g_AnimTransFrame[animNum1];
			animNum = animNum1;

			// �A�j���[�V�����̈ڍs�����������^�C�~���O�Ń��Z�b�g
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

	// �̗͂��Ȃ��Ȃ�����p�[�e�B�N���Ȃǂ��Z�b�g
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
// �{�X���X�|�[��
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

// ���G����`�F�b�N
void ColCheck(XMFLOAT3 pos, ENEMY &enemy, ENEMY parts[ENEMY_PARTS_MAX])
{
	if (enemy.colCnt > 0)
	{
		// ���m��Ԃɂ���
		enemy.look = TRUE;

		// �v���C���[�̕�������������
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

		// ���G���肪�I���^�C�~���O�ŐF�����Ƃɖ߂�
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
// �A�^�b�N�֐�
//=============================================================================
void AttackCheck(ENEMY &enemy, ENEMY parts[ENEMY_PARTS_MAX])
{
	PLAYER* player = GetPlayer();

	enemy.atkCnt++;
	XMFLOAT3 pos = enemy.pos;

	pos.x -= (float)sinf(enemy.rot.y) * ATTACK_DEPTH * 0.5f;
	pos.z -= (float)cosf(enemy.rot.y) * ATTACK_DEPTH * 0.5f;

	// �����U���Ă���Ƃ��̂ݔ���
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
	// �{�X�o�����ɃG�l�~�[�͂��ׂď���
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