//====================================================================================
// BitmapFont.h
//  Management of bitmap fonts converted from TrueType fonts for use in embedded systems
//  such as Arduino and others. 
//  Features:
//  - Supports two export formats:
//    - C header file (.h) for inclusion in application code
//    - Binary file for loading into application flash memory
//
// Copyright (c) 2025 Dad Design.
//====================================================================================

#include "pch.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "CBitmapFont.h"

//***********************************************************************************
// class cBitmapFont
//***********************************************************************************

// --------------------------------------------------------------------------
// Constructor for cBitmapFont.
// Initializes member variables.
cBitmapFont::cBitmapFont() {
	for (int i = 0; i < 256; ++i) {
		m_TabBuffer[i] = nullptr;
	}
	m_FontCreated = false;
}

// --------------------------------------------------------------------------
// Creates a bitmap font based on the provided LOGFONT structure.
// @param lgFont The LOGFONT structure containing font parameters.
// @return True if the font creation is successful, otherwise false.
bool cBitmapFont::CreateBitmapFont(LOGFONT lgFont) {
	DeleteBitmapFont();
	HDC hdc = GetDC(NULL);
	HFONT hFont = CreateFontIndirect(&lgFont);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

	GLYPHMETRICS gm;
	MAT2 mat2 = {
		{0, 1}, {0, 0},
		{0, 0}, {0, 1}
	};

	for (int i = 1; i < 256; ++i) {
		memset(&gm, 0, sizeof(GLYPHMETRICS));
		DWORD bufferSize = GetGlyphOutline(
			hdc,
			i,
			GGO_BITMAP,
			&gm,
			0,
			nullptr,
			&mat2
		);
		if (GDI_ERROR == bufferSize) {
			DeleteBitmapFont();
			return false;
		}
		if (bufferSize != 0) {
			BYTE* buffer = new BYTE[bufferSize];
			if (buffer == nullptr) {
				return false;
			}
			DWORD result = GetGlyphOutline(
				hdc,
				i,
				GGO_BITMAP,
				&gm,
				bufferSize,
				buffer,
				&mat2
			);
			if (GDI_ERROR == result) {
				DeleteBitmapFont();
				return false;
			}
			m_TabGM[i] = gm;
			m_TabBuffer[i] = buffer;
		}
		else {
			m_TabGM[i] = gm;
			m_TabBuffer[i] = nullptr;
		}

		uint16_t WidthCharacter = gm.gmBlackBoxX + gm.gmptGlyphOrigin.x;
		int16_t UnderBaseLine = gm.gmBlackBoxY - gm.gmptGlyphOrigin.y;
		if (UnderBaseLine < 0) {
			UnderBaseLine = 0;
		}
		int16_t OverBaseLine = gm.gmBlackBoxY;
		
		if (OverBaseLine > m_MaxOverBaseLine) {
			m_MaxOverBaseLine = OverBaseLine;
		}
		if (UnderBaseLine > m_MaxUnderBaseLine) {
			m_MaxUnderBaseLine = UnderBaseLine;
		}
		if (WidthCharacter > m_MaxWidthChar) {
			m_MaxWidthChar = WidthCharacter;
		}

	}
	m_FontCreated = true;

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hdc);
	return true;
}

// --------------------------------------------------------------------------
// Deletes the bitmap font and releases allocated memory.
void cBitmapFont::DeleteBitmapFont() {
	for (int i = 1; i < 256; ++i) {
		if (m_TabBuffer[i] != nullptr) {
			delete[] m_TabBuffer[i];
			m_TabBuffer[i] = nullptr;
		}
	}
	m_FontCreated = false;
	m_MaxWidthChar = 0;
	m_MaxOverBaseLine = 0;
	m_MaxUnderBaseLine = 0;
}

// --------------------------------------------------------------------------
// Draws a character on the given device context. (no tested)
// @param pDC Pointer to the device context.
// @param Character The character to draw.
// @param x The x-coordinate for drawing.
// @param y The y-coordinate for drawing.
// @param pixelSize The size of the pixel for rendering.
// @return The horizontal position after drawing the character.
int cBitmapFont::DrawCharacter(CDC* pDC, char Character, int x0, int y0, int pixelSize) {
	const GLYPHMETRICS* pGM;
	const BYTE* pBuffer;
	GetMetrics(Character, pGM, pBuffer);
	if(pBuffer != nullptr){
		int rowSize = ((pGM->gmBlackBoxX + 31) / 32) * 4; // Alignement des lignes sur 4 octets
		int yOffset = m_MaxOverBaseLine - pGM->gmptGlyphOrigin.y;
		for (int y = 0; y < pGM->gmBlackBoxY; ++y) {
			for (int x = 0; x < pGM->gmBlackBoxX; ++x) {
				int byteIndex = y * rowSize + x / 8;
				int bitIndex = 7 - (x % 8);
				bool isSet = pBuffer[byteIndex] & (1 << bitIndex);
				if (isSet) {
					// Dessiner un "gros pixel" noir
					pDC->FillSolidRect(
						(x0 + pGM->gmptGlyphOrigin.x + x) * pixelSize,
						(yOffset + y) * pixelSize,
						pixelSize,
						pixelSize,
						RGB(0, 0, 0)
					);
				}
			}
		}
	} 
	return pGM->gmCellIncX;
}

// --------------------------------------------------------------------------
// Exports the font data to a C-compatible format.
// @param FontName Name of the font to export.
// @param FirstChar First character in the ASCII range.
// @param LastChar Last character in the ASCII range.
// @return True if the export is successful, otherwise false.
bool cBitmapFont::ExportToC(const CString& FontNameIn, uint16_t FirstChar, uint16_t LastChar) {
	// Create a local copy of the font name and adjust it for file-safe naming
	CString FontName = FontNameIn;
	CString FileName;

	// Replace spaces in the font name with underscores to create a valid identifier
	FontName.Replace(' ', '_');

	// Format the default file name for the header file
	FileName.Format(L"%s.h", FontName.GetString());

	// Open a file save dialog for the user to specify the export location
	CFileDialog dlg(FALSE, L"h", FileName, OFN_HIDEREADONLY, L"Header C Files (*.h)||");
	if (dlg.DoModal()) {

		CString filePath = dlg.GetPathName();
		// Check if the file already exists
		if (PathFileExists(filePath)) {
			// Ask for confirmation before overwriting
			if (AfxMessageBox(L"The file already exists. Do you want to overwrite it?", MB_YESNO | MB_ICONQUESTION) == IDNO) {
				return false; // Cancel operation if the user selects "No"
			}
		}

		// Open the file stream to write the exported font data
		std::ofstream file(dlg.GetPathName());
		if (!file.is_open()) {
			return false;
		}

		// Write the header and metadata for the font
		file << "#include <stdint.h>\n\n";
		file << "// Font Bitmap \n";

		// Initialize storage for glyph metrics and track bitmap offsets
		std::vector<DadGFX::GFXglyph> glyphMetrics;
		uint16_t currentBitmapOffset = 0;

		// Begin writing the bitmap data array
		file << "const uint8_t __" << CT2A(FontName) << "_Bitmap[] = {\n";

		// Loop through each character in the specified range
		for (uint16_t Character = FirstChar; Character <= LastChar; Character++) {
			const GLYPHMETRICS* pGM; // Pointer to glyph metrics
			const BYTE* pBuffer;     // Pointer to bitmap buffer
			uint8_t ValOctet = 0;    // Accumulator for byte assembly
			uint8_t CtBits = 0;      // Bit counter for each byte

			// Retrieve metrics and buffer for the character
			GetMetrics(Character, pGM, pBuffer);

			// Populate the glyph metrics structure
			DadGFX::GFXglyph glyph = {};
			glyph.bitmapOffset = currentBitmapOffset;
			glyph.width = pGM->gmBlackBoxX;
			glyph.height = pGM->gmBlackBoxY;
			glyph.xAdvance = pGM->gmCellIncX;
			glyph.xOffset = pGM->gmptGlyphOrigin.x;
			glyph.yOffset = m_MaxOverBaseLine - pGM->gmptGlyphOrigin.y;
			glyphMetrics.push_back(glyph);

			if (pBuffer != nullptr) {
				// Calculate row size with 4-byte alignment
				int rowSize = ((pGM->gmBlackBoxX + 31) / 32) * 4;

				// Process the bitmap data pixel by pixel
				for (int y = 0; y < pGM->gmBlackBoxY; ++y) {
					for (int x = 0; x < pGM->gmBlackBoxX; ++x) {
						int byteIndex = y * rowSize + x / 8;  // Locate byte in buffer
						int bitIndex = 7 - (x % 8);          // Locate bit within byte
						bool isSet = pBuffer[byteIndex] & (1 << bitIndex);

						// Accumulate bits into bytes
						ValOctet = (ValOctet << 1) | (isSet ? 1 : 0);
						CtBits++;

						// Write completed byte to the file
						if (CtBits == 8) {
							file << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
								<< static_cast<int>(ValOctet) << ",";
							CtBits = 0;
							ValOctet = 0;
							currentBitmapOffset++;
						}
					}
				}

				// Write remaining bits in the final byte
				if (CtBits > 0) {
					ValOctet <<= (8 - CtBits); // Pad with zeros
					file << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase
						<< static_cast<int>(ValOctet) << ",";
					currentBitmapOffset++;
				}

				file << "\n"; // New line after each character
			}
			else {
				// Write a placeholder if no bitmap data is available
				file << "0x00," << "\n";
				currentBitmapOffset++;
			}
		}

		// End the bitmap data array
		file << "0x0D,0x0A,0x0D\n};\n\n";

		// Begin writing the glyph metrics array
		file << "// Glyph Metrics\n";
		file << "const DadGFX::GFXglyph __" << CT2A(FontName) << "_Glyphs[] = {\n";
		for (const auto& glyph : glyphMetrics) {
			file << std::dec << "    {" << glyph.bitmapOffset << ", " << static_cast<int>(glyph.width) << ", "
				<< static_cast<int>(glyph.height) << ", " << static_cast<int>(glyph.xAdvance) << ", "
				<< static_cast<int>(glyph.xOffset) << ", " << static_cast<int>(glyph.yOffset) << "},\n";
		}
		file << "    {0, 0, 0, 0, 0, 0}\n};\n\n";

		// Write the font descriptor structure
		file << "// Font descriptor\n";
		file << "const DadGFX::GFXCFont __" << CT2A(FontName) << " = {\n";
		file << "    (uint8_t*) __" << CT2A(FontName) << "_Bitmap,\n";
		file << "    (DadGFX::GFXglyph *) __" << CT2A(FontName) << "_Glyphs,\n";
		file << "    " << static_cast<int>(FirstChar) << ",\n";
		file << "    " << static_cast<int>(LastChar) << ",\n";
		file << "    " << static_cast<int>(getMaxHeight()) << "\n};\n";

		// Close the file stream
		file.close();
	}

	// Return success
	return true;
}
// --------------------------------------------------------------------------
// Exports the font data to a binary format.
// @param FileName Name of the file to export.
// @param FirstChar First character in the ASCII range.
// @param LastChar Last character in the ASCII range.
// @return True if the export is successful, otherwise false.
bool cBitmapFont::ExportToBinary(const CString& FileNameIn, uint16_t FirstChar, uint16_t LastChar) {
	// Format the default file name for the binary file
	CString FileName = FileNameIn;
	FileName.Replace(' ', '_');
	FileName.Format(L"%s.bin", FileName.GetString());

	// Open a file save dialog for the user to specify the export location
	CFileDialog dlg(FALSE, L"bin", FileName, OFN_HIDEREADONLY, L"Binary Files (*.bin)||");
	if (dlg.DoModal()) {
		CString filePath = dlg.GetPathName();

		// Check if the file already exists
		if (PathFileExists(filePath)) {
			if (AfxMessageBox(L"The file already exists. Do you want to overwrite it?", MB_YESNO | MB_ICONQUESTION) == IDNO) {
				return false;
			}
		}

		// Open the file stream to write the exported font data
		std::ofstream file(filePath, std::ios::binary);
		if (!file.is_open()) {
			return false;
		}

		// Placeholder for header data
		uint32_t offsetBitmap = 0;
		uint32_t offsetGlyphs = 0;
		uint32_t headerSize = sizeof(DadGFX::GFXBinFont);

		// Reserve space for the header (GFXBinFont structure)
		file.seekp(headerSize, std::ios::beg);

		// Store glyph metrics and bitmaps
		std::vector<DadGFX::GFXglyph> glyphMetrics;
		std::vector<uint8_t> bitmapData;
		uint32_t currentBitmapOffset = 0;

		// Loop through characters in the specified range
		for (uint16_t Character = FirstChar; Character <= LastChar; ++Character) {
			const GLYPHMETRICS* pGM;
			const BYTE* pBuffer;
			uint8_t ValOctet = 0;    // Accumulator for byte assembly
			uint8_t CtBits = 0;      // Bit counter for each byte
			
			GetMetrics(Character, pGM, pBuffer);

			DadGFX::GFXglyph glyph = {};
			glyph.bitmapOffset = currentBitmapOffset;
			glyph.width = pGM->gmBlackBoxX;
			glyph.height = pGM->gmBlackBoxY;
			glyph.xAdvance = pGM->gmCellIncX;
			glyph.xOffset = pGM->gmptGlyphOrigin.x;
			glyph.yOffset = m_MaxOverBaseLine - pGM->gmptGlyphOrigin.y;
			glyphMetrics.push_back(glyph);

			if (pBuffer) {
				// Calculate row size with 4 - byte alignment
				int rowSize = ((pGM->gmBlackBoxX + 31) / 32) * 4; 
				
				for (int y = 0; y < pGM->gmBlackBoxY; ++y) {
					for (int x = 0; x < pGM->gmBlackBoxX; ++x) {
						int byteIndex = y * rowSize + (x / 8);
						int bitIndex = 7 - (x % 8);
						bool isSet = pBuffer[byteIndex] & (1 << bitIndex);

						// Accumulate bits into bytes
						ValOctet = (ValOctet << 1) | (isSet ? 1 : 0);
						CtBits++;

						// Write completed byte to the file
						if (CtBits == 8) {
							bitmapData.push_back(ValOctet);
							CtBits = 0;
							ValOctet = 0;
							currentBitmapOffset++;
						}
					}
				}

				// Write remaining bits in the final byte
				if (CtBits > 0) {
					ValOctet <<= (8 - CtBits); // Pad with zeros
					bitmapData.push_back(ValOctet);
					currentBitmapOffset++;
				}
			}
			else {
				bitmapData.push_back(0x00);
				currentBitmapOffset++;
			}
		}

		// Write bitmap data and calculate offsets
		offsetBitmap = file.tellp();
		file.write(reinterpret_cast<const char*>(bitmapData.data()), bitmapData.size());

		// Write glyph data
		offsetGlyphs = file.tellp();
		file.write(reinterpret_cast<const char*>(glyphMetrics.data()), sizeof(DadGFX::GFXglyph) * glyphMetrics.size());

		// Write the GFXBinFont structure at the beginning of the file
		DadGFX::GFXBinFont font = {};
		font.bitmap = offsetBitmap;
		font.glyph = offsetGlyphs;
		font.first = FirstChar;
		font.last = LastChar;
		font.yAdvance = getMaxHeight();

		file.seekp(0, std::ios::beg);
		file.write(reinterpret_cast<const char*>(&font), sizeof(DadGFX::GFXBinFont));

		// Close the file
		file.close();
		return true;
	}

	return false;
}
