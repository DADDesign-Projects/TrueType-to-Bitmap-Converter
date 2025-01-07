
// TTFont2Bitmap.h : fichier d'en-tête principal de l'application PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "incluez 'pch.h' avant d'inclure ce fichier pour PCH"
#endif

#include "resource.h"		// symboles principaux


// CTTFont2BitmapApp :
// Consultez TTFont2Bitmap.cpp pour l'implémentation de cette classe
//

class CTTFont2BitmapApp : public CWinApp
{
public:
	CTTFont2BitmapApp();

// Substitutions
public:
	virtual BOOL InitInstance();

// Implémentation

	DECLARE_MESSAGE_MAP()
};

extern CTTFont2BitmapApp theApp;
