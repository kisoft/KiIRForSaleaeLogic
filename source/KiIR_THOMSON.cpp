#include "KiIR_THOMSON.h"

const Protocol THOMSON_ProtocolInfo =
{
	peTHOMSON, "THOMSON",						// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 12, 12, 100000 } }, bdMSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 550, 2000 } },				// �������� ���� "0"
	{ 1, bcMARK, { 550, 4500 } },				// �������� ���� "1"
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 35000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 550, 2000 } }			// ��������� ���
};
