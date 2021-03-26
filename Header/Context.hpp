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

	// パラメータを取得してポインタを先に進める
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

	// リセット
	void reset() {
		pos = 0;
	}
};
