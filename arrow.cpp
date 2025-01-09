//=============================================================================
//
// ���m�t���O���� [arrow.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "arrow.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX				(1)				// �e�N�X�`���̐�

#define	TEXTURE_WIDTH			(5.0f)			// ���_�T�C�Y
#define	TEXTURE_HEIGHT			(5.0f)			// ���_�T�C�Y

#define	VALUE_MOVE				(5.0f)			// �v���C���[�𒆐S�̉~�̔��a
#define	ARROW_Y					(-4.0f)		// �v���C���[�𒆐S�̉~�̔��a
#define RADIAN					(XM_PI / 180.0f)					// ���W�A���ϊ��p

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexArrow(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static ARROW					g_Arrow[ENEMY_MAX];	// �}�[�N���[�N

static int					g_TexNo;			// �e�N�X�`���ԍ�

static char *g_TextureName[] =
{
	"data/TEXTURE/arrow.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitArrow(void)
{
	MakeVertexArrow();

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
		ZeroMemory(&g_Arrow[i].material, sizeof(g_Arrow[i].material));
		g_Arrow[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Arrow[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Arrow[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Arrow[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Arrow[i].width = TEXTURE_WIDTH;
		g_Arrow[i].height = TEXTURE_HEIGHT;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitArrow(void)
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
void UpdateArrow(void)
{
	PLAYER* player = GetPlayer();
	if (player->use == TRUE)
	{
		ENEMY* enemy = GetEnemy();
		ENEMY* boss = GetBoss();

		// �G�l�~�[�̕����ɃZ�b�g
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == FALSE) continue;
			float x = (player->pos.x - enemy[i].pos.x);
			float z = (player->pos.z - enemy[i].pos.z);
			float rad = atan2f(z, x);

			g_Arrow[i].pos.x = player->pos.x - cosf(rad) * VALUE_MOVE;
			g_Arrow[i].pos.y = player->pos.y + ARROW_Y;
			g_Arrow[i].pos.z = player->pos.z - sinf(rad) * VALUE_MOVE;

			g_Arrow[i].rot.y = -rad;
		}

		// �{�X�̕����ɃZ�b�g
		for (int i = 0; i < BOSS_MAX; i++)
		{
			if (boss[i].use == FALSE) continue;
			float x = (player->pos.x - boss[i].pos.x);
			float z = (player->pos.z - boss[i].pos.z);
			float rad = atan2f(z, x);

			g_Arrow[i].pos.x = player->pos.x - cosf(rad) * VALUE_MOVE;
			g_Arrow[i].pos.y = player->pos.y + ARROW_Y;
			g_Arrow[i].pos.z = player->pos.z - sinf(rad) * VALUE_MOVE;

			g_Arrow[i].rot.y = -rad;
		}
	}

#ifdef _DEBUG

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawArrow(void)
{
	PLAYER* player = GetPlayer();
	if (player->use == TRUE)
	{
		ENEMY* enemy = GetEnemy();
		ENEMY* boss = GetBoss();
		// ���e�X�g�ݒ�
		// ���e�X�g��L����
		SetAlphaTestEnable(TRUE);

		// ���C�e�B���O�𖳌�
		SetLightEnable(FALSE);

		// �J�����O����
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxTranslate, mtxRot, mtxWorld, mtxView;
		CAMERA* cam = GetCamera();

		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// �v���~�e�B�u�g�|���W�ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == FALSE) continue;
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Arrow[i].scl.x, g_Arrow[i].scl.y, g_Arrow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Arrow[i].rot.x, g_Arrow[i].rot.y, g_Arrow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Arrow[i].pos.x, g_Arrow[i].pos.y, g_Arrow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Arrow[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}

		// �{�X�̕���
		for (int i = 0; i < BOSS_MAX; i++)
		{
			if (boss[i].use == FALSE) continue;
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);


			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Arrow[i].scl.x, g_Arrow[i].scl.y, g_Arrow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Arrow[i].rot.x, g_Arrow[i].rot.y, g_Arrow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Arrow[i].pos.x, g_Arrow[i].pos.y, g_Arrow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_Arrow[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}

		// �J�����O�ݒ��߂�
		SetCullingMode(CULL_MODE_BACK);

		// ���C�e�B���O��L����
		SetLightEnable(TRUE);

		// ���e�X�g�𖳌���
		SetAlphaTestEnable(FALSE);
	}
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexArrow(void)
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
