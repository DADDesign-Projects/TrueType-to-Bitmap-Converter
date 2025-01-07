#pragma once
#include <stdint.h>
#pragma pack(push, 1)

// En-t�te principal du fichier TTF
struct TTFHeader {
    uint32_t sfntVersion;    // Version du fichier. Pour TTF, il est g�n�ralement 0x00010000
    uint16_t numTables;      // Nombre de tables
    uint16_t searchRange;    // La plus grande puissance de 2 <= numTables
    uint16_t entrySelector;  // Log2 de la plus grande puissance de 2 <= numTables
    uint16_t rangeShift;     // Nombre de tables restantes apr�s la plus grande puissance de 2
};

// Table Directory Entry (R�pertorie les tables)
struct TableDirectoryEntry {
    uint8_t tag1;            // Identifiant de la table (quatre caract�res ASCII, comme 'name', 'glyf', etc.)
    uint8_t tag2;            // Identifiant de la table (quatre caract�res ASCII, comme 'name', 'glyf', etc.)
    uint8_t tag3;            // Identifiant de la table (quatre caract�res ASCII, comme 'name', 'glyf', etc.)
    uint8_t tag4;            // Identifiant de la table (quatre caract�res ASCII, comme 'name', 'glyf', etc.)
    uint32_t checkSum;       // Somme de contr�le de la table
    uint32_t offset;         // D�calage de la table dans le fichier
    uint32_t length;         // Taille de la table
};

// Structure de la table 'name'
struct NameTableHeader {
    uint16_t format;          // Format de la table (habituellement 0)
    uint16_t count;           // Nombre d'entr�es de noms
    uint16_t stringOffset;    // Offset de la cha�ne de caract�res
};

struct NameRecord {
    uint16_t platformID;      // Identifiant de la plateforme (par exemple, 1 pour Windows)
    uint16_t encodingID;      // Identifiant d'encodage (par exemple, 0 pour ANSI)
    uint16_t languageID;      // Identifiant de la langue (par exemple, 0x0409 pour l'anglais)
    uint16_t nameID;          // Identifiant du nom (par exemple, 4 pour le nom de la police)
    uint16_t length;          // Longueur de la cha�ne de caract�res
    uint16_t offset;          // Offset de la cha�ne de caract�res par rapport au d�but de la table 'name'
};

#pragma pack(pop)
