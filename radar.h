//=============================================================================
//
// ���m�͈͕`�揈�� [radar.h]
// Author : 
//
//=============================================================================
#pragma once
class RADAR
{
public:
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// ��]
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		width;			// ��
	float		height;			// ����
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitRadar(void);
void UninitRadar(void);
void UpdateRadar(void);
void DrawRadar(void);
//void SetSectorData(SECTOR* sector);
void SetRadarFlag(BOOL flag);


