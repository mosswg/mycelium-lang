#include "function.h"

namespace mycelium {
	void function::call(std::vector<std::shared_ptr<mycelium::variable>>& call_args, std::shared_ptr<mycelium::variable>& return_value) const {
		for (int i = 0; i < call_args.size(); i++) {
			this->scope->variables[i]->set_value(call_args[i]);
		}
		for (auto& pt : body) {
			if (pt->type == function_return) {
				std::shared_ptr<return_from_function> return_expression = std::static_pointer_cast<return_from_function>(pt);
				if (return_value) {
					throw_error("setting return value multiple times");
				}
				if (return_expression->has_value()) {
					return_value = return_expression->get_value();
					return;
				}
				return_value = mycelium::constant::make_constant(0);
				return;
			}
			pt->execute();

			if (pt->type == parsed_token_type::cond_call) {
				auto cond_call = std::static_pointer_cast<mycelium::conditional_call>(pt);
				if (cond_call->return_value) {
					return_value = cond_call->return_value;
					if (show_debug_lines) {
						std::cout << "Returning from " << this->to_string() << " with " << return_value->to_string() << " from conditional\n";
					}
					return;
				}
			}
		}
	}
};
