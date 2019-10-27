// Copyright 2019 Olaru Cristian & Nichita Radu, 311CA, TEMA3 STRUCTURI DE DATE
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <utility>
#include <cassert>
#include "./aegraph.h"

std::string strip(std::string s) {
    // deletes whitespace from the beginning and end of the string
    s.erase(0, s.find_first_not_of(" \n\r\t"));
    s.erase(s.find_last_not_of(" \n\r\t")+1);
    return s;
}

std::pair<std::string, std::string> split_first(std::string s,
    char delimiter = ',') {
    // returns a pair that contains: <first_cut, rest_of_graph>

    int numOpen = 0;

    int len = s.size();
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c == delimiter && numOpen == 0)
            return std::make_pair(
                    strip(s.substr(0, i)), strip(s.substr(i+1, len)));
        if (c == '[')
            numOpen += 1;
        if (c == ']')
            numOpen -= 1;
    }

    return {strip(s), std::string()};
}


std::vector<std::string> split_level(std::string s, char delimiter = ',') {
    // splits 's' into separate entities (atoms, subgraphs)

    std::vector<std::string> result;
    auto snd = s;
    while (true) {
        auto p = split_first(snd, delimiter);
        auto fst = p.first;
        snd = p.second;

        result.push_back(fst);

        if (snd.empty())
            return result;
    }
}


int AEGraph::num_subgraphs() const {
    return subgraphs.size();
}


int AEGraph::num_atoms() const {
    return atoms.size();
}


int AEGraph::size() const {
    return num_atoms() + num_subgraphs();
}

bool AEGraph::operator<(const AEGraph& other) const {
    return this->repr() < other.repr();
}

bool AEGraph::operator==(const AEGraph& other) const {
    return this->repr() == other.repr();
}

bool AEGraph::operator!=(const AEGraph& other) const {
    return this->repr() != other.repr();
}

AEGraph AEGraph::operator[](const int index) const {
    // offers an easier way of accessing the nested graphs
    if (index < num_subgraphs()) {
        return subgraphs[index];
    }

    if (index < num_subgraphs() + num_atoms()) {
        return AEGraph('(' + atoms[index - num_subgraphs()] + ')');
    }

    return AEGraph("()");
}

std::ostream& operator<<(std::ostream &out, const AEGraph &g) {
    out << g.repr();
    return out;
}

AEGraph::AEGraph(std::string representation) {
    // constructor that creates an AEGraph structure from a
    // serialized representation
    char left_sep = representation[0];
    char right_sep = representation[representation.size() - 1];

    assert((left_sep == '(' && right_sep == ')')
        || (left_sep == '[' && right_sep == ']'));

    // if the left separator is '(' then the AEGraph is the entire
    // sheet of assertion
    if (left_sep == '(') {
        is_SA = true;
    } else {
        is_SA = false;
    }

    // eliminate the first pair of [] or ()
    representation = representation.substr(1, representation.size() - 2);

    // split the graph into separate elements
    auto v = split_level(representation);
    // add them to the corresponding vector
    for (auto s : v) {
        if (s[0] != '[') {
            atoms.push_back(s);
        } else {
            subgraphs.push_back(AEGraph(s));
        }
    }

    // also internally sort the new graph
    this->sort();
}

std::string AEGraph::repr() const {
    // returns the serialized representation of the AEGraph

    std::string left, right;
    if (is_SA) {
        left = '(';
        right = ')';
    } else {
        left = '[';
        right = ']';
    }

    std::string result;
    for (auto subgraph : subgraphs) {
        result += subgraph.repr() + ", ";
    }

    int len = atoms.size();
    if (len != 0) {
        for (int i = 0; i < len - 1; i++) {
            result += atoms[i] + ", ";
        }
        result += atoms[len - 1];
    } else {
        if (subgraphs.size() != 0)
            return left + result.substr(0, result.size() - 2) + right;
    }

    return left + result + right;
}


void AEGraph::sort() {
    std::sort(atoms.begin(), atoms.end());

    for (auto& sg : subgraphs) {
        sg.sort();
    }

    std::sort(subgraphs.begin(), subgraphs.end());
}

bool AEGraph::contains(const std::string other) const {
    // checks if an atom is in a graph
    if (find(atoms.begin(), atoms.end(), other) != atoms.end())
        return true;

    for (const auto& sg : subgraphs)
        if (sg.contains(other))
            return true;

    return false;
}

bool AEGraph::contains(const AEGraph& other) const {
    // checks if a subgraph is in a graph
    if (find(subgraphs.begin(), subgraphs.end(), other) != subgraphs.end())
        return true;

    for (const auto& sg : subgraphs)
        if (sg.contains(other))
            return true;

    return false;
}

std::vector<std::vector<int>> AEGraph::get_paths_to(const std::string other)
    const {
    // returns all paths in the tree that lead to an atom like <other>
    std::vector<std::vector<int>> paths;

    int len_atoms = num_atoms();
    int len_subgraphs = num_subgraphs();

    for (int i = 0; i < len_atoms; i++) {
        if (atoms[i] == other && size() > 1) {
            paths.push_back({i + len_subgraphs});
        }
    }

    for (int i = 0; i < len_subgraphs; i++) {
        if (subgraphs[i].contains(other)) {
            auto r = subgraphs[i].get_paths_to(other);
            for (auto& v : r)
                v.insert(v.begin(), i);
            copy(r.begin(), r.end(), back_inserter(paths));
        }
    }

    return paths;
}

std::vector<std::vector<int>> AEGraph::get_paths_to(const AEGraph& other)
    const {
    // returns all paths in the tree that lead to a subgraph like <other>
    std::vector<std::vector<int>> paths;
    int len_subgraphs = num_subgraphs();

    for (int i = 0; i < len_subgraphs; i++) {
        if (subgraphs[i] == other && size() > 1) {
            paths.push_back({i});
        } else {
            auto r = subgraphs[i].get_paths_to(other);
            for (auto& v : r)
                v.insert(v.begin(), i);
            copy(r.begin(), r.end(), back_inserter(paths));
        }
    }

    return paths;
}


std::vector<std::vector<int>> AEGraph::possible_double_cuts() const {
    // returns all paths in the tree where double cut action can be applied
    std::vector<std::vector<int>> paths;
    int len_subgraphs = num_subgraphs();

    for (int i = 0; i < len_subgraphs; i++) {
        if (subgraphs[i].size() == 1 && subgraphs[i].atoms.size() == 0) {
            paths.push_back({i});
        }
            auto r = subgraphs[i].possible_double_cuts();
            for (auto& v : r)
                v.insert(v.begin(), i);
            copy(r.begin(), r.end(), back_inserter(paths));
    }

    return paths;
}

AEGraph AEGraph::double_cut(std::vector<int> where) const {
    // returns the graph after double cut action is applied to certain node
    AEGraph graph = *this;
    AEGraph *tmp = &graph;
    size_t dim = where.size() - 1;
    for (size_t i = 0; i < dim; i++) {
        tmp  = &(tmp->subgraphs[where[i]]);
    }

    AEGraph *curr = &(tmp->subgraphs[where[dim]].subgraphs[0]);

    tmp->subgraphs.insert(tmp->subgraphs.end(),
                                curr->subgraphs.begin(), curr->subgraphs.end());
    tmp->atoms.insert(tmp->atoms.end(), curr->atoms.begin(), curr->atoms.end());
    tmp->subgraphs.erase(tmp->subgraphs.begin() + where[dim]);

    return graph;
}

std::vector<std::vector<int>> AEGraph::possible_erasures(int level) const {
    // returns all paths in the tree where erase action can be applied
     std::vector<std::vector<int>> paths;
     int len_subgraphs = num_subgraphs();
    int len_atoms = num_atoms();
     int total_size = len_subgraphs + len_atoms;
    for (int i = 0; i < total_size; i++) {
            if (i < len_subgraphs) {
                auto r = subgraphs[i].possible_erasures(level + 1);
                // checks if there are at least 2 elements downward
                if (r.size() != 0 || level == -1) {
                     paths.push_back({i});
                }
                std::vector<std::vector<int>> to_add = {};
                for (auto& v : r) {
                    v.insert(v.begin(), i);
                    // checks if paths is on the same parity with level
                    if (v.size() % 2 == (size_t) (abs(level) % 2)) {
                        to_add.push_back(v);
                    }
                }
                if (to_add.size() > 0) {
                    copy(to_add.begin(), to_add.end(), back_inserter(paths));
                }
            } else {
                if ((len_atoms > 1 && (level % 2 == 1)) || (level == -1)) {
                    paths.push_back({i});
                }
            }
    }
    // remove duplicates and sort the path vector
    std::sort(paths.begin(), paths.end());
    paths.erase(unique(paths.begin(), paths.end()), paths.end());
    return paths;
}

AEGraph AEGraph::erase(std::vector<int> where) const {
     return AEGraph("()");
}


std::vector<std::vector<int>> AEGraph::possible_deiterations() const {
    return std::vector< std::vector<int> >() ;
}

AEGraph AEGraph::deiterate(std::vector<int> where) const {
       return AEGraph("()");
}

