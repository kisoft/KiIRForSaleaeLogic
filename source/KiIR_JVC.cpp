#include "KiIR_JVC.h"

const Protocol JVC_ProtocolInfo =
{
	peJVC, "JVC",								// Идентификатор и название протокола
	ptPulseDistance,							// Тип протокола
	{ { 0, 16, 16, 50000 } }, bdLSB,			// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 526, 1050 - 526 } },			// Описание бита "0"
	{ 1, bcMARK, { 526, 2100 - 526 } },			// Описание бита "1"
	true, { 1, bcMARK, { 8400, 4200 } },		// Наличие битов для Lead, Описание Lead
	false, { 0, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtCodeWithoutLead, ritPeriod, 55000 },	// Повтор (всё о повторе)
	true, { 0, bcMARK, { 526, 1050 - 526 } }	// Финальный бит
};
