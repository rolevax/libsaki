#ifndef SAKI_FORM_CTX_H
#define SAKI_FORM_CTX_H



namespace saki
{



struct FormCtx
{
    bool ippatsu = false;
    bool bless = false;
    bool duringKan = false;
    bool emptyMount = false;
    int riichi = 0;
    int roundWind = 0; // not any wind
    int selfWind = 0; // not any wind
    int extraRound = 0;
};



} // namespace saki



#endif // SAKI_FORM_CTX_H



