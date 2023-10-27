#ifndef RINHA_INTERPRETER_PARSED_SOURCE_H
#define RINHA_INTERPRETER_PARSED_SOURCE_H

#include <boost/smart_ptr/local_shared_ptr.hpp>
#include <unordered_set>

namespace rinha::interpreter
{
	class Node;
	class TermNode;

	class ParsedSource final
	{
	public:
		ParsedSource(const TermNode* term, std::unordered_set<boost::local_shared_ptr<Node>>&& nodes)
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
		std::unordered_set<boost::local_shared_ptr<Node>> nodes;
	};
}  // namespace rinha::interpreter

#endif  // RINHA_INTERPRETER_PARSED_SOURCE_H
