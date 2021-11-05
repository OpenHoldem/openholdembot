#pragma once


// CWaitDialog dialog

class CWaitDialog : public CDialog
{
	DECLARE_DYNAMIC(CWaitDialog)

public:
	CWaitDialog(BOOL*, LPCTSTR dialogTitle = NULL, LPCTSTR dialogText = NULL);   // standard constructor
	virtual ~CWaitDialog();
	virtual void OnCancel();
	BOOL Pump();
	void SetPercentComplete(int);
	void SetPercentCompleteAndPump(int);
	void SetMessageText(LPCTSTR);
	void Close(); 
	bool Cancel() const;    // did user press cancel button?


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WAITDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL* m_pFlag;
	DECLARE_MESSAGE_MAP()
};
