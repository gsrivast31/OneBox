#pragma once
#include <stdexcept>
namespace NewGroup
{
	namespace Core
	{
		class Exception:public std::runtime_error
		{
		public:
			Exception(const char *message):std::runtime_error(message)
			{
			}
		};
	}
}