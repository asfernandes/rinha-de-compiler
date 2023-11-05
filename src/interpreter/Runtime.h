#ifndef RINHA_INTERPRETER_RUNTIME_H
#define RINHA_INTERPRETER_RUNTIME_H

#include "./Nodes.h"
#include "./Values.h"
#include <exception>

namespace rinha::interpreter
{
	class Runtime
	{
	public:
		static Value binaryOp(BinaryOpNode::Op op, const Value& firstValue, const Value& secondValue)
		{
			// TODO: Short circuit with logical operators.

			switch (op)
			{
				case BinaryOpNode::Op::ADD:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() + secondInt->getValue());
					}
					else if ((std::holds_alternative<StrValue>(firstValue) ||
								 std::holds_alternative<IntValue>(firstValue)) &&
						(std::holds_alternative<StrValue>(secondValue) ||
							std::holds_alternative<IntValue>(secondValue)))
					{
						const auto& firstString = std::visit([](auto&& arg) { return arg.toString(); }, firstValue);
						const auto& secondString = std::visit([](auto&& arg) { return arg.toString(); }, secondValue);
						return StrValue(firstString + secondString);
					}
					else
						throw RinhaException("Invalid datatypes with operator '+'.");

				case BinaryOpNode::Op::SUB:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() - secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '-'.");

				case BinaryOpNode::Op::MUL:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() * secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '*'.");

				case BinaryOpNode::Op::DIV:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() / secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '/'.");

				case BinaryOpNode::Op::REM:
					if (const auto firstInt = std::get_if<IntValue>(&firstValue),
						secondInt = std::get_if<IntValue>(&secondValue);
						firstInt && secondInt)
					{
						return IntValue(firstInt->getValue() % secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '%'.");

				case BinaryOpNode::Op::EQ:
				case BinaryOpNode::Op::NEQ:
				case BinaryOpNode::Op::LT:
				case BinaryOpNode::Op::GT:
				case BinaryOpNode::Op::LTE:
				case BinaryOpNode::Op::GTE:
				{
					if (firstValue.index() != secondValue.index())
						throw RinhaException("Cannot compare the two values of different types.");

					std::strong_ordering cmp = std::strong_ordering::equal;

					if (const auto& firstVal = std::get_if<BoolValue>(&firstValue))
						cmp = firstVal->getValue() <=> std::get_if<BoolValue>(&secondValue)->getValue();
					else if (const auto& firstVal = std::get_if<IntValue>(&firstValue))
						cmp = firstVal->getValue() <=> std::get_if<IntValue>(&secondValue)->getValue();
					else if (const auto& firstVal = std::get_if<StrValue>(&firstValue))
						cmp = firstVal->getValue() <=> std::get_if<StrValue>(&secondValue)->getValue();
					else
						throw RinhaException("Invalid datatypes with comparare operator.");

					switch (op)
					{
						case BinaryOpNode::Op::EQ:
							return BoolValue(cmp == std::strong_ordering::equal);

						case BinaryOpNode::Op::NEQ:
							return BoolValue(cmp != std::strong_ordering::equal);

						case BinaryOpNode::Op::LT:
							return BoolValue(cmp == std::strong_ordering::less);

						case BinaryOpNode::Op::GT:
							return BoolValue(cmp == std::strong_ordering::greater);

						case BinaryOpNode::Op::LTE:
							return BoolValue(cmp != std::strong_ordering::greater);

						case BinaryOpNode::Op::GTE:
							return BoolValue(cmp != std::strong_ordering::less);

						default:
							assert(false);
					}

					break;
				}

				case BinaryOpNode::Op::AND:
					if (const auto firstInt = std::get_if<BoolValue>(&firstValue),
						secondInt = std::get_if<BoolValue>(&secondValue);
						firstInt && secondInt)
					{
						return BoolValue(firstInt->getValue() && secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '&&'.");

				case BinaryOpNode::Op::OR:
					if (const auto firstInt = std::get_if<BoolValue>(&firstValue),
						secondInt = std::get_if<BoolValue>(&secondValue);
						firstInt && secondInt)
					{
						return BoolValue(firstInt->getValue() || secondInt->getValue());
					}
					else
						throw RinhaException("Invalid datatypes with operator '||'.");
			}

			assert(false);
			throw std::logic_error("Invalid binary op");
		}
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_RUNTIME_H
