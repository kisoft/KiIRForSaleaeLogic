#include "KiIR_PENTAX.h"

const Protocol PENTAX_ProtocolInfo =
{
	pePENTAX, "Pentax (Camera)",				// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 6, 6, 70000 } }, bdMSB,				// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 1000, 1000 } },				// Описание бита "0"
	{ 1, bcMARK, { 1000, 3000 } },				// Описание бита "1"
	true, { 1, bcMARK, { 2200, 27100 } },		// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	false, { rtNormal, ritPeriod, 47000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 1000, 1000 } }			// Финальный бит
};
