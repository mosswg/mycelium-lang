#pragma once

#include "base.h"
#include "token.h"
#include "type.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>


namespace mycelium {

	/// Forward class declarations from other files
	class variable;
	class function;

	enum parsed_token_type {
		func,
		oper,
		cond,
		var,
		expr,
		oper_use,
		func_call,
		cond_call,
		function_return,
		bad
	};

	class parsed_token {
	public:

		mycelium::token token;

		mycelium::parsed_token_type type;

		parsed_token() : token(), type(bad) {}

		explicit parsed_token(mycelium::token token, parsed_token_type type) : token(std::move(token)), type(type) {}

		virtual ~parsed_token() = default;

		virtual std::string to_string() const = 0;

		virtual void execute() = 0;

		virtual bool is_similar(std::shared_ptr<parsed_token> compare) = 0;
	};


	class expression : public parsed_token {
	public:
		explicit expression(mycelium::token name) : parsed_token(std::move(name), expr) {}

		explicit expression(mycelium::token name, mycelium::parsed_token_type type) : parsed_token(std::move(name), type) {}

		virtual std::shared_ptr<variable> get_value() = 0;

		virtual mycelium::type get_type() = 0;

		std::string to_string() const override {
			return "expr";
		}
	};

	std::string function_to_string(const std::shared_ptr<function>&);

	class variable : public expression, public std::enable_shared_from_this<variable> {
	public:
		mycelium::type type;
		long value;
		std::shared_ptr<std::string> str;
		std::shared_ptr<mycelium::function> fn_ptr;
		std::shared_ptr<std::fstream> file;
		bool good; // used for file
		std::shared_ptr<std::vector<std::shared_ptr<variable>>> list_ptr;

		variable(mycelium::token name, const mycelium::type& type) : expression(std::move(name)), type(type) {
			if (type == type::string) {
				this->str = std::make_shared<std::string>();
			}
			if (type == type::func) {
				// Do nothing
			}
			if (type == type::file) {
				this->str = std::make_shared<std::string>();
				this->file = std::make_shared<std::fstream>();
				this->good = true;
			}
			else if (type == type::list) {
				this->list_ptr = std::make_shared<std::vector<std::shared_ptr<variable>>>();
			}
			else if (type == type::tuple) {
				throw_error("Unsupported Creation of tuple");
			}
			else {
				this->value = 0;
			}
		}

		variable(mycelium::token name, long value) : expression(std::move(name)), type(type::integer), value(value) {}

		variable(mycelium::token name, std::string str) : expression(std::move(name)), type(type::string), str(std::make_shared<std::string>(str)) {}

		variable(mycelium::token name, std::shared_ptr<mycelium::function> fn_ptr) : expression(std::move(name)), type(type::func), fn_ptr(std::move(fn_ptr)) {}

		variable(mycelium::token name, std::shared_ptr<std::fstream> file_ptr) : expression(std::move(name)), type(type::file), file(std::move(file_ptr)) {
			this->str = std::make_shared<std::string>("");
		}

		variable(mycelium::token name, std::shared_ptr<std::vector<std::shared_ptr<variable>>> list_ptr) : expression(std::move(name)), type(type::list), list_ptr(std::move(list_ptr)) {}

		variable(mycelium::token name, const std::vector<std::shared_ptr<variable>>& list_ptr) : expression(std::move(name)), type(type::list), list_ptr(std::make_shared<std::vector<std::shared_ptr<variable>>>(list_ptr)) {}

		variable(mycelium::token name, std::shared_ptr<variable> other) : expression(std::move(name)), type(other->type) {
			set_value(other);
		}

		~variable() override {
			this->destroy();
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, long value) {
			std::shared_ptr<variable> out = std::make_shared<variable>(name, value);
			return out;
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, const std::string& str) {
			std::shared_ptr<variable> out = std::make_shared<variable>(name, str);
			return out;
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, const std::shared_ptr<function>& fn_ptr) {
			std::shared_ptr<variable> out = std::make_shared<variable>(name, fn_ptr);
			return out;
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, const std::shared_ptr<std::fstream>& file_ptr) {
			return std::make_shared<variable>(name, file_ptr);
		}

		static std::shared_ptr<variable> make_variable_without_scope(const mycelium::token& name, const std::shared_ptr<std::vector<std::shared_ptr<variable>>>& list_ptr) {
			return std::make_shared<variable>(name, list_ptr);
		}

		static std::vector<std::shared_ptr<mycelium::variable>> convert_tokens_to_variables(const std::vector<mycelium::token>& tks) {
			/// HACK: This is a very stupid conversion in the sense that it doesn't care about anything other than types.
			/// Might want to change this later.
			std::vector<std::shared_ptr<variable>> out;
			for (int i = 0; i < tks.size(); i++) {
				if (tks[i].type == ttype) {
					int type_code = type::validate_type(tks[i]);
					const mycelium::token& name = tks[i + 1];
					out.push_back(std::make_shared<variable>(name, type::types[type_code]));
					i++;
				}
			}
			return out;
		}

		static std::vector<std::shared_ptr<mycelium::variable>> convert_types_to_variables(const std::vector<mycelium::type>& types) {
			/// HACK: This is a very stupid conversion in the sense that it doesn't care about anything other than types.
			/// Might want to change this later.
			std::vector<std::shared_ptr<variable>> out;
			for (auto& type : types) {
				out.push_back(std::make_shared<variable>(mycelium::token(word, type.name + std::to_string(out.size())), type));
			}
			return out;
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			if (compare->type == var) {
				auto* compvar = (variable*)compare.get();

				return compvar->type.code == this->type.code;
			}
			return false;
		}

		void destroy() {
			if (type.code == type::string.code) {
				str->clear();
			}
			else if (type.code == type::func.code) {
				// explicitly do nothing
			}
			else if (type.code == type::list.code) {
				this->list_ptr->clear();
			}
			else if (type.code == type::tuple.code) {
				throw_error("Unsupported Destroy of tuple");
			}

			this->type = type::none;
			this->value = 0;
		}

		void execute() override {}

		virtual void set_value(const mycelium::variable& other) {
			if (other.type == type::string) {
				if (this->type == type::string) {
					*this->str = *other.str;
				}
				else if (this->type == type::integer) {
					throw_error("Cannot convert string to int");
				}
				else if (this->type == type::file) {
					*this->str = *other.str;
					this->file = std::make_shared<std::fstream>(*other.str, std::fstream::in | std::fstream::out);
					this->good = other.good;
				}
			}
			else if (other.type == type::func) {
				throw_error("Cannot mutate function");
			}
			else if (other.type == type::integer) {
				if (this->type == type::string) {
					*this->str = std::to_string(other.value);
				}
				else if (this->type == type::integer) {
					this->value = other.value;
				}
			}
			else if (other.type == type::list) {
				*this->list_ptr = *other.list_ptr;
			}
		}

		virtual void set_value(const std::shared_ptr<mycelium::variable>& other) {
			return set_value(*other);
		}

		mycelium::type get_type() override {
			return this->type;
		}

		std::shared_ptr<variable> get_value() override {
			return shared_from_this();
		}

		virtual std::string get_as_string() const {
			if (type == type::string) {
				return *this->str;
			}
			else if (type.code == type::list.code) {
				std::string out;
				out += "{";
				for ( const auto& var : *this->list_ptr ) {
					out += var->get_as_string();
					if ( &var != &this->list_ptr->back() ) {
						out += ", ";
					}
				}
				out += "}";
				return out;
			}
			else if (type.code == type::tuple.code) {
				throw_error("Unsupported Get As String of tuple");
			}
			else if (type == type::func) {
				return function_to_string(this->fn_ptr);
			}
			else if (type == type::file) {
				if (this->str.get()) {
					if (this->good) {
						return "file(" + *this->str + ")";
					}
					else {
						return "file*(" + *this->str + ")";
					}
				}
				if (this->good) {
					return "file";
				}
				else {
					return "file*";
				}
			}
			else if (type == type::boolean) {
				return this->value ? "true" : "false";
			}
			else {
				return std::to_string(this->value);
			}
			return "ERROR IN STRING CONVERSION";
		}


		std::string to_string() const override {
			std::string out = "variable ";
			out += type.name;
			out += ' ';
			out += token.string;
			out += ": ";
			out += get_as_string();
			return out;
		}
	};



	class scope {
	public:
		std::shared_ptr<scope> parent_scope;
		std::vector<std::shared_ptr<variable>> variables = {};
		std::unordered_map<int, std::shared_ptr<variable>> registered_variables = {};
		int next_available_reg_index = 0;

		explicit scope(std::shared_ptr<scope> parent_scope) : parent_scope(std::move(parent_scope)) {}

		/// I think this is how I want to do this. I don't really want to create the variable until were actually executing the program but I think this is an okay compromise.
		/// Register is kind of a weird name but thats okay

		int register_variable(token name, type type) {
			auto var = std::make_shared<variable>(name, type);
			// TODO: I think we should explicitly allow multiple of the same name here but disallow once we are converting to regular variable. Make sure thats right.
			int id = next_available_reg_index++;
			this->registered_variables.emplace(id, var);
			return id;
		}

		void unregister_variable(int id, token name) {
			if (id == -1 || registered_variables.find(id) == registered_variables.end()) {
				return;
			}
			auto& var = this->registered_variables[id];
			if (!var.get() || var->token.type == invalid || var->token != name) {
				warn("Unregister: Invalid registered variable with id: " + std::to_string(id));
				return;
			}
			this->registered_variables.erase(id);
		}



		std::shared_ptr<variable> make_variable_from_registered_variable(int id, std::shared_ptr<expression> initial_value = {}) {
			if (id == -1 || registered_variables.find(id) == registered_variables.end()) {
				warn("Unknown registered variable id: " + std::to_string(id));
				return {};
			}
			auto var = this->registered_variables[id];
			if (get_variable_in_current_scope_only(var->token.string)) {
				throw_error("Cannot create variable from registered variabled with name \"" + var->token.string + "\" because a variable with that name already exists");
			}
			if (initial_value.get()) {
				var->set_value(initial_value->get_value());
			}
			this->variables.push_back(var);
			this->registered_variables.erase(id);
			return var;
		}


		std::shared_ptr<variable> make_variable(std::shared_ptr<variable> out) {
			if (get_variable_in_current_scope_only(out->token.string)) {
				throw_error("Cannot create variable with name \"" + out->token.string + "\" because a variable with that name already exists");
			}
			this->variables.push_back(out);
			return out;
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, std::shared_ptr<variable> value) {
			return make_variable(std::make_shared<variable>(name, value));
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, const mycelium::type& type) {
			return make_variable(std::make_shared<variable>(name, type));
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, long value) {
			return make_variable(std::make_shared<variable>(name, value));
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, const std::string& str) {
			return make_variable(std::make_shared<variable>(name, str));
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, const std::shared_ptr<mycelium::function>& fn_ptr) {
			return make_variable(std::make_shared<variable>(name, fn_ptr));
		}

		std::shared_ptr<variable> make_variable(const mycelium::token& name, const std::vector<std::shared_ptr<variable>>& list_ptr) {
			return make_variable(std::make_shared<variable>(name, list_ptr));
		}

		std::shared_ptr<variable> get_variable(const std::string& name) {
			for (auto& var : variables) {
				if (var->token.string == name) {
					return var;
				}
			}
			// Allow referencing registered variables
			for (auto& reg_var : registered_variables) {
				if (reg_var.second->token.string == name) {
					return reg_var.second;
				}
			}
			if (parent_scope) {
				return parent_scope->get_variable(name);
			}
			return {};
		}

		std::shared_ptr<variable> get_variable_in_current_scope_only(const std::string& name) {
			for (auto& var : variables) {
				if (var->token.string == name) {
					return var;
				}
			}
			return {};
		}


		std::string to_string() const {
			std::string out;
			if (!parent_scope) {
				out += "global ";
			}
			out += "scope: {";
			for (const auto& var : variables) {
				out += var->to_string();
				if (&var != &variables.back()) {
					out += ", ";
				}
			}
			out += "}";
			return out;
		}
	};


	class constant : public variable {
	public:
		explicit constant(long value) : variable({}, value) {}

		explicit constant(std::string value) : variable({}, std::move(value)) {}

		explicit constant(const std::vector<std::shared_ptr<variable>>& list) : variable({}, std::make_shared<std::vector<std::shared_ptr<variable>>>(list)) {}


		void set_value(const mycelium::variable&) override {
			throw_error("Cannot Set Value of Constant");
		}


		void set_value(const std::shared_ptr<mycelium::variable>&) override {
			throw_error("Cannot Set Value of Constant");
		}

		std::shared_ptr<variable> get_value() override {
			return shared_from_this();
		}

		static std::shared_ptr<constant> make_constant(long value) {
			return std::make_shared<constant>(value);
		}

		static std::shared_ptr<constant> make_constant(const std::string& value) {
			return std::make_shared<constant>(value);
		}


		static std::shared_ptr<constant> make_constant(const std::vector<std::shared_ptr<variable>>& value) {
			return std::make_shared<constant>(value);
		}


		static std::shared_ptr<constant> make_bool_constant(bool value) {
			auto out = std::make_shared<constant>(value);
			out->type = type::boolean;
			return out;
		}

		std::string to_string() const override {
			std::string out = "constant ";
			out += type.name;
			out += ' ';
			out += token.string;
			out += ": ";
			out += get_as_string();
			return out;
		}
	};


	class variable_creation : public expression {

	public:
		mycelium::type var_type;
		int registered_variable_id = -1;
		std::shared_ptr<expression> var_value;
		std::shared_ptr<scope> var_scope;
		std::shared_ptr<variable> var;


		variable_creation(mycelium::token var_name, std::shared_ptr<expression> initial_value, std::shared_ptr<scope> scope) : expression(var_name), var_type(initial_value->get_type()), var_value(initial_value), var_scope(scope) {
			registered_variable_id = scope->register_variable(var_name, var_type);
		}

		variable_creation(mycelium::token var_name, mycelium::type var_type, std::shared_ptr<scope> scope) : expression(var_name), var_type(var_type), var_scope(scope), var_value({}) {
			registered_variable_id = scope->register_variable(var_name, var_type);
		}

		~variable_creation() {
			// NOTE: If we do not call execute. Destructor will cause a SIGSEGV error.
			// This can happen if this is passed directly to a call instead of calling 'get_value' then passing the result. Like I just fixed smh. What kind of idiot would make that mistake lmao.
			if (registered_variable_id != -1) {
				var_scope->unregister_variable(registered_variable_id, token);
			}
		}

		std::string to_string() const override {
			std::string out = "variable creation of " + var_type.to_string() + " " + token.string + " " + std::to_string(registered_variable_id) + " ";
			if (var_value.get()) {
				out += " <- " + var_value->to_string();
			}
			return out;
		}

		void execute() override {
			if (var_value.get()) {
				var = var_scope->make_variable_from_registered_variable(registered_variable_id, var_value);
			}
			else {
				var = var_scope->make_variable_from_registered_variable(registered_variable_id);
			}
			registered_variable_id = -1;
		}

		bool is_similar(std::shared_ptr<parsed_token> compare) override {
			return false;
		}

		std::shared_ptr<variable> get_value() override {
			// UNSURE: Should execute always be called before this function. As it stands execute is only called in the case where variable creation is alone. E.G. `int a`k.
			if (!var.get()) {
				this->execute();
			}
			return var;
		}

		mycelium::type get_type() override {
			if (var.get()) {
				return var->get_type();
			}
			else if (var_value.get()) {
				return var_value->get_type();
			}

			return var_type;
		}
	};


}
