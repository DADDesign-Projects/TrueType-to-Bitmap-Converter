
// TTFont2BitmapDlg.h : fichier d'en-tête
//

#pragma once
#include "CBitmapFont.h"
#include <vector>



// boîte de dialogue de CTTFont2BitmapDlg
class CTTFont2BitmapDlg : public CDialogEx
{
// Construction
public:
	CTTFont2BitmapDlg(CWnd* pParent = nullptr);	// constructeur standard
	~CTTFont2BitmapDlg();

// Données de boîte de dialogue
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TTFONT2BITMAP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Prise en charge de DDX/DDV


// Implémentation
protected:
	HICON		m_hIcon;
	LOGFONT		m_lgFont;
	cBitmapFont m_BitmapFont;
	HANDLE		m_hFontFile;
	BYTE*		m_pFontBuffer;

	// Fonctions générées de la table des messages
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadSystemFont();
	afx_msg void OnEnChangeSampleText();
	afx_msg void OnBnClickedLoadFileFont();
	afx_msg void OnBnClickedExportC();
	afx_msg void OnBnClickedExportBin();
};
