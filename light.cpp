//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "player.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
#define	DIRECTIONAL_LIGHT_Y		(30.0f)				// ���s�����̍���

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LIGHT	g_Light[LIGHT_MAX];

static FOG		g_Fog;

static BOOL		g_FogEnable = FALSE;

static 	XMFLOAT3 g_pos;
static 	XMFLOAT3 g_dir;
static PLAYER* g_player = GetPlayer();

//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{

	//���C�g������
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3( 0.0f, 0.0f, 0.0f );
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f);
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 100.0f;	// ��������
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ���C�g�̃^�C�v
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// ���s�����̐ݒ�i���E���Ƃ炷���j
	g_Light[0].Position = XMFLOAT3( 0.0f, DIRECTIONAL_LIGHT_Y, 0.0f );		// ���̌���
	g_Light[0].Direction = XMFLOAT3(-0.5f, -1.0f, 0.2f);		// ���̌���
	g_Light[0].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );	// ���̐F
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// ���s����
	g_Light[0].Enable = TRUE;									// ���̃��C�g��ON
	SetLight(0, &g_Light[0]);									// ����Őݒ肵�Ă���

	// �t�H�O�̏������i���̌��ʁj
	g_Fog.FogStart = 100.0f;									// ���_���炱�̋��������ƃt�H�O��������n�߂�
	g_Fog.FogEnd   = 250.0f;									// �����܂ŗ����ƃt�H�O�̐F�Ō����Ȃ��Ȃ�
	g_Fog.FogColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );		// �t�H�O�̐F
	SetFog(&g_Fog);
	SetFogEnable(g_FogEnable);				// ���̏ꏊ���`�F�b�N���� shadow

}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateLight(void)
{


}


//=============================================================================
// ���C�g�̐ݒ�
// Type�ɂ���ăZ�b�g���郁���o�[�ϐ����ς���Ă���
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}

LIGHT* GetLight(void)
{
	return &g_Light[0];
}

LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// �t�H�O�̐ݒ�
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}


BOOL	GetFogEnable(void)
{
	return(g_FogEnable);
}

void SetLightProjectViewMatrix()
{
	XMFLOAT3 p_pos = { 0.0f,1.0f,0.0f };

	p_pos = g_player->pos;
	g_pos = g_player->pos;

	XMFLOAT3 up = { 0.0f,1.0f,0.0f };


	XMVECTOR  l_dir = XMVector3Normalize(XMLoadFloat3(&g_dir));
	float lightHeight = 500.0f;
	XMVECTOR player_pos = XMLoadFloat3(&p_pos);


	XMVECTOR lightPosition = player_pos - l_dir * lightHeight;


	XMMATRIX lightView = XMMatrixLookAtLH(lightPosition, XMLoadFloat3(&p_pos), XMLoadFloat3(&up));
	XMMATRIX lightProjection = XMMatrixOrthographicLH(1000.0f, 1000.0f, 0.1f, 1000.0f);
	XMMATRIX lightProjView = XMMatrixMultiply(lightView, lightProjection);


	lightProjView = XMMatrixTranspose(lightProjView);
}
