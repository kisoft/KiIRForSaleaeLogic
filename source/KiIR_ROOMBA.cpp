#include "KiIR_ROOMBA.h"

const Protocol ROOMBA_ProtocolInfo =
{
	peROOMBA, "ROOMBA",							// Идентификатор и название протокола
	ptPulseDistanceWidth,						// Тип протокола
	{ { 0, 7, 7, 18000 }, { 1, 7, 7, 18000 }, { 2, 7, 7, 70000 } }, bdMSB,	// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 930, 2790 } },				// Описание бита "0"
	{ 1, bcMARK, { 2790, 930 } },				// Описание бита "1"
	true, { 1, bcMARK, { 2790, 930 } },			// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 18000 },		// Повтор (всё о повторе)
	false, { 0, bcMARK, { 0, 0 } }				// Финальный бит
};
