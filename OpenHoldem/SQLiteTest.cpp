//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:		Defines the entry point for the SQLite test console application
//
//******************************************************************************


#include "StdAfx.h"
#include "SQLiteWrapper.h"
#include <iostream>

using namespace std;



int main(int argc, _TCHAR* argv[])
{	
	
	SQLiteDB *pSQLite;
	pSQLite=new SQLiteDB();

	//Connect TO database
	if(!pSQLite->OpenConnection("SQLiteTest.db","D:\\"))
	{
		cout<<"\nConnecting To DB Failed :"<<pSQLite->GetLastError().c_str();
		return -1;
	}

	//Create DB
	int effected=pSQLite->Execute("create table test (seq int(2),name varchar(50));");

	if(effected>0)
		cout<<"\nColoumn Effected :"<<effected;

	else
		cout<<"\nError in Query : "<<pSQLite->GetLastError().c_str();

	//Insert Into DB
	effected=pSQLite->Execute("insert into test (seq,name) values (1,'SQLLiteDBtest');");
	if(effected>0)
		cout<<"\nColoumn Effected :"<<effected;
	else
		cout<<"\nError in Query : "<<pSQLite->GetLastError().c_str();


	//Get Data From DB
	IResult *res=pSQLite->ExecuteSelect("Select * from test;");
	if(!res)
	  cout<<"\n Error:"<<pSQLite->GetLastError().c_str();
	
	else
	{
		//Get Column Count
		int i = res->GetColumnCount();
		
		//Print Colomn Name
		for(int k=0;k<i;k++)
		{
			printf("%s\t",res->NextColomnName(k));	
		}
		
		cout<<endl;

		//Print Result
		while(res->Next())
		{
			for(int k=0;k<i;k++)
				printf("%s\t",res->ColomnData(k));		
			cout<<endl;
		}

		//release Result Data
		res->Release();
	}


	//Insert Heavey Data in DB
	pSQLite->BeginTransaction();
	
	for(int i=0;i<10000;i++)
		pSQLite->Execute("insert into test (seq,name) values (1,'SQLLiteDBtest');");

	pSQLite->CommitTransaction();

	delete pSQLite;

	cin.get();
	return 0;
}

