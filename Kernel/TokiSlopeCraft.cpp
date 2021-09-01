#include "TokiSlopeCraft.h"
#ifdef WITH_QT
TokiSlopeCraft::TokiSlopeCraft(const string & ,QObject *parent) : QObject(parent)
#else
TokiSlopeCraft::TokiSlopeCraft(const string &)
#endif
{
}
