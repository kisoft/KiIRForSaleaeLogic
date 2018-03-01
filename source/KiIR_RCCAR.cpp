#include "KiIR_RCCAR.h"

const Protocol RCCAR_ProtocolInfo =
{
	peRCCAR, "RCCAR",							// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 13, 13, 100000 } }, bdLSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 600, 900 } },				// �������� ���� "0"
	{ 1, bcMARK, { 600, 450 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 2000, 2000 } },		// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 40000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 600, 450 } }			// ��������� ���
};
