#include "KiIR_SAMSUNG32.h"

const Protocol SAMSUNG32_ProtocolInfo =
{
	peSAMSUNG32, "Samsung32",					// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 32, 32, 150000 } }, bdLSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 550, 550 } },				// �������� ���� "0"
	{ 1, bcMARK, { 550, 1650 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 4500, 4500 } },		// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 47000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 550, 550 } }			// ��������� ���
};
