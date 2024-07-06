#pragma once


// OptionsDlg dialog

class OptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(OptionsDlg)

public:
	OptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~OptionsDlg();

// Dialog Data
	enum { IDD = IDD_OPTIONSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// move popup menu outside right edge
	BOOL m_unobstructivePopup;
};
