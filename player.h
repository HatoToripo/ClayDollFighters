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
#define	PLAYER_HP_MAX	(10)				// �v���C���[�̗̑͂̍ő�l

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
// �\���̒�`
//*****************************************************************************
class PLAYER
{
public:
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	int					hp;					// �̗�
	float				gauge;				// �Q�[�W
	float				spd;				// �ړ��X�s�[�h

	BOOL				attack;				// �A�^�b�N�t���O
	int					atkVal;				// �U����
	int					atkCnt;				// �A�^�b�N���̃J�E���g

	BOOL				load;
	DX11_MODEL			model;				// ���f�����

	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�

	BOOL				use;

	float				size;

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float				time[PLAYER_ANIM_MAX];				// ���`��ԗp
	int					tblNo[PLAYER_ANIM_MAX];				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax[PLAYER_ANIM_MAX];			// ���̃e�[�u���̃f�[�^��
	int					animNum;							// ���݂̃A�j���[�V�����ԍ�

	BOOL				jump;			// �W�����v�t���O
	int					jumpCnt;		// �W�����v���̃J�E���g

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	PLAYER				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// �����������Ă��鏊

	void Animation(int animNum);
	void Animation(int animNum1, int animNum2);
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);
PLAYER* GetPlayerParts(void);

//void PlayerAnimation(int animNum, int i);
//void PlayerAnimationBlend(int animNum1, int animNum2, int i);

