#pragma once
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <list>

using namespace std;

#define FPSCOUNTER_QUERYPER_COUNTER        1
#define FPSCOUNTER_TIMEGETTIME             0

class CFPSCounter {
private:
	bool m_iCounterFlag : 1;           // �g�p����v���֐��̔���t���O
	LARGE_INTEGER m_Counter;           // �N���b�N�J�E���g��
	double m_dFreq;                    // 1�b������N���b�N�J�E���g���i���g���j
	LONGLONG m_OldLongCount;           // �ȑO�̃N���b�N�J�E���g��
	DWORD m_dwTGTOldCount;             // �ȑO�̎����i�~���b�j
	list<double> m_dwDefTimeLst;       // ���ԃ��X�g
	UINT m_uiNum;                      // �ړ����όv�Z���̃f�[�^��
	double m_dwSumTimes;               // ���ʕ����̍��v�l

public:
	CFPSCounter(const unsigned int smp = 10);

	virtual ~CFPSCounter();

	// FPS�l���擾
	double GetFPS();

	// �T���v������ύX
	void SetSampleNum(const unsigned int smp);

protected:
	// ���݂̎������擾
	double GetCurDefTime();

	// FPS���X�V
	double UpdateFPS(const double Def);
};