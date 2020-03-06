//
// Created by KH on 3/5/2020.
//

#include "Domain.h"

namespace igen {

VarEntry::VarEntry(PContext ctx) : Object(move(ctx)), zvar(zctx()) {}

expr VarEntry::eq(int val) const { return zvar_eq_val.at(val); }

//===================================================================================

void intrusive_ptr_release(Domain *d) {
    intrusive_ptr_add_ref(d);
}

Domain::Domain(PContext ctx) : Object(move(ctx)) {

}

std::istream &Domain::parse(std::istream &input) {
    CHECK(input);

    n_all_values_ = 0;

    std::string line;
    while (std::getline(input, line)) {
        std::stringstream ss(line);
        str name;
        vec<str> labels;

        ss >> name;
        if (name.empty() || name[0] == '#') continue;

        std::string val;
        while (ss >> val) {
            if (val.empty() || val[0] == '#') break;
            labels.push_back(std::move(val));
        }
        int n_vals = (int) labels.size();

        //===
        z3::expr zvar = zctx().int_const(name.c_str());
        zsolver().add(0 <= zvar && zvar < n_vals);
        PVarEntry entry = vars.emplace_back(new VarEntry(ctx));
        entry->id_ = (int) vars.size() - 1;
        entry->name_ = name;
        entry->labels_ = move(labels);

        entry->zvar_eq_val.reserve(n_vals);
        for (int i = 0; i < n_vals; i++)
            entry->zvar_eq_val.push_back(zvar == i);

        n_all_values_ += n_vals;
    }

    return input;
}


std::istream &operator>>(std::istream &input, Domain &d) {
    return d.parse(input);
}

}