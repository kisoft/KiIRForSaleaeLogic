#include "KiIR_JVC.h"

const Protocol JVC_ProtocolInfo =
{
	peJVC, "JVC",								// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 16, 16, 50000 } }, bdLSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 526, 1050 - 526 } },			// �������� ���� "0"
	{ 1, bcMARK, { 526, 2100 - 526 } },			// �������� ���� "1"
	true, { 1, bcMARK, { 8400, 4200 } },		// ������� ����� ��� Lead, �������� Lead
	false, { 0, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtCodeWithoutLead, ritPeriod, 55000 },	// ������ (�� � �������)
	true, { 0, bcMARK, { 526, 1050 - 526 } }	// ��������� ���
};
