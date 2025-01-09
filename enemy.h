//=============================================================================
//
// �G�l�~�[���f������ [enemy.h]
// Author : 
//
//=============================================================================
#pragma once
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_MAX		(10)					// �G�l�~�[�̐�
#define BOSS_MAX		(1)						// �{�X�̐�

#define	ENEMY_SIZE		(4.0f)				// �����蔻��̑傫��
#define	BOSS_SPONE_TIME	(180)				// �{�X�o����J�����ɉf��t���[��
#define ENEMY_OFFSET_Y		(7.0f)						// �G�l�~�[�̑��������킹��

enum
{
	ENEMY_PARTS_HEAD,
	ENEMY_PARTS_ARM_L,
	ENEMY_PARTS_ARM_R,
	ENEMY_PARTS_HAND_L,
	ENEMY_PARTS_HAND_R,
	ENEMY_PARTS_LEG_L,
	ENEMY_PARTS_LEG_R,
	ENEMY_PARTS_FOOT_L,
	ENEMY_PARTS_FOOT_R,
	ENEMY_PARTS_SWORD_R,
	//ENEMY_PARTS_SWORD_B,
	//ENEMY_PARTS_SCABBARD,

	ENEMY_PARTS_MAX
};

enum
{
	ENEMY_ANIM_STOP,
	ENEMY_ANIM_MOVE,
	ENEMY_ANIM_ATTACK,

	ENEMY_ANIM_MAX
};
//*****************************************************************************
// �\���̒�`
//*****************************************************************************
class ENEMY
{
public:
	XMFLOAT4X4			mtxWorld;			// ���[���h�}�g���b�N�X
	XMFLOAT3			pos;				// ���f���̈ʒu
	XMFLOAT3			rot;				// ���f���̌���(��])
	XMFLOAT3			scl;				// ���f���̑傫��(�X�P�[��)

	BOOL				use;
	BOOL				load;
	DX11_MODEL			model;				// ���f�����
	XMFLOAT4			diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	int					hp;					// �G�l�~�[�̗̑�
	BOOL				attack;				// �A�^�b�N�t���O
	int					atkVal;				// �U����
	int					atkCnt;				// �A�^�b�N���̃J�E���g
	float				spd;				// �ړ��X�s�[�h
	int					moveCnt;			// �ړ�����Ԋu
	BOOL				moveFlg;			// �ړ��t���O

	int					colCnt;				// ���G����
	float				size;				// �����蔻��̑傫��
	int					shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	BOOL				look;				// �v���C���[�𔭌��������

	// �K�w�A�j���[�V�����p�̃����o�[�ϐ�
	float				time[ENEMY_ANIM_MAX];				// ���`��ԗp
	int					tblNo[ENEMY_ANIM_MAX];				// �s���f�[�^�̃e�[�u���ԍ�
	int					tblMax[ENEMY_ANIM_MAX];				// ���̃e�[�u���̃f�[�^��
	int					animNum;							// ���݂̃A�j���[�V�����ԍ�

	// �e�́ANULL�A�q���͐e�̃A�h���X������
	ENEMY* parent;			// �������e�Ȃ�NULL�A�������q���Ȃ�e��player�A�h���X

	// �A�j���[�V�����֐�
	void Animation(int animNum);
	void Animation(int animNum1, int animNum2);
	int DecHP(int atk);
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);
ENEMY *GetEnemyParts(void);
ENEMY *GetBoss(void);
void ColCheck(XMFLOAT3 pos, ENEMY& enemy, ENEMY parts[ENEMY_PARTS_MAX]);
BOOL GetBossFlg(void);
void AttackCheck(ENEMY& enemy, ENEMY parts[ENEMY_PARTS_MAX]);
void SetBoss(void);
int GetBossSponeCnt(void);
void AddBossSponeCnt(void);
void ReleaseEnemy(void);
