#ifndef SAKI_AI_H
#define SAKI_AI_H

#include "table.h"

#include <functional>



#define AI_CTORS(Name) \
    explicit Name(Who who) : Ai(who) { }



namespace saki
{



/// should give same output when given same input
/// need to be reproducible for implementing prediction-related skills
class Ai : public TableOperator
{
public:
    static Ai *create(Who who, Girl::Id id);
    static std::vector<Action> filter(const std::vector<Action> &orig,
                                      std::function<bool(const Action &)> pass);

    virtual ~Ai() = default;

    Ai(const Ai &copy) = delete;
    Ai &operator=(const Ai &assign) = delete;

    void onActivated(Table &table) final;

protected:
    Ai(Who who);

    Action maxHappy(const TableView &view);
    virtual Action forward(const TableView &view);

    /// return value must be an element in 'choices'
    virtual Action think(const TableView &view, const std::vector<Action> &choices);

    Action placeHolder(const TableView &view);
    Action thinkTrivial(const std::vector<Action> &choices);

private:
    Action thinkAggress(const std::vector<Action> &choices, const TableView &view);
    Action thinkDefense(const std::vector<Action> &choices, const TableView &view,
                        const std::vector<Who> &threats);
    Action thinkDefenseDiscard(const std::vector<Action> &choices, const TableView &view,
                               const std::vector<Who> &threats);
    Action thinkAttack(const std::vector<Action> &choices, const TableView &view);
    Action thinkAttackDiscard(std::vector<Action> &choices, const TableView &view);
    Action thinkAttackDiscardWide(std::vector<Action> &choices, const TableView &view);

    bool riichi(const TableView &view);

    int chance(const TableView &view, Who tar, T34 t);
    int ruleChance(const TableView &view, Who tar, T34 t);
    int logicChance(const TableView &view, T34 t);
};



} // namespace saki



#endif // SAKI_AI_H


