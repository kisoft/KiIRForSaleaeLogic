#include "KiIR_NUBERT.h"

const Protocol NUBERT_ProtocolInfo =
{
	peNUBERT, "NUBERT",							// Идентификатор и название протокола
	ptPulseDistanceWidth,						// Тип протокола
	{ { 0, 10, 10, 35000 }, { 1, 10, 10, 70000 } }, bdMSB,	// Количество бит, Кол-во бит для тестового фрейма, Количество фреймов, Начало LSB или MSB
	{ 0, bcMARK, { 500, 1300 } },				// Описание бита "0"
	{ 1, bcMARK, { 1340, 340 } },				// Описание бита "1"
	true, { 1, bcMARK, { 1340, 340 } },			// Наличие битов для Lead, Описание Lead
	false, { 1, bcMARK, { 0, 0 } },				// Наличие битов для Repeat Lead, Описание Repeat Lead
	true, { rtNormal, ritPeriod, 35000 },		// Повтор (всё о повторе)
	true, { 0, bcMARK, { 500, 500 } }			// Финальный бит
};
