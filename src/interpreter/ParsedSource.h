#ifndef RINHA_INTERPRETER_PARSED_SOURCE_H
#define RINHA_INTERPRETER_PARSED_SOURCE_H

#include "./Nodes.h"
#include <memory>
#include <unordered_set>

namespace rinha::interpreter
{
	class ParsedSource final
	{
	public:
		ParsedSource(const TermNode* term, std::unordered_set<std::shared_ptr<Node>>&& nodes)
			: term(term),
			  nodes(std::move(nodes))
		{
		}

	public:
		const TermNode* getTerm() const
		{
			return term;
		}

	private:
		const TermNode* term;
		std::unordered_set<std::shared_ptr<Node>> nodes;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_PARSED_SOURCE_H
