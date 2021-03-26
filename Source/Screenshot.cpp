#include "Screenshot.h"

#include "_DxLib.h"
#include "ConfigData.h"
#include <direct.h>

#include <shlwapi.h>
#pragma comment( lib , "shlwapi.lib" )

extern ConfigData cConfigData;

static void TakeSS(const char str[]) {
	extern int WINDOWS_H, WINDOWS_V;
	
	switch (cConfigData.SSType) {	//SSTypeに応じてSS用関数を変更
		case eSSType_BMP: SaveDrawScreenToBMP (0, 0, WINDOWS_H, WINDOWS_V, str);		 break;
		case eSSType_PNG: SaveDrawScreenToPNG (0, 0, WINDOWS_H, WINDOWS_V, str, 9);	 break;
		case eSSType_JPG: SaveDrawScreenToJPEG(0, 0, WINDOWS_H, WINDOWS_V, str, 100); break;
	}
}

void Screenshot() {
	extern int WINDOWS_H, WINDOWS_V;
	
	char cdir[256];	//カレントディレクトリ収納バッファ
	if (GetCurrentDirectory(sizeof(cdir), cdir) == 0) return; //カレントディレクトリ取得＆取得失敗時脱出

	std::string cdirstr = cdir; //stringに収納
	cdirstr += "/Screenshot";	//SS用ディレクトリの文字列を追記
	int errchkmkd = _mkdir(cdirstr.c_str()); //ディレクトリ作成
	
	DATEDATA DateStr;
	GetDateTime(&DateStr);	//現在時刻を取得する
	char str[49]; // = "Screenshot/Screenshot-0000-00-00_00.00.00-00.xxx";
	snprintf(str, sizeof(str) - 6U,	//6Uはファイル重複分3U+拡張子分3U
		"Screenshot/Screenshot-%04d-%02d-%02d_%02d.%02d.%02d.",
		DateStr.Year, DateStr.Mon, DateStr.Day, DateStr.Hour, DateStr.Min, DateStr.Sec);
	std::string SSdir = str;
	switch (cConfigData.SSType) {	//SSTypeに応じて拡張子を変更
		case eSSType_BMP: SSdir += "bmp"; break;
		case eSSType_PNG: SSdir += "png"; break;
		case eSSType_JPG: SSdir += "jpg"; break;
	}
	if (PathFileExists(SSdir.c_str()) != 0) {	//同じファイル名が存在する場合末尾に数字追加
		for (int i = 1; i < 60; ++i) {
			snprintf(str, sizeof(str) - 3U,	//3Uは拡張子分
				"Screenshot/Screenshot-%04d-%02d-%02d_%02d.%02d.%02d-%d.",
				DateStr.Year, DateStr.Mon, DateStr.Day, DateStr.Hour, DateStr.Min, DateStr.Sec, i);
			SSdir = str;
			switch (cConfigData.SSType) {	//SSTypeに応じて拡張子を変更
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