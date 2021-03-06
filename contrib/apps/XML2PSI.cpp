/*
 * XML2PSI.cpp
 *
 *  Created on: May 17, 2016
 *      Author: ballance
 */

#include "XML2PSI.h"
#include "PSIUtil.h"
#include <stdio.h>
#include <assert.h>

namespace psi {
namespace apps {

XML2PSI::XML2PSI() {
	// TODO Auto-generated constructor stub

}

void XML2PSI::process(const std::string &content, IModel *model) {
	LIBXML_TEST_VERSION
	m_model = model;

	xmlDocPtr doc;

	// TODO: call validater
	doc = xmlReadMemory(
			content.c_str(),
			content.length(),
			"content.xml",
			0,
			0);

	xmlNode *n = xmlDocGetRootElement(doc);

	for (; n; n=n->next) {
		if (n->type != XML_ELEMENT_NODE) {
			continue;
		}

		std::string name;
		name.append(reinterpret_cast<const char *>(n->name));

		if (name == "model") {
			elaborate_model(n);
		} else {
			fprintf(stdout, "Error: unhandled root element %s\n",
					name.c_str());
		}
	}

	xmlFreeDoc(doc);
}

XML2PSI::~XML2PSI() {
	// TODO Auto-generated destructor stub
}

void XML2PSI::elaborate_model(xmlNode *m) {
	strmap attr;

	for (xmlNode *n=m->children; n; n=n->next) {

		if (n->type != XML_ELEMENT_NODE) {
			continue;
		}
		std::string name(reinterpret_cast<const char *>(n->name));

		get_attributes(n, attr);
		if (name == "package") {
			elaborate_package(n, attr);
		} else {
			IBaseItem *it = 0;

			if (name == "component") {
				it = elaborate_component(n, attr);
			} else if (name == "struct") {
				it = elaborate_struct(n, attr);
			} else {
				fprintf(stdout, "Error: unhandled model element %s\n",
						name.c_str());
			}

			if (it) {
				if (name == "component") {
					m_model->add(it);
				} else {
					m_model->getGlobalPackage()->add(it);
				}
			} else {
				fprintf(stdout, "Error: Failed to elaborate %s\n", name.c_str());
			}
		}
	}
}

IAction *XML2PSI::elaborate_action(xmlNode *p, const strmap &attr) {
	strmap attr_m;
	IAction *super = 0;

	if (attr.find("super") != attr.end()) {
		// TODO: look up super
		fprintf(stdout, "Error: action 'super' unsupported\n");
	}

	IAction *a = m_model->mkAction(attr.find("name")->second, super);

	for (xmlNode *n=p->children; n; n=n->next) {
		if (n->type != XML_ELEMENT_NODE) {
			continue;
		}

		get_attributes(n, attr_m);
		a->add(elaborate_action_struct_component_item(n, attr_m));
	}

	return a;
}

IComponent *XML2PSI::elaborate_component(xmlNode *p, const strmap &attr) {
	IComponent *c = m_model->mkComponent(attr.find("name")->second);
	strmap attr_m;

	for (xmlNode *n=p->children; n; n=n->next) {
		if (n->type != XML_ELEMENT_NODE) {
			continue;
		}

		std::string name(reinterpret_cast<const char *>(n->name));
		get_attributes(n, attr_m);

		if (name == "action") {
			c->add(elaborate_action(n, attr_m));
		} else if (name == "field") {
			c->add(elaborate_field(n, attr_m));
		} else {
			fprintf(stdout, "Error: unhandled component element %s\n",
					name.c_str());
		}
	}

	return c;
}

IConstraint *XML2PSI::elaborate_constraint_set(xmlNode *p, const strmap &attr) {
	IConstraint *ret = 0;

	std::string name = reinterpret_cast<const char *>(p->name);

	fprintf(stdout, "elaborate_constraint_set: %s\n", name.c_str());
	fflush(stdout);

	if (name == "stmt") {
		// Find the first child node
		xmlNode *n = p->children;

		while (n && n->type != XML_ELEMENT_NODE) { n=n->next; }
		strmap attr_n;

		get_attributes(n, attr_n);

		ret = m_model->mkConstraintExpr(elaborate_expr(n, attr_n));
	} else if (name == "if") {
		fprintf(stdout, "Error: unsupported constraint %s\n", name.c_str());
	} else if (name == "block") {
		std::vector<IConstraint *> cl;

		for (xmlNode *cn=p->children; cn; cn=cn->next) {
			if (cn->type != XML_ELEMENT_NODE) {
				continue;
			}

			strmap attr_m;
			get_attributes(cn, attr_m);
			cl.push_back(elaborate_constraint_set(cn, attr_m));
		}
		ret = m_model->mkConstraintBlock(name, cl);
	} else if (name == "foreach") {
		fprintf(stdout, "Error: unsupported constraint %s\n", name.c_str());
	} else if (name == "implies") {
		fprintf(stdout, "Error: unsupported constraint %s\n", name.c_str());
	} else {
		fprintf(stdout, "Error: unknown constraint %s\n", name.c_str());
	}

	return ret;
}

IConstraintBlock *XML2PSI::elaborate_constraint_declaration(xmlNode *p, const strmap &attr) {
	std::vector<IConstraint *> cl;
	IConstraintBlock *c = 0;
	std::string c_name;

	fprintf(stdout, "elaborate_constraint_declaration: %s\n", p->name);

	if (attr.find("name") != attr.end()) {
		c_name = attr.find("name")->second;
	}

	for (xmlNode *cn=p->children; cn; cn=cn->next) {
		if (cn->type != XML_ELEMENT_NODE) {
			continue;
		}

		strmap attr_m;
		get_attributes(cn, attr_m);
		cl.push_back(elaborate_constraint_set(cn, attr_m));
	}

	c = m_model->mkConstraintBlock(c_name, cl);

	return c;
}

IExpr *XML2PSI::elaborate_expr(xmlNode *p, const strmap &attr) {
	std::string name;
	IExpr *ret = 0;

	// Skip leading non-element nodes

	name = reinterpret_cast<const char *>(p->name);

	fprintf(stdout, "elaborate_expr: %s\n", name.c_str());
	fflush(stdout);

	if (name == "binexp") {
		xmlNode *lhs = 0;
		xmlNode *rhs = 0;

		for (xmlNode *cn=p->children; cn; cn=cn->next) {
			if (cn->type != XML_ELEMENT_NODE) {
				continue;
			}
			name = reinterpret_cast<const char *>(cn->name);


			if (name == "lhs") {
				lhs = cn;
			} else if (name == "rhs") {
				rhs = cn;
			} else {
				fprintf(stdout, "Error: unknown binexp child %s\n", name.c_str());
			}
		}

		assert(lhs);
		assert(rhs);

		// Find the first element child of lhs and rhs
		lhs = lhs->children;
		while (lhs && lhs->type != XML_ELEMENT_NODE) { lhs = lhs->next; }

		rhs = rhs->children;
		while (rhs && rhs->type != XML_ELEMENT_NODE) { rhs = rhs->next; }

		assert(lhs);
		assert(rhs);

		strmap attr_lhs, attr_rhs;
		get_attributes(lhs, attr_lhs);
		get_attributes(rhs, attr_rhs);

		IBinaryExpr::BinOpType op = toBinOp(attr.find("op")->second);

		ret = m_model->mkBinExpr(
				elaborate_expr(lhs, attr_lhs),
				toBinOp(attr.find("op")->second),
				elaborate_expr(rhs, attr_rhs));
	} else if (name == "unexp") {
		fprintf(stdout, "Error: unhandled expr element %s\n", name.c_str());
	} else if (name == "inside") {
		fprintf(stdout, "Error: unhandled expr element %s\n", name.c_str());
	} else if (name == "number") {
		const char *v = reinterpret_cast<const char *>(xmlNodeListGetString(p->doc, p->children, 1));
		uint64_t v_i = strtoul(v, 0, 0);
		ret = m_model->mkIntLiteral(v_i);
	} else if (name == "paren") {
		fprintf(stdout, "Error: unhandled expr element %s\n", name.c_str());
	} else if (name == "ref") {
		IBaseItem *ctxt = m_scope_stack.top();
		IField *f = 0;
		std::vector<IField *> path;

		for (xmlNode *cn = p->children; cn; cn=cn->next) {
			if (cn->type != XML_ELEMENT_NODE) {
				continue;
			}

			std::string c_name = reinterpret_cast<const char *>(cn->name);

			if (c_name == "path") {
				std::string v = reinterpret_cast<const char *>(xmlNodeListGetString(cn->doc, cn->children, 1));

				IScopeItem *scope;

				if (!(scope = dynamic_cast<IScopeItem *>(ctxt))) {
					fprintf(stdout, "Error: context isnt't a scope\n");
				}

				IField *f_i = 0;
				// Search for this element in the current scope
				for (std::vector<IBaseItem *>::const_iterator it=scope->getItems().begin();
						it!=scope->getItems().end(); it++) {
					if ((f_i = dynamic_cast<IField *>(*it)) && f_i->getName() == v) {
						break;
					} else {
						f_i = 0;
					}
				}

				if (!f_i) {
					fprintf(stdout, "Error: Failed to find element %s\n", v.c_str());
					f = 0;
					break;
				} else {
					path.push_back(f_i);
					f = f_i;
					ctxt = f_i->getDataType();
				}
			} else {
				fprintf(stdout, "Error: unknown ref sub-element %s\n", c_name.c_str());
			}
		}

		if (!f) {
			fprintf(stdout, "Error: Failed to find referenced field\n");
		}

		ret = m_model->mkFieldRef(path);
	} else if (name == "call") {
		fprintf(stdout, "Error: unhandled expr element %s\n", name.c_str());
	} else {
		fprintf(stdout, "Error: unknown expr element %s\n", name.c_str());
	}

	return ret;
}

IBaseItem *XML2PSI::elaborate_type(xmlNode *p, const strmap &attr) {
	IBaseItem *type = 0;
	xmlNode *n = p->children;
	std::string name;

	while (n && n->type != XML_ELEMENT_NODE) { n = n->next; }

	name = reinterpret_cast<const char *>(n->name);

	if (name == "int" || name == "bit") {
		strmap attr_m;

		// Must look inside the containing element

		IExpr *msb = 0, *lsb=0;

		for (n=n->children; n; n=n->next) {
			if (n->type != XML_ELEMENT_NODE) {
				continue;
			}

			std::string c_name = reinterpret_cast<const char *>(n->name);

			if (c_name == "msb") {
				xmlNode *cn = n->children;
				while (cn && cn->type != XML_ELEMENT_NODE) { cn=cn->next; }
				assert(cn);
				get_attributes(cn, attr_m);
				msb = elaborate_expr(cn, attr_m);
			} else if (c_name == "lsb") {
				xmlNode *cn = n->children;
				while (cn && cn->type != XML_ELEMENT_NODE) { cn=cn->next; }
				assert(cn);
				get_attributes(cn, attr_m);
				lsb = elaborate_expr(cn, attr_m);
			}
		}

		if (name == "int") {
			type = m_model->mkScalarType(IScalarType::ScalarType_Int, msb, lsb);
		} else {
			type = m_model->mkScalarType(IScalarType::ScalarType_Bit, msb, lsb);
		}
	} else if (name == "bool") {
		type = m_model->mkScalarType(IScalarType::ScalarType_Bool, 0, 0);
	} else if (name == "chandle") {
		type = m_model->mkScalarType(IScalarType::ScalarType_Chandle, 0, 0);
	} else if (name == "string") {
		type = m_model->mkScalarType(IScalarType::ScalarType_String, 0, 0);
	} else if (name == "user") {
		type = find_type(n);
	} else {
		// user-defined type
		fprintf(stdout, "Error: Unhandled type \"%s\"\n", name.c_str());
	}

	return type;
}


IField *XML2PSI::elaborate_field(xmlNode *p, const strmap &attr) {
	IField *field = 0;
	IField::FieldAttr field_attr = IField::FieldAttr_None;
	strmap attr_m;
	strmap::const_iterator q = attr.find("qualifier");

	if (q != attr.end()) {
		if (q->second == "rand") {
			field_attr = IField::FieldAttr_Rand;
		} else if (q->second == "input") {
			field_attr = IField::FieldAttr_Input;
		} else if (q->second == "output") {
			field_attr = IField::FieldAttr_Output;
		} else if (q->second == "lock") {
			field_attr = IField::FieldAttr_Lock;
		} else if (q->second == "share") {
			field_attr = IField::FieldAttr_Share;
		} else if (q->second == "pool") {
			field_attr = IField::FieldAttr_Pool;
		} else {
			// TODO: Error
		}
	}

	IBaseItem *field_t = 0;
	xmlNode *n = p->children;
	std::string name;

	while (n && n->type != XML_ELEMENT_NODE) { n = n->next; }

	// Expect this to be 'type'
	name = reinterpret_cast<const char *>(n->name);

	if (name == "type") {
		get_attributes(n, attr_m);
		field_t = elaborate_type(n, attr_m);
	} else {
		fprintf(stdout, "Error: unhandled field element %s\n", name.c_str());
	}

	// May have an array dimension component
	IExpr *array_dim = 0;
	if (n) {
		n = n->next;
		while (n && n->type != XML_ELEMENT_NODE) { n = n->next; }

		if (n) {
			name = reinterpret_cast<const char *>(n->name);

			if (name == "dim") {
				get_attributes(n, attr_m);
				array_dim = elaborate_expr(n, attr_m);
			} else {
				fprintf(stdout, "Error: unhandled field element %s\n", name.c_str());
			}
		}
	}

	field = m_model->mkField(
			attr.at("name"),
			field_t,
			field_attr,
			array_dim);

	return field;
}

void XML2PSI::elaborate_package(xmlNode *p, const strmap &attr) {
	std::string name;
	strmap attr_m;
	IPackage *pkg;

	if (attr.find("name") != attr.end()) {
		name = attr.find("name")->second;
	}

	if (name == "") {
		pkg = m_model->getGlobalPackage();
	} else {
		pkg = m_model->findPackage(name, true);
	}

	for (xmlNode *n=p->children; n; n=n->next) {
		if (n->type != XML_ELEMENT_NODE) {
			continue;
		}
		std::string name(reinterpret_cast<const char *>(n->name));
		get_attributes(n, attr_m);

		if (name == "action") {
			fprintf(stdout, "Error: unhandled package element %s\n", name.c_str());
		} else if (name == "struct") {
			pkg->add(elaborate_struct(n, attr_m));
		} else {
			fprintf(stdout, "Error: unhandled package element %s\n",
					name.c_str());
		}
	}
}

IStruct *XML2PSI::elaborate_struct(xmlNode *p, const strmap &attr) {
	strmap::const_iterator name_i = attr.find("name");
	strmap::const_iterator qual_i = attr.find("qualifier");

	IStruct::StructType type = IStruct::Base;

	if (qual_i != attr.end()) {
		// Qualfied
		if (qual_i->second == "memory") {
			type = IStruct::Memory;
		} else if (qual_i->second == "stream") {
			type = IStruct::Stream;
		} else if (qual_i->second == "state") {
			type = IStruct::State;
		} else if (qual_i->second == "resource") {
			type = IStruct::Resource;
		}
	}

	xmlNode *c = p->children;
	IStruct *super_type = 0;

	if (c && std::string("super") == reinterpret_cast<const char *>(c->name)) {
		// TODO: locate supertype
		c = c->next;
	}

	IStruct *ret = m_model->mkStruct(
			name_i->second, type, super_type);
	m_scope_stack.push(ret);

	// Process body items
	for (; c; c=c->next) {
		strmap attr;
		if (c->type != XML_ELEMENT_NODE) {
			continue;
		}

		get_attributes(c, attr);

		ret->add(elaborate_action_struct_component_item(c, attr));
	}

	m_scope_stack.pop();

	return ret;
}

IBaseItem *XML2PSI::elaborate_action_struct_component_item(xmlNode *p, const strmap &attr) {
	IBaseItem *ret = 0;
	std::string p_n = reinterpret_cast<const char *>(p->name);

	if (p_n == "field") {
		ret = elaborate_field(p, attr);
	} else if (p_n == "constraint") {
		ret = elaborate_constraint_declaration(p, attr);
	} else {
		fprintf(stdout, "Error: unhandled action_struct_component_item %s\n", p_n.c_str());
	}

	return ret;
}

void XML2PSI::enter_action(const strmap &attr) {
	IAction *super = 0;

	if (attr.find("super") != attr.end()) {
		// TODO: look up super
	}

	IAction *action = m_model->mkAction(
			attr.find("name")->second, super);
	PSIUtil::toScopeItem(top())->add(action);
	push(action);
}

void XML2PSI::enter_component(const strmap &attr) {
	IComponent *comp = m_model->mkComponent(attr.find("name")->second);

	PSIUtil::toScopeItem(top())->add(comp);
	push(comp);
}

void XML2PSI::enter_field(const strmap &attr) {
	IField::FieldAttr field_attr = IField::FieldAttr_None;
	strmap::const_iterator type = attr.find("type");

	if (type != attr.end()) {
		if (type->second == "rand") {
			field_attr = IField::FieldAttr_Rand;
		} else if (type->second == "input") {
			field_attr = IField::FieldAttr_Input;
		} else if (type->second == "output") {
			field_attr = IField::FieldAttr_Output;
		} else if (type->second == "lock") {
			field_attr = IField::FieldAttr_Lock;
		} else if (type->second == "share") {
			field_attr = IField::FieldAttr_Share;
		} else if (type->second == "pool") {
			field_attr = IField::FieldAttr_Pool;
		} else {
			// TODO: Error
		}
	}

	// TODO: handle array_dim
	IField *field = m_model->mkField(attr.at("name"), 0, field_attr, 0);

	PSIUtil::toScopeItem(top())->add(field);
	push(field);
}

void XML2PSI::enter_literal(const strmap &attr) {
	strmap::const_iterator type_i = attr.find("type");
	strmap::const_iterator value_i = attr.find("value");

	// TODO:
	push(0);
}

void XML2PSI::enter_package(const strmap &attr) {
	std::string name;

	if (attr.find("name") != attr.end()) {
		name = attr.find("name")->second;
	}

	if (name == "") {
		push(m_model->getGlobalPackage());
	} else {
		push(m_model->findPackage(name, true));
	}
}

void XML2PSI::enter_struct_type(const strmap &attr) {
	std::string type = attr.find("type")->second;
	IBaseItem *type_h = 0;

	std::vector<std::string> path = split_path(type);

	IBaseItem *p = pop(); // parent -- probably a field
	IScopeItem *s = PSIUtil::toScopeItem(top());
	push(p); // replace the parent

	type_h = find_type(s, path);

	if (top()->getType() == IBaseItem::TypeField) {
		dynamic_cast<IField *>(top())->setDataType(type_h);
		push(type_h);
	} else {
		// not a field
		fprintf(stdout, "Error: Field not found on the stack\n");
	}
}

void XML2PSI::enter_struct(const strmap &attr) {
	strmap::const_iterator name_it = attr.find("name");
	strmap::const_iterator super_it = attr.find("super");
	strmap::const_iterator type_it = attr.find("type");
	std::string name;
	IStruct *super=0;
	IStruct::StructType type=IStruct::Base;

	if (type_it != attr.end()) {
		if (type_it->second == "memory") {
			type = IStruct::Memory;
		} else if (type_it->second == "stream") {
			type = IStruct::Stream;
		} else if (type_it->second == "state") {
			type = IStruct::State;
		} else if (type_it->second == "resource") {
			type = IStruct::Resource;
		} else {
			// TODO: Error handling
		}
	}

	IStruct *s = m_model->mkStruct(name, type, super);
	IScopeItem *scope = PSIUtil::toScopeItem(top());

	if (scope) {
		scope->add(s);
		push(s);
	} else {
		fprintf(stdout, "top %p %d is not a scope\n",
				top(), top()->getType());
	}
}

void XML2PSI::start(const std::string &el, const char **attr) {
	strmap attr_m;

	for (int i=0; attr[i]; i+=2) {
		attr_m.insert(strmap_ent(attr[i], attr[i+1]));
	}

//	if (el == "model") {
//		push(m_model);
//	} else if (el == "action") {
//		enter_action(attr_m);
//	} else if (el == "bit" || el == "int") {
//		enter_bit_int_type((el == "bit"), attr_m);
//	} else if (el == "component") {
//		enter_component(attr_m);
//	} else if (el == "literal") {
//		enter_literal(attr_m);
//	} else if (el == "null") {
//		push(0);
//	} else if (el == "package") {
//		enter_package(attr_m);
//	} else if (el == "field") {
//		enter_field(attr_m);
//	} else if (el == "struct") {
//		if (top()->getType() == IBaseItem::TypeField) {
//			// Scope is <type>
//			enter_struct_type(attr_m);
//		} else {
//			enter_struct(attr_m);
//		}
//	} else {
//		enter_unhandled(el, attr_m);
//		push(0);
//	}
}

void XML2PSI::end(const std::string &el) {
	pop();
//	fprintf(stdout, "</%s> %s\n", el.c_str(), m_scope_stack.empty()?"true":"false");
//	fflush(stdout);
}

IBaseItem *XML2PSI::find_type(IScopeItem *curr, const std::vector<std::string> &path) {
	IBaseItem *ret = 0;

	if (path.size() == 1) {
		// First look in the containing scope
		if (curr) {
			for (std::vector<IBaseItem *>::const_iterator it=curr->getItems().begin();
					it!=curr->getItems().end(); it++) {
				INamedItem *n_it = PSIUtil::toNamedItem(*it);
				if (n_it && n_it->getName() == path.at(0)) {
					ret = *it;
					break;
				}
			}
		}

		// If we didn't find it there, look in the global scope
		if (!ret) {
			const std::vector<IBaseItem *> &items =
					m_model->getGlobalPackage()->getItems();
			for (std::vector<IBaseItem *>::const_iterator it=items.begin();
					it!=items.end(); it++) {
				INamedItem *n_it = PSIUtil::toNamedItem(*it);

				if (n_it && n_it->getName() == path.at(0)) {
					ret = *it;
					break;
				}
			}
		}
	} else {
		// absolute path (?)
		fprintf(stdout, "Error: TODO - qualified scope search\n");
		fflush(stdout);

		IScopeItem *scope = m_model;

		for (std::vector<std::string>::const_iterator p_it=path.begin();
				p_it!=path.end(); p_it++) {
			IBaseItem *n = 0;
			for (std::vector<IBaseItem *>::const_iterator it=scope->getItems().begin();
					it!=scope->getItems().end(); it++) {
				INamedItem *n_it = PSIUtil::toNamedItem(*it);
				if (n_it && n_it->getName() == *p_it) {
					n = *it;
					break;
				}
			}

			if (n) {
				scope = PSIUtil::toScopeItem(n);
				if (!scope) {
					break;
				}
			} else {
				scope = 0;
				break;
			}
		}

		ret = dynamic_cast<IBaseItem *>(scope);
	}

	return ret;
}

std::vector<std::string> XML2PSI::split_path(const std::string &path) {
	std::vector<std::string> ret;

	uint32_t i;
	int32_t start=0;

	for (i=0; i<path.size(); i++) {
		if (path.at(i) == ':') {
			ret.push_back(path.substr(start, i-start));
			i++; // skip next colon
			start = i;
		}
	}

	ret.push_back(path.substr(start, i-start));

	return ret;
}

void XML2PSI::push(IBaseItem *it) {
	m_scope_stack.push(it);
}

IBaseItem *XML2PSI::pop() {
	IBaseItem *ret = m_scope_stack.top();
	m_scope_stack.pop();
	return ret;
}

IBaseItem *XML2PSI::top() {
	return m_scope_stack.top();
}

void XML2PSI::get_attributes(xmlNode *n, strmap &attr_m) {
	attr_m.clear();

	for (xmlAttrPtr attr=n->properties; attr; attr=attr->next) {
		std::string name = reinterpret_cast<const char *>(attr->name);
		std::string value = reinterpret_cast<const char *>(xmlGetProp(n, attr->name));
		attr_m.insert(strmap_ent(name, value));
	}
}

IBinaryExpr::BinOpType XML2PSI::toBinOp(const std::string &op) {
	if (op == "Eq") {
		return IBinaryExpr::BinOp_Eq;
	} else if (op == "EqEq") {
		return IBinaryExpr::BinOp_EqEq;
	} else if (op == "NotEq") {
		return IBinaryExpr::BinOp_NotEq;
	} else if (op == "GE") {
		return IBinaryExpr::BinOp_GE;
	} else if (op == "GT") {
		return IBinaryExpr::BinOp_GT;
	} else if (op == "LE") {
		return IBinaryExpr::BinOp_LE;
	} else if (op == "LT") {
		return IBinaryExpr::BinOp_LT;
	} else if (op == "And") {
		return IBinaryExpr::BinOp_And;
	} else if (op == "AndAnd") {
		return IBinaryExpr::BinOp_AndAnd;
	} else if (op == "Or") {
		return IBinaryExpr::BinOp_Or;
	} else if (op == "OrOr") {
		return IBinaryExpr::BinOp_OrOr;
	} else if (op == "Minus") {
		return IBinaryExpr::BinOp_Minus;
	} else if (op == "Plus") {
		return IBinaryExpr::BinOp_Plus;
	} else if (op == "Mul") {
		return IBinaryExpr::BinOp_Multiply;
	} else if (op == "Mod") {
		return IBinaryExpr::BinOp_Mod;
	} else if (op == "ArrRef") {
		return IBinaryExpr::BinOp_ArrayRef;
	} else {
		fprintf(stdout, "Error: Unknown binary op %s\n", op.c_str());
		return IBinaryExpr::BinOp_Eq;
	}
}

IBaseItem *XML2PSI::find_type(xmlNode *p) {
	IScopeItem *scope = 0;
//	dynamic_cast<IScopeItem *>(m_scope_stack.top());
	std::vector<std::string> pl;

	for (xmlNode *c=p->children; c; c=c->next) {
		if (c->type != XML_ELEMENT_NODE) {
			continue;
		}

		std::string path = reinterpret_cast<const char *>(
				xmlNodeListGetString(c->doc, c->children, 1));
		pl.push_back(path);
	}

	return find_type(dynamic_cast<IScopeItem *>(m_scope_stack.top()), pl);
}

}
} /* namespace psi */
