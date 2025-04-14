//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

#include "pch.h"

#include "CLevDistance.h"

//#include <string.h>
//#include <malloc.h>

//****************************
// Get minimum of three values
//****************************

int CLevDistance::Minimum(int a, int b, int c)
{
	int mi = a;

	if (b < mi) 
	{
		mi = b;
	}
	if (c < mi) 
	{
		mi = c;
	}
	return mi;
}

//**************************************************
// Get a pointer to the specified cell of the matrix
//**************************************************

int *CLevDistance::GetCellPointer(int *pOrigin, int col, int row, int nCols)
{
	return pOrigin + col + (row * (nCols + 1));
}

//*****************************************************
// Get the contents of the specified cell in the matrix
//*****************************************************

int CLevDistance::GetAt(int *pOrigin, int col, int row, int nCols)
{
	int *pCell;

	pCell = GetCellPointer(pOrigin, col, row, nCols);
	return *pCell;
}

//*******************************************************
// Fill the specified cell in the matrix with the value x
//*******************************************************

void CLevDistance::PutAt(int *pOrigin, int col, int row, int nCols, int x)
{
	int *pCell;

	pCell = GetCellPointer(pOrigin, col, row, nCols);
	*pCell = x;
}

//*****************************
// Compute Levenshtein distance
//*****************************

int CLevDistance::LD(char const *s, char const *t)
{
	int *d = NULL; // pointer to matrix
	int n = 0; // length of s
	int m = 0; // length of t
	int i = 0; // iterates through s
	int j = 0; // iterates through t
	char s_i = '\0'; // ith character of s
	char t_j = '\0'; // jth character of t
	int cost = 0; // cost
	int result = 0; // result
	int cell = 0; // contents of target cell
	int above = 0; // contents of cell immediately above
	int left = 0; // contents of cell immediately to left
	int diag = 0; // contents of cell immediately above and to left
	int sz = 0; // number of cells in matrix

	// Step 1

	n = strlen (s);
	m = strlen (t);
	if (n == 0) 
	{
		return m;
	}
	if (m == 0) 
	{
		return n;
	}
	sz = (n+1) * (m+1) * sizeof (int);
	d = (int *) malloc (sz);

	// Step 2

	for (int i = 0; i <= n; i++) 
	{
		PutAt (d, i, 0, n, i);
	}

	for (int j = 0; j <= m; j++) 
	{
		PutAt (d, 0, j, n, j);
	}

	// Step 3

	for (int i = 1; i <= n; i++) 
	{

		s_i = s[i-1];

		// Step 4

		for (int j = 1; j <= m; j++) 
		{

			t_j = t[j-1];

			// Step 5

			if (s_i == t_j) 
			{
				cost = 0;
			}
			else 
			{
				cost = 1;
			}

			// Step 6

			above = GetAt (d,i-1,j, n);
			left = GetAt (d,i, j-1, n);
			diag = GetAt (d, i-1,j-1, n);
			cell = Minimum (above + 1, left + 1, diag + cost);
			PutAt (d, i, j, n, cell);
		}
	}

	// Step 7

	result = GetAt (d, n, m, n);
	free (d);
	return result;
}
