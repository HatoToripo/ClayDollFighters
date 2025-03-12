//=============================================================================
//
// ���f������ [player.cpp]
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
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player_body.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_ARM_L	"data/MODEL/player_arm_l.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_ARM_R	"data/MODEL/player_arm_r.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_HAND_L	"data/MODEL/player_hand_l.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_HAND_R	"data/MODEL/player_hand_r.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LEG_L	"data/MODEL/player_leg_l.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_LEG_R	"data/MODEL/player_leg_r.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_FOOT_L	"data/MODEL/player_foot_l.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_FOOT_R	"data/MODEL/player_foot_r.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_HEAD	"data/MODEL/player_head.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_SWORD_R	"data/MODEL/player_sword.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_SWORD_B	"data/MODEL/player_sword_back.obj"			// �ǂݍ��ރ��f����
#define	MODEL_PLAYER_SCABBARD	"data/MODEL/player_scabbard.obj"			// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(0.5f)							// �ړ���
#define	VALUE_DASH			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)				// ��]��

#define	BARRIER_FRAME		(40)							// ���G����
#define	BARRIER_WHITE		(BARRIER_FRAME / 5)				// ���G���Ԃ̓_�ŊԊu

#define PLAYER_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define RADIAN				(XM_PI / 180)					// ���W�A���ϊ��p

#define PLAYER_HEAD_Y		(2.7f)								// ����x���W

#define PLAYER_ARM_X		(1.5f)								// �r��x���W
#define PLAYER_ARM_Y		(2.5f)								// �r��y���W

#define PLAYER_HAND_X		(0.0f)								// ���x���W
#define PLAYER_HAND_Y		(-1.5f)								// ���y���W

#define PLAYER_LEG_X		(1.1f)								// �r��x���W
#define PLAYER_LEG_Y		(-2.4f)								// �r��y���W

#define PLAYER_FOOT_X		(0.0f)								// ����x���W
#define PLAYER_FOOT_Y		(-1.8f)								// ����y���W

#define PLAYER_SWORD_R_X	(0.0f)								// ����x���W
#define PLAYER_SWORD_R_Y	(-2.0f)								// ����y���W

#define PLAYER_SWORD_B_X	(0.0f)								// ��(�w��)��x���W
#define PLAYER_SWORD_B_Y	(1.0f)								// ��(�w��)��y���W
#define PLAYER_SWORD_B_Z	(-1.5f)								// ��(�w��)��z���W

#define ANIM_FRAME_STOP		(15.0f)							// �ҋ@�A�j���[�V�����̊Ԋu
#define ANIM_FRAME_MOVE		(30.0f)							// �ړ��A�j���[�V�����̊Ԋu
#define ANIM_FRAME_DASH		(ANIM_FRAME_MOVE * 0.5f)		// �_�b�V���A�j���[�V�����̊Ԋu

#define ANIM_FRAME_JUMP		(15.0f)								// �W�����v�A�j���[�V�����̊Ԋu
#define ANIM_FRAME_ATTACK	(10.0f)								// �W�����v�A�j���[�V�����̊Ԋu

#define PLAYER_JUMP_Y		(50.0f)								// �W�����v��
#define PLAYER_JUMP_CNT_MAX	(ANIM_FRAME_JUMP * 3.0f - 5.0f)			// �W�����v�S�̃t���[��
#define PLAYER_ATK_CNT_MAX	(ANIM_FRAME_ATTACK * 3.0f)			// �A�^�b�N�S�̃t���[��

#define BLEND_FRAME_STOP	(60.0f)							// �ҋ@���[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_MOVE	(120.0f)							// �ړ����[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_DASH	(120.0f)							// �_�b�V�����[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_JUMP	(120.0f)							// �W�����v���[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_ATTACK	(120.0f)							// �U�����[�V�����ɑJ�ڂ���܂ł̎���

#define ATTACK_WIDTH		(5.0f)							// �U���̓����蔻��̕�
#define ATTACK_DEPTH		(10.0f)							// �U���̓����蔻��̉��s��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player[PLAYER_MAX];						// �v���C���[

static PLAYER		g_Parts[PLAYER_MAX][PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static float		roty = 0.0f;
static float		g_RotDead;						// �v���C���[���S���ɂ��񂾂񔖂�����

static LIGHT		g_Light;

// �v���C���[�̊K�w�A�j���[�V�����f�[�^

// �ҋ@���[�V�����f�[�^
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

// �����A�j���f�[�^
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

// �_�b�V���A�j���f�[�^
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

// �W�����v���[�V�����f�[�^
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

// �A�^�b�N���[�V�����f�[�^
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

// �I�������A�j���[�V�����Ɉڍs����t���[��
static float g_AnimTransFrame[PLAYER_ANIM_MAX] =
{
	BLEND_FRAME_STOP,
	BLEND_FRAME_MOVE,
	BLEND_FRAME_DASH,
	BLEND_FRAME_JUMP,
	BLEND_FRAME_ATTACK,
};

static float g_AnimTransFrameCnt[PLAYER_ANIM_MAX];			// �A�j���[�V�����Ɉڍs����t���[���̃J�E���^

//=============================================================================
// ����������
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

		g_Player[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Player[i].hp = 20;				// �̗͂̏�����
		g_Player[i].gauge = 0;			// �̗͂̏�����
		g_Player[i].colCnt = 0;			// �̗͂̏�����

		g_Player[i].attack = FALSE;		// �A�^�b�N�t���O�N���A
		g_Player[i].atkVal = 1;			// �U���͏�����
		g_Player[i].atkCnt = 0;			// �ړ��X�s�[�h�N���A
		g_Player[i].dissolveCnt = 0.0f;	// �f�B�]���u�̃J�E���^�N���A

		g_Player[i].use = TRUE;			// TRUE:�����Ă�
		g_Player[i].size = PLAYER_SIZE;	// �����蔻��̑傫��
		g_Player[i].jump = FALSE;			// �W�����v�t���O�N���A
		g_Player[i].jumpCnt = 0;			// �W�����v�J�E���^�N���A

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Player[i].model, &g_Player[i].diffuse[0]);

		// �����Ńv���C���[�p�̉e���쐬���Ă���
		XMFLOAT3 pos = g_Player[i].pos;
		pos.y -= (PLAYER_OFFSET_Y - 0.1f);
		//g_Player[i].shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
		//          ��
		//        ���̃����o�[�ϐ������������e��Index�ԍ�

		// �L�[�����������̃v���C���[�̌���
		roty = 0.0f;

		// �v���C���[���S�����X�ɔ������� 
		g_RotDead = 0.0f;

		g_Player[i].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������


		// �K�w�A�j���[�V�����̏�����
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = FALSE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// �e�q�֌W
			g_Parts[i][j].parent = &g_Player[i];		// �� �����ɐe�̃A�h���X������
			//	g_Parts[�r].parent= &g_Player[i];		// �r��������e�͖{�́i�v���C���[�j
			//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

				// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			for (int j = 0; j < PLAYER_ANIM_MAX; j++)
			{
				g_Parts[i][j].time[j] = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
				g_Parts[i][j].tblNo[j] = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
				g_Parts[i][j].tblMax[j] = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
			}
			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_Parts[i][j].load = FALSE;


			// �p�[�c�̏����A�j���[�V������ݒ�
			g_Parts[i][j].animNum = PLAYER_ANIM_STOP;
		}

		g_Parts[i][PLAYER_PARTS_HEAD].use = TRUE;
		g_Parts[i][PLAYER_PARTS_HEAD].parent = &g_Player[i];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HEAD].load = 1;
		LoadModel(MODEL_PLAYER_HEAD, &g_Parts[i][PLAYER_PARTS_HEAD].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_HEAD].model, &g_Parts[i][PLAYER_PARTS_HEAD].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_ARM_L].use = TRUE;
		g_Parts[i][PLAYER_PARTS_ARM_L].parent = &g_Player[i];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_L].load = 1;
		LoadModel(MODEL_PLAYER_ARM_L, &g_Parts[i][PLAYER_PARTS_ARM_L].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_ARM_L].model, &g_Parts[i][PLAYER_PARTS_ARM_L].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_ARM_R].use = TRUE;
		g_Parts[i][PLAYER_PARTS_ARM_R].parent = &g_Player[i];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_ARM_R].load = 1;
		LoadModel(MODEL_PLAYER_ARM_R, &g_Parts[i][PLAYER_PARTS_ARM_R].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_ARM_R].model, &g_Parts[i][PLAYER_PARTS_ARM_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_HAND_L].use = TRUE;
		g_Parts[i][PLAYER_PARTS_HAND_L].parent = &g_Parts[i][PLAYER_PARTS_ARM_L];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_L].load = 1;
		LoadModel(MODEL_PLAYER_HAND_L, &g_Parts[i][PLAYER_PARTS_HAND_L].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_HAND_L].model, &g_Parts[i][PLAYER_PARTS_HAND_L].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_HAND_R].use = TRUE;
		g_Parts[i][PLAYER_PARTS_HAND_R].parent = &g_Parts[i][PLAYER_PARTS_ARM_R];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_HAND_R].load = 1;
		LoadModel(MODEL_PLAYER_HAND_R, &g_Parts[i][PLAYER_PARTS_HAND_R].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_HAND_R].model, &g_Parts[i][PLAYER_PARTS_HAND_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_LEG_L].use = TRUE;
		g_Parts[i][PLAYER_PARTS_LEG_L].parent = &g_Player[i];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_L].load = 1;
		LoadModel(MODEL_PLAYER_LEG_L, &g_Parts[i][PLAYER_PARTS_LEG_L].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_LEG_L].model, &g_Parts[i][PLAYER_PARTS_LEG_L].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_LEG_R].use = TRUE;
		g_Parts[i][PLAYER_PARTS_LEG_R].parent = &g_Player[i];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_LEG_R].load = 1;
		LoadModel(MODEL_PLAYER_LEG_R, &g_Parts[i][PLAYER_PARTS_LEG_R].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_LEG_R].model, &g_Parts[i][PLAYER_PARTS_LEG_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_FOOT_L].use = TRUE;
		g_Parts[i][PLAYER_PARTS_FOOT_L].parent = &g_Parts[i][PLAYER_PARTS_LEG_L];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_L].load = 1;
		LoadModel(MODEL_PLAYER_FOOT_L, &g_Parts[i][PLAYER_PARTS_FOOT_L].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_FOOT_L].model, &g_Parts[i][PLAYER_PARTS_FOOT_L].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_FOOT_R].use = TRUE;
		g_Parts[i][PLAYER_PARTS_FOOT_R].parent = &g_Parts[i][PLAYER_PARTS_LEG_R];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_FOOT_R].load = 1;
		LoadModel(MODEL_PLAYER_FOOT_R, &g_Parts[i][PLAYER_PARTS_FOOT_R].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_FOOT_R].model, &g_Parts[i][PLAYER_PARTS_FOOT_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_SWORD_R].use = FALSE;
		g_Parts[i][PLAYER_PARTS_SWORD_R].parent = &g_Parts[i][PLAYER_PARTS_HAND_R];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_SWORD_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_sword_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_R].load = 1;
		LoadModel(MODEL_PLAYER_SWORD_R, &g_Parts[i][PLAYER_PARTS_SWORD_R].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_SWORD_R].model, &g_Parts[i][PLAYER_PARTS_SWORD_R].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_SWORD_B].use = TRUE;
		g_Parts[i][PLAYER_PARTS_SWORD_B].parent = &g_Player[i];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_SWORD_B;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_SWORD_B;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_SWORD_B;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_SWORD_B;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_SWORD_B;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_sword_b) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SWORD_B].load = 1;
		LoadModel(MODEL_PLAYER_SWORD_B, &g_Parts[i][PLAYER_PARTS_SWORD_B].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_SWORD_B].model, &g_Parts[i][PLAYER_PARTS_SWORD_B].diffuse[0]);

		g_Parts[i][PLAYER_PARTS_SCABBARD].use = TRUE;
		g_Parts[i][PLAYER_PARTS_SCABBARD].parent = &g_Player[i];	// �e���Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_STOP] = PLAYER_PARTS_SCABBARD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_STOP] = sizeof(stop_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_MOVE] = PLAYER_PARTS_SCABBARD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_MOVE] = sizeof(move_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_DASH] = PLAYER_PARTS_SCABBARD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_DASH] = sizeof(dash_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_JUMP] = PLAYER_PARTS_SCABBARD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_JUMP] = sizeof(jump_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblNo[PLAYER_ANIM_ATTACK] = PLAYER_PARTS_SCABBARD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].tblMax[PLAYER_ANIM_ATTACK] = sizeof(attack_tbl_scabbard) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][PLAYER_PARTS_SCABBARD].load = 1;
		LoadModel(MODEL_PLAYER_SCABBARD, &g_Parts[i][PLAYER_PARTS_SCABBARD].model);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Parts[i][PLAYER_PARTS_SCABBARD].model, &g_Parts[i][PLAYER_PARTS_SCABBARD].diffuse[0]);

		// �ŏ�����p�[�c���������Ă���
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
		{
			g_Parts[i][j].Animation(PLAYER_ANIM_STOP);
		}

		// �N�H�[�^�j�I���̏�����
		XMStoreFloat4(&g_Player[i].Quaternion, XMQuaternionIdentity());

		for (int j = 0; j < PLAYER_ANIM_MAX; j++)
		{
			g_AnimTransFrameCnt[j] = 0.0f;
		}
	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// ���f���̉������
		if (g_Player[i].load == TRUE)
		{
			UnloadModel(&g_Player[i].model);
			g_Player[i].load = FALSE;
		}

		// �p�[�c�̉������
		for (int j = 0; j < PLAYER_PARTS_MAX; j++)
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
void UpdatePlayer(void)
{
	if (GetMode() == MODE_TITLE)
	{
		// �K�w�A�j���[�V����
		for (int i = 0; i < PLAYER_PARTS_MAX; i++)
		{
			// �g���Ă���Ȃ珈������
			// ���`��Ԃ̏���
			g_Parts[0][i].Animation(PLAYER_ANIM_MOVE);
		}
	}

	// �Q�[�����[�h�̏���
	else
	{

		for (int i = 0; i < PLAYER_MAX; i++)
		{


			if (g_Player[i].use == TRUE && GetFade() == FADE_NONE)
			{
				// ���G���Ԃ͐F�ւ�������
				if (g_Player[i].colCnt > 0)
				{
					// �������点��
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

					// ���̐F
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
						// ���G���ԏI�����Ɍ��̐F�ɖ߂�
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
				// �ړ��t���O���Z�b�g
				g_Player[i].dir = XMFLOAT2(0.0f, 0.0f);
				// �A�j���[�V������ҋ@���[�V�����Ƀ��Z�b�g
				int animNum = PLAYER_ANIM_STOP;

				// �ړ�����
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

				// �Q�[���p�b�g����
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

				// �_�b�V������
				if (GetKeyboardPress(DIK_LSHIFT) || IsButtonPressed(0, BUTTON_X))
				{
					g_Player[i].spd *= VALUE_DASH;

					// �ړ��L�[�������Ă���Ԃ̓_�b�V���A�j���[�V�������Z�b�g
					if (g_Player[i].spd >= VALUE_MOVE * VALUE_DASH)
					{
						animNum = PLAYER_ANIM_DASH;
					}
				}

				// �ݒ��ʂ��J��
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

				//    // Key���͂���������ړ���������
				if (g_Player[i].spd > 0.0f)
				{
					g_Player[i].rot.y = Turn(roty + cam->rot.y, g_Player[i].rot.y);

					// ���͂̂����������փv���C���[���������Ĉړ�������
					g_Player[i].pos.x -= sinf(roty + cam->rot.y) * g_Player[i].spd;
					g_Player[i].pos.z -= cosf(roty + cam->rot.y) * g_Player[i].spd;
				}

				//{	// �����������Ƀv���C���[���ړ�������
				//	// �����������Ƀv���C���[���������Ă��鏊
				//	g_Player[i].rot.y = roty + cam->rot.y;

				//	g_Player[i].pos.x -= sinf(g_Player[i].rot.y) * g_Player[i].spd;
				//	g_Player[i].pos.z -= cosf(g_Player[i].rot.y) * g_Player[i].spd;
				//}
				
				// ���C�L���X�g���đ����̍��������߂�
				XMFLOAT3 HitPosition;		// ��_
				XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
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

				//�ǂ̍��W�𒴂��Ȃ��悤�ɂ���
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


				// �e���ˏ���
				//if (GetKeyboardTrigger(DIK_SPACE))
				//{
				//	SetBullet(g_Player[i].pos, g_Player[i].rot);
				//}

				// �e���v���C���[�̈ʒu�ɍ��킹��
				//XMFLOAT3 pos = g_Player[i].pos;
				//pos.y -= (PLAYER_OFFSET_Y - 0.1f);
				//SetPositionShadow(g_Player[i].shadowIdx, pos);

				// �W�����v������
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
				// �W�����v�{�^���������H
				else if ((g_Player[i].jump == FALSE) && (GetKeyboardPress(DIK_X)))
				{
					g_Player[i].jump = TRUE;
					g_Player[i].jumpCnt = 0;
				}

				ENEMY* enemy = GetEnemy();
				ENEMY* boss = GetBoss();
				// �A�^�b�N������
				if (g_Player[i].attack == TRUE)
				{
					// �A�^�b�N�A�j���[�V�������Z�b�g
					animNum = PLAYER_ANIM_ATTACK;
					g_Player[i].atkCnt++;
					// ���𔲂����悤�Ɍ�����Ƃ�������Ɏ��悤�ɃZ�b�g
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
							// �����Ă���G�l�~�[�̂ݔ���
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
				// �A�^�b�N�{�^���������H
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

				// �K�w�A�j���[�V����
				for (int j = 0; j < PLAYER_PARTS_MAX; j++)
				{
					if (i == 0)
					{
						// �g���Ă���Ȃ珈������
						// ���`��Ԃ̏���
						if (g_Player[i].jump == TRUE)
						{
							// �A�^�b�N�A�j���[�V������D��
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

				// �|�C���g���C�g�̃e�X�g
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
				// �p������
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
				// �f�o�b�O�\��
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
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (GetViewPortTypeGame() == TYPE_FULL_SCREEN && i == 1) continue;

		if (g_Player[i].use == TRUE || (GetMode() == MODE_TITLE && i == 0))
		{
			if (GetViewPortTypeGame() == TYPE_LEFT_HALF_SCREEN && i != GetPlayerNum()) continue;

			// �}�e���A���ޔ�p
			//XMFLOAT4 playerDiffuse[MODEL_MAX_MATERIAL];
			//XMFLOAT4 partsDiffuse[PLAYER_PARTS_MAX][MODEL_MAX_MATERIAL];
			// ���R���s��̉e�`��
			//{
			//	GetModelDiffuse(&g_Player[i].model, &playerDiffuse[0]);
			//	for (int j = 0; j < g_Player[i].model.SubsetNum; j++)
			//	{
			//		SetModelDiffuse(&g_Player[i].model, j, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.8f));
			//	}

			//	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

			//	// ���[���h�}�g���b�N�X�̏�����
			//	mtxWorld = XMMatrixIdentity();

			//	// �X�P�[���𔽉f
			//	mtxScl = XMMatrixScaling(g_Player[i].scl.x * 1.2f, 0.1f, g_Player[i].scl.z * 1.2f);
			//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			//	// ��]�𔽉f
			//	mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
			//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//	// �ړ��𔽉f
			//	mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y - PLAYER_OFFSET_Y, g_Player[i].pos.z);
			//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			//	// ���[���h�}�g���b�N�X�̐ݒ�
			//	SetWorldMatrix(&mtxWorld);

			//	XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);


			//	// ���f���`��
			//	DrawModel(&g_Player[i].model);

			//	// �K�w�A�j���[�V����
			//	for (int j = 0; j < PLAYER_PARTS_MAX; j++)
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
			//		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
			//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			//		// �ړ��𔽉f
			//		mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
			//		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			//		if (g_Parts[i][j].parent != NULL)	// �q����������e�ƌ�������
			//		{
			//			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
			//			// ��
			//			// g_Player[i].mtxWorld���w���Ă���
			//		}

			//		XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

			//		// �g���Ă���Ȃ珈������
			//		if (g_Parts[i][j].use == FALSE) continue;

			//		// ���[���h�}�g���b�N�X�̐ݒ�
			//		SetWorldMatrix(&mtxWorld);

			//		// ���f���`��
			//		DrawModel(&g_Parts[i][j].model);

			//	}
			//}

			// ���f���`��
			{
				//for (int j = 0; j < g_Player[i].model.SubsetNum; j++)
				//{
				//	SetModelDiffuse(&g_Player[i].model, j, playerDiffuse[j]);
				//}

				// �{�X��ɓ���������点��
				//if (GetBossFlg() == TRUE)
				{
					SetRimLight(2);
				}
				XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

				// �J�����O����
				SetCullingMode(CULL_MODE_NONE);

				// ���[���h�}�g���b�N�X�̏�����
				mtxWorld = XMMatrixIdentity();

				// �X�P�[���𔽉f
				mtxScl = XMMatrixScaling(g_Player[i].scl.x, g_Player[i].scl.y, g_Player[i].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// ��]�𔽉f
				mtxRot = XMMatrixRotationRollPitchYaw(g_Player[i].rot.x, g_Player[i].rot.y + XM_PI, g_Player[i].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// �N�H�[�^�j�I���𔽉f
				quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player[i].Quaternion));
				mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(g_Player[i].pos.x, g_Player[i].pos.y, g_Player[i].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&mtxWorld);

				XMStoreFloat4x4(&g_Player[i].mtxWorld, mtxWorld);

				// ���f���`��
				DrawModel(&g_Player[i].model);


				// �K�w�A�j���[�V����
				for (int j = 0; j < PLAYER_PARTS_MAX; j++)
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
						// g_Player[i].mtxWorld���w���Ă���
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
		}
	}
	SetRimLight(0);
	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player[0];
}

//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER* GetPlayerParts(void)
{
	return &g_Parts[0][0];
}

//=============================================================================
// ��]�ʒ���
//=============================================================================
float NormalizeAngle(float angle)
{

	while (angle > XM_PI) angle -= 2 * XM_PI;
	while (angle < -XM_PI) angle += 2 * XM_PI;
	return angle;
}

//=============================================================================
// �v���C���[�̌�����ύX
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
// �A�j���[�V�����֐�
//=============================================================================
void PLAYER::Animation(int animNum)
{
	INTERPOLATION_DATA* tbl;

	switch (animNum)
	{
	case PLAYER_ANIM_STOP:
		tbl = g_StopTblAdr[tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	case PLAYER_ANIM_MOVE:
		tbl = g_MoveTblAdr[tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	case PLAYER_ANIM_DASH:
		tbl = g_DashTblAdr[tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	case PLAYER_ANIM_JUMP:
		tbl = g_JumpTblAdr[tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
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
void PLAYER::Animation(int animNum1, int animNum2)
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
			case PLAYER_ANIM_STOP:
				tbl[i] = g_StopTblAdr[tblNo[AnimNum[i]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			case PLAYER_ANIM_MOVE:
				tbl[i] = g_MoveTblAdr[tblNo[AnimNum[i]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			case PLAYER_ANIM_DASH:
				tbl[i] = g_DashTblAdr[tblNo[AnimNum[i]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			case PLAYER_ANIM_JUMP:
				tbl[i] = g_JumpTblAdr[tblNo[AnimNum[i]]];	// �ҋ@�e�[�u���̃A�h���X���擾
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

//=============================================================================
// ���[�V�����u�����h�֐�
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


