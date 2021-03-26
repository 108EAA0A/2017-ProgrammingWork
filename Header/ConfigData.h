#pragma once

struct ConfigData {
	char WindowResolution;
	bool FullScr;

	char MasterVol;
	unsigned char BGMVol;
	unsigned char SEVol;

	char DrawMode;
	bool AntialiasFlag;

	bool VSyncFlag;

	char ShadowLevel;

	char ReflectionLevel;

	char SSType;
};