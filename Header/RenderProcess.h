#pragma once

#define BASE_RENDER_SIZE_W 1920
#define BASE_RENDER_SIZE_H 1080

// �Q�[�����C���̕`�揈��������������
void GameMainRender_Initialize();

// �Q�[�����C���̕`�揈���̌�n��������
void GameMainRender_Terminate();

// �Q�[�����C���̕`�揈���̎��O����������
void GameMainRender_FrontProcess();

// �Q�[�����C���̕`�揈���̎��㏈��������
void GameMainRender_RearProcess();