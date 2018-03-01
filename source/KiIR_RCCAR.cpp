#include "KiIR_RCCAR.h"

const Protocol RCCAR_ProtocolInfo =
{
	peRCCAR, "RCCAR",							// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 13, 13, 100000 } }, bdLSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 600, 900 } },				// Описание бита "0"
	{ 1, bcMARK, { 600, 450 } },				// Описание бита "1"
	true, { 1, bcMARK, { 2000, 2000 } },		// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 40000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 600, 450 } }			// Финальный бит
};
