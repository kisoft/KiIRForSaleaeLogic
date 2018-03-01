#include "KiIR_PANASONIC.h"

const Protocol PANASONIC_ProtocolInfo =
{
	pePANASONIC, "Panasonic",					// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 56, 56, 100000 } }, bdLSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 565, 316 } },				// Описание бита "0"
	{ 1, bcMARK, { 565, 1140 } },				// Описание бита "1"
	true, { 1, bcMARK, { 3600, 1600 } },		// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 47000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 565, 565 } }			// Финальный бит
};
