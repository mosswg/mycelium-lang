#pragma once

#include "parsed_token.h"
#include "pattern.h"

namespace mycelium {
	class function_base : public parsed_token {
	public:
		/// Why name and token?
		mycelium::token name;
		std::vector<mycelium::type> ret;
		int body_start_index, body_end_index;

		std::vector<std::shared_ptr<mycelium::parsed_token>> body;
		std::shared_ptr<mycelium::scope> scope;

		pattern_match args;

		function_base(mycelium::token token, mycelium::token name, std::vector<mycelium::type> ret, pattern_match args, parsed_token_type type, std::shared_ptr<mycelium::scope> scope) : parsed_token(std::move(token), type),
																																														  name(std::move(name)), ret(std::move(ret)), args(std::move(args)), scope(std::move(scope)), body_start_index(0), body_end_index(0) {}

		void execute() override {
			throw_error("Unimplemented Function");
		}

	};

	class return_from_function : public expression {
		public:
		std::shared_ptr<function_base> fn;
		std::shared_ptr<expression> return_value;

		return_from_function(std::shared_ptr<function_base> fn, const std::shared_ptr<expression>& value) : fn(fn), return_value(value), expression(mycelium::token("return", token_type::keyword), function_return) {}


		return_from_function(std::shared_ptr<function_base> fn) : fn(fn), return_value({}), expression(mycelium::token("return", token_type::keyword), function_return) {}

		std::string to_string() const override {
			if (return_value.get()) {
				return "Return from " + fn->to_string() + " with " + return_value->to_string();
			}

			return "Return from " + fn->to_string();
		}


		void execute() override {}

		std::shared_ptr<variable> get_value() override {
			return return_value->get_value();
		}

		mycelium::type  get_type() override {
			if (return_value.get()) {
				return return_value->get_type();
			}

			return type::none;
		}


		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return false;
		}

		bool has_value() const {
			return this->return_value.get();
		}
	};

	class function : public function_base {
	public:
		function(mycelium::token token, mycelium::token name, std::vector<mycelium::type> ret, pattern_match args, std::shared_ptr<mycelium::scope> scope) :
			function_base(std::move(token), std::move(name),
						  std::move(ret), std::move(args), func, std::move(scope)) {
			auto arg_exprs =  this->args.get_expressions();
			for (auto& arg_expr : arg_exprs) {
				this->scope->variables.push_back(arg_expr->get_value());
			}
		}


		static pattern_match generate_arguments_pattern_match_from_variables(const std::vector<std::shared_ptr<variable>>& args) {
			pattern_match out;

			for (auto& arg : args) {
				out.pattern.push_back(make_pattern_token(arg));
				if (&arg != &args.back()) {
					out.pattern.push_back(make_pattern_token(","));
				}
			}

			return out;
		}

		static std::shared_ptr<variable> make_anonymous_function(std::vector<std::shared_ptr<parsed_token>> body, const std::vector<mycelium::type>& ret, const std::vector<std::shared_ptr<variable>>& args, std::shared_ptr<mycelium::scope>& scope) {
			std::shared_ptr<function> out = std::make_shared<function>(mycelium::token(""), mycelium::token(""), ret,
																	   generate_arguments_pattern_match_from_variables(args), scope);
			out->body = std::move(body);
			return variable::make_variable_without_scope(mycelium::token(), out);
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (function*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->args.is_match(this->args));
			}
			return false;
		}

		std::string to_string() const override {
			std::string out = "function";
			if (!ret.empty()) {
				out += "<";
				for (int i = 0; i < ret.size(); i++) {
					out += ret[i].name;
					if (i < ret.size()-1) {
						out += ", ";
					}
				}
				out += ">";
			}
			out += " " + name.string + '(' + args.to_string() + ')';

			return out;
		}

		void execute() override {
			throw_error("Unimplemented Function: Function execute");
		}

		virtual std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::variable>>& call_args) const {
			for (int i = 0; i < call_args.size(); i++) {
				this->scope->variables[i]->set_value(call_args[i]);
			}
			for (auto& pt : body) {
				if (pt->type == function_return) {
					std::shared_ptr<return_from_function> return_expression = std::static_pointer_cast<return_from_function>(pt);
					if (return_expression->has_value()) {
						return return_expression->get_value();
					}
					return mycelium::constant::make_constant(0);
				}
				pt->execute();
			}
			return {};
		}
	};

	class builtin_function : public function {
	public:
		std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::variable>>&)> exec;

		builtin_function(const std::string& name, std::vector<mycelium::type> ret, const std::vector<mycelium::type>& args, std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, std::shared_ptr<mycelium::scope> scope) :
			function(mycelium::token(name), mycelium::token(name),
					 std::move(ret), generate_arguments_pattern_match_from_variables(variable::convert_types_to_variables(args)), std::move(scope)), exec(std::move(exec)) {
			this->body_start_index = 0;
			this->body_end_index = 0;
		}

		std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::variable>>& args) const override {
			return exec(args);
		}
	};

	class function_call : public expression {

	public:
		std::shared_ptr<function> fn;
		std::vector<std::shared_ptr<expression>> args;


		function_call(const std::shared_ptr<function>& fn, std::vector<std::shared_ptr<expression>> args) : fn(fn), args(std::move(args)), expression(fn->name) {}

		std::string to_string() const override {
			std::string out = "Call to " + fn->to_string() + " with ";
			for (auto& arg : args) {
				out += arg->to_string() + " ";
			}
			return out;
		}

		void execute() override {
			std::vector<std::shared_ptr<variable>> var_args;
			for (auto& arg : args) {
				var_args.push_back(arg->get_value());
			}
			fn->call(var_args);
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return false;
		}

		std::shared_ptr<variable> get_value() override {
			std::vector<std::shared_ptr<variable>> var_args;
			for (auto& arg : args) {
				var_args.push_back(arg->get_value());
			}
			return fn->call(var_args);
		}

		mycelium::type get_type() override {
			if (this->fn->ret.empty()) {
				return type::none;
			}
			else if (this->fn->ret.size() == 1) {
				return this->fn->ret[0];
			}
			else {
				return type::tuple;
			}
		}
	};

	class operatr : public function_base {
	public:
		int number_of_args;
		int priority;

		std::vector<mycelium::token> context;

		operatr(mycelium::token token, std::vector<mycelium::token> context, mycelium::pattern_match args, std::string name, std::vector<mycelium::type> ret, int priority, std::shared_ptr<mycelium::scope> scope) : function_base(std::move(token), mycelium::token(word,std::move(name)), std::move(ret), std::move(args), parsed_token_type::oper, std::move(scope)), context(std::move(context)), priority(priority) {
			number_of_args = 0;

			auto arg_exprs =  this->args.get_expressions();
			for (auto& arg_expr : arg_exprs) {
				number_of_args++;
				this->scope->variables.push_back(arg_expr->get_value());
			}
		}


		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == func) {
				auto* compfunc = (operatr*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->context == this->context);
			}
			return false;
		}

		std::string to_string() const override {
			std::string out = "operator ";
			out += name.string;
			out += '<';
			for (int i = 0; i < context.size(); i++) {
				out += context[i].string;
				if (i < context.size()-1) {
					out += ", ";
				}
			}
			out += '>';
			out += ": ";
			for (int i = 0; i < ret.size(); i++) {
				out += ret[i].name;
				if (i < ret.size()-1) {
					out += ", ";
				}
			}

			return out;
		}

		static std::string encode_operator(const std::string& oper);

		static std::string generate_name_from_context(std::vector<mycelium::token>& context);


		void execute() override {
			throw_error("Unimplemented Function");
		}

		virtual std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::variable>>& call_args) const {
			for (int i = 0; i < call_args.size(); i++) {
				this->scope->variables[i]->set_value(call_args[i]);
			}
			for (auto& pt : body) {
				if (pt->type == function_return) {
					std::shared_ptr<return_from_function> return_expression = std::static_pointer_cast<return_from_function>(pt);
					if (return_expression->has_value()) {
						return return_expression->get_value();
					}
					return mycelium::constant::make_constant(0);
				}
				pt->execute();
			}
			return {};
		}


		//virtual std::shared_ptr<variable> call(int index) const override {
		//	for (; index < body.size(); index++) {
		//		body[index]->execute();
		//	}
		//	pop_from_call_stack(call_stack);
		//}
	};

	class builtin_operator : public operatr {
	public:
		std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::variable>>&)> exec;

		builtin_operator(const std::string& op_token, const std::vector<mycelium::token>& pattern_match_tokens, const std::string& name, const std::vector<mycelium::type>& ret, std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::variable>>&)> exec, int priority, std::shared_ptr<mycelium::scope> scope) :
			operatr(mycelium::token(op_token), pattern_match_tokens, pattern_match::create_from_tokens(pattern_match_tokens), name, ret, priority, std::move(scope)), exec(std::move(exec)) {
		}

		std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::variable>>& call_args) const override {
			return exec(call_args);
		}
	};

	class operator_use : public expression {
	public:
		std::shared_ptr<operatr> op;
		std::vector<std::shared_ptr<expression>> args;

		operator_use(std::shared_ptr<operatr> op, std::vector<std::shared_ptr<expression>> args) : expression(op->name), op(std::move(op)), args(std::move(args)) {}

		std::string to_string() const override {
			return "operator_use: " + op->to_string();
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return compare->token.string == this->token.string;
		}

		void execute() override {
			std::vector<std::shared_ptr<variable>> var_args;
			for (auto& arg : args) {
				var_args.push_back(arg->get_value());
				// If we get an invalid value throw an error
				if (!var_args.back().get()) {
					throw_error("Failed to get value from \"" + arg->to_string() + "\" in \"" + this->to_string() + "\"");
				}
			}
			op->call(var_args);
		}

		std::shared_ptr<variable> get_value() override {
			std::vector<std::shared_ptr<variable>> var_args;
			for (auto& arg : args) {
				var_args.push_back(arg->get_value());
			}
			return op->call(var_args);
		}

		mycelium::type get_type() override {
			if (this->op->ret.empty()) {
				return type::none;
			}
			else if (this->op->ret.size() == 1) {
				return this->op->ret[0];
			}
			else {
				return type::tuple;
			}
		}


		static std::shared_ptr<parsed_token> create_operator_call(std::shared_ptr<operatr>& op, std::vector<std::shared_ptr<parsed_token>> pts, int index) {
			std::vector<std::shared_ptr<expression>> args;
			for (int i = index; i < index + op->args.pattern.size(); i++) {
				if (pts[i]->type == var) {
					auto* var = (variable*)pts[i].get();
					args.push_back(static_cast<const std::shared_ptr<variable>>(var));
				}
			}
			return std::make_shared<operator_use>(operator_use(op, args));
		}
	};

	class conditional : public function_base {
	public:
		conditional(mycelium::token token, mycelium::token name, pattern_match args, std::shared_ptr<mycelium::scope> scope) :
			function_base(std::move(token), std::move(name),
						  {type::boolean}, std::move(args), cond, std::move(scope)) {
			auto arg_exprs =  this->args.get_expressions();
			for (auto& arg_expr : arg_exprs) {
				this->scope->variables.push_back(arg_expr->get_value());
			}
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == cond) {
				auto* compfunc = (conditional*)compare.get();

				return (compfunc->ret == this->ret) && (compfunc->args.is_match(this->args));
			}
			return false;
		}

		std::string to_string() const override {
			std::string out = "conditional ";
			out += name.string;
			out += '(';
			out += args.to_string();
			out += ')';
			out += ": ";
			for (int i = 0; i < ret.size(); i++) {
				out += ret[i].name;
				if (i < ret.size()-1) {
					out += ", ";
				}
			}

			return out;
		}

		void execute() override {
			throw_error("Unimplemented Function");
		}

		virtual std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::expression>>& call_args) const {
			throw_error("Unimplemented: User Defined Conditional Calls");
			return {};
		}
	};

	class builtin_conditional : public conditional {
	public:

		std::vector<mycelium::token> args;
		std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::expression>>&)> exec;

		builtin_conditional(const std::string& name, const std::vector<mycelium::type>& args, std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::expression>>&)> exec, std::shared_ptr<mycelium::scope> scope) :
			conditional(mycelium::token(name), mycelium::token(name), function::generate_arguments_pattern_match_from_variables(variable::convert_types_to_variables(args)), std::move(scope)), exec(std::move(exec)) {}


		builtin_conditional(const std::string& name, const std::vector<mycelium::token>& pattern_match_tokens, std::function<std::shared_ptr<variable>(std::vector<std::shared_ptr<mycelium::expression>>&)> exec, std::shared_ptr<mycelium::scope> scope) :
			conditional(mycelium::token(name), mycelium::token(name), pattern_match::create_from_tokens(pattern_match_tokens), std::move(scope)), exec(std::move(exec)) {}

		std::shared_ptr<variable> call(std::vector<std::shared_ptr<mycelium::expression>>& call_args) const override {
			if (call_args[0]->get_type().code != type::func.code) {
				throw_error("Calling builtin conditional without function body");
			}
			return exec(call_args);
		}
	};

	class conditional_call : public expression {
	public:
		std::shared_ptr<conditional> cn;
		std::vector<std::shared_ptr<expression>> args;
		std::shared_ptr<mycelium::scope> body_scope;
		bool has_body;


		conditional_call(std::shared_ptr<conditional> cn, std::vector<std::shared_ptr<expression>> args,
						 std::shared_ptr<mycelium::scope> body_scope, bool has_body = true) : cn(std::move(cn)), args(std::move(args)), expression(cn->name, cond_call), body_scope(std::move(body_scope)), has_body(has_body) {}

		std::string to_string() const override {
			std::string out = "Call to " + cn->to_string() + " with ";
			for (auto& arg : args) {
				out += arg->to_string() + " ";
			}

			if (args[0]->get_value()->fn_ptr) {
				out += ": Body: {\n";

				for (auto& pt : args[0]->get_value()->fn_ptr->body) {
					out += pt->to_string() + "\n";
				}

				out += "}";
			}

			return out;
		}

		void execute() override {
			cn->call(args);
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return false;
		}

		std::shared_ptr<variable> get_value() override {
			return cn->call(args);
		}

		mycelium::type get_type() override {
			if (this->cn->ret.empty()) {
				return type::none;
			}
			else if (this->cn->ret.size() == 1) {
				return this->cn->ret[0];
			}
			else {
				return type::tuple;
			}
		}
	};
}
