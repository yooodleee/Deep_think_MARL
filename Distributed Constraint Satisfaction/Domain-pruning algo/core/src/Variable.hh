
#ifndef VARIABLE_H_
#define VARIABLE_H_


#include "Stats.hh"


#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>


typedef int ind;    /// index of both arrays that start at 0
typedef int indLocal;   /// index of both arrays that start at domainStart
typedef int indom;  /// index of domValues that start at 0
typedef int indDomLocal;    /// index of domValues that start at domainStart
typedef int indVp;  /// index of varProps that start at 0
typedef int indVpLocal; /// index of varProps that start at domainStart


class Constraint;
class Variable;


#define UNDEF 2
#define NEG 1
#define POS 0



class VP {
public:
    int val;
    bool lastAssigned : 1;
    bool marked: 1;
    unsigned state: 2;
    indDomLocal posInVar: 28;
    Variable* toVar;
    double activity;


    VP(int value, indDomLocal pos, Variable* var, double act)
        : val(value)
        , lastAssigned(false)
        , marked(false)
        , state(UNDEF)
        , posInVar(pos)
        , toVar(var)
        , activity(act) {};
};



struct VarSave {
    int level;
    int sizeDom;
    int puLastSize;
};



/// Represent the explication of a removed vp, if ctr = nullptr then it's a decision
class Expl {
public:
    Constraint* ctr = nullptr;
    int level = -1;
    unsigned long long order = 0;


    Expl();


    void lock(Constraint* c, int lvl, unsigned long long ord);
    void unlock();
};



/// Functor used to create the conflict analysis stack
struct varOrderCmp {
    bool operator()(std::pair<unsigned, Expl*> x, std::pair<unsigned, Expl*> y)
    {
        return std::get<1>(x)->order > std::get<1>(y)->order;
    }
};



class Variable {
private:
    int id;
    std::string name;


    // Bounds infos
    indVp lowerBound;   /// Index of the lowerBound of this variable in varProps vector
    indVp upperBound;   /// Index of the upperBOund of this variable in varProps vector


    /// Heuristics
    indVp lastChoice = -1;  /// Index in varProps vector of the last value choosen for this variable

    std::unordered_map<int, indVp> valueToVarPropInd;   /// Map between real values and varProp indices
    std::vector<Constraint*> consts;    /// Vector containing the constraints where this variable appears


    Variable(std::string);


public:
    Variable(std::string n, std::vector<int> v);    /// Ctr from a vector of values
    Variable(std::string n, int min, int max);  /// Ctr from a range of values
    Variable(std::string n, int onlyVal);   /// Ctr for a unique value variable


    inline int getId() { return id; }   /// Getter for the solver id of this variable
    inline std::string getName() { return name; }   /// Getter for the name given by the instance
    inline void init() { domainAfterInit = domainCurSize; }


    // ##########################################################################
    // #                             statics                                    #
    // ##########################################################################
    static unsigned long long gblOrder; /// stamp for propagation order
    // ------------------------------- Big vectors -------------------------------
    static std::vector<Expl> vpExpl;    /// The explanations of conflicts for conflict analysis mode
    static std::vector<indVp> domValues;    /// The indices of the values in varprops (kind of a reversible sparse)
    static std::vector<VP> varProps;    /// The real var props, contains the true value, the position of this value in domValues and a pointer the variable
    //____________________________________________________________________________


    // ###########################################################################
    // #                            Variables                                    #
    // ###########################################################################
    std::vector<VarSave> saveSize;  /// Save the pair <size, level> for the backtrack when the domain of the variable is touched
    bool lastPushed = false;    /// Used to know if this variable was the last pushed by the solver
    bool wasPushed = false;     /// Used to know if this variable was pushed during this run
    bool useless = false;   /// Marker to print a star instead of a value if the variable is useless
    // ---------------------------------- Sizes ------------------------------------
    int domainAfterInit = 0;    /// Size of the domain after the initialization (first AC call)
    int domainInitSize; /// The num of values in the variable domain
    int domainCurSize;  /// The current num of values in the variable domain
    int domainStart;    /// Index in both varProps and domValues to the beginning of the domain
    // ---------------------------------- Stamps -----------------------------------
    unsigned long long caStamp = 0; /// Conflict analysis stamp to avoid to handle the same variable
    unsigned long long propagateStamp = 0;  /// Stamp for the consistency handler to know if the variable is already in the propagation queue
    unsigned long long clStamp = 0; /// Stamp for the consistency handler to know if you need to revise the clauses
    unsigned long long ctrStamp = 0;    /// Stamp for the consistency handler to know if you need to revise the constraints
    // ---------------------------------- Heurstics --------------------------------
    unsigned long long score = 1;   /// wDeg score
    //______________________________________________________________________________


    // ##############################################################################
    // #                                 domainBoolUtil                             #
    // ##############################################################################
    bool* domainBoolUtil;
    size_t DBUSize = 0;


    inline void cleanDBU()
    {
        DBUSize = 0;
        std::memset(domainBoolUtil, 0, sizeof(bool) * domainInitSize);
    }

    inline void isertInDBU(indVpLocal ivp)
    {
        if (domainBoolUtil[ivp])
            return;
        DBUSize++;
        domainBoolUtil[ivp] = true;
    }

    inline bool delInDBU(int level, Constraint* ctr)
    {
        for (int i = domainCurSize - 1; i >= 0; --i)
            if (!domainBoolUtil[indDomLocalToIndVPLocal(i)] && removeAt(i, level, ctr))
                return true;
        
        return false;
    }

    inline unsigned computePosFromDBU()
    {
        double activityMax = std::numeric_limits<double>::min();
        int pos = domainAfterInit;

        for (int i = domainAfterInit - 1; i >= 0; --i)
            if (!domainBoolUtil[indDomLocalToIndVPLocal(i)] && getVarPropFromLocalDomInd(i).activity > activityMax) {
                activityMax = getVarPropFromLocalDomInd(i).activity;
                pos = i;
            }

        assert(pos != domainAfterInit);

        return domainStart + pos;
    }
    // _____________________________________________________________________
    
    static inline bool isValid(indVp ivp) { return Variable::varProps[ivp].state != NEG; }

    inline int getPuSizeLast() {
        return (saveSize.empty() ? domainAfterInit : saveSize.top().puLastSize);
    }
    inline void setPuSizeLast(int n, int level)
    {
        if (saveSize.empty())
            saveSize.push({ level, domainCurSize, n});
        else
            saveSize.top().puLastSize = n;
    }

    inline std::vector<Constraint*>& getConsts() { return consts; } /// Getter for the constraint vector
    inline void addConsts(Constraint* c) { consts.push_back(c); }   /// Used to add a new constraint to the variable constraint vector

    inline bool isAssigned() { return domainCurSize == 1; } /// return true is the variable is singleton
    inline bool isAssignedTo(int value) { return (isAssigned() ? getVarPropFromLocalDomInd(0).val == value : false); }  /// return true is the variable is assinged to the given value
    inline bool isAssignedToIndVp(indVp ivp) { return (isAssigned() ? indDomLocalToIndVP(0) == ivp : false); }  /// return true is the variable is assigned to the given value

    inline indVp getLowerBoundInd() { return lowerBound; }  /// return the index of the lowerbound in the varProps vector
    inline indVp getUpperBoundInd() { return upperBound; }  /// return the index of the upperBound in the varProps vector
    inline indVpLocal getLowerBoundLocalInd() { return lowerBound - domainStart; }  /// return the local (i.e. starting from domainStart) index of the lowerBound in the varProps vector
    inline indVpLocal getUpperBoundLocalInd() { return upperBound - domainStart; }  /// return the local (i/e/ starting from domainStart) index of the upperBound in the varProps vector
    inline int getLowerBoundVal() { return Variable::varProps[lowerBound].val; }    /// return the real value of the lowerbond
    inline int getUpperBoundVal() { return Variable::varProps[upperBound].val; }    /// return the real value of the upperbound
    inline indDomLocal getLowerBoundLocalDomInd() { return Variable::varProps[lowerBound].posInVar; }   /// return the local position of the lowerBound in the domValues vector
    inline indDomLocal getUpperBoundLocalDomInd() { return Variable::varProps[upperBound].posInVar; }   /// return thhe local position of the upperBound in the domValues vector

    
    inline int getOnlyValue()
    {
        assert(isAssigned());
        return getLowerBoundVal();
    }

    inline VP& getVarPropFromLocalInd(indVpLocal index) { return Variable::varProps[domainStart + index]; } /// Get varProp pointer from a local index
    inline VP& getVarPropFromLocalDomInd(indDomLocal index) { return Variable::varProps[Variable::domValues[domainStart + index]]; }    /// Get varProp pointer from a local index
    inline indVp getVarPropIndFromValue(int value) { return valueToVarPropInd[value]; } /// Get the index in varProps from a real value when you sure it exist (use getIndFromValue if not sure)
    inline int LocalDomIndToVal(indDomLocal index) { return Variable::varProps[Variable::domValues[domainStart + index]].val; }

    inline indVp indDomLocalToIndVP(indDomLocal index) { return Variable::domValues[domainStart + index]; } /// Convert domValues local index to the varprop index associated
    inline indVpLocal indDomLocalToIndVPLocal(indDomLocal index) { return Variable::domValues[domainStart + index] - domainStart; } /// Convert domValues local index to the varprop local index associated
    inline indDomLocal indVPLocalToIndDomLocal(indVpLocal index) { return Variable::varProps[domainStart + index].posInVar; }   /// Convert varprop local index to the domValues local index associated
    inline indVpLocal indVpToIndVpLocal(indVp ivp) { return ivp - domainStart; }

    inline indDomLocal lastChoiceToDomLocalInd() { return Variable::varProps[lastChoice].posInVar; }    /// Getter for the current local domValues index of the last choice (last value assigned )
    inline bool isvalidLastChoice() { return lastChoice != -1 && isValidVpInd(lastChoice); }    /// Return true if the last choice is still viable

    inline bool isValidVpInd(indVp val) { return Variable::varProps[val].state != NEG; }    /// Return true if the index of varProp isn't deleted
    inline bool isValidLocalVpInd(indVpLocal val) { return Variable::varProps[domainStart + val].state != NEG; }    /// Return true if the local index of varProp isn't deleted
    inline bool isValidPosInVar(int posInVar) { return posInVar < domainCurSize; }  /// Return true if the position is valid

    inline indVp getlastDeleteInd() { return Variable::domValues[domainStart + domainCurSize]; }
    inline indVpLocal getLastDeleteLocalInd() { return Variable::domValues[domainStart + domainCurSize] - domainStart; }

    void backtrackToLevel(int level);   /// perform a backtrack i.e. restore the values deleted after @level

    friend std::ostream& operator<<(std::ostream& out, const Variable& var);    /// override operator << to print a variable

    
    /**
     * Keep only one value in the domain. 
     * 
     * @param[in] index, the position of the value which is kept. 
     * @param[in] level, the level when the assignation occurs. 
     */
    bool assignAt(indDomLocal index, int level, Constraint* ctr = nullptr);


    /**
     * Keep only the value @val in the domain. 
     * 
     * @param[in] value, the value which is kept. 
     * @param[in] level, the level when the assignation ocurrs. 
     * 
     * @return false if something went wrong, i.e. if the value doesn't exist or was already delete
     */
    inline bool assignTo(int val, int level = 0, Constraint* ctr = nullptr)
    {
        for (int i = 0; i < domainCurSize; ++i)
            if (getVarPropFromLocalDomInd(i).val == val) {
                return assignAt(i, level, ctr);
            }
        return false;
    }


    /**
     * Delete a value for the current decision level. 
     * 
     * @param[in] index, the position of the value which is removed in domValues. 
     * @param[in] level, the level when the deletion occurs. 
     * 
     * @return true if deleting leads to domain wipeout
     */
    bool removeAt(indDomLocal index, int level = 0, Constraint* ctr = nullptr);


    /**
     * Keep in current var only the values a + k with a in @var domain. 
     * 
     * @param[in] var the variable where get the values
     * @param[in] k the integer add to each values
     * @param[in] level the level when the deletion occurs. 
     * 
     * @return true if a value is deleted. 
     */
    bool keepOnlyValues(Variable* var, int k, int level, Constraint* ctr = nullptr);


    /**
     * Keep in current var only the values in the vector @val. 
     * 
     * @param[in] vals the vector where get the values
     * @param[in] level the level when the deletion occurs. 
     * 
     * @return true if a value is deleted. 
     */
    bool keepOnlyValues(std::vector<int>& vals, int level, Constraint* ctr = nullptr);


    /**
     * 
     */
}