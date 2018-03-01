#include "KiIR_THOMSON.h"

const Protocol THOMSON_ProtocolInfo =
{
	peTHOMSON, "THOMSON",						// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 12, 12, 100000 } }, bdMSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 550, 2000 } },				// Описание бита "0"
	{ 1, bcMARK, { 550, 4500 } },				// Описание бита "1"
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 35000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 550, 2000 } }			// Финальный бит
};
