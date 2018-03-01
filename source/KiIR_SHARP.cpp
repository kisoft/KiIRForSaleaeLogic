#include "KiIR_SHARP.h"

const Protocol SHARP_ProtocolInfo =
{
	peSHARP, "SHARP",							// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 13, 13, 65000 }, { 1, 13, 13, 150000 } }, bdLSB,	// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 320, 2000 - 320 } },			// Описание бита "0"
	{ 1, bcMARK, { 320, 1000 - 320 } },			// Описание бита "1"
	false, { 0, bcMARK, { 0, 0 } },				// Наличие битов для Lead, Описание Lead
	false, { 0, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritGap, 40000 },			// Повтор (всё о повторе)
	true, { 0, bcMARK, { 320, 1000 - 320 } }	// Финальный бит
};
