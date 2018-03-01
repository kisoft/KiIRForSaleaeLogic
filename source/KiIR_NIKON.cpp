#include "KiIR_NIKON.h"

const Protocol NIKON_ProtocolInfo =
{
	peNIKON, "Nikon (Camera)",					// ������������� � �������� ���������
	ptPulseDistance,							// ��� ���������
	{ { 0, 2, 2, 70000 } }, bdMSB,				// ���������� ���, ���-�� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 500, 1500 } },				// �������� ���� "0"
	{ 1, bcMARK, { 500, 3500 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 2200, 27100 } },		// ������� ����� ��� Lead, �������� Lead
	false, { 1, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	false, { rtNormal, ritPeriod, 47000 },		// ������ (�� � �������)
	true, { 0, bcMARK, { 500, 1500 } }			// ��������� ���
};
