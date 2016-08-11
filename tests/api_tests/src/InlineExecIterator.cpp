/*
 * InlineExecIterator.cpp
 *
 *  Created on: Aug 10, 2016
 *      Author: ballance
 */

#include "InlineExecIterator.h"
#include "TestCallbackContextImpl.h"

InlineExecIterator::InlineExecIterator(
		IModel				*model,
		const std::string	&comp_name,
		const std::string	&action_name,
		const std::string	&initial) {
	m_model = model;
	m_comp_name = comp_name;
	m_action_name = action_name;
	m_initial = initial;
}

InlineExecIterator::~InlineExecIterator() {
	// TODO Auto-generated destructor stub
}

bool InlineExecIterator::traverse() {
	TestCallbackContextImpl *ctxt = dynamic_cast<TestCallbackContextImpl *>(m_model->getCallbackContext());

	if (!ctxt) {
		fprintf(stdout, "Error: CallbackContext is not TestCallbackContextImpl\n");
		return false;
	}

	IComponent *c = 0;
	IAction *a = 0;

	for (std::vector<IBaseItem *>::const_iterator it=m_model->getItems().begin();
			it!=m_model->getItems().end(); it++) {
		if ((*it)->getType() == IBaseItem::TypeComponent &&
				static_cast<IComponent *>(*it)->getName() == m_comp_name) {
			c = static_cast<IComponent *>(*it);
			break;
		}
	}

	if (!c) {
		fprintf(stdout, "Error: call_inline_exec - failed to find component %s\n",
				m_comp_name.c_str());
		return false;
	}

	for (std::vector<IBaseItem *>::const_iterator it=c->getItems().begin();
			it!=c->getItems().end(); it++) {
		if ((*it)->getType() == IBaseItem::TypeAction &&
				static_cast<IAction *>(*it)->getName() == m_action_name) {
			a = static_cast<IAction *>(*it);
			break;
		}
	}

	if (!a) {
		fprintf(stdout, "Error: call_inline_exec - failed to find action %s\n",
				m_action_name.c_str());
		return false;
	}

	ctxt->init(m_initial, m_model, c);

	// Now, traverse through the action hierarchy
	// - Call pre_solve
	// - Call post_solve
	// - Call body
	psshandle_t hndl = ctxt->getActionNode();
	traverse(a, hndl, PreSolve);
	traverse(a, hndl, PostSolve);
	traverse(a, hndl, Body);

}

void InlineExecIterator::traverse(IScopeItem *scope, psshandle_t hndl, cb_t cb) {
	IExecCallback *exec_cb = 0;

	// Figure out whether there's a matching exec
	for (std::vector<IBaseItem *>::const_iterator it=scope->getItems().begin();
			it!=scope->getItems().end(); it++) {
		if ((*it)->getType() == IBaseItem::TypeExec) {
			IExec *exec = static_cast<IExec *>(*it);
			switch (cb) {
			case PreSolve: {
				if (exec->getExecKind() == IExec::PreSolve) {
					exec_cb = exec->getInlineExec();
				}
			} break;
			case PostSolve: {
				if (exec->getExecKind() == IExec::PostSolve) {
					exec_cb = exec->getInlineExec();
				}
			} break;
			case Body: {
				if (exec->getExecKind() == IExec::Body) {
					exec_cb = exec->getInlineExec();
				}
			} break;
			}
		}
	}

	if (exec_cb) {
		exec_cb->exec(m_model, hndl);
	}
}
