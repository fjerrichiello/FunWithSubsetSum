/*
 *  Fun With Sumset Sum
 * 
 *  CS401 F19 Prog1
 *  Frank Errichiello & Tomasz Hulka
 * 
 *  Given a set of elements and a target, finds if achieving that target is feasible, and if it is calculates:
 *    - Number of distinct solution
 *    - Size of smallest satisfying subset
 *    - Number of min-sized satisfying subsets
 *    - Lexicographically first min-sized solution
 */


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip> // for std::setw
#include <algorithm> // for std::min


struct ssum_elem {
  unsigned long int x;
  std::string name;
};


struct ssum_info {

  unsigned long long int total; // total solutions
  unsigned long long int smallest; // size of smallest subset
  unsigned long long int numSmallest; // count of solutions with smallest size
  std::vector<bool> opt; //lexigraphically first shortest subset

};


void reset(std::vector<bool> &v) {

  for (bool e : v) {
    e = false;
  }

  return;

}


// prints the desired info, taking in the 4 answers and the original list of elements 
void printReport(int target, ssum_info info, std::vector<ssum_elem> elems) {

  std::cout << std::endl << std::endl;

  std::cout << "    Target sum of " << target << " is FEASIBLE!\n" << std::endl;
  std::cout << "    Number of distinct solutions:" << std::setw(14) << info.total << std::endl;
  std::cout << "    Size of smallest satisfying subset:" << std::setw(8) << info.smallest << std::endl;
  std::cout << "    Number of min-sized satisfying subsets:" << std::setw(4) << info.numSmallest << std::endl;
  std::cout << "    Lexicographically first min-sized solution:" << std::endl << std::endl;

  std::cout << "    {";
  
  bool printed = false; // for formatting
  for (int i = 0; i < elems.size(); i++) {    
    if (info.opt[i] == true) {
      if (printed) 
        std::cout << ", ";
      
      printed = true;
      std::cout << elems[i].name;
      
    }

  }

  std::cout << "}\n\n\n";
  return;
}



class ssum_instance {

    unsigned long int target=0;

    std::vector<ssum_elem> elems;
  

    std::vector<std::vector<std::pair<bool, unsigned long long int>>> feasible;  
    // - feasible[i][x].first = TRUE if there is a subset
    //  of a[0..i] adding up to x; FALSE otherwise
    //  (once instacne has been solved)
    // - feasible[i][x].second stores the smallest size of the vector adding up to x
    
    int done=false;    // flag indicating if dp has been run or 
                       //   not on this instance


    ssum_info solution; // a struct containing the 4 things we are interested in 


    public:

    // helper function for reading the feasible table 
    std::vector<std::vector<std::pair<bool, unsigned long long int>>> getTable() {
      return feasible;
    }

   
    // returns the elements vector to be used for printing in main 
    std::vector<ssum_elem> getElems() {
      return elems;
    }

    // returns the solution struct 
    ssum_info getInfo() {
      return solution;
    }

    // Function:  read_elems
    // Description:  reads elements from standard-input; 
    //   Format:  sequence of <number> <name> pairs where
    //      <number> is non-negative int and
    //      <name> is a string associated with element
    void read_elems( std::istream &stream) {
        ssum_elem e;

        elems.clear();
        // while(std::cin >> e.x && std::cin >> e.name) {
        while(stream>> e.x && stream >> e.name) {
            elems.push_back(e);
        }
        done = false;
    }

    // Function:  solve
    // Desc:  populates dynamic programming table of
    //    calling object for specified target sum.
    //    Returns true/false depending on whether the
    //    target sum is achievalble or not.
    //    Table remains intact after call.
    //    Object can be reused for alternative target sums.
    bool solve(unsigned int tgt) {

        unsigned long long int total = 0;
        for (const ssum_elem &e : elems) 
          total += e.x;


        if (tgt < 0 || tgt > total) // checking for out of bounds, although we can assume no negative targets are tried 
          return false;  

        unsigned int n = elems.size();
        unsigned int i, x;

        std::vector<bool> empty(n, false);
        
        // initialize solutions
        solution.numSmallest = 0;
        solution.total = 0;
        solution.smallest = 0;
        solution.opt = empty;

        if(tgt == 0) { // only the empty set is a valid set (and the min set) for target = 0
          solution.numSmallest = 1;
          solution.total = 1;
          return true; 
        }

        if(target == tgt && done) 
            return feasible[n-1][tgt].first;

        target = tgt;


      
        feasible = 
            std::vector<std::vector<std::pair<bool, unsigned long long int>>>(n, std::vector<std::pair<bool, unsigned long long int>>(target+1, std::pair<bool, unsigned long long int>(false, 0)));


        std::vector<std::vector<unsigned long long int>> SUMS(n, std::vector<unsigned long long int>(tgt + 1, 0));

        //  SUMS[i][x] = the count of distinct subsets 
        //  of a[0..i] adding up to x; 


        std::vector<std::vector<std::vector<bool>>> PRE(n, std::vector<std::vector<bool>>(tgt + 1, empty));


        // PRE[i][x] = the lexicographically first subset of a[0..i] that achieves a target of x 
        // which is stored as a vector V of booleans of length n where V[0...n] = true if it is part of the
        // lexicographically first subset 

        

        std::vector<std::vector<unsigned long long int>> MIN(n, std::vector<unsigned long long int>(tgt + 1, 0));

        // MIN[i][x] stores the number of min-sized subsets from a[0..i] that result in a target of x

        // leftmost column (column zero) is all TRUE because 
        //    a target sum of zero is always achievable (via the
        //    empty set).
        for(i=0; i<n; i++) {
            feasible[i][0].first = true;
            feasible[i][0].second = 0;
            SUMS[i][0] = 0;
            PRE[i][0] = std::vector<bool>(n, false); 
            MIN[i][0] = 0;
        }

        
        for(x=1; x<=target; x++) {
            if(elems[0].x == x) {
                feasible[0][x].first = true;
                feasible[0][x].second = 1; 
                SUMS[0][x]++; 
                PRE[0][x][0] = true;
                MIN[0][x] = 1; //singleton
            }              

        }

        if (feasible[0][tgt].first) {
          solution.smallest = 1; // first entry must be the smallest one and of length 1
          solution.numSmallest = 1; // increase count of smallest
          solution.opt[0] = true;
          solution.total = 1;
        }

        for(i=1; i<n; i++) {    

            for(x=1; x<=tgt; x++) {

                bool case1 = feasible[i-1][x].first; //exclude
                bool case2 = x >= elems[i].x && feasible[i-1][x-elems[i].x].first; //include

                if (elems[i].x == x) {
                    feasible[i][x].first = true;
                    feasible[i][x].second = 1; 
                    SUMS[i][x] = 1 + SUMS[i-1][x];
                    
                    if (case1 && feasible[i-1][x].second == 1) { // if we also have a singleton 
                      PRE[i][x] = PRE[i-1][x]; // then exclude case is always first 
                      MIN[i][x] = MIN[i-1][x] + 1; // if we have a singleton before then just + 1 

                    } else {
                      PRE[i][x][i] = true;
                      MIN[i][x] = MIN[i-1][x - elems[i].x] + 1;
                    }

                    // otherwise its a new unique value

                    
                }

                else { 

                  
                  if(case1 && case2) {
                    feasible[i][x].first = true;
                    feasible[i][x].second = std::min(feasible[i-1][x].second, feasible[i-1][x-elems[i].x].second + 1);
                    SUMS[i][x] = SUMS[i][x] + SUMS[i-1][x - elems[i].x] + SUMS[i-1][x];
                    
                    if (feasible[i-1][x].second == (feasible[i-1][x-elems[i].x].second + 1)) {
                      
                      if (PRE[i-1][x - elems[i].x] > PRE[i-1][x]) {
                         PRE[i][x] = PRE[i-1][x - elems[i].x];
                         PRE[i][x][i] = true; 

                      } else {
                       
                        PRE[i][x] = PRE[i-1][x];
                      }

                      MIN[i][x] = MIN[i][x] + MIN[i-1][x - elems[i].x] + MIN[i-1][x];
                      

                    }
                    
                    // inclusion case smaller
                    else if ((feasible[i-1][x-elems[i].x].second + 1) < feasible[i-1][x].second) {
                      PRE[i][x] = PRE[i-1][x - elems[i].x];
                      PRE[i][x][i] = true; 
                      MIN[i][x] = MIN[i-1][x - elems[i].x] + MIN[i][x];
                    }

                    else {
                      PRE[i][x] = PRE[i-1][x]; 
                      MIN[i][x] = MIN[i-1][x];

                    }
                  } 

                  else if (case1) {
                    feasible[i][x].first = true;
                    feasible[i][x].second = feasible[i-1][x].second;
                    SUMS[i][x] += SUMS[i-1][x];
                    PRE[i][x] = PRE[i-1][x]; 
                    MIN[i][x] = MIN[i-1][x];

                  }

                  else if (case2) {
                    feasible[i][x].first = true;
                    feasible[i][x].second = feasible[i-1][x-elems[i].x].second + 1;
                    SUMS[i][x] += SUMS[i-1][x - elems[i].x];
                    PRE[i][x] = PRE[i-1][x - elems[i].x];
                    PRE[i][x][i] = true; 
                    // the min ways for the include case is the min ways for the 'leftover' and min ways of getting x
                    MIN[i][x] = MIN[i-1][x - elems[i].x] + MIN[i][x];
                   
                  }
                }
            }

            // otherwise, feasible[0][x] remains false
        }
        done = true;
       
        solution.smallest = feasible[n-1][target].second;
        solution.total = SUMS[n-1][target];
        solution.opt = PRE[n-1][target];
        solution.numSmallest = MIN[n-1][target];
        
        return feasible[n-1][target].first;
    }
};  // end class





/**
* usage:  ssum  <target> < <input-file>
*
*
* input file format:
*
*     sequence of non-negative-int, string pairs
*
*     example:

    12 alice
    9  bob
    22 cathy
    12 doug

* such a file specifies a collection of 4 integers: 12, 9, 22, 12
* "named" alice, bob, cathy and doug.
*/
int main(int argc, char *argv[]) {
  unsigned int target;
  ssum_instance ssi;

  if(argc != 2) {
    fprintf(stderr, "one cmd-line arg expected: target sum\n");
    return 0;
  }
  if(sscanf(argv[1], "%u", &target) != 1) {
    fprintf(stderr, "bad argument '%s'\n", argv[1]);
    fprintf(stderr, "   Expected unsigned integer\n");
    return 0;
  }

  ssi.read_elems(std::cin);

  if(ssi.solve(target) ) {
    
    printReport(target, ssi.getInfo(), ssi.getElems());

  }
  else {
    std::cout << "SORRY!  Apparently, the target sum of " <<
      target << " is NOT achievable\n";
  }

  



  

}
