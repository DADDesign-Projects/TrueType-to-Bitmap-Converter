#pragma once
#include <stdint.h>
#pragma pack(push, 1)

// En-tête principal du fichier TTF
struct TTFHeader {
    uint32_t sfntVersion;    // Version du fichier. Pour TTF, il est généralement 0x00010000
    uint16_t numTables;      // Nombre de tables
    uint16_t searchRange;    // La plus grande puissance de 2 <= numTables
    uint16_t entrySelector;  // Log2 de la plus grande puissance de 2 <= numTables
    uint16_t rangeShift;     // Nombre de tables restantes après la plus grande puissance de 2
};

// Table Directory Entry (Répertorie les tables)
struct TableDirectoryEntry {
    uint8_t tag1;            // Identifiant de la table (quatre caractères ASCII, comme 'name', 'glyf', etc.)
    uint8_t tag2;            // Identifiant de la table (quatre caractères ASCII, comme 'name', 'glyf', etc.)
    uint8_t tag3;            // Identifiant de la table (quatre caractères ASCII, comme 'name', 'glyf', etc.)
    uint8_t tag4;            // Identifiant de la table (quatre caractères ASCII, comme 'name', 'glyf', etc.)
    uint32_t checkSum;       // Somme de contrôle de la table
    uint32_t offset;         // Décalage de la table dans le fichier
    uint32_t length;         // Taille de la table
};

// Structure de la table 'name'
struct NameTableHeader {
    uint16_t format;          // Format de la table (habituellement 0)
    uint16_t count;           // Nombre d'entrées de noms
    uint16_t stringOffset;    // Offset de la chaîne de caractères
};

struct NameRecord {
    uint16_t platformID;      // Identifiant de la plateforme (par exemple, 1 pour Windows)
    uint16_t encodingID;      // Identifiant d'encodage (par exemple, 0 pour ANSI)
    uint16_t languageID;      // Identifiant de la langue (par exemple, 0x0409 pour l'anglais)
    uint16_t nameID;          // Identifiant du nom (par exemple, 4 pour le nom de la police)
    uint16_t length;          // Longueur de la chaîne de caractères
    uint16_t offset;          // Offset de la chaîne de caractères par rapport au début de la table 'name'
};

#pragma pack(pop)
