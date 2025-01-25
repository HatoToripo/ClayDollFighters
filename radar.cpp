//=============================================================================
//
// ���m�͈͕`�揈�� [radar.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "radar.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX				(1)				// �e�N�X�`���̐�

#define SECTOR_DIVIDE			(100)			// ��`�̕�����
#define	TEXTURE_WIDTH			(LOOK_CIRCLE / SECTOR_DIVIDE * 2)			// ���_�T�C�Y
#define	TEXTURE_HEIGHT			(LOOK_CIRCLE)			// ���_�T�C�Y
#define RADIAN					(XM_PI / 180.0f)					// ���W�A���ϊ��p
#define	VALUE_MOVE				(10.0f)			// �G�l�~�[�𒆐S�̉~�̔��a
#define VALUE_ROTATE			(LOOK_CIRCLE_RAD / SECTOR_DIVIDE)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexRadar(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static RADAR			g_Radar[ENEMY_MAX];	// ���m�͈̓��[�N
//static SECTOR			g_Sector[ENEMY_MAX];	// ���m�͈̓��[�N

static int					g_TexNo;			// �e�N�X�`���ԍ�

static BOOL					g_RadarFlg;		// ���[�_�[�t���O

static char *g_TextureName[] =
{
	"data/TEXTURE/rectangle.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitRadar(void)
{
	MakeVertexRadar();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �}�[�N���[�N�̏�����
	for(int i = 0; i < ENEMY_MAX; i++)
	{
		// ���m�t���O�̏�����
		ZeroMemory(&g_Radar[i].material, sizeof(g_Radar[i].material));
		g_Radar[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Radar[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Radar[i].rot = XMFLOAT3(RADIAN * 90.0f, 0.0f, RADIAN * 90.0f);
		g_Radar[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Radar[i].width = TEXTURE_WIDTH;
		g_Radar[i].height = TEXTURE_HEIGHT;

		//g_Sector[i].startAngle = 0.0f;
		//g_Sector[i].endAngle = 0.0f;
		//g_Sector[i].flag = false;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitRadar(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateRadar(void)
{
	if (GetKeyboardTrigger(DIK_J))
	{
		if (g_RadarFlg == TRUE)
		{
			g_RadarFlg = FALSE;
		}
		else
		{
			g_RadarFlg = TRUE;
		}
	}


	ENEMY* enemy = GetEnemy();
	for(int i = 0; i < ENEMY_MAX; i++)
	{
		if (enemy[i].use == FALSE) continue;
		// �G�l�~�[�̓���ɍ��W���X�V
		g_Radar[i].pos = enemy[i].pos;
		g_Radar[i].pos.y = 0.1f;
		// �G�l�~�[�̑O�ɃZ�b�g
		g_Radar[i].rot.y = enemy[i].rot.y - RADIAN * 90.0f - LOOK_CIRCLE_RAD * 0.5f;
		
		//// �`�悷���`�̊p�x���Z�b�g
		//g_Sector[i].startAngle = g_Radar[i].rot.y - LOOK_CIRCLE_RAD * 0.5f;
		//g_Sector[i].endAngle = LOOK_CIRCLE_RAD;
	}
#ifdef _DEBUG

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawRadar(void)
{
	if (g_RadarFlg == TRUE)
	{
		ENEMY* enemy = GetEnemy();
		// ���e�X�g�ݒ�
	// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		// �J�����O����
		SetCullingMode(CULL_MODE_NONE);

		// ��`�t���O�L��
		//SetSectorFlag(TRUE);

		XMMATRIX mtxScl, mtxTranslate, mtxRot, mtxWorld;

		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// �v���~�e�B�u�g�|���W�ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == FALSE) continue;
			for (int j = 0; j < SECTOR_DIVIDE; j++)
			{
				// ���[���h�}�g���b�N�X�̏�����
				mtxWorld = XMMatrixIdentity();

				// �X�P�[���𔽉f
				mtxScl = XMMatrixScaling(g_Radar[i].scl.x, g_Radar[i].scl.y, g_Radar[i].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// ��]�𔽉f
				mtxRot = XMMatrixRotationRollPitchYaw(g_Radar[i].rot.x, g_Radar[i].rot.y + VALUE_ROTATE * j, g_Radar[i].rot.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

				// �ړ��𔽉f
				mtxTranslate = XMMatrixTranslation(g_Radar[i].pos.x, g_Radar[i].pos.y, g_Radar[i].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&mtxWorld);

				// �}�e���A���ݒ�
				SetMaterial(g_Radar[i].material);

				//SetSectorData(&g_Sector[i]);

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

				// �|���S���̕`��
				GetDeviceContext()->Draw(4, 0);
			}

		}

		// ��`�t���O����
		//SetSectorFlag(FALSE);

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// ���e�X�g�𖳌���
		SetAlphaTestEnable(FALSE);
	}
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexRadar(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = TEXTURE_WIDTH;
	float fHeight = TEXTURE_HEIGHT;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);


	return S_OK;
}

//=============================================================================
// ��`�̐ݒ�
//=============================================================================
//void SetSectorData(SECTOR* sector)
//{
//	SetSector(sector);
//}

void SetRadarFlag(BOOL flag)
{
	g_RadarFlg = flag;
}