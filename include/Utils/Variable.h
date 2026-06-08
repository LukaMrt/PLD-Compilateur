#pragma once

#include "Type.h"

struct Variable
{
	Type type;
	int offset;
	bool isPointer;
	Variable* pointedVariable; // Valable seulement si isPointer == true, sinon inutilisé et offset est l'offset de la variable elle-même. Permet d'accéder au type de la variable pointée pour les opérations sur pointeurs (déréférencement, arithmétique de pointeurs).

	Variable()
		: type(), offset(0), isPointer(false), pointedVariable(nullptr)
	{
	}

	Variable getPointedVariable() const
	{
		if (!isPointer || pointedVariable == nullptr)
		{
			return *this;
		}
		return pointedVariable->getPointedVariable();
	}
};
