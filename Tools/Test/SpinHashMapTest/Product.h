#pragma once
class Product
{
public:
	Product();
	Product(const char* apData, int nSize);
	Product(int nData);
	~Product(void);

	int GetSize();
	char* GetData();
	int GetNumData();

public:
	int m_nSize;
	int m_nData;
	char* m_pData;
};

