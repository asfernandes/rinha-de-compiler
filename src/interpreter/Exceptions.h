#ifndef RINHA_INTERPRETER_EXCEPTIONS_H
#define RINHA_INTERPRETER_EXCEPTIONS_H

#include <stdexcept>


namespace rinha::interpreter
{
	class RinhaException final : public std::runtime_error
	{
	public:
		using std::runtime_error::runtime_error;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_EXCEPTIONS_H
