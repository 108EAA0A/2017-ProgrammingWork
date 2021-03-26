#pragma once

#include <map>

class Table {
public:
	///////////////////////////
	// Variant
	/////
	struct Variant {
		int iVal;
		float fVal;
		std::string str;
		Variant() : iVal(), fVal() {}
		Variant(const std::string &s) : iVal(atoi(s.c_str())), fVal((float)atof(s.c_str())), str(s) {}
		operator std::string() { return str; }
		operator int() { return iVal; }
		operator float() { return fVal; }
	};


	///////////////////////////
	// Record
	/////
	class Record {
	private:
		//static Variant nullVariant;
		std::map<std::string, Variant*> vals;

	public:
		Record() {}
		~Record();
		Table::Variant& operator[](const char *const key);
		void add(const char *const key, Variant &val);
	};


	////////////////////////////
	// Table
	/////
private:
	Table(const Table &src);
	Table& operator=(const Table &src) {}

protected:
	//static Record nullRecord;
	std::map<std::string, Record*> records;

public:
	Table() {}
	Table(const char *const filePath);
	virtual ~Table();

	void MakeTable(const char *const filePath);

	// ƒŒƒR[ƒhæ“¾
	Record& operator[](const char *const key);
};
