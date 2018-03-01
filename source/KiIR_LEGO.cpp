#include "KiIR_LEGO.h"

const Protocol LEGO_ProtocolInfo =
{
	peLEGO, "LEGO",								// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 16, 16, 70000 } }, bdMSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 158, 263 } },				// Описание бита "0"
	{ 1, bcMARK, { 158, 553 } },				// Описание бита "1"
	true, { 1, bcMARK, { 158, 1026 } },			// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	false, { rtNormal, ritPeriod, 35000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 158, 263 } }			// Финальный бит
};
