#ifndef TYPE_H
#define TYPE_H

enum class Type
{
	VOID,
	CHAR,
	INT32,
	DOUBLE
};

inline int typeSize(Type type)
{
	switch (type)
	{
	case Type::VOID:
		return 0;
	case Type::CHAR:
		return 1;
	case Type::INT32:
		return 4;
	case Type::DOUBLE:
		return 8;
	}
	return 0;
}

#endif
