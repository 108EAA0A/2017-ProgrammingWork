#pragma once

#define BASE_RENDER_SIZE_W 1920
#define BASE_RENDER_SIZE_H 1080

// ゲームメインの描画処理を初期化する
void GameMainRender_Initialize();

// ゲームメインの描画処理の後始末をする
void GameMainRender_Terminate();

// ゲームメインの描画処理の事前準備をする
void GameMainRender_FrontProcess();

// ゲームメインの描画処理の事後処理をする
void GameMainRender_RearProcess();