#pragma once

template<int N>

class Context {
public:
	char str[N];

private:
	int size;
	int pos;

public:
	Context() : size(N), pos() {}

	// �p�����[�^���擾���ă|�C���^���ɐi�߂�
	std::string getAndNext() {
		char c[N] = {};
		int p = 0;
		for (int i = pos; i < size; ++i, ++p, ++pos) {
			if (str[i] == ',' || str[i] == '\0') {
				++pos;
				return c;
			}
			c[p] = str[i];
		}
		return c;
	}

	// ���Z�b�g
	void reset() {
		pos = 0;
	}
};
