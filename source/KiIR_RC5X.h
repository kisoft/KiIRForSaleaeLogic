#pragma once

#ifndef __KIIR_RC5X_H
#define __KIIR_RC5X_H

#include "KiIRProtocolsCommon.h"

extern const Protocol RC5X_ProtocolInfo;

#define RC5X_TOGGLEBIT_MASK	(RC5_TOGGLEBIT_MASK)
#define RC5X_EXTRADATA_MASK	(0x1000ull)
#define RC5X_FULLDATA_MASK	(0x17FFull)

#endif
