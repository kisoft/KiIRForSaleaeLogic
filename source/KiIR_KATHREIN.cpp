#include "KiIR_KATHREIN.h"

const Protocol KATHREIN_ProtocolInfo =
{
	peKATHREIN, "KATHREIN",						// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 11, 11, 70000 } }, bdMSB,			// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 210, 1400 } },				// �������� ���� "0"
	{ 1, bcMARK, { 210, 3000 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 210, 6218 } },			// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 35000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 210, 1400 } }			// ��������� ���
};
