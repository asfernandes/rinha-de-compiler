#include "./Parser.h"
#include "./Nodes.h"
#include "grammar/RinhaLexer.h"
#include "grammar/RinhaParser.h"
#include "grammar/RinhaBaseListener.h"
#include "antlr4-runtime.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

// grammar
using rinha::grammar::RinhaLexer;
using rinha::grammar::RinhaListener;
using rinha::grammar::RinhaParser;

// algorithm
namespace ranges = std::ranges;

// cctype
using std::toupper;

// fstream
using std::istream;

// iterator
using std::inserter;

// memory
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;

// string
using std::stoi;

// type_traits
using std::derived_from;

// unordered_map
using std::unordered_map;

// unordered_set
using std::unordered_set;


namespace
{
	using namespace rinha::interpreter;

	template <typename>
	struct NodeFromContext
	{
		using T = TermNode;
	};

	// Start of NodeFromContext specializations

	template <>
	struct NodeFromContext<RinhaParser::LogicalOpRuleContext>
	{
		using T = BinaryOpNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::ArithmeticOpRuleContext>
	{
		using T = BinaryOpNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::FactorOpRuleContext>
	{
		using T = BinaryOpNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::TermTupleRuleContext>
	{
		using T = TupleNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::TermLetRuleContext>
	{
		using T = LetNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::TermIfRuleContext>
	{
		using T = IfNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::TermFnRuleContext>
	{
		using T = FnNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::PrimaryTermRuleContext>
	{
		using T = TermNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::PrimaryTrueRuleContext>
	{
		using T = LiteralNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::PrimaryFalseRuleContext>
	{
		using T = LiteralNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::PrimaryIntRuleContext>
	{
		using T = LiteralNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::PrimaryStringRuleContext>
	{
		using T = LiteralNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::PrimaryVarRuleContext>
	{
		using T = VarNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::CallPrintRuleContext>
	{
		using T = PrintNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::CallFirstRuleContext>
	{
		using T = TupleIndexNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::CallSecondRuleContext>
	{
		using T = TupleIndexNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::CallApplyRuleContext>
	{
		using T = CallNode;
	};

	template <>
	struct NodeFromContext<RinhaParser::ReferenceContext>
	{
		using T = ReferenceNode;
	};

	// End of NodeFromContext specializations

	template <typename T>
	requires derived_from<T, antlr4::ParserRuleContext>
	using NodeFromContextType = typename NodeFromContext<T>::T;

	class Listener : public RinhaListener
	{
	public:
		void enterRootRule(RinhaParser::RootRuleContext* ctx) override { }

		void exitRootRule(RinhaParser::RootRuleContext* ctx) override
		{
			refNode(ctx, ctx->term());
		}

		void enterTermLogicalRule(RinhaParser::TermLogicalRuleContext* ctx) override { }

		void exitTermLogicalRule(RinhaParser::TermLogicalRuleContext* ctx) override
		{
			refNode(ctx, ctx->logical());
		}

		void enterTermTupleRule(RinhaParser::TermTupleRuleContext* ctx) override { }

		void exitTermTupleRule(RinhaParser::TermTupleRuleContext* ctx) override
		{
			newNode(ctx, getNode(ctx->term(0)), getNode(ctx->term(1)));
		}

		void enterTermLetRule(RinhaParser::TermLetRuleContext* ctx) override { }

		void exitTermLetRule(RinhaParser::TermLetRuleContext* ctx) override
		{
			newNode(ctx, getNode(ctx->reference()), getNode(ctx->value), getNode(ctx->next));
		}

		void enterTermIfRule(RinhaParser::TermIfRuleContext* ctx) override { }

		void exitTermIfRule(RinhaParser::TermIfRuleContext* ctx) override
		{
			newNode(ctx, getNode(ctx->condition), getNode(ctx->then), getNode(ctx->otherwise));
		}

		void enterTermFnRule(RinhaParser::TermFnRuleContext* ctx) override { }

		void exitTermFnRule(RinhaParser::TermFnRuleContext* ctx) override
		{
			std::vector<const ReferenceNode*> parameters;

			ranges::transform(ctx->reference(), inserter(parameters, parameters.begin()),
				[&](auto& parameter) { return getNode(parameter); });

			newNode(ctx, std::move(parameters), getNode(ctx->term()));
		}

		void enterTermTermRule(RinhaParser::TermTermRuleContext* ctx) override { }

		void exitTermTermRule(RinhaParser::TermTermRuleContext* ctx) override
		{
			refNode(ctx, ctx->term());
		}

		void enterLogicalArithmeticRule(RinhaParser::LogicalArithmeticRuleContext* ctx) override { }
		void exitLogicalArithmeticRule(RinhaParser::LogicalArithmeticRuleContext* ctx) override
		{
			refNode(ctx, ctx->arithmetic());
		}

		void enterLogicalOpRule(RinhaParser::LogicalOpRuleContext* ctx) override { }

		void exitLogicalOpRule(RinhaParser::LogicalOpRuleContext* ctx) override
		{
			BinaryOpNode::Op op;

			if (ctx->op->getText() == "&&")
				op = BinaryOpNode::Op::AND;
			else if (ctx->op->getText() == "||")
				op = BinaryOpNode::Op::OR;
			else if (ctx->op->getText() == "==")
				op = BinaryOpNode::Op::EQ;
			else if (ctx->op->getText() == "!=")
				op = BinaryOpNode::Op::NEQ;
			else if (ctx->op->getText() == "<=")
				op = BinaryOpNode::Op::LTE;
			else if (ctx->op->getText() == ">=")
				op = BinaryOpNode::Op::GTE;
			else if (ctx->op->getText() == "<")
				op = BinaryOpNode::Op::LT;
			else if (ctx->op->getText() == ">")
				op = BinaryOpNode::Op::GT;
			else
				throw std::logic_error("Invalid logical op");

			newNode(ctx, op, getNode(ctx->arithmetic()), getNode(ctx->logical()));
		}

		void enterArithmeticFactorRule(RinhaParser::ArithmeticFactorRuleContext* ctx) override { }

		void exitArithmeticFactorRule(RinhaParser::ArithmeticFactorRuleContext* ctx) override
		{
			refNode(ctx, ctx->factor());
		}

		void enterArithmeticOpRule(RinhaParser::ArithmeticOpRuleContext* ctx) override { }

		void exitArithmeticOpRule(RinhaParser::ArithmeticOpRuleContext* ctx) override
		{
			BinaryOpNode::Op op;

			switch (ctx->op->getText()[0])
			{
				case '+':
					op = BinaryOpNode::Op::ADD;
					break;

				case '-':
					op = BinaryOpNode::Op::SUB;
					break;

				default:
					throw std::logic_error("Invalid arithmetic op");
			}

			newNode(ctx, op, getNode(ctx->factor()), getNode(ctx->arithmetic()));
		}

		void enterFactorApplyRule(RinhaParser::FactorApplyRuleContext* ctx) override { }

		void exitFactorApplyRule(RinhaParser::FactorApplyRuleContext* ctx) override
		{
			refNode(ctx, ctx->apply());
		}

		void enterFactorOpRule(RinhaParser::FactorOpRuleContext* ctx) override { }

		void exitFactorOpRule(RinhaParser::FactorOpRuleContext* ctx) override
		{
			BinaryOpNode::Op op;

			switch (ctx->op->getText()[0])
			{
				case '*':
					op = BinaryOpNode::Op::MUL;
					break;

				case '/':
					op = BinaryOpNode::Op::DIV;
					break;

				case '%':
					op = BinaryOpNode::Op::REM;
					break;

				default:
					throw std::logic_error("Invalid factor op");
			}

			newNode(ctx, op, getNode(ctx->apply()), getNode(ctx->factor()));
		}

		void enterApplyPrimaryRule(RinhaParser::ApplyPrimaryRuleContext* ctx) override { }

		void exitApplyPrimaryRule(RinhaParser::ApplyPrimaryRuleContext* ctx) override
		{
			refNode(ctx, ctx->primary());
		}

		void enterApplyCallRule(RinhaParser::ApplyCallRuleContext* ctx) override { }

		void exitApplyCallRule(RinhaParser::ApplyCallRuleContext* ctx) override
		{
			refNode(ctx, ctx->call());
		}

		void enterPrimaryTermRule(RinhaParser::PrimaryTermRuleContext* ctx) override { }

		void exitPrimaryTermRule(RinhaParser::PrimaryTermRuleContext* ctx) override
		{
			refNode(ctx, ctx->term());
		}

		void enterPrimaryTrueRule(RinhaParser::PrimaryTrueRuleContext* ctx) override { }

		void exitPrimaryTrueRule(RinhaParser::PrimaryTrueRuleContext* ctx) override
		{
			newNode(ctx, BoolValue(true));
		}

		void enterPrimaryFalseRule(RinhaParser::PrimaryFalseRuleContext* ctx) override { }

		void exitPrimaryFalseRule(RinhaParser::PrimaryFalseRuleContext* ctx) override
		{
			newNode(ctx, BoolValue(false));
		}

		void enterPrimaryIntRule(RinhaParser::PrimaryIntRuleContext* ctx) override { }

		void exitPrimaryIntRule(RinhaParser::PrimaryIntRuleContext* ctx) override
		{
			const auto& text = ctx->INT()->getText();
			newNode(ctx, IntValue(stoi(text)));
		}

		void enterPrimaryStringRule(RinhaParser::PrimaryStringRuleContext* ctx) override { }

		void exitPrimaryStringRule(RinhaParser::PrimaryStringRuleContext* ctx) override
		{
			const auto& text = ctx->STRING()->getText();
			newNode(ctx, StrValue(text.substr(1, text.length() - 2)));
		}

		void enterPrimaryVarRule(RinhaParser::PrimaryVarRuleContext* ctx) override { }

		void exitPrimaryVarRule(RinhaParser::PrimaryVarRuleContext* ctx) override
		{
			newNode(ctx, VarNode(getNode(ctx->reference())));
		}

		void enterCallPrintRule(RinhaParser::CallPrintRuleContext* ctx) override { }

		void exitCallPrintRule(RinhaParser::CallPrintRuleContext* ctx) override
		{
			newNode(ctx, getNode(ctx->term()));
		}

		void enterCallFirstRule(RinhaParser::CallFirstRuleContext* ctx) override { }

		void exitCallFirstRule(RinhaParser::CallFirstRuleContext* ctx) override
		{
			newNode(ctx, getNode(ctx->term()), 0);
		}

		void enterCallSecondRule(RinhaParser::CallSecondRuleContext* ctx) override { }

		void exitCallSecondRule(RinhaParser::CallSecondRuleContext* ctx) override
		{
			newNode(ctx, getNode(ctx->term()), 1);
		}

		void enterCallApplyRule(RinhaParser::CallApplyRuleContext* ctx) override { }

		void exitCallApplyRule(RinhaParser::CallApplyRuleContext* ctx) override
		{
			std::vector<const TermNode*> arguments;

			ranges::transform(ctx->term(), inserter(arguments, arguments.begin()),
				[&](auto& parameter) { return getNode(parameter); });

			newNode(ctx, getNode(ctx->apply()), std::move(arguments));
		}

		void enterReference(RinhaParser::ReferenceContext* ctx) override { }

		void exitReference(RinhaParser::ReferenceContext* ctx) override
		{
			newNode(ctx, ctx->getText());
		}

		void enterEveryRule(antlr4::ParserRuleContext* ctx) override { }
		void exitEveryRule(antlr4::ParserRuleContext* ctx) override { }
		void visitTerminal(antlr4::tree::TerminalNode* node) override { }
		void visitErrorNode(antlr4::tree::ErrorNode* node) override { }

	public:
		template <typename T>
		requires derived_from<T, antlr4::ParserRuleContext> NodeFromContextType<T>
		*getNode(T* ctx)
		{
			if (!ctx)
				return nullptr;

			return static_cast<NodeFromContextType<T>*>(ctxNodeMap[ctx].get());
		}

	private:
		template <typename T>
		requires derived_from<T, antlr4::ParserRuleContext> NodeFromContextType<T>
		*newNode(T* ctx, auto&&... args)
		{
			assert(!ctxNodeMap.contains(ctx));

			auto node = make_shared<NodeFromContextType<T>>(std::forward<decltype(args)>(args)...);

			const antlr4::Token* startToken = ctx->getStart();
			node->startLine = startToken->getLine();
			node->startColumn = startToken->getCharPositionInLine() + 1;

			ctxNodeMap[ctx] = node;
			return node.get();
		}

		template <typename T, typename U>
		requires derived_from<T, antlr4::ParserRuleContext> && derived_from<U, antlr4::ParserRuleContext> &&
			derived_from<NodeFromContextType<U>, NodeFromContextType<T>>
				NodeFromContextType<T>
		*refNode(T* ctx, U* referencedCtx)
		{
			assert(!ctxNodeMap.contains(ctx));

			if (auto nodeIt = ctxNodeMap.find(referencedCtx); nodeIt != ctxNodeMap.end())
			{
				ctxNodeMap[ctx] = nodeIt->second;
				return static_cast<NodeFromContextType<U>*>(nodeIt->second.get());
			}
			else
				return nullptr;
		}

	public:
		unordered_map<antlr4::ParserRuleContext*, shared_ptr<Node>> ctxNodeMap;
	};

	class ErrorListener : public antlr4::BaseErrorListener
	{
	public:
		ErrorListener(shared_ptr<Diagnostics> diagnostics)
			: diagnostics(std::move(diagnostics))
		{
		}

	public:
		void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line,
			size_t charPositionInLine, const std::string& msg, std::exception_ptr ex) override
		{
			diagnostics->add({Diagnostic::Type::ERROR, (unsigned) line, (unsigned) charPositionInLine + 1u, msg});
		}

	private:
		shared_ptr<Diagnostics> diagnostics;
	};
}  // namespace


namespace rinha::interpreter
{
	struct Parser::Hidden
	{
		Hidden(istream& stream, shared_ptr<Diagnostics> diagnostics)
			: antlrInputStream(stream),
			  errorListener(std::move(diagnostics))
		{
			lexer.removeErrorListeners();
			lexer.addErrorListener(&errorListener);

			parser.removeParseListeners();
			parser.removeErrorListeners();
			parser.addErrorListener(&errorListener);

			parser.addParseListener(&listener);
		}

		antlr4::ANTLRInputStream antlrInputStream;
		RinhaLexer lexer{&antlrInputStream};
		antlr4::CommonTokenStream tokens{&lexer};
		RinhaParser parser{&tokens};
		Listener listener;
		ErrorListener errorListener;
	};


	Parser::Parser(unique_ptr<istream> _stream)
		: stream(std::move(_stream)),
		  diagnostics(make_shared<Diagnostics>()),
		  hidden(make_unique<Hidden>(*stream.get(), diagnostics))
	{
		auto root = hidden->parser.root();
		rootTerm = hidden->listener.getNode(root);

		unordered_set<shared_ptr<Node>> nodeSet;

		ranges::transform(
			hidden->listener.ctxNodeMap, inserter(nodeSet, nodeSet.begin()), [](auto& pair) { return pair.second; });

		parsedSource = make_shared<ParsedSource>(rootTerm, std::move(nodeSet));
	}

	Parser::~Parser() = default;
}  // namespace rinha::interpreter
