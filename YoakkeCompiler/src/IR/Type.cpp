#include "Type.h"

namespace yk
{
	namespace IR
	{
		Type::Type(ystr const& nm, ysize bits)
			: Identifier(nm), BitCount(bits)
		{
		}

		Integer::Integer(ysize bits)
			: Type("i" + std::to_string(bits), bits)
		{
		}
	}
}