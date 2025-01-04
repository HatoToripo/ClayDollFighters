//=============================================================================
//
// ���f������ [player.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_PLAYER		(1)					// �v���C���[�̐�

#define	PLAYER_SIZE		(5.0f)				// �����蔻��̑傫��

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
// �\���̒�`
//*****************************************************************************
class PLAYER
{
public:
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	float				spd;				// �ړ��X�s�[�h
	
	BOOL				load;
	DX11_MODEL			model;				// ���f�����

	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	BOOL				use;

	float				size;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float				time[ANIM_MAX];				// ���`��ԗp
	int					tblNo[ANIM_MAX];				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax[ANIM_MAX];				// ���̃e�[�u���̃f�[�^��

	BOOL				jump;			// �W�����v�t���O
	int					jumpCnt;		// �W�����v���̃J�E���g

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// �����������Ă��鏊

};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
void Animation(int animNum, int i);
void AnimationBlend(int animNum1, int animNum2, int i);

