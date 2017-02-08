#pragma once

namespace yk
{
	template <typename T, typename U>
	class Iterator
	{
	public:
		virtual T Next() = 0;
		virtual bool HasNext() = 0;
		virtual U Position() = 0;
		virtual void Rollback(U const& pos) = 0;
	};
}