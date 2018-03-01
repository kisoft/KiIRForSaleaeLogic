#include "KiIR_NEC.h"

const Protocol NEC_ProtocolInfo =
{
	peNEC, "NEC",								// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 32, 32, 200000 } }, bdLSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 560, 560 /* 1120 - 560 */ } },	// Описание бита "0"
	{ 1, bcMARK, { 560, 1690 /* 2250 - 560 */ } },		// Описание бита "1"
	true, { 1, bcMARK, { 9000, 4500 } },		// Наличие битов для Lead, Описание Lead
	true, { 1, bcMARK, { 9000, 2250 } },		// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtSpecialLead, ritPeriod, 110000 },	// Повтор (всё о повторе)
	true, { 0, bcMARK, { 560, 1120 - 560 } }	// Финальный бит
};
