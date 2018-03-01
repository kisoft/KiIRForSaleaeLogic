#include "KiIR_SIRC.h"

const Protocol SIRC_ProtocolInfo =
{
	peSIRC, "SIRC",								// ������������� � �������� ���������
	ptPulseWidth,								// ��� ���������
	{ { 0, static_cast<U8>(FLOAT_BITS_COUNT), 12, 150000 } }, bdLSB, // ���������� ���, ���������� ��� ��� ��������� ������, ���������� �������, ������ LSB ��� MSB
	{ 0, bcMARK, { 600, 600 } },				// �������� ���� "0"
	{ 1, bcMARK, { 1200, 600 } },				// �������� ���� "1"
	true, { 1, bcMARK, { 2400, 600 } },			// ������� ����� ��� Lead, �������� Lead
	false, { 0, bcMARK, { 0, 0 } },				// ������� ����� ��� Repeat Lead, �������� Repeat Lead
	true, { rtNormal, ritPeriod, 45000 },		// ������ (�� � �������)
	false, { 0, bcMARK, { 0, 0 } }				// ��������� ���
};
