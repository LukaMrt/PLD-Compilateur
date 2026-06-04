#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <iostream>

enum class Type
{
	VOID,
	CHAR,
	INT32,
	DOUBLE
};

inline Type stringToType(const std::string &text)
{
	if (text == "int")
		return Type::INT32;
	if (text == "double")
		return Type::DOUBLE;
	if (text == "void")
		return Type::VOID;
	if (text == "char")
		return Type::CHAR;

	std::cerr << "Error: unknown type '" << text << "'." << std::endl;
	exit(1);
}

// Type résultant d'une opération binaire (promotions C) : double domine,
// sinon le résultat est un int (char est promu en int).
inline Type promote(Type left, Type right)
{
	if (left == Type::DOUBLE || right == Type::DOUBLE)
		return Type::DOUBLE;
	return Type::INT32;
}

inline int typeSize(Type type)
{
	switch (type)
	{
	case Type::VOID:
		return 0;
	case Type::CHAR:
		// Approche simple : un char occupe 4 octets pour que le backend puisse
		// l'écrire avec movl sans déborder sur la variable voisine (sizeof faux assumé).
		return 4;
	case Type::INT32:
		return 4;
	case Type::DOUBLE:
		return 8;
	}
	return 0;
}

#endif
