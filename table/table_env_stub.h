#ifndef SAKI_TABLE_ENV_STUB_H
#define SAKI_TABLE_ENV_STUB_H

#include "table_env.h"



namespace saki
{



class TableEnvStub : public TableEnv
{
public:
    TableEnvStub() = default;
    virtual ~TableEnvStub() = default;

    int hour24() const override;
};



} // namespace saki



#endif // SAKI_TABLE_ENV_STUB_H
