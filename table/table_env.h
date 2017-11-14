#ifndef SAKI_TABLE_ENV_H
#define SAKI_TABLE_ENV_H



namespace saki
{



class TableEnv
{
public:
    TableEnv() = default;
    TableEnv(const TableEnv &copy) = delete;
    TableEnv &operator=(const TableEnv &assign) = delete;
    virtual ~TableEnv() = default;

    /// \return [0, 24)
    virtual int hour24() const = 0;
};



} // namespace saki



#endif // SAKI_TABLE_ENV_H


