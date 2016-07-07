/*
 * classlis_dac_tutorial_data_flow.cpp
 *
 *  Created on: May 10, 2016
 *      Author: ballance
 */

#include "psi_tests.h"

class S : public Struct {
public:

	S(const Scope &p, psi_name &name="S") : Struct(p, name) { }

};
psi_global_type(S);

class CA : public Component {
public:
	CA(BaseItem *p=0, psi_name name="CA") : Component(p, name) { }

	class A : public Action {
	public:
		A(const Scope &p, psi_name name="A") : Action(p, name) { }

		Output<S>			out_s {this, "out_s"};
	};
	psi_type(A);

};
psi_global_type(CA);

class CB : public Component {
public:
	CB(BaseItem *p=0, psi_name name="CB") : Component(p, name) { }

	class B : public Action {
	public:
		B(const Scope &p=0, psi_name name) : Action(p, name) { }

		Input<S>			in_s {this, "in_s"};
	};
	psi_type(B);
};
psi_global_type(CB);

class static_structure : public Component {
public:

	Field<CA>			ca1 {this, "ca1"};
	Field<CA>			ca2 {this, "ca2"};
	Field<CB>			cb1 {this, "cb1"};
	Field<CB>			cb2 {this, "cb2"};

	static_structure(BaseItem *p=0, psi_name name="static_structure")
		: Component(p, name) { }

	Bind b2 { this,
			ca1._A_t.out_s, ca2._A_t.out_s,
			cb1._B_t.in_s, cb2._B_t.in_s };

	class entry_point : public Action {
	public:
		entry_point(const Scope &p=0, psi_name name) : Action(p, name) { }

	};
	psi_type(entry_point);

};
psi_global_type(static_structure);



