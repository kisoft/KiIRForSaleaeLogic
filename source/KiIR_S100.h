#pragma once

#ifndef __KIIR_S100_H
#define __KIIR_S100_H

#include "KiIRProtocolsCommon.h"

extern const Protocol S100_ProtocolInfo;

#define S100_TOGGLEBIT_MASK	(0x1000ull)
#define S100_EXTRADATA_MASK	(0x2000ull)
#define S100_FULLDATA_MASK	(0x2FFFull)

#endif
