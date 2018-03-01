#include "KiIR_SAMSUNG48.h"

const Protocol SAMSUNG48_ProtocolInfo =
{
	peSAMSUNG48, "Samsung48",					// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 48, 48, 150000 } }, bdLSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 550, 550 } },				// Описание бита "0"
	{ 1, bcMARK, { 550, 1650 } },				// Описание бита "1"
	true, { 1, bcMARK, { 4500, 4500 } },		// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 45000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 550, 550 } }			// Финальный бит
};
