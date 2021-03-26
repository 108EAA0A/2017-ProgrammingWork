#include "Screenshot.h"

#include "_DxLib.h"
#include "ConfigData.h"
#include <direct.h>

#include <shlwapi.h>
#pragma comment( lib , "shlwapi.lib" )

extern ConfigData cConfigData;

static void TakeSS(const char str[]) {
	extern int WINDOWS_H, WINDOWS_V;
	
	switch (cConfigData.SSType) {	//SSType�ɉ�����SS�p�֐���ύX
		case eSSType_BMP: SaveDrawScreenToBMP (0, 0, WINDOWS_H, WINDOWS_V, str);		 break;
		case eSSType_PNG: SaveDrawScreenToPNG (0, 0, WINDOWS_H, WINDOWS_V, str, 9);	 break;
		case eSSType_JPG: SaveDrawScreenToJPEG(0, 0, WINDOWS_H, WINDOWS_V, str, 100); break;
	}
}

void Screenshot() {
	extern int WINDOWS_H, WINDOWS_V;
	
	char cdir[256];	//�J�����g�f�B���N�g�����[�o�b�t�@
	if (GetCurrentDirectory(sizeof(cdir), cdir) == 0) return; //�J�����g�f�B���N�g���擾���擾���s���E�o

	std::string cdirstr = cdir; //string�Ɏ��[
	cdirstr += "/Screenshot";	//SS�p�f�B���N�g���̕������ǋL
	int errchkmkd = _mkdir(cdirstr.c_str()); //�f�B���N�g���쐬
	
	DATEDATA DateStr;
	GetDateTime(&DateStr);	//���ݎ������擾����
	char str[49]; // = "Screenshot/Screenshot-0000-00-00_00.00.00-00.xxx";
	snprintf(str, sizeof(str) - 6U,	//6U�̓t�@�C���d����3U+�g���q��3U
		"Screenshot/Screenshot-%04d-%02d-%02d_%02d.%02d.%02d.",
		DateStr.Year, DateStr.Mon, DateStr.Day, DateStr.Hour, DateStr.Min, DateStr.Sec);
	std::string SSdir = str;
	switch (cConfigData.SSType) {	//SSType�ɉ����Ċg���q��ύX
		case eSSType_BMP: SSdir += "bmp"; break;
		case eSSType_PNG: SSdir += "png"; break;
		case eSSType_JPG: SSdir += "jpg"; break;
	}
	if (PathFileExists(SSdir.c_str()) != 0) {	//�����t�@�C���������݂���ꍇ�����ɐ����ǉ�
		for (int i = 1; i < 60; ++i) {
			snprintf(str, sizeof(str) - 3U,	//3U�͊g���q��
				"Screenshot/Screenshot-%04d-%02d-%02d_%02d.%02d.%02d-%d.",
				DateStr.Year, DateStr.Mon, DateStr.Day, DateStr.Hour, DateStr.Min, DateStr.Sec, i);
			SSdir = str;
			switch (cConfigData.SSType) {	//SSType�ɉ����Ċg���q��ύX
				case eSSType_BMP: SSdir += "bmp"; break;
				case eSSType_PNG: SSdir += "png"; break;
				case eSSType_JPG: SSdir += "jpg"; break;
			}
			if (PathFileExists(SSdir.c_str()) == 0) {
				TakeSS(SSdir.c_str());
				return;
			}
		}
	}
	TakeSS(SSdir.c_str());
}