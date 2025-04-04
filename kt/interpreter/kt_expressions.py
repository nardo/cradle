# The kt programming language compiler - copyright KAGR LLC. The use of this source code is governed by the MIT license agreement described in the "license.txt" file in the parent "kt" directory.

from kt_program_tree import *
from kt_slot import *
from kt_locator_types import *
from kt_locator import *
from kt_type_qualifier import *

def compile_boolean_expression(func, expr):
	return expr.compile(func, ('boolean'))

def compile_void_expression(func, expr):
	return expr.compile(func, ('any'))

class node_expression(program_node):
	def analyze_lvalue(self, func, result_type_qualifier):
		raise compile_error(self, "Expression is not an l-value.")
		pass
	def compile(self, func, result_symbol, result_type_qualifier):
		pass
	def compile_lvalue(self, func):
		# the compile_lvalue method returns a touple containing first a compiled expression string representing the lvalue location and the type specifier for that lvalue.
		return "symbol_name", None
		pass
	def get_preferred_type_qualifier(self, func):
		return func.facet.builtin_type_qualifier_variable

class node_locator_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.location = None
		self.string = None

	def analyze_expr_linkage(self, func):
		self.location = resolve_locator(func, self.string, self)

	def get_preferred_type_qualifier(self, func):
		return self.location.get_type_qualifier()

	def analyze_expr_types(self, func, result_type_qualifier):
		self.location.get_type_qualifier().check_conversion(result_type_qualifier)

	def analyze_lvalue(self, func, type_spec):
		if self.location.locator_type > locator_types.last_lvalue:
			raise compile_error(self, "Symbol " + self.string + " was not found or cannot be assigned a value.")

	def compile(self, func, result_symbol, type_qual):
		if self.location.get_type_qualifier().is_equivalent(type_qual):
			if result_symbol is not None:
				func.append_code(result_symbol + " = " + self.location.get_c_name() + ";\n")
				return result_symbol
			else:
				return self.location.get_c_name()
		else:
			if result_symbol is None:
				result_symbol = func.add_register(type_qual)
			func.append_type_conversion(self.location.get_c_name(), self.location.get_type_qualifier(), result_symbol, type_qual)
			return result_symbol

	def compile_lvalue(self, func):
		return self.location.get_c_name(), self.location.get_type_qualifier()


class node_int_constant_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.value = None
	def analyze_expr_types(self, func, result_type_qualifier):
		if not result_type_qualifier.is_numeric:
			raise compile_error(self, "integer constant expression is not valid here.")

	def analyze_expr_linkage(self, func): pass
	def compile(self, func, result_symbol, type_qual):
		if func.facet.type_dictionary.builtin_type_qualifier_integer.is_equivalent(type_qual):
			if result_symbol is None:
				result_symbol = func.add_register(type_qual)
			func.append_code(result_symbol + " = " + str(self.value) + ";\n")
			return result_symbol
		else:
			if result_symbol is None:
				result_symbol = func.add_register(type_qual)
			const_reg = func.add_register(func.facet.type_dictionary.builtin_type_qualifier_integer)
			func.append_code(const_reg + " = " + str(self.value) + ";\n")
			func.append_type_conversion(const_reg, func.facet.type_dictionary.builtin_type_qualifier_integer, result_symbol, type_qual)
			return result_symbol

	def get_preferred_type_qualifier(self, func):
		return func.facet.type_dictionary.builtin_type_qualifier_integer

class node_float_constant_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.value = None
	def analyze_expr_types(self, func, result_type_qualifier):
		if not result_type_qualifier.is_numeric():
			raise compile_error(self, "floating point constant expression is not valid here.")
	def get_preferred_type_qualifier(self, func):
		return func.facet.type_dictionary.builtin_type_qualifier_float
	def compile(self, func, result_symbol, type_spec):
		if result_symbol is None:
			result_symbol = func.add_register(type_spec)
		func.append_code(result_symbol + " = " + str(self.value) + ";\n")
		return result_symbol

class node_string_constant(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.value = None
	def analyze_expr_types(self, func, result_type_qualifier):
		if not result_type_qualifier.is_string:
			raise compile_error(self, "string constant expression is not valid here.")
		self.string_index = func.facet.add_string_constant(self.value)
	def analyze_expr_linkage(self, func): pass
	def get_preferred_type_qualifier(self, func):
		return func.facet.type_dictionary.builtin_type_qualifier_string
	def compile(self, func, result_symbol, type_spec):
		if result_symbol is None:
			result_symbol = func.add_register(type_spec)
		func.append_code(result_symbol + " = __string_constants[" + str(self.string_index) + "];\n")
		return result_symbol

class node_strcat_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.left = None
		self.right = None
		self.op = None
	op_table = { 'cat_none' : "\"\"", 'cat_newline' : "\"\\n\"", 'cat_space' : " ", 'cat_tab' : "\"\\t\"" }

	def get_cat_str(self):
		if self.op not in node_strcat_expr.op_table:
			raise compile_error(self, "Unknown string cat operator" + str(self.op))
		return node_strcat_expr.op_table[self.op]
	def analyze_expr_linkage(self, func):
		self.left.analyze_expr_linkage(func)
		self.right.analyze_expr_linkage(func)
	def analyze_expr_types(self, func, result_type_qualifier):
		func.facet.type_dictionary.builtin_type_qualifier_string.check_conversion(result_type_qualifier)
		self.left.analyze_expr_types(func, func.facet.type_dictionary.builtin_type_qualifier_string)
		self.right.analyze_expr_types(func, func.facet.type_dictionary.builtin_type_qualifier_string)

	def compile(self, func, result_symbol, type_qual):
		if result_symbol is None:
			result_symbol = func.add_register(type_qual)

		if func.facet.type_dictionary.builtin_type_qualifier_string.is_equivalent(type_qual):
			intermediate_result = result_symbol
		else:
			intermediate_result = func.add_register(func.facet.type_dictionary.builtin_type_qualifier_string)
		left_symbol = self.left.compile(func, None, func.facet.type_dictionary.builtin_type_qualifier_string)
		right_symbol = self.right.compile(func, None, func.facet.type_dictionary.builtin_type_qualifier_string)
		func.append_code(intermediate_result + " = _concatenate_string(" + left_symbol + ", " + self.get_cat_str() + ", " + right_symbol + ");\n")
		if result_symbol != intermediate_result:
			func.append_type_conversion(intermediate_result, func.facet.type_dictionary.builtin_type_qualifier_string, result_symbol, type_qual)
		return result_symbol

	def get_preferred_type_spec(self, func):
		return func.facet.type_dictionary.builtin_type_qualifier_string

class node_array_index_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.array_expr = None
		self.index_expr = None
		self.resolved = False
		self.container_type = None
		self.container_value_type = None
		self.container_key_type = None

	def resolve(self, func):
		if self.resolved:
			return
		self.resolved = 1
		self.container_type = self.array_expr.get_preferred_type_qualifier(func)
		if not self.container_type.is_container:
			raise compile_error(self, "this type is not a container.")
		self.container_value_type = self.container_type.container_value_type
		self.container_key_type = self.container_type.container_key_type

	def get_preferred_type_qualifier(self, func):
		self.resolve(func)
		return self.container_value_type

	def analyze_expr_linkage(self, func):
		self.array_expr.analyze_expr_linkage(func)
		self.index_expr.analyze_expr_linkage(func)

	def analyze_expr_types(self, func, result_type_qualifier):
		self.resolve(func)
		self.array_expr.analyze_expr_types(func, self.container_type)
		self.index_expr.analyze_expr_types(func, self.container_key_type)
		self.container_value_type.check_conversion(result_type_qualifier)

	def analyze_lvalue(self, func, type_spec):
		self.analyze(func, type_spec)
		type_spec.check_conversion(self.container_value_type)

	def compile(self, func, result_symbol, type_spec):
		index_symbol = self.index_expr.compile(func, None, self.container_key_type)
		container_symbol = self.array_expr.compile(func, None, self.container_type)
		if result_symbol is None:
			result_symbol = func.add_register(type_spec)
		if self.container_value_type.is_equivalent(type_spec):
			func.append_code(result_symbol + " = " + container_symbol + "[" + index_symbol + "];\n")
		else:
			func.append_type_conversion(container_symbol + "[" + index_symbol + "]", self.container_value_type, result_symbol, type_spec)
		return result_symbol
	def compile_lvalue(self, func):
		index_symbol = self.index_expr.compile(func, None, self.container_key_type)
		container_symbol = self.array_expr.compile(func, None, self.container_type)
		return container_symbol + "[" + index_symbol + "]", self.container_value_type

class node_func_call_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.func_expr = None
		self.args = None

	def analyze_expr_linkage(self, func):
		self.func_expr.analyze_expr_linkage(func)
		for arg in self.args:
			arg.analyze_expr_linkage(func)

	def analyze_expr_types(self, func, result_type_qualifier):
		func_type = self.func_expr.get_preferred_type_qualifier(func)
		if not func_type.is_callable:
			raise compile_error(self, "expression cannot be called as a function.")
		self.func_expr.analyze_expr_types(func, func_type)
		if func_type.callable_has_signature():
			type_list = func_type.get_callable_parameter_types()
			if len(self.args) != len(type_list):
				raise compile_error(self, "Wrong number of arguments in function call.")
			for arg, type in zip(self.args, type_list):
				arg.analyze_expr_types(func, type)
		else:
			for arg in self.args:
				arg.analyze_expr_types(func, func.facet.type_dictionary.builtin_type_qualifier_variable)
		func_type.get_callable_return_type().check_conversion(result_type_qualifier)

	def get_preferred_type_qualifier(self, func):
		func_type = self.func_expr.get_preferred_type_qualifier(func)
		if func_type.is_callable():
			return func_type.get_callable_return_type()
		return func.facet.builtin_type_spec_none

	def compile(self, func, result_symbol, type_qual):
		func_type = self.func_expr.get_preferred_type_qualifier(func)
		callable_return_type = func_type.get_callable_return_type()
		has_return_value = type_qual.type_kind != type_qualifier.kind.none_type
		return_register = None
		if has_return_value:
			if result_symbol is None:
				result_symbol = func.add_register(type_qual)
			if callable_return_type.type_kind == type_qualifier.kind.none_type:
				raise compile_error(self, "Assigning value from function that returns none.")
			return_register = result_symbol
			if not type_qual.is_equivalent(callable_return_type):
				return_register = func.add_register(callable_return_type)

		func_symbol = self.func_expr.compile(func, None, func_type)

		if func_type.parameter_type_list is not None:
			# if the callable has a specific signature, we can call its symbol directly
			arg_symbols = [arg.compile(func, None, type) for arg, type in zip(self.args, func_type.parameter_type_list)]
			if has_return_value:
				func.append_code(return_register + " = ")
			func.append_code(func_symbol + "(")
			if func_type.needs_closure:
				func.append_code("&__self_closure__")
				if(len(arg_symbols) > 0):
					func.append_code(", ")
			func.append_code(", ".join(arg_symbols) + ");\n")
		else:
			# if the signature is unknown, we compile everything into a variable and let the dynamic dispatcher take care of things.
			arg_symbols = [arg.compile(func, None, func.facet.type_dictionary.builtin_type_qualifier_variable) for arg in self.args]
			func.append_code("__dynamic_dispatch__(" + func_symbol + ", &" + return_register + ", " + str(len(arg_symbols)) + "".join(", &" + symbol for symbol in arg_symbols))
		if return_register is not None and return_register != result_symbol:
			func.append_type_conversion(return_register, callable_return_type, result_symbol, type_qual)
		return result_symbol

class node_method_call(node_func_call_expr):
	def __init__(self):
		node_func_call_expr.__init__(self)
		self.selector_list = None
		self.primary_name = None
		self.object_expr = None

	def resolve(self, func):
		if not self.resolved:
			args = [pair.expr for pair in self.selector_list]
			sel_str = self.primary_name + "".join(str(pair.string if pair.string is not None else "") + ":" for pair in self.selector_list )
			print("Got selector: " + sel_str)
			self.func_expr = node_slot_expr()
			self.func_expr.object_expr = self.object_expr
			self.func_expr.slot_name = sel_str
			self.args = args
			node_func_call_expr.resolve(self)

class node_slot_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.object_expr = None
		self.slot_name = None
		self.is_parent_reference = False
		self.resolved = False
		self.dynamic_lookup = False
		self.slot = None
		self.slot_type = None
	def resolve(self, func):
		if not self.resolved:
			self.resolved = True
			object_expr = self.object_expr
			if object_expr.__class__ == node_locator_expr and object_expr.string == 'parent':
				self.is_parent_reference = True
				if not func.compound:
					raise compile_error(self, "Function has no compound.")
				parent_node = func.compound.parent_node
				if parent_node is None:
					raise compile_error(self, "compound " + func.compound.name + " has no declared parent.")
				if self.slot_name not in parent_node.members:
					raise compile_error(self, "Parent of " + func.compound.name + " has no member named " + self.slot_name)
				slot = parent_node.members[self.slot_name]
				if slot.type != 'function':
					raise compile_error(self, "parent expression must reference a function.")
				self.parent_function = slot.function_decl
				self.slot_type = self.parent_function.get_type_signature()
			else:
				self.compound_type = self.object_expr.get_preferred_type_spec()
				if not self.compound_type.is_compound():
					raise compile_error(self, "Field access not allowed on this type.")
				members = self.compound_type.get_compound_members()
				if members is None:
					# if the compound members cannot be determined we'll resolve dynamically
					self.slot_type = func.facet.builtin_type_spec_variable
					self.dynamic_lookup = True
				else:
					if self.slot_name not in members:
						raise compile_error(self, "Slot " + self.slot_name + " is not a valid member.")
					self.slot_type = members[self.slot_name].type_spec
	def get_preferred_type_spec(self, func):
		self.resolve(func)
		return self.slot_type
	def analyze(self, func, type_spec):
		self.resolve(func)
		if self.is_parent_reference:
			if not type_spec.is_equivalent(self.slot_type):
				raise compile_error(self, "Parent function reference cannot be converted to a different type")
		else:
			self.object_expr.analyze(func, self.compound_type)
			self.slot_type.check_conversion(type_spec)
	def compile(self, func, result_symbol, type_spec):
		if self.is_parent_reference:
			if result_symbol is None:
				result_symbol = func.add_register(type_spec)
			func.append_code(result_symbol + ".this_object = this_object;\n" + result_symbol + ".func = " + self.parent_function.get_c_name() + ";\n")
			return result_symbol
		else:
			compound_symbol = self.object_expr.compile(func, None, self.compound_type)
			if self.dynamic_lookup:
				slot_ref = "__dynamic_field_lookup(" + compound_symbol + ", \"" + self.slot_name + "\")"
			else:
				deref_string = "->" if self.compound_type.is_compound_pointer() else "."
				slot_ref = compound_symbol + deref_string + self.slot_name
			if type_spec.is_equivalent(self.slot_type):
				if result_symbol is None:
					return slot_ref
				else:
					func.append_code(result_symbol + " = " + slot_ref)
					return result_symbol
			else:
				if result_symbol is None:
					result_symbol = func.add_register(type_spec)
				func.append_type_conversion(slot_ref, self.slot_type, result_symbol, type_spec)
				return result_symbol
	def analyze_lvalue(self, func, return_type_spec):
		if self.is_parent_reference:
			raise compile_error(self, "Parent call slot reference cannot be used as an l-value")
		self.object_expr.analyze(func, self.compound_type)
		self.slot_type.check_conversion(return_type_spec)
	def compile_lvalue(self, func):
		compound_symbol = self.object_expr.compile(func, None, self.compound_type)
		if self.dynamic_lookup:
			slot_ref = "__dynamic_field_lookup(" + compound_symbol + ", \"" + self.slot_name + "\")"
		else:
			deref_string = "->" if self.compound_type.is_compound_pointer() else "."
			slot_ref = compound_symbol + deref_string + self.slot_name
		return slot_ref, self.slot_type

class node_unary_lvalue_op_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.expression = None
		self.op = None

	def get_preferred_type_spec(self, func):
		return self.expression.get_preferred_type_spec(func)
	def analyze(self, func, type_spec):
		self.expression.analyze_lvalue(func, type_spec)
		if not self.expression.get_preferred_type_spec(func).is_numeric():
			raise compile_error(self, "Cannot apply a unary operation to a non-numeric expression")
	def compile(self, func, return_symbol, type_spec):
		expression_symbol, expression_type = self.expression.compile_lvalue(func)
		# op can be one of pre_increment, post_increment, pre_decrement, post_decrement
		if self.op[0:4] == "post":
			if return_symbol is None:
				return_symbol = func.add_register(type_spec)
			if type_spec.is_equivalent(expression_type):
				func.append_code(return_symbol + " = " + expression_symbol + ";\n")
			else:
				func.append_type_conversion(expression_symbol, expression_type, return_symbol, type_spec)
		if self.op[-9:] == "increment":
			func.append_code(expression_symbol + " += 1;\n")
		if self.op[0:4] == "post":
			return return_symbol
		if return_symbol is None and type_spec.is_equivalent(expression_type):
			return expression_symbol
		else:
			if return_symbol is None:
				return_symbol = func.add_register(type_spec)
			if type_spec.is_equivalent(expression_type):
				func.append_code(return_symbol + " = " + expression_symbol + ";\n")
			else:
				func.append_type_conversion(expression_symbol, expression_type, return_symbol, type_spec)
			return return_symbol

class node_unary_minus_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.expression = None
	def get_preferred_type_spec(self, func):
		return self.expression.get_preferred_type_spec(func)
	def analyze(self, func, type_spec):
		self.expression.analyze(func, type_spec)
		if not self.expression.get_preferred_type_spec(func).is_numeric():
			raise compile_error(self, "Cannot negate a non-numeric expression")

	def compile(self, func, return_symbol, type_spec):
		expression_type = self.expression.get_preferred_type_spec(func)
		expression_symbol = self.expression.compile(func, None, expression_type)
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		if not expression_type.is_equivalent(type_spec):
			expression_result = func.add_register(expression_type)
		else:
			expression_result = return_symbol
		func.append_code(expression_result + " = -" + expression_symbol + ";\n")
		if return_symbol != expression_result:
			func.append_type_conversion(expression_result, expression_type, return_symbol, type_spec)
		return return_symbol

class node_logical_not_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.expression = None
	def get_preferred_type_spec(self, func):
		return func.facet.builtin_type_spec_boolean
	def analyze(self, func, type_spec):
		expression_type = self.expression.get_preferred_type_spec(func)
		if not expression_type.is_numeric():
			raise compile_error(self, "Cannot apply a logical not to a non-numeric expression")
		self.expression.analyze(func, expression_type)
		func.facet.builtin_type_spec_boolean.check_conversion(type_spec)
	def compile(self, func, return_symbol, type_spec):
		expression_type = self.expression.get_preferred_type_spec(func)
		expression_symbol = self.expression.compile(func, None, expression_type)
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		if not func.facet.builtin_type_spec_boolean.is_equivalent(type_spec):
			expression_result = func.add_register(func.facet.builtin_type_spec_boolean)
		else:
			expression_result = return_symbol
		func.append_code(expression_result + " = (" + expression_symbol + " == 0);\n")
		if return_symbol != expression_result:
			func.append_type_conversion(expression_result, func.facet.builtin_type_spec_boolean, return_symbol, type_spec)
		return return_symbol

class node_bitwise_not_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.expression = None
	def get_preferred_type_spec(self, func):
		return func.facet.builtin_type_spec_integer
	def analyze(self, func, type_spec):
		func.facet.builtin_type_spec_integer.check_conversion(type_spec)
		self.expression.analyze(func, func.facet.builtin_type_spec_integer)
	def compile(self, func, return_symbol, type_spec):
		expression_symbol = self.expression.compile(func, None, func.facet.builtin_type_spec_integer)
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		if not func.facet.builtin_type_spec_integer.is_equivalent(type_spec):
			expression_result = func.add_register(func.facet.builtin_type_spec_integer)
		else:
			expression_result = return_symbol
		func.append_code(expression_result + " = ~" + expression_symbol + ";\n")
		if return_symbol != expression_result:
			func.append_type_conversion(expression_result, func.facet.builtin_type_spec_integer, return_symbol, type_spec)
		return return_symbol

class node_float_binary_expr(node_expression):
	#TODO: this should actually generate code that works with any numeric, and calls for dynamic resolution when used with variable type.  To get the language up and running just use floats for all operands and results.
	op_table = {
		"multiply" : " * ",
	    "divide" : " / ",
	    "add" : " + ",
	    "subtract" : " - "
	}
	def get_preferred_type_spec(self, func):
		return func.facet.builtin_type_spec_float
	def analyze(self, func, type_spec):
		self.left.analyze(func, func.facet.builtin_type_spec_float)
		self.right.analyze(func, func.facet.builtin_type_spec_float)
		func.facet.builtin_type_spec_float.check_conversion(type_spec)
	def compile(self, func, return_symbol, type_spec):
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		left_symbol = self.left.compile(func, None, func.facet.builtin_type_spec_float)
		right_symbol = self.right.compile(func, None, func.facet.builtin_type_spec_float)
		if not func.facet.builtin_type_spec_float.is_equivalent(type_spec):
			expression_result = func.add_register(func.facet.builtin_type_spec_float)
		else:
			expression_result = return_symbol
		func.append_code(expression_result + " = " + left_symbol + node_float_binary_expr.op_table[self.op] + right_symbol + ";\n")
		if return_symbol != expression_result:
			func.append_type_conversion(expression_result, func.facet.builtin_type_spec_float, return_symbol, type_spec)
		return return_symbol

class node_int_binary_expr(node_expression):
	op_table = {
		"modulus" : " % ",
		"shift_left" : " << ",
		"shift_right" : " >> ",
		"bitwise_and" : " & ",
		"bitwise_or" : " | ",
		"bitwise_xor" : " ^ "
	}
	def get_preferred_type_spec(self, func):
		return func.facet.builtin_type_spec_integer
	def analyze(self, func, type_spec):
		self.left.analyze(func, func.facet.builtin_type_spec_integer)
		self.right.analyze(func, func.facet.builtin_type_spec_integer)
		func.facet.builtin_type_spec_integer.check_conversion(type_spec)
	def compile(self, func, return_symbol, type_spec):
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		left_symbol = self.left.compile(func, None, func.facet.builtin_type_spec_integer)
		right_symbol = self.right.compile(func, None, func.facet.builtin_type_spec_integer)
		if not func.facet.builtin_type_spec_integer.is_equivalent(type_spec):
			expression_result = func.add_register(func.facet.builtin_type_spec_integer)
		else:
			expression_result = return_symbol
		func.append_code(expression_result + " = " + left_symbol + node_int_binary_expr.op_table[self.op] + right_symbol + ";\n")
		if return_symbol != expression_result:
			func.append_type_conversion(expression_result, func.facet.builtin_type_spec_integer, return_symbol, type_spec)
		return return_symbol

class node_bool_binary_expr(node_expression):
	op_table = {
		"compare_less" : " < ",
		"compare_greater" : " > ",
		"compare_less_or_equal" : " <= ",
		"compare_greater_or_equal" : " >= ",
		"compare_equal" : " == ",
		"compare_not_equal" : " != ",
	    "logical_and" : " && ",
		"logical_or" : " || ",
	}
	def __init__(self):
		node_expression.__init__(self)
		self.operand_type = None
		self.op = None
		self.left = None
		self.right = None

	def get_preferred_type_qualifier(self, func):
		return func.facet.type_dictionary.builtin_type_qualifier_boolean

	def analyze_expr_linkage(self, func):
		self.left.analyze_expr_linkage(func)
		self.right.analyze_expr_linkage(func)

	def analyze_expr_types(self, func, return_type_qualifier):
		self.operand_type = func.facet.type_dictionary.builtin_type_qualifier_float if self.op[0:7] == "compare" else func.facet.type_dictionary.builtin_type_qualifier_boolean

		self.left.analyze_expr_types(func, self.operand_type)
		self.right.analyze_expr_types(func, self.operand_type)
		func.facet.type_dictionary.builtin_type_qualifier_boolean.check_conversion(return_type_qualifier)

	def compile(self, func, return_symbol, type_spec):
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		if not func.facet.type_dictionary.builtin_type_qualifier_boolean.is_equivalent(type_spec):
			expression_result = func.add_register(func.facet.type_dictionary.builtin_type_qualifier_boolean)
		else:
			expression_result = return_symbol

		# two paths here: for float operand types (compares), evaluate both and set the result.  For the logical operations it's a little more complicated: follow the c convention of early out (i.e. in an OR if the first operand is true don't eval the second and in an and if the first operand is false, same.

		if self.operand_type == func.facet.type_dictionary.builtin_type_qualifier_float:
			left_symbol = self.left.compile(func, None, self.operand_type)
			right_symbol = self.right.compile(func, None, self.operand_type)
			func.append_code(expression_result + " = " + left_symbol + node_bool_binary_expr.op_table[self.op] + right_symbol + ";\n")
		else:
			end_label_id = func.get_next_label_id()
			self.left.compile(func, expression_result, self.operand_type)
			test = expression_result if self.op == "logical_or" else "!" + expression_result
			func.append_code("if(" + test + ") " + goto_label(end_label_id))
			self.right.compile(func, expression_result, self.operand_type)
			func.append_code(label(end_label_id))
		if return_symbol != expression_result:
			func.append_type_conversion(expression_result, func.facet.type_dictionary.builtin_type_qualifier_boolean, return_symbol, type_spec)
		return return_symbol


class node_conditional_expr(node_expression):
	# test_expression, true_expression, false_expression
	def get_preferred_type_spec(self, func):
		return self.true_expression.get_preferred_type_spec(func)
	def analyze(self, func, type_spec):
		self.test_expression.analyze(func, func.facet.builtin_type_spec_boolean)
		self.true_expression.analyze(func, type_spec)
		self.false_expression.analyze(func, type_spec)
	def compile(self, func, return_symbol, type_spec):
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		true_label_id = func.get_next_label_id()
		end_label_id = func.get_next_label_id()
		test_symbol = self.test_expression.compile(func, None, func.facet.builtin_type_spec_boolean)
		func.append_code("if(" + test_symbol + ") " + goto_label(true_label_id))
		self.false_expression.compile(func, return_symbol, type_spec)
		func.append_code(goto_label(end_label_id))
		func.append_code(label(true_label_id))
		self.true_expression.compile(func, return_symbol, type_spec)
		func.append_code(label(end_label_id))

class node_assign_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.left = None
		self.right = None
	def get_preferred_type_qualifier(self, func):
		return self.left.get_preferred_type_qualifier(func)
	def analyze_expr_linkage(self, func):
		self.left.analyze_expr_linkage(func)
		self.right.analyze_expr_linkage(func)
	def analyze_expr_types(self, func, result_type_qualifier):
		self.left.analyze_lvalue(func, result_type_qualifier)
		left_type_qualifier = self.left.get_preferred_type_qualifier(func)
		self.right.analyze_expr_types(func, left_type_qualifier)

	def compile(self, func, return_symbol, type_qual):
		lvalue_symbol, lvalue_type = self.left.compile_lvalue(func)
		if return_symbol is None:
			if not type_qual.is_none():
				if type_qual.is_equivalent(lvalue_type):
					return_symbol = lvalue_symbol
				else:
					return_symbol = func.add_register(type_qual)
		self.right.compile(func, lvalue_symbol, lvalue_type)
		if return_symbol is not None and return_symbol != lvalue_symbol:
			func.append_type_conversion(lvalue_symbol, lvalue_type, return_symbol, type_qual)
		return return_symbol

class node_float_assign_expr(node_expression):
	def __init__(self):
		node_expression.__init__(self)
		self.left = None
		self.right = None
		self.op = None

	def get_preferred_type_spec(self, func):
		return self.left.get_preferred_type_spec(func)
	# left, right, op
	def analyze_expr_linkage(self, func):
		self.left.analyze_expr_linkage(func)
		self.right.analyze_expr_linkage(func)

	def analyze_expr_types(self, func, result_type_qualifier):
		float_type = func.facet.type_dictionary.builtin_type_qualifier_float
		self.left.analyze_lvalue(func, float_type)
		self.right.analyze_expr_types(func, float_type)
		float_type.check_conversion(result_type_qualifier)

	def compile(self, func, return_symbol, result_type_qualifier):
		if return_symbol is None and result_type_qualifier.type_kind != type_qualifier.kind.none_type:
			return_symbol = func.add_register(result_type_qualifier)
		lvalue_symbol, lvalue_type = self.left.compile_lvalue(func)
		operand_symbol = self.right.compile(func, None, func.facet.type_dictionary.builtin_type_qualifier_float)
		op_string = " " + node_float_binary_expr.op_table[self.op][1] + "= "
		if lvalue_type.is_equivalent(func.facet.type_dictionary.builtin_type_qualifier_float):
			func.append_code(lvalue_symbol + " " + op_string + operand_symbol + ";\n")
			result_register = lvalue_symbol
		else:
			result_register = func.add_register(func.facet.type_dictionary.builtin_type_qualifier_float)
			func.append_type_conversion(lvalue_symbol, lvalue_type, result_register, func.facet.type_dictionary.builtin_type_qualifier_float)
			func.append_code(result_register + op_string + operand_symbol + ";\n")
			func.append_type_conversion(result_register, func.facet.type_dictionary.builtin_type_qualifier_float, lvalue_symbol, lvalue_type)
		if return_symbol is not None:
			if result_type_qualifier.is_equivalent(func.facet.type_dictionary.builtin_type_qualifier_float):
				func.append_code(return_symbol + " = " + result_register + ";\n")
			else:
				func.append_type_conversion(result_register, func.facet.type_dictionary.builtin_type_qualifier_float, return_symbol, result_type_qualifier)
		return return_symbol

class node_int_assign_expr(node_expression):
	def get_preferred_type_spec(self, func):
		return self.left.get_preferred_type_spec(func)
	# left, right, op
	def analyze(self, func, type_spec):
		self.left.analyze_lvalue(func, func.facet.builtin_type_spec_integer)
		self.right.analyze(func, func.facet.builtin_type_spec_integer)
		func.facet.builtin_type_spec_integer.check_conversion(type_spec)
	def compile(self, func, return_symbol, type_spec):
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		lvalue_symbol, lvalue_type = self.left.compile_lvalue(func)
		operand_symbol = self.right.compile(func, None, func.facet.builtin_type_spec_integer)
		op_string = " " + node_int_binary_expr.op_table[self.op][1] + "= "
		if lvalue_type.is_equivalent(func.facet.builtin_type_spec_integer):
			func.append_code(lvalue_symbol + " " + op_string + operand_symbol + ";\n")
			result_register = lvalue_symbol
		else:
			result_register = func.add_register(func.facet.builtin_type_spec_integer)
			func.append_type_conversion(lvalue_symbol, lvalue_type, result_register, func.facet.builtin_type_spec_integer)
			func.append_code(result_register + op_string + operand_symbol + ";\n")
			func.append_type_conversion(result_register, func.facet.builtin_type_spec_integer, lvalue_symbol, lvalue_type)
		if type_spec.is_equivalent(func.facet.builtin_type_spec_integer):
			func.append_code(return_symbol + " = " + result_register + ";\n")
		else:
			func.append_type_conversion(result_register, func.facet.builtin_type_spec_integer, return_symbol, type_spec)
		return return_symbol

class node_array_expr(node_expression):
	def get_preferred_type_spec(self, func):
		return func.facet.builtin_type_spec_variable
	def analyze(self, func, type_spec):
		if not type_spec.is_container():
			raise compile_error(self, "object is not a container.")
		if not type_spec.is_container_sequential():
			raise compile_error(self, "array can only be assigned to sequentially accessable containers (arrays).")
		container_size = type_spec.get_container_size()
		if container_size != "variable":
			if len(self.array_values) != container_size:
				raise compile_error(self, "expecting " + container_size + " array value initializers; got " + len(self.array_values))
		value_type = type_spec.get_container_value_type()
		for sub_expr in self.array_values:
			sub_expr.analyze(func, value_type)
	def compile(self, func, return_symbol, type_spec):
		# if this is a variable type_spec, create a new array
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		if type_spec.is_equivalent(func.facet.builtin_type_spec_variable):
			func.append_code(return_symbol + " = new array<variable>;")
		if type_spec.get_container_size() == "variable":
			func.append_code("set_array_size(" + return_symbol + ", " + str(len(self.array_values)))
		value_type = type_spec.get_container_value_type()
		for index, value in enumerate(self.array_values):
			value.compile(func, return_symbol + "[" + str(index) + "]", type_spec)

class node_map_expr(node_expression):
	def get_preferred_type_spec(self, func):
		return func.facet.builtin_type_spec_variable
	def analyze(self, func, type_spec):
		if not type_spec.is_container():
			raise compile_error(self, "object is not a container.")
		container_size = type_spec.get_container_size()
		if container_size != "variable":
			raise compile_error(self, "maps cannot be assigned to fixed arrays")
		key_type = type_spec.get_container_key_type()
		value_type = type_spec.get_container_value_type()
		for pair in self.map_pairs:
			pair.key.analyze(func, key_type)
			pair.value.analyze(func, value_type)
	def compile(self, func, return_symbol, type_spec):
		# if this is a variable type_spec, create a new array
		if return_symbol is None:
			return_symbol = func.add_register(type_spec)
		if type_spec.is_equivalent(func.facet.builtin_type_spec_variable):
			func.append_code(return_symbol + " = new array<variable>;\n")
		else:
			func.append_code(return_symbol + ".clear();\n")
		func.append_code(return_symbol + "reserve(" + str(len(self.map_pairs)) + ");\n")

		key_type = type_spec.get_container_key_type()
		value_type = type_spec.get_container_value_type()

		for pair in self.map_pairs:
			key_symbol = pair.key.compile(func, None, key_type)
			pair.value.compile(func, return_symbol + "[" + key_symbol + "]", value_type)

class node_map_pair(program_node):
	# key, value
	pass
