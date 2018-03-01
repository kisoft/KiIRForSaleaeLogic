#include "KiIR_NUBERT.h"

const Protocol NUBERT_ProtocolInfo =
{
	peNUBERT, "NUBERT",							// ������������� � �������� ���������
	ptPulseDistanceWidth,						// ��� ���������
	{ { 0, 10, 10, 35000 }, { 1, 10, 10, 70000 } }, bdMSB,	// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 500, 1300 } },				// �������� ���� "0"
	{ 1, bcMARK, { 1340, 340 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 1340, 340 } },			// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 35000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 500, 500 } }			// ��������� ���
};
