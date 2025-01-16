
// TTFont2BitmapDlg.cpp : fichier d'implémentation
//

#include "pch.h"
#include "framework.h"
#include "TTFont2Bitmap.h"
#include "TTFont2BitmapDlg.h"
#include "afxdialogex.h"
#include "TTFStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// boîte de dialogue CAboutDlg utilisée pour la boîte de dialogue 'À propos de' pour votre application

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Données de boîte de dialogue
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Prise en charge de DDX/DDV

// Implémentation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// boîte de dialogue de CTTFont2BitmapDlg

CTTFont2BitmapDlg::CTTFont2BitmapDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TTFONT2BITMAP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_pFontBuffer = nullptr;
}
CTTFont2BitmapDlg::~CTTFont2BitmapDlg() {
	if (m_pFontBuffer != nullptr) {
		delete[] m_pFontBuffer;
		m_pFontBuffer = nullptr;
	}
}
void CTTFont2BitmapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTTFont2BitmapDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD_SYSTEM_FONT, &CTTFont2BitmapDlg::OnBnClickedLoadSystemFont)
	ON_EN_CHANGE(IDC_SAMPLE_TEXT, &CTTFont2BitmapDlg::OnEnChangeSampleText)
	ON_BN_CLICKED(IDC_LOAD_FILE_FONT, &CTTFont2BitmapDlg::OnBnClickedLoadFileFont)
	ON_BN_CLICKED(IDC_EXPORT_C, &CTTFont2BitmapDlg::OnBnClickedExportC)
	ON_BN_CLICKED(IDC_EXPORT_BIN, &CTTFont2BitmapDlg::OnBnClickedExportBin)
END_MESSAGE_MAP()


// gestionnaires de messages de CTTFont2BitmapDlg

BOOL CTTFont2BitmapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Ajouter l'élément de menu "À propos de..." au menu Système.

	// IDM_ABOUTBOX doit se trouver dans la plage des commandes système.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Définir l'icône de cette boîte de dialogue.  L'infrastructure effectue cela automatiquement
	//  lorsque la fenêtre principale de l'application n'est pas une boîte de dialogue
	SetIcon(m_hIcon, TRUE);			// Définir une grande icône
	SetIcon(m_hIcon, FALSE);		// Définir une petite icône

	// TODO: ajoutez ici une initialisation supplémentaire
	m_lgFont = { 0 };										// Initialise tout à zéro
	m_lgFont.lfHeight = -16;								// Taille de la police (négatif pour taille en pixels)
	m_lgFont.lfWidth = 0;									// Largeur automatique
	m_lgFont.lfWeight = FW_NORMAL;							// gras
	m_lgFont.lfItalic = FALSE;								// Italique
	m_lgFont.lfUnderline = FALSE;							// Pas de soulignement
	m_lgFont.lfStrikeOut = FALSE;							// Pas de texte barré
	m_lgFont.lfCharSet = DEFAULT_CHARSET;					// Jeu de caractères par défaut
	m_lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_lgFont.lfQuality = DEFAULT_QUALITY;
	m_lgFont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;	// Style de la police
	wcscpy_s(m_lgFont.lfFaceName, L"Arial");				// Nom de la police;

	SetDlgItemTextW(IDC_FONT_NAME, m_lgFont.lfFaceName);
	SetDlgItemInt(IDC_FONT_SIZE, 12, true);
	SetDlgItemTextW(IDC_SAMPLE_TEXT, L"The quick brown fox jumps over the lazy dog.");
	CheckDlgButton(IDC_BOLD, BST_UNCHECKED);
	CheckDlgButton(IDC_UNDERLINE, BST_UNCHECKED);
	CheckDlgButton(IDC_ITALIC, BST_UNCHECKED);
	CheckDlgButton(IDC_STRIKE_OUT, BST_UNCHECKED);
	SetDlgItemInt(IDC_FIRST_CHAR, 32, true);
	SetDlgItemInt(IDC_LAST_CHAR, 126, true);

	m_BitmapFont.CreateBitmapFont(m_lgFont);

	return TRUE;  // retourne TRUE, sauf si vous avez défini le focus sur un contrôle
}

void CTTFont2BitmapDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// Si vous ajoutez un bouton Réduire à votre boîte de dialogue, vous devez utiliser le code ci-dessous
//  pour dessiner l'icône.  Pour les applications MFC utilisant le modèle Document/Vue,
//  cela est fait automatiquement par l'infrastructure.

void CTTFont2BitmapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // contexte de périphérique pour la peinture

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrer l'icône dans le rectangle client
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Dessiner l'icône
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialogEx::OnPaint();

		CWnd* pStatic = GetDlgItem(IDC_RENDELER);
		CDC* pDC = pStatic->GetDC();
		CRect rect;
		pStatic->GetClientRect(&rect);
		pDC->FillSolidRect(&rect, ::GetSysColor(COLOR_3DFACE)); // Efface le fond

		CFont Font;
		Font.CreateFontIndirect(&m_lgFont);
		CFont* pOldFont = pDC->SelectObject(&Font);

		CString SampleText;
		GetDlgItemTextW(IDC_SAMPLE_TEXT, SampleText);
		pDC->DrawText(SampleText, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);


		pDC->SelectObject(pOldFont);
		pStatic->ReleaseDC(pDC);
	}
}

// Le système appelle cette fonction pour obtenir le curseur à afficher lorsque l'utilisateur fait glisser
//  la fenêtre réduite.
HCURSOR CTTFont2BitmapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CTTFont2BitmapDlg::OnBnClickedLoadSystemFont()
{
	CFontDialog dlg(&m_lgFont, CF_TTONLY);					// Opens a font selection dialog for TrueType fonts only
	if (dlg.DoModal() == IDOK) {							// Checks if the user confirmed the selection
		dlg.GetCurrentFont(&m_lgFont);					// Retrieves the selected font and updates 'm_lgFont'
		SetDlgItemText(IDC_FONT_NAME, dlg.GetFaceName());	// Updates the text field with the font name
		SetDlgItemInt(IDC_FONT_SIZE, dlg.GetSize() / 10); // Updates the font size (converted to points)

		// Updates checkboxes using compact ternary syntax
		CheckDlgButton(IDC_BOLD, dlg.IsBold() ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_ITALIC, dlg.IsItalic() ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_UNDERLINE, dlg.IsUnderline() ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_STRIKE_OUT, dlg.IsStrikeOut() ? BST_CHECKED : BST_UNCHECKED);

		m_BitmapFont.CreateBitmapFont(m_lgFont);  // Create a bitmap font for rendering
		InvalidateRect(NULL, FALSE); // Redraws the window to reflect changes
	}
}


void CTTFont2BitmapDlg::OnEnChangeSampleText()
{
	// TODO:  S'il s'agit d'un contrôle RICHEDIT, le contrôle ne
	// envoyez cette notification sauf si vous substituez CDialogEx::OnInitDialog()
	// fonction et appelle CRichEditCtrl().SetEventMask()
	// avec l'indicateur ENM_CHANGE ajouté au masque grâce à l'opérateur OR.

	// TODO:  Ajoutez ici le code de votre gestionnaire de notification de contrôle
	InvalidateRect(NULL, FALSE);
}

bool is16BitEncoding(uint16_t platformID, uint16_t encodingID) {
	if (platformID == 0) {
		// Unicode platform: Always 16-bit
		return true;
	}
	if (platformID == 3 && (encodingID == 1 || encodingID == 10)) {
		// Microsoft platform with Unicode encoding
		return true;
	}
	// Other platforms are typically 8-bit
	return false;
}

CStringW ConvertUTF16BEToCStringW(const BYTE* buffer, size_t length) {
	CStringW result;
	for (size_t i = 0; i < length; i += 2) {
		wchar_t wchar = (buffer[i] << 8) | buffer[i + 1];
		result.AppendChar(wchar);
	}
	return result;
}


void CTTFont2BitmapDlg::OnBnClickedLoadFileFont()
{
	// Triggered when the "Load File Font" button is clicked

	// Open a file dialog to select a TrueType font file
	CFileDialog dlg(TRUE, L"ttf", L"*.ttf", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, L"TrueType Font Files (*.ttf)||");

	if (dlg.DoModal() == IDOK) { // Check if the user selected a file
		RemoveFontResourceEx(dlg.GetPathName(), FR_PRIVATE, NULL);
		CFile file;
		if (file.Open(dlg.GetPathName(), CFile::modeRead)) {	// Open the selected file
			DWORD dwSize = file.GetLength();										// Get the file size
			BYTE* pFontBuffer = new BYTE[dwSize];									// Allocate buffer to read the font file
			file.Read(pFontBuffer, dwSize);								// Read the file into the buffer
			file.Close();															// Close the file

			// Look for the font name in the file
			TTFHeader* pHeader = (TTFHeader*)pFontBuffer; // Cast buffer to TTFHeader
			TableDirectoryEntry* pDirectoryEntry = (TableDirectoryEntry*)(pHeader + 1); // Get the table directory entry

			CString FontName;
			bool bNameFontFound = false;

			// Search for the "name" table
			for (int Index = 0; Index < ntohl(pHeader->numTables); Index++) {
				if ((pDirectoryEntry->tag1 == 'n') && (pDirectoryEntry->tag2 == 'a') &&
					(pDirectoryEntry->tag3 == 'm') && (pDirectoryEntry->tag4 == 'e')) {
					// "name" table found
					NameTableHeader* pNameTableHeader = (NameTableHeader*)&pFontBuffer[ntohl(pDirectoryEntry->offset)];
					NameRecord* pNameRecords = (NameRecord*)(pNameTableHeader + 1);
					BYTE* stringStorage = ((BYTE*)pNameTableHeader) + ntohs(pNameTableHeader->stringOffset);

					// Search for the font's name in the records
					for (int i = 0; i < ntohs(pNameTableHeader->count); i++) {
						if (ntohs(pNameRecords[i].nameID) == 1) {					        // Look for nameID 1 (font name)
							uint16_t length = ntohs(pNameRecords[i].length);
							uint16_t offset = ntohs(pNameRecords[i].offset);
							if (is16BitEncoding(ntohs(pNameRecords[i].platformID), ntohs(pNameRecords[i].encodingID))) {
								FontName = ConvertUTF16BEToCStringW(stringStorage + offset, length);
							}
							else {
								CStringA fontName(reinterpret_cast<const char*>(stringStorage + offset), length);
								FontName = CString(fontName);  // Conversion CStringA vers CString
							}
							bNameFontFound = true;		// Mark as found
							break;
						}
					}
					break; // Exit the loop once the "name" table is processed
				}
				pDirectoryEntry++; // Move to the next table
			}
			delete[] pFontBuffer; // Free the font buffer

			if (bNameFontFound) {
				// Configure LOGFONT structure for the font
				LOGFONT lgFont = { 0 };
				lgFont.lfHeight = -16;										// Font size in pixels (negative for exact height)
				lgFont.lfWidth = 0;											// Automatic width
				lgFont.lfWeight = 0;										// Normal weight
				lgFont.lfItalic = FALSE;									// No italics
				lgFont.lfUnderline = FALSE;									// No underline
				lgFont.lfStrikeOut = FALSE;									// No strikethrough
				lgFont.lfCharSet = DEFAULT_CHARSET;							// Default character set
				lgFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
				lgFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
				lgFont.lfQuality = DEFAULT_QUALITY;
				lgFont.lfPitchAndFamily = DEFAULT_PITCH;					// Default pitch and family
				wcscpy_s(lgFont.lfFaceName, FontName);	// Set the font name

				// Add the font resource for private use
				AddFontResourceEx(dlg.GetPathName(), FR_PRIVATE, NULL);
				CFontDialog FontDlg(&lgFont, CF_TTONLY);					// Font selection dialog
				if (FontDlg.DoModal() == IDOK) {							// If the font is selected
					FontDlg.GetCurrentFont(&m_lgFont);					// Get the selected font attributes
					SetDlgItemText(IDC_FONT_NAME, FontDlg.GetFaceName());	// Display font name
					SetDlgItemInt(IDC_FONT_SIZE, FontDlg.GetSize() / 10); // Display font size

					// Update font style checkboxes
					CheckDlgButton(IDC_BOLD, FontDlg.IsBold() ? BST_CHECKED : BST_UNCHECKED);
					CheckDlgButton(IDC_ITALIC, FontDlg.IsItalic() ? BST_CHECKED : BST_UNCHECKED);
					CheckDlgButton(IDC_UNDERLINE, FontDlg.IsUnderline() ? BST_CHECKED : BST_UNCHECKED);
					CheckDlgButton(IDC_STRIKE_OUT, FontDlg.IsStrikeOut() ? BST_CHECKED : BST_UNCHECKED);

					
					m_BitmapFont.CreateBitmapFont(m_lgFont);  // Create a bitmap font for rendering
					InvalidateRect(NULL, FALSE); // Refresh the dialog
				}
				else {
					// Remove the font resource if not selected
					RemoveFontResourceEx(dlg.GetPathName(), FR_PRIVATE, NULL);
				}
			}
		}
	}
}

void CTTFont2BitmapDlg::OnBnClickedExportC()
{
	// TODO: ajoutez ici le code de votre gestionnaire de notification de contrôle
	CString FontName;
	GetDlgItemText(IDC_FONT_NAME, FontName);
	int FontSize = GetDlgItemInt(IDC_FONT_SIZE);
	FontName.Format(L"%s_%dp", FontName.GetString(), FontSize);
	m_BitmapFont.ExportToC(FontName,GetDlgItemInt(IDC_FIRST_CHAR), GetDlgItemInt(IDC_LAST_CHAR));
}


void CTTFont2BitmapDlg::OnBnClickedExportBin()
{
	// TODO: ajoutez ici le code de votre gestionnaire de notification de contrôle
	CString FontName;
	GetDlgItemText(IDC_FONT_NAME, FontName);
	int FontSize = GetDlgItemInt(IDC_FONT_SIZE);
	FontName.Format(L"%s_%dp", FontName.GetString(), FontSize);
	m_BitmapFont.ExportToBinary(FontName, GetDlgItemInt(IDC_FIRST_CHAR), GetDlgItemInt(IDC_LAST_CHAR));
}
