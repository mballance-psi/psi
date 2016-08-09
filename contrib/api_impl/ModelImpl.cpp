/*
 * ModelImpl.cpp
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *  Created on: Apr 26, 2016
 *      Author: ballance
 */

#include "ModelImpl.h"

#include <stdio.h>
#include <iterator>
#include <string>
#include <vector>

#include "ActionImpl.h"
#include "BinaryExprImpl.h"
#include "BindImpl.h"
#include "BindPathImpl.h"
#include "ComponentImpl.h"
#include "ConstraintBlockImpl.h"
#include "ConstraintExprImpl.h"
#include "ConstraintIfImpl.h"
#include "ExecImpl.h"
#include "ExtendImpl.h"
#include "FieldImpl.h"
#include "FieldRefImpl.h"
#include "GraphBlockStmtImpl.h"
#include "GraphRepeatStmtImpl.h"
#include "GraphTraverseStmtImpl.h"
#include "LiteralImpl.h"
#include "ScalarTypeImpl.h"
#include "StructImpl.h"
#include "api/IBaseItem.h"

namespace psi {

ModelImpl::ModelImpl() : m_global_pkg("") {
	m_callback_ctxt = 0;

}

ModelImpl::~ModelImpl() {
	// TODO Auto-generated destructor stub
}

//const std::vector<IPackage*> &ModelImpl::getPackages() {
//	return m_packages;
//}

const std::vector<IBaseItem *> &ModelImpl::getItems() const {
	return m_children;
}

void ModelImpl::add(IBaseItem *it) {
	m_children.push_back(it);
}

IPackage *ModelImpl::getGlobalPackage() {
	return &m_global_pkg;
}

IPackage *ModelImpl::findPackage(const std::string &name, bool create) {
	std::vector<IBaseItem *>::iterator it;

	for (it=m_children.begin(); it!=m_children.end(); it++) {
		if ((*it)->getType() == IBaseItem::TypePackage) {
			IBaseItem *bi = *it;
			IPackage *pkg_ir = reinterpret_cast<IPackage *>(bi);
			IPackage *pkg_is = static_cast<IPackage *>(bi);
			IPackage *pkg = reinterpret_cast<IPackage *>(*it);
			if (pkg->getName() == name) {
				return pkg;
			}
		}
	}

	if (create) {
		PackageImpl *pkg = new PackageImpl(name);
		m_children.push_back(pkg);
		return pkg;
	} else {
		return 0;
	}
}

/**
 * Data Types
 */

/**
 * Creates a scalar type. The msb and lsb parameters are ignored for types
 * other than Int and Bit
 */
IScalarType *ModelImpl::mkScalarType(
		IScalarType::ScalarType t,
		uint32_t				msb,
		uint32_t				lsb) {
	if (t != IScalarType::ScalarType_Bit && t != IScalarType::ScalarType_Int) {
		msb = 0;
		lsb = 0;
	}
	return new ScalarTypeImpl(t, msb, lsb);
}

IBind *ModelImpl::mkBind(const std::vector<IBindPath *> &targets) {
	return new BindImpl(targets);
}

IBindPath *ModelImpl::mkBindPath(const std::vector<IBaseItem *> &path) {
	return new BindPathImpl(path);
}

/**
 * Action
 */
IAction *ModelImpl::mkAction(const std::string &name, IAction *super_type) {
	return new ActionImpl(name, super_type);
}

IComponent *ModelImpl::mkComponent(const std::string &name) {
	return new ComponentImpl(name);
}

IExec *ModelImpl::mkTargetTemplateExec(
		IExec::ExecKind			kind,
		const std::string		&language,
		const std::string		&text) {
	return new ExecImpl(kind, language, text);
}

IExec *ModelImpl::mkInlineExec(
		IExec::ExecKind			kind,
		IExecCallback				*exec) {
	return new ExecImpl(kind, exec);
}

IExec *ModelImpl::mkNativeExec(
		IExec::ExecKind			kind,
		IExpr					*stmts) {
	return new ExecImpl(kind, stmts);
}

IStruct *ModelImpl::mkStruct(
		const std::string 		&name,
		IStruct::StructType		t,
		IStruct 				*super_type) {
	return new StructImpl(name, t, super_type);
}

IExtend *ModelImpl::mkExtend(IBaseItem *target) {
	return new ExtendImpl(target);
}

/**
 * Create a field for use in declaring the contents of an
 * action or struct data type
 */
IField *ModelImpl::mkField(
		const std::string		&name,
		IBaseItem				*field_type,
		IField::FieldAttr		attr,
		IExpr					*array_dim) {
	return new FieldImpl(name, field_type, attr, array_dim);
}

IGraphBlockStmt *ModelImpl::mkGraphBlockStmt(IGraphStmt::GraphStmtType type) {
	return new GraphBlockStmtImpl(type);
}

IGraphRepeatStmt *ModelImpl::mkGraphRepeatStmt(
		IGraphRepeatStmt::RepeatType type,
		IExpr *expr, IGraphStmt *body) {
	return new GraphRepeatStmtImpl(type, expr, body);
}

IGraphTraverseStmt *ModelImpl::mkGraphTraverseStmt(
		IFieldRef *action, IConstraint *with_c) {
	return new GraphTraverseStmtImpl(action, with_c);
}

IBinaryExpr *ModelImpl::mkBinExpr(
		IExpr 					*lhs,
		IBinaryExpr::BinOpType	op,
		IExpr 					*rhs) {
	return new BinaryExprImpl(lhs, op, rhs);
}

IFieldRef *ModelImpl::mkFieldRef(
		const std::vector<IField *>		&field_path) {
	return new FieldRefImpl(field_path);
}

ILiteral *ModelImpl::mkIntLiteral(int64_t v) {
	return new LiteralImpl(v);
}

ILiteral *ModelImpl::mkBitLiteral(uint64_t v) {
	return new LiteralImpl(v);
}

ILiteral *ModelImpl::mkBoolLiteral(bool v) {
	return new LiteralImpl(v);
}

ILiteral *ModelImpl::mkStringLiteral(const std::string &v) {
	return new LiteralImpl(v);
}

IConstraintBlock *ModelImpl::mkConstraintBlock(
		const std::string 		&name,
		IConstraint				*c) {
	IConstraintBlock *ret = new ConstraintBlockImpl(name);

	if (c) {
		ret->add(c);
	}

	return ret;
}

IConstraintBlock *ModelImpl::mkConstraintBlock(
		const std::string 					&name,
		const std::vector<IConstraint *>	&cl) {
	IConstraintBlock *ret = new ConstraintBlockImpl(name);

	ret->add(cl);

	return ret;
}

IConstraintExpr *ModelImpl::mkConstraintExpr(IExpr *expr) {
	return new ConstraintExprImpl(expr);
}


IConstraintIf *ModelImpl::mkConstraintIf(
		IExpr 			*cond,
		IConstraint 	*true_c,
		IConstraint 	*false_c) {
	return new ConstraintIfImpl(cond, true_c, false_c);
}

ICallbackContext *ModelImpl::getCallbackContext() {
	if (!m_callback_ctxt) {
		fprintf(stdout, "Error: getCallbackContext returning null handle\n");
		fflush(stdout);
	}
	return m_callback_ctxt;
}

void ModelImpl::setCallbackContext(ICallbackContext *ctxt) {
	m_callback_ctxt = ctxt;
}

} /* namespace psi */
