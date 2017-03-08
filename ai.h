#ifndef SAKI_AI_H
#define SAKI_AI_H

#include "table.h"



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

    virtual ~Ai() = default;

    Ai(const Ai &copy) = delete;
    Ai &operator=(const Ai &assign) = delete;

    void onActivated(Table &table) final;

protected:
    Ai(Who who);

    Action maxHappy(const TableView &view);
    virtual Action forward(const TableView &view);

    virtual int happy(const TableView &view, int iter, const Action &action);

private:
    Action placeHolder(const TableView &view);

    int happy0(const TableView &view, const Action &action);
    int happy1(const TableView &view, const Action &action);
    int happy1D(const TableView &view, const Action &action);
    int happy1A(const TableView &view, const Action &action);
    int happy2(const TableView &view, const Action &action);
    int happy2D(const TableView &view, const Action &action);
    int happy2A(const TableView &view, const Action &action);
    int happy3(const TableView &view, const Action &action);

    bool riichi(const TableView &view);

    int chance(const TableView &view, Who tar, T34 t);
    int ruleChance(const TableView &view, Who tar, T34 t);
    int logicChance(const TableView &view, T34 t);

private:
    struct Picture
    {
        int lastUpdate = 0;
        bool defense = false;
        std::vector<Who> threats;
    } mPicture;
};



} // namespace saki



#endif // SAKI_AI_H


