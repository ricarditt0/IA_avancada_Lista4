#include "canonical_pdbs.h"

#include "../algorithms/max_cliques.h"
#include <limits> 

using namespace std;

namespace planopt_heuristics {

bool affects_pattern(const TNFOperator &op, const Pattern &pattern) {
    for (TNFOperatorEntry entry : op.entries) {
        for (int var_id : pattern) {
            if (entry.variable_id == var_id && entry.precondition_value != entry.effect_value) {
                return true;
            }
        }
    }
    return false;
}

vector<vector<int>> build_compatibility_graph(const vector<Pattern> &patterns, const TNFTask &task) {
    /*
      Build the compatibility graph of the given pattern collection in the form
      of adjacency lists: the outer vector has one entry for each pattern
      representing the vertices of the graph. Each such entry is a vector of
      ints that represents the outgoing edges of that vertex, i.e., an edge
      to each other vertex that represents an additive pattern.
    */

    vector<vector<int>> graph(patterns.size());

    // TODO: add your code for exercise (d) here.
    for (int i = 0; i < (int) patterns.size() - 1; i++)
        {
            for (int j = i + 1 ; j < (int) patterns.size(); j++)
            {
                bool add = true;
                for (const auto &op : task.operators)
                { 
                    if((affects_pattern(op, patterns[i]) && affects_pattern(op, patterns[j])))
                    {   
                        add = false;
                        break;
                    }
                } 
                if(add){
                    graph[i].push_back(j);
                    graph[j].push_back(i);
                }      
            }   
        }

    return graph;
}

CanonicalPatternDatabases::CanonicalPatternDatabases(
    const TNFTask &task, const vector<Pattern> &patterns) {
    for (const Pattern &pattern : patterns) {
        pdbs.emplace_back(task, pattern);
    }

    vector<vector<int>> compatibility_graph = build_compatibility_graph(patterns, task);
    max_cliques::compute_max_cliques(compatibility_graph, maximal_additive_sets);

}

int CanonicalPatternDatabases::compute_heuristic(const TNFState &original_state) {
    /*
         To avoid the overhead of looking up the heuristic value of a PDB multiple
         times (if that PDB occurs in multiple cliques), we pre-compute all
         heuristic values. Use heuristic_values[i] for the heuristic value of
         pdbs[i] in your code below.
       */
       vector<int> heuristic_values;
       heuristic_values.reserve(pdbs.size());
       for (const PatternDatabase &pdb : pdbs) {
           heuristic_values.push_back(pdb.lookup_distance(original_state));
           /*
             special case: if one of the PDBs detects unsolvability, we can
             return infinity right away. Otherwise, we would have to deal with
             integer overflows when adding numbers below.
           */
           if (heuristic_values.back() == numeric_limits<int>::max()) {
               return numeric_limits<int>::max();
           }
       }

       /*
         Use maximal_additive_sets and heuristic_values to compute the value
         of the canonical heuristic.
       */
       int h = 0;

       // TODO: add your code for exercise (d) here.
       for(const auto &maximal_clique : maximal_additive_sets){
            int current_clique = 0;
            for(int i = 0; i < (int) maximal_clique.size(); i++){
                current_clique += heuristic_values[i];
            }
            
            if(h < current_clique){
                h = current_clique;
            }
       }
       return h;
}






}
