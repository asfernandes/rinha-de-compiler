#ifndef RINHA_INTERPRETER_CONTEXT_H
#define RINHA_INTERPRETER_CONTEXT_H

#include "./Environment.h"
#include "./Exceptions.h"
#include "./Values.h"
#include <optional>
#include <string>
#include <unordered_map>

namespace rinha::interpreter
{
	class LetNode;

	class Context final
	{
	public:
		explicit Context(std::shared_ptr<Environment> environment)
			: environment(std::move(environment))
		{
		}

		explicit Context(std::shared_ptr<Context> outer)
			: environment(outer->environment),
			  outer(std::move(outer))
		{
		}

		void createVariable(const std::string& name)
		{
			variables.insert_or_assign(name, std::nullopt);
		}

		Value getVariable(const std::string& name) const
		{
			for (auto context = this; context; context = context->outer.get())
			{
				if (const auto it = context->variables.find(name);
					it != context->variables.end() && it->second.has_value())
				{
					return it->second.value();
				}
			}

			throw RinhaException("Variable '" + name + "' does not exist.");
		}

		void setVariable(const std::string& name, const Value& value)
		{
			variables[name] = value;
		}

		auto getEnvironment() noexcept
		{
			return environment;
		}

	private:
		std::shared_ptr<Environment> environment;
		std::shared_ptr<Context> outer;
		std::unordered_map<std::string, std::optional<Value>> variables;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_CONTEXT_H
