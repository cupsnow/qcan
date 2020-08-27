#include "CanbusCfg.h"

CanbusCfg::CanbusCfg()
{

}

CanbusCfg& CanbusCfg::instance()
{
    static CanbusCfg *self = nullptr;

    if (!self) self = new CanbusCfg();
    return *self;
}
