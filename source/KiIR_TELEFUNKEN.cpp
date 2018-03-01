#include "KiIR_TELEFUNKEN.h"

const Protocol TELEFUNKEN_ProtocolInfo =
{
	peTELEFUNKEN, "TELEFUNKEN",					// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 15, 15, 70000 } }, bdMSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 600, 600 } },				// Описание бита "0"
	{ 1, bcMARK, { 600, 1500 } },				// Описание бита "1"
	true, { 1, bcMARK, { 600, 1500 } },			// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	false, { rtNormal, ritPeriod, 35000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 550, 2000 } }			// Финальный бит
};
