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
#include "player.h"
#include "shadow.h"
#include "bullet.h"
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

#define	VALUE_MOVE			(0.5f)							// �ړ���
#define	VALUE_DASH			(2.0f)							// �ړ���
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)				// ��]��

#define PLAYER_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(7.0f)							// �v���C���[�̑��������킹��
#define RADIAN				(XM_PI / 180)					// ���W�A���ϊ��p

#define PLAYER_HEAD_Y		(2.7f)								// ����x���W

#define PLAYER_ARM_X		(1.5f)								// �r��x���W
#define PLAYER_ARM_Y		(2.5f)								// �r��y���W

#define PLAYER_HAND_X		(0.0f)								// �r��x���W
#define PLAYER_HAND_Y		(-1.5f)								// �r��y���W

#define PLAYER_LEG_X		(1.1f)								// �r��x���W
#define PLAYER_LEG_Y		(-2.4f)								// �r��y���W

#define PLAYER_FOOT_X		(0.0f)								// �r��x���W
#define PLAYER_FOOT_Y		(-1.4f)								// �r��y���W

#define PLAYER_JUMP_Y		(50.0f)								// �W�����v��
#define PLAYER_JUMP_CNT_MAX	(28.0f)								// �W�����v�S�̃t���[��

#define ANIM_FRAME_STOP		(15.0f)							// �ҋ@�A�j���[�V�����̊Ԋu
#define ANIM_FRAME_MOVE		(30.0f)							// �ړ��A�j���[�V�����̊Ԋu
#define ANIM_FRAME_DASH		(ANIM_FRAME_MOVE * 0.5f)		// �_�b�V���A�j���[�V�����̊Ԋu

#define ANIM_FRAME_JUMP		(15.0f)								// �W�����v�A�j���[�V�����̊Ԋu

#define BLEND_FRAME_STOP	(60.0f)							// �ҋ@���[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_MOVE	(90.0f)							// �ړ����[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_DASH	(90.0f)							// �_�b�V�����[�V�����ɑJ�ڂ���܂ł̎���
#define BLEND_FRAME_JUMP	(90.0f)							// �W�����v���[�V�����ɑJ�ڂ���܂ł̎���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER		g_Player;						// �v���C���[

static PLAYER		g_Parts[PLAYER_PARTS_MAX];		// �v���C���[�̃p�[�c�p

static float		roty = 0.0f;

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

// �W�����v���[�V�����f�[�^
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

// �I�������A�j���[�V�����Ɉڍs����t���[��
static float g_AnimTransFrame[ANIM_MAX] =
{
	BLEND_FRAME_STOP,
	BLEND_FRAME_MOVE,
	BLEND_FRAME_DASH,
	BLEND_FRAME_JUMP
};

static float g_AnimTransFrameCnt[ANIM_MAX];			// �A�j���[�V�����Ɉڍs����t���[���̃J�E���^

static int g_AnimNum[PLAYER_PARTS_MAX];		// ���݂̃A�j���[�V�����ԍ�

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player.load = TRUE;
	LoadModel(MODEL_PLAYER, &g_Player.model);

	g_Player.pos = XMFLOAT3(-10.0f, 0.0f - 50.0f, -50.0f);
	g_Player.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Player.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A

	g_Player.use = TRUE;			// TRUE:�����Ă�
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��
	g_Player.jump = FALSE;
	g_Player.jumpCnt = 0;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�

	// �L�[�����������̃v���C���[�̌���
	roty = 0.0f;

	g_Player.parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������


	// �K�w�A�j���[�V�����̏�����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		g_Parts[i].use = FALSE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Parts[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// �e�q�֌W
		g_Parts[i].parent = &g_Player;		// �� �����ɐe�̃A�h���X������
		//	g_Parts[�r].parent= &g_Player;		// �r��������e�͖{�́i�v���C���[�j
		//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

			// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
		for (int j = 0; j < ANIM_MAX; j++)
		{
			g_Parts[i].time[j] = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[i].tblNo[j] = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Parts[i].tblMax[j] = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
		}
		// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
		g_Parts[i].load = FALSE;


		// �p�[�c�̏����A�j���[�V������ݒ�
		g_AnimNum[i] = ANIM_STOP;

	}

	g_Parts[PARTS_HEAD].use = TRUE;
	g_Parts[PARTS_HEAD].parent = &g_Player;	// �e���Z�b�g
	g_Parts[PARTS_HEAD].tblNo[ANIM_STOP] = PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HEAD].tblMax[ANIM_STOP] = sizeof(stop_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HEAD].tblNo[ANIM_MOVE] = PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HEAD].tblMax[ANIM_MOVE] = sizeof(move_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HEAD].tblNo[ANIM_DASH] = PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HEAD].tblMax[ANIM_DASH] = sizeof(dash_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HEAD].tblNo[ANIM_JUMP] = PARTS_HEAD;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HEAD].tblMax[ANIM_JUMP] = sizeof(jump_tbl_head) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HEAD].load = 1;
	LoadModel(MODEL_PLAYER_HEAD, &g_Parts[PARTS_HEAD].model);

	g_Parts[PARTS_ARM_L].use = TRUE;
	g_Parts[PARTS_ARM_L].parent = &g_Player;	// �e���Z�b�g
	g_Parts[PARTS_ARM_L].tblNo[ANIM_STOP] = PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_ARM_L].tblMax[ANIM_STOP] = sizeof(stop_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_ARM_L].tblNo[ANIM_MOVE] = PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_ARM_L].tblMax[ANIM_MOVE] = sizeof(move_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_ARM_L].tblNo[ANIM_DASH] = PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_ARM_L].tblMax[ANIM_DASH] = sizeof(dash_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_ARM_L].tblNo[ANIM_JUMP] = PARTS_ARM_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_ARM_L].tblMax[ANIM_JUMP] = sizeof(jump_tbl_arm_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_ARM_L].load = 1;
	LoadModel(MODEL_PLAYER_ARM_L, &g_Parts[PARTS_ARM_L].model);

	g_Parts[PARTS_ARM_R].use = TRUE;
	g_Parts[PARTS_ARM_R].parent = &g_Player;	// �e���Z�b�g
	g_Parts[PARTS_ARM_R].tblNo[ANIM_STOP] = PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_ARM_R].tblMax[ANIM_STOP] = sizeof(stop_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_ARM_R].tblNo[ANIM_MOVE] = PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_ARM_R].tblMax[ANIM_MOVE] = sizeof(move_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_ARM_R].tblNo[ANIM_DASH] = PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_ARM_R].tblMax[ANIM_DASH] = sizeof(dash_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_ARM_R].tblNo[ANIM_JUMP] = PARTS_ARM_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_ARM_R].tblMax[ANIM_JUMP] = sizeof(jump_tbl_arm_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_ARM_R].load = 1;
	LoadModel(MODEL_PLAYER_ARM_R, &g_Parts[PARTS_ARM_R].model);

	g_Parts[PARTS_HAND_L].use = TRUE;
	g_Parts[PARTS_HAND_L].parent = &g_Parts[PARTS_ARM_L];	// �e���Z�b�g
	g_Parts[PARTS_HAND_L].tblNo[ANIM_STOP] = PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HAND_L].tblMax[ANIM_STOP] = sizeof(stop_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HAND_L].tblNo[ANIM_MOVE] = PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HAND_L].tblMax[ANIM_MOVE] = sizeof(move_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HAND_L].tblNo[ANIM_DASH] = PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HAND_L].tblMax[ANIM_DASH] = sizeof(dash_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HAND_L].tblNo[ANIM_JUMP] = PARTS_HAND_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HAND_L].tblMax[ANIM_JUMP] = sizeof(jump_tbl_hand_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HAND_L].load = 1;
	LoadModel(MODEL_PLAYER_HAND_L, &g_Parts[PARTS_HAND_L].model);

	g_Parts[PARTS_HAND_R].use = TRUE;
	g_Parts[PARTS_HAND_R].parent = &g_Parts[PARTS_ARM_R];	// �e���Z�b�g
	g_Parts[PARTS_HAND_R].tblNo[ANIM_STOP] = PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HAND_R].tblMax[ANIM_STOP] = sizeof(stop_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HAND_R].tblNo[ANIM_MOVE] = PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HAND_R].tblMax[ANIM_MOVE] = sizeof(move_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HAND_R].tblNo[ANIM_DASH] = PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HAND_R].tblMax[ANIM_DASH] = sizeof(dash_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HAND_R].tblNo[ANIM_JUMP] = PARTS_HAND_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_HAND_R].tblMax[ANIM_JUMP] = sizeof(jump_tbl_hand_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_HAND_R].load = 1;
	LoadModel(MODEL_PLAYER_HAND_R, &g_Parts[PARTS_HAND_R].model);

	g_Parts[PARTS_LEG_L].use = TRUE;
	g_Parts[PARTS_LEG_L].parent = &g_Player;	// �e���Z�b�g
	g_Parts[PARTS_LEG_L].tblNo[ANIM_STOP] = PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_LEG_L].tblMax[ANIM_STOP] = sizeof(stop_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_LEG_L].tblNo[ANIM_MOVE] = PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_LEG_L].tblMax[ANIM_MOVE] = sizeof(move_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_LEG_L].tblNo[ANIM_DASH] = PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_LEG_L].tblMax[ANIM_DASH] = sizeof(dash_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_LEG_L].tblNo[ANIM_JUMP] = PARTS_LEG_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_LEG_L].tblMax[ANIM_JUMP] = sizeof(jump_tbl_leg_l) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_LEG_L].load = 1;
	LoadModel(MODEL_PLAYER_LEG_L, &g_Parts[PARTS_LEG_L].model);

	g_Parts[PARTS_LEG_R].use = TRUE;
	g_Parts[PARTS_LEG_R].parent = &g_Player;	// �e���Z�b�g
	g_Parts[PARTS_LEG_R].tblNo[ANIM_STOP] = PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_LEG_R].tblMax[ANIM_STOP] = sizeof(stop_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_LEG_R].tblNo[ANIM_MOVE] = PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_LEG_R].tblMax[ANIM_MOVE] = sizeof(move_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_LEG_R].tblNo[ANIM_DASH] = PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_LEG_R].tblMax[ANIM_DASH] = sizeof(dash_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_LEG_R].tblNo[ANIM_JUMP] = PARTS_LEG_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_LEG_R].tblMax[ANIM_JUMP] = sizeof(jump_tbl_leg_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_LEG_R].load = 1;
	LoadModel(MODEL_PLAYER_LEG_R, &g_Parts[PARTS_LEG_R].model);

	g_Parts[PARTS_FOOT_L].use = TRUE;
	g_Parts[PARTS_FOOT_L].parent = &g_Parts[PARTS_LEG_L];	// �e���Z�b�g
	g_Parts[PARTS_FOOT_L].tblNo[ANIM_STOP] = PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_FOOT_L].tblMax[ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_FOOT_L].tblNo[ANIM_MOVE] = PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_FOOT_L].tblMax[ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_FOOT_L].tblNo[ANIM_DASH] = PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_FOOT_L].tblMax[ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_FOOT_L].tblNo[ANIM_JUMP] = PARTS_FOOT_L;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_FOOT_L].tblMax[ANIM_JUMP] = sizeof(jump_tbl_foot_r) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_FOOT_L].load = 1;
	LoadModel(MODEL_PLAYER_FOOT_L, &g_Parts[PARTS_FOOT_L].model);

	g_Parts[PARTS_FOOT_R].use = TRUE;
	g_Parts[PARTS_FOOT_R].parent = &g_Parts[PARTS_LEG_R];	// �e���Z�b�g
	g_Parts[PARTS_FOOT_R].tblNo[ANIM_STOP] = PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_FOOT_R].tblMax[ANIM_STOP] = sizeof(stop_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_FOOT_R].tblNo[ANIM_MOVE] = PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_FOOT_R].tblMax[ANIM_MOVE] = sizeof(move_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_FOOT_R].tblNo[ANIM_DASH] = PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_FOOT_R].tblMax[ANIM_DASH] = sizeof(dash_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_FOOT_R].tblNo[ANIM_JUMP] = PARTS_FOOT_R;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_Parts[PARTS_FOOT_R].tblMax[ANIM_JUMP] = sizeof(jump_tbl_foot_r) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
	g_Parts[PARTS_FOOT_R].load = 1;
	LoadModel(MODEL_PLAYER_FOOT_R, &g_Parts[PARTS_FOOT_R].model);
	// �N�H�[�^�j�I���̏�����
	XMStoreFloat4(&g_Player.Quaternion, XMQuaternionIdentity());

	for (int i = 0; i < ANIM_MAX; i++)
	{
		g_AnimTransFrameCnt[i] = 0.0f;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (g_Player.load == TRUE)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}

	// �p�[�c�̉������
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		if (g_Parts[i].load == TRUE)
		{
			// �p�[�c�̉������
			UnloadModel(&g_Parts[i].model);
			g_Parts[i].load = FALSE;
		}
	}



}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();

	g_Player.spd *= 0.4f;

	// �A�j���[�V������ҋ@���[�V�����Ƀ��Z�b�g
	int animNum = ANIM_STOP;

	// �ړ�����
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

	{	// �����������Ƀv���C���[���ړ�������
		// �����������Ƀv���C���[���������Ă��鏊
		g_Player.rot.y = roty + cam->rot.y;

		g_Player.pos.x -= sinf(g_Player.rot.y) * g_Player.spd;
		g_Player.pos.z -= cosf(g_Player.rot.y) * g_Player.spd;
	}


	// ���C�L���X�g���đ����̍��������߂�
	XMFLOAT3 HitPosition;		// ��_
	XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
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


	// �e���ˏ���
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		SetBullet(g_Player.pos, g_Player.rot);
	}

	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);

	// �W�����v������
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
	// �W�����v�{�^���������H
	else if ((g_Player.jump == FALSE) && (GetKeyboardTrigger(DIK_X)))
	{
		g_Player.jump = TRUE;
		g_Player.jumpCnt = 0;
	}

	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// �g���Ă���Ȃ珈������
		// ���`��Ԃ̏���
		if (g_Player.jump == TRUE)
		{
			AnimationBlend(ANIM_JUMP, animNum, i);
		}

		else
		{
			AnimationBlend(animNum, g_AnimNum[i], i);
		}
	}

	// �|�C���g���C�g�̃e�X�g
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
	// �p������
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
	// �f�o�b�O�\��
	PrintDebugProc("Player X:%f Y:%f Z:% N:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z, Normal.y);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, quatMatrix;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �N�H�[�^�j�I���𔽉f
	quatMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&g_Player.Quaternion));
	mtxWorld = XMMatrixMultiply(mtxWorld, quatMatrix);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// �����̐ݒ�
	//SetRimLight(1);

	// ���f���`��
	DrawModel(&g_Player.model);

	SetRimLight(0);

	// �K�w�A�j���[�V����
	for (int i = 0; i < PLAYER_PARTS_MAX; i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i].rot.x, g_Parts[i].rot.y, g_Parts[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		if (g_Parts[i].parent != NULL)	// �q����������e�ƌ�������
		{
			mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i].parent->mtxWorld));
			// ��
			// g_Player.mtxWorld���w���Ă���
		}

		XMStoreFloat4x4(&g_Parts[i].mtxWorld, mtxWorld);

		// �g���Ă���Ȃ珈������
		if (g_Parts[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// ���f���`��
		DrawModel(&g_Parts[i].model);

	}

	//SetRimLight(0);

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}

//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER* GetPlayerParts(void)
{
	return &g_Parts[0];
}

void Animation(int animNum, int i)
{
	// ���`��Ԃ̏���
	INTERPOLATION_DATA* tbl;

	switch (animNum)
	{
	case ANIM_STOP:
		tbl = g_StopTblAdr[g_Parts[i].tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	case ANIM_MOVE:
		tbl = g_MoveTblAdr[g_Parts[i].tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	case ANIM_DASH:
		tbl = g_DashTblAdr[g_Parts[i].tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	default:
		tbl = g_JumpTblAdr[g_Parts[i].tblNo[animNum]];	// �ҋ@�e�[�u���̃A�h���X���擾
		break;
	}

	int nowNo = (int)g_Parts[i].time[animNum];			// �������ł���e�[�u���ԍ������o���Ă���
	int maxNo = g_Parts[i].tblMax[animNum];				// �o�^�e�[�u�����𐔂��Ă���
	int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���

	XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
	XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
	XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

	XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
	XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
	XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

	float nowTime = g_Parts[i].time[animNum] - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

	Pos = nowPos + Pos * nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
	Rot = nowRot + Rot * nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
	Scl = nowScl + Scl * nowTime;								// ���݂̊g�嗦���v�Z���Ă���

	// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
	XMStoreFloat3(&g_Parts[i].pos, Pos);

	// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
	XMStoreFloat3(&g_Parts[i].rot, Rot);

	// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
	XMStoreFloat3(&g_Parts[i].scl, Scl);

	// frame���g�Ď��Ԍo�ߏ���������
	g_Parts[i].time[animNum] += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
	if ((int)g_Parts[i].time[animNum] >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
	{
		g_Parts[i].time[animNum] -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
	}
}

//=============================================================================
// ���[�V�����u�����h�֐�
//=============================================================================
void AnimationBlend(int animNum1, int animNum2, int i)
{
	// �A�j���[�V�����ԍ��������������ꍇ
	if (animNum1 == animNum2)
	{
		Animation(animNum1, i);
		return;
	}

	// �A�j���[�V�������ς�����^�C�~���O�ŃJ�E���g�����Z�b�g����
	if (g_AnimTransFrameCnt[animNum1] < g_AnimTransFrameCnt[animNum2])
	{
		g_AnimTransFrameCnt[animNum2] = 0.0f;
	}

	int animNum[2] = { animNum1, animNum2 };

	// �g���Ă���Ȃ珈������
	if ((g_Parts[i].use == TRUE) && (g_Parts[i].tblMax[animNum1] > 0))
	{
		INTERPOLATION_DATA* tbl[2];
		XMVECTOR Pos[2];
		XMVECTOR Rot[2];
		XMVECTOR Scl[2];

		for (int j = 0; j < 2; j++)
		{	// ���`��Ԃ̏���
			// 1�ڂ̃A�j���[�V�����̌v�Z
			switch (animNum[j])
			{
			case ANIM_STOP:
				tbl[j] = g_StopTblAdr[g_Parts[i].tblNo[animNum[j]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			case ANIM_MOVE:
				tbl[j] = g_MoveTblAdr[g_Parts[i].tblNo[animNum[j]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			case ANIM_DASH:
				tbl[j] = g_DashTblAdr[g_Parts[i].tblNo[animNum[j]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			default:
				tbl[j] = g_JumpTblAdr[g_Parts[i].tblNo[animNum[j]]];	// �ҋ@�e�[�u���̃A�h���X���擾
				break;
			}

			int nowNo = (int)g_Parts[i].time[animNum[j]];			// �������ł���e�[�u���ԍ������o���Ă���
			int maxNo = g_Parts[i].tblMax[animNum[j]];				// �o�^�e�[�u�����𐔂��Ă���
			int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���

			XMVECTOR nowPos = XMLoadFloat3(&tbl[j][nowNo].pos);	// XMVECTOR�֕ϊ�
			XMVECTOR nowRot = XMLoadFloat3(&tbl[j][nowNo].rot);	// XMVECTOR�֕ϊ�
			XMVECTOR nowScl = XMLoadFloat3(&tbl[j][nowNo].scl);	// XMVECTOR�֕ϊ�

			Pos[j] = XMLoadFloat3(&tbl[j][nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
			Rot[j] = XMLoadFloat3(&tbl[j][nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
			Scl[j] = XMLoadFloat3(&tbl[j][nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

			float nowTime = g_Parts[i].time[animNum[j]] - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

			Pos[j] = nowPos + Pos[j] * nowTime;
			Rot[j] = nowRot + Rot[j] * nowTime;
			Scl[j] = nowScl + Scl[j] * nowTime;

			// frame���g�Ď��Ԍo�ߏ���������
			g_Parts[i].time[animNum[j]] += 1.0f / tbl[j][nowNo].frame;	// ���Ԃ�i�߂Ă���
			if ((int)g_Parts[i].time[animNum[j]] >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
			{
				g_Parts[i].time[animNum[j]] -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
			}
		}
		// �d�݂Â�
		float weight = (1.0f / g_AnimTransFrame[animNum1]) * g_AnimTransFrameCnt[animNum1];
		// ���[�V�����u�����h�̍��W�v�Z
		XMVECTOR blendPos = Pos[0] * weight + Pos[1] * (1.0f - weight);
		XMVECTOR blendRot = Rot[0] * weight + Rot[1] * (1.0f - weight);
		XMVECTOR blendScl = Scl[0] * weight + Scl[1] * (1.0f - weight);

		// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
		XMStoreFloat3(&g_Parts[i].pos, blendPos);

		// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
		XMStoreFloat3(&g_Parts[i].rot, blendRot);

		// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
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