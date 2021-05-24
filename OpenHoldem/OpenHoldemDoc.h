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

#ifndef INC_OPENHOLDEMDOC_H
#define INC_OPENHOLDEMDOC_H

// OpenHoldemDoc.h : interface of the COpenHoldemDoc class
//
class COpenHoldemDoc : public CDocument 
{
protected:
	COpenHoldemDoc();
	DECLARE_DYNCREATE(COpenHoldemDoc)
	DECLARE_MESSAGE_MAP()

public:
	static COpenHoldemDoc* GetDocument();
    BOOL SaveModified();
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual ~COpenHoldemDoc();

	BOOL init_load = TRUE;
};

#endif /* INC_OPENHOLDEMDOC_H */