#pragma once

#include "Type.h"

struct Variable
{
	Type type;
	int offset;
	int pointerDepth;
	bool used;
	int array_size; // -1 si ce n'est pas un tableau, sinon la taille du tableau

	Variable() : type(), offset(0), pointerDepth(0), used(false), array_size(-1) {}
	Variable(Type type, int pointerDepth, int array_size = -1, bool used = false)
		: type(type), offset(0), pointerDepth(pointerDepth), used(used), array_size(array_size) {}

	// Largeur en octets de la valeur : un pointeur fait toujours 8 octets,
	// sinon c'est la taille du type pointé. Le backend s'appuie là-dessus pour
	// choisir movl (4) ou movq (8) et dimensionner le slot de pile.
	int size() const { return pointerDepth > 0 ? 8 : typeSize(type); }
};
