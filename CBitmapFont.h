#pragma once

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
#include "GFXFont.h"


//***********************************************************************************
// class cBitmapFont
//***********************************************************************************
class cBitmapFont
{
public:
    // --------------------------------------------------------------------------
    // Constructor for cBitmapFont.
    // Initializes member variables.
    cBitmapFont();

    // --------------------------------------------------------------------------
    // Destructor for cBitmapFont.
    // Deletes the created font if any.
    ~cBitmapFont() { DeleteBitmapFont(); };

    // --------------------------------------------------------------------------
    // Creates a bitmap font based on the provided LOGFONT structure.
    // @param lgFont The LOGFONT structure containing font parameters.
    // @return True if the font creation is successful, otherwise false.
    bool CreateBitmapFont(LOGFONT lgFont);

    // --------------------------------------------------------------------------
    // Deletes the bitmap font and releases allocated memory.
    void DeleteBitmapFont();

    // --------------------------------------------------------------------------
    // Retrieves metrics and the bitmap buffer for a given character.
    // @param Char The character to retrieve metrics for.
    // @param[out] pGM Pointer to the GLYPHMETRICS structure.
    // @param[out] pBuffer Pointer to the bitmap buffer.
    void GetMetrics(char Char, const GLYPHMETRICS*& pGM, const BYTE*& pBuffer) {
        pGM = &m_TabGM[Char];
        pBuffer = m_TabBuffer[Char];
    }

    // --------------------------------------------------------------------------
    // Gets the maximum height of the font.
    // @return Maximum height including above and below the baseline.
    inline uint16_t getMaxHeight() {
        return m_MaxOverBaseLine + m_MaxUnderBaseLine;
    }

    // --------------------------------------------------------------------------
    // Gets the maximum character width.
    // @return The width of the widest character.
    inline uint16_t getMaxWidth() {
        return m_MaxWidthChar;
    }

    // --------------------------------------------------------------------------
    // Gets the maximum height above the baseline.
    // @return Maximum height above the baseline.
    inline uint16_t getMaxOverBaseLine() {
        return m_MaxOverBaseLine;
    }

    // --------------------------------------------------------------------------
    // Draws a character on the given device context.
    // @param pDC Pointer to the device context.
    // @param Character The character to draw.
    // @param x The x-coordinate for drawing.
    // @param y The y-coordinate for drawing.
    // @param pixelSize The size of the pixel for rendering.
    // @return The horizontal position after drawing the character.
    int DrawCharacter(CDC* pDC, char Character, int x, int y, int pixelSize);

    // --------------------------------------------------------------------------
    // Exports the font data to a C-compatible format.
    // @param FontName Name of the font to export.
    // @param FirstChar First character in the ASCII range.
    // @param LastChar Last character in the ASCII range.
    // @return True if the export is successful, otherwise false.
    bool ExportToC(const CString& FontName, uint16_t FirstChar, uint16_t LastChar);

    // --------------------------------------------------------------------------
    // Exports the font data to a binary format.
    // @param FileName Name of the file to export.
    // @param FirstChar First character in the ASCII range.
    // @param LastChar Last character in the ASCII range.
    // @return True if the export is successful, otherwise false.
    bool cBitmapFont::ExportToBinary(const CString& FileNameIn, uint16_t FirstChar, uint16_t LastChar);

protected:
    GLYPHMETRICS m_TabGM[256]; // Array of glyph metrics for ASCII characters
    BYTE* m_TabBuffer[256];    // Array of pointers to glyph bitmaps
    bool m_FontCreated = false; // Indicates if a font is created
    uint16_t m_MaxWidthChar = 0; // Maximum character width
    uint16_t m_MaxUnderBaseLine = 0; // Maximum height below the baseline
    uint16_t m_MaxOverBaseLine = 0;  // Maximum height above the baseline
};
