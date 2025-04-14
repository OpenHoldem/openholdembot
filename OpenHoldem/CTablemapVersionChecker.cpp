#include "pch.h"
#include "CTablemapVersionChecker.h"

CTablemapVersionChecker::CTablemapVersionChecker()
{
	ResetErrorFlag();
}

void CTablemapVersionChecker::ResetErrorFlag()
{
	_error_flag = false;
}

bool CTablemapVersionChecker::IsOutDatedMap()
{
	return _error_flag;
}

const int k_number_of_outdated_strings = 5;
const CString outdated_strings[k_number_of_outdated_strings][2] =
{
	{".wsdb1", "Winscrape \"profiles\" no longer supported."},
	{".ohdb1", ""},
	{".ohdb2", ""},
	{".osdb1", ""},
	{".osdb2", ""}
};

bool CTablemapVersionChecker::IsOutdatedString(CString s)
{
	for (int i=0; i<k_number_of_outdated_strings; i++)
	{
		int length = outdated_strings[i][0].GetLength();
		if (s.Left(length) == outdated_strings[i][0])
		{
			_error_flag = true;
			return true;
		}
	}
	return false;
}