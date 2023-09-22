#ifndef RINHA_INTERPRETER_DIAGNOSTIC_H
#define RINHA_INTERPRETER_DIAGNOSTIC_H

#include <algorithm>
#include <string>
#include <vector>

namespace rinha::interpreter
{
	struct Diagnostic final
	{
		enum class Type : uint8_t
		{
			WARNING,
			ERROR
		};

		Type type;
		unsigned line;
		unsigned column;
		std::string message;
	};

	class Diagnostics final
	{
	public:
		void add(Diagnostic&& diagnostic)
		{
			list.push_back(std::move(diagnostic));
		}

		const auto isEmpty() const
		{
			return list.empty();
		}

		const auto& getList() const
		{
			return list;
		}

		bool hasError() const
		{
			return std::ranges::any_of(
				list, [](const auto& diagnostic) { return diagnostic.type == Diagnostic::Type::ERROR; });
		}

	private:
		std::vector<Diagnostic> list;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_DIAGNOSTIC_H
