#include <stdio.h>
#include "ConfigData.h"
#include "Structure.h"
#include "Screenshot.h"

extern ConfigData cConfigData;

void SaveConfig() {
	FILE *fp;
	fopen_s(&fp, "config.dat", "wb");
	if (fp == NULL) std::exit(EXIT_FAILURE); //mythrow("configƒtƒ@ƒCƒ‹‚Ì•Û‘¶‚ÉŽ¸”s‚µ‚Ü‚µ‚½");

	fwrite(&cConfigData.FullScr, sizeof(bool), 1, fp);
	fwrite(&cConfigData.MasterVol, sizeof(char), 1, fp);
	fwrite(&cConfigData.BGMVol, sizeof(unsigned char), 1, fp);
	fwrite(&cConfigData.SEVol, sizeof(unsigned char), 1, fp);
	fwrite(&cConfigData.WindowResolution, sizeof(char), 1, fp);
	fwrite(&cConfigData.DrawMode, sizeof(char), 1, fp);
	fwrite(&cConfigData.AntialiasFlag, sizeof(bool), 1, fp);
	fwrite(&cConfigData.VSyncFlag, sizeof(bool), 1, fp);
	fwrite(&cConfigData.ShadowLevel, sizeof(char), 1, fp);
	fwrite(&cConfigData.ReflectionLevel, sizeof(char), 1, fp);
	fwrite(&cConfigData.SSType, sizeof(char), 1, fp);
	//fwrite(&cConfigData, sizeof(ConfigData), 1, fp);
	fclose(fp);
}

void CreateConfig() {
	cConfigData.FullScr = true;
	cConfigData.MasterVol = 100;
	cConfigData.BGMVol = 0xFF;
	cConfigData.SEVol = 0xFF;
	cConfigData.WindowResolution = FullHD;
	cConfigData.DrawMode = DX_DRAWMODE_ANISOTROPIC;
	cConfigData.AntialiasFlag = true;
	cConfigData.VSyncFlag = false;
	cConfigData.ShadowLevel = Highest;
	cConfigData.ReflectionLevel = Highest;
	cConfigData.SSType = eSSType_PNG;

	SaveConfig();
}

void LoadConfig() {
	FILE *fp;
	fopen_s(&fp, "config.dat", "rb");
	if (fp == NULL) {
		CreateConfig();
		return;
	}

	fread(&cConfigData.FullScr, sizeof(bool), 1, fp);
	fread(&cConfigData.MasterVol, sizeof(char), 1, fp);
	fread(&cConfigData.BGMVol, sizeof(unsigned char), 1, fp);
	fread(&cConfigData.SEVol, sizeof(unsigned char), 1, fp);
	fread(&cConfigData.WindowResolution, sizeof(char), 1, fp);
	fread(&cConfigData.DrawMode, sizeof(char), 1, fp);
	fread(&cConfigData.AntialiasFlag, sizeof(bool), 1, fp);
	fread(&cConfigData.VSyncFlag, sizeof(bool), 1, fp);
	fread(&cConfigData.ShadowLevel, sizeof(char), 1, fp);
	fread(&cConfigData.ReflectionLevel, sizeof(char), 1, fp);
	fread(&cConfigData.SSType, sizeof(char), 1, fp);
	//fread(&cConfigData, sizeof(ConfigData), 1, fp);
	fclose(fp);

	extern int WINDOWS_H, WINDOWS_V;
	switch (cConfigData.WindowResolution) {
		case BlackBerry: WINDOWS_H = 640;  WINDOWS_V = 480;  break;
		case SVGA:		 WINDOWS_H = 800;  WINDOWS_V = 600;  break;
		case XGA:		 WINDOWS_H = 1024; WINDOWS_V = 768;  break;
		case SXGA:		 WINDOWS_H = 1280; WINDOWS_V = 1024; break;
		case HDTV:		 WINDOWS_H = 1080; WINDOWS_V = 720;  break;
		case FullHD:	 WINDOWS_H = 1920; WINDOWS_V = 1080; break;
	}

	return;
}