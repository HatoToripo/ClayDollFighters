//=============================================================================
//
// �G�f�B�^�[�v���C���[���� [editplayer.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"
#include "player.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define EDITPLAYER_MAX		(1)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class EDITPLAYER
{
public:
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

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
	EDITPLAYER				*parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �N�H�[�^�j�I��
	XMFLOAT4			Quaternion;

	XMFLOAT3			UpVector;			// �����������Ă��鏊

	void Animation(int animNum);
	void Animation(int animNum1, int animNum2);
	void DecHP(int atk);
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEditPlayer(void);
void UninitEditPlayer(void);
void UpdateEditPlayer(void);
void DrawEditPlayer(void);

EDITPLAYER *GetEditPlayer(void);
EDITPLAYER* GetEditPlayerParts(void);

//void PlayerAnimation(int animNum, int i);
//void PlayerAnimationBlend(int animNum1, int animNum2, int i);

INTERPOLATION_DATA* GetStopTbl(int animNum);
INTERPOLATION_DATA* GetMoveTbl(int animNum);
INTERPOLATION_DATA* GetDashTbl(int animNum);
