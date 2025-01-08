//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

class PARTICLE
{
public:
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			width;			// ��
	float			height;			// ����
	BOOL			use;			// �g�p���Ă��邩�ǂ���

};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

void SetParticle(XMFLOAT3 pos, XMFLOAT4 col);
int SetParticle(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float width, float height);
void SetColorParticle(int nIdxParticle, XMFLOAT4 col);

