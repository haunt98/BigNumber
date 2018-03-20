#include "qint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Ham phu tro
void cong_1_bit(bool *bits, int size);
void tru_1_bit(bool *bits, int size);
void in_bit(bool *bits, int size);
void nghich_dao_bit(bool *bits, int size);
void doi_dau_bit(bool *bits, int size);
bool la_chu_so(char c, int radix);
bool la_hop_le_QInt(char *num, int radix);

/* Chuyen input -> QInt
 * cu the, input -> bit[128]
 * bit[128] chia ra 4 sequence dai 32 bit
 * moi sequence la mot block cua QInt
 */
int seq_to_int(bool *bits, int from, int to);
void chia_2_str10(char *num);
bool *str10_to_bit(char *num);
// TODO chuyen string dang 2-digits va 16-digits ve bit[128]
bool *str2_to_bit(char *num);
bool *str16_to_bit(char *num);

/* Chuyen QInt -> input
 * cu the, doi tung block cua QInt ve sequence dai 32 bit
 * gop 4 sequence -> bit[128]
 * bit[128] -> input (so dang string)
 */
void int_to_seq(int x, bool *bits, int from, int to);
void cong_str10(char *A, char *B);
void nhan_2_str10(char *num);
char *bit_to_str10(bool *bits, int size);
// TODO chuyen bit ve dang string 2-digits
char *bit_to_str2(bool *bit, int size);

int seq_to_uint(bool *bits, int from, int to);

// Dich bit
void dich_trai_1_bit(bool *bits, int size);
void dich_phai_1_bit(bool *bits, int size);

// Ham phu tro
void cong_1_bit(bool *bits, int size)
{
	if (bits[size - 1] == 0) {
		// 0 + 1 = 1
		bits[size - 1] = 1;
	} else {
		// 1 + 1 = 10
		bits[size - 1] = 0;
		int d = 1;
		for (int i = size - 2; i >= 0; --i) {
			if (d == 0)
				break;
			if (bits[i] == 0) {
				bits[i] = 1;
				d = 0;
			} else {
				bits[i] = 0;
			}
		}
	}
}

void tru_1_bit(bool *bits, int size)
{
	if (bits[size - 1] == 1) {
		// 1 - 1 = 0
		bits[size - 1] = 0;
	} else {
		// (1)0 - 1 = 1
		bits[size - 1] = 1;
		int d = 1;
		for (int i = size - 2; i >= 0; --i) {
			if (d == 0)
				break;
			if (bits[i] == 1) {
				bits[i] = 0;
				d = 0;
			} else {
				bits[i] = 1;
			}
		}
	}
}

void in_bit(bool *bits, int size)
{
	for (int i = 0; i < size; ++i) {
		printf("%d", bits[i]);
	}
	printf("\n");
}

// 101 -> 010
void nghich_dao_bit(bool *bits, int size)
{
	for (int i = 0; i < size; ++i) {
		bits[i] = 1 - bits[i];
	}
}

// am -> duong va nguoc lai (bu 2)
void doi_dau_bit(bool *bits, int size)
{
	if (bits[0] == 0) {
		// so duong
		nghich_dao_bit(bits, size);
		cong_1_bit(bits, size);
	} else {
		tru_1_bit(bits, size);
		nghich_dao_bit(bits, size);
	}
}

// radix la co so (10, 2, 16)
bool la_chu_so(char c, int radix)
{
	if (radix == 10)
		return c >= '0' && c <= '9';
	if (radix == 2)
		return c == '0' || c == '1';
	if (radix == 16)
		return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
	return 0;
}

bool la_hop_le_QInt(char *num, int radix)
{
	if (!num || strlen(num) <= 0)
		return 0;
	int i = 0;
	if (radix == 10) {
		if (num[i] == '+' || num[i] == '-')
			++i;
	}
	for (; num[i] != '\0'; ++i) {
		if (!la_chu_so(num[i], radix))
			return 0;
	}
	return 1;
}

/* Chuyen input -> QInt
 * cu the, input -> bit[128]
 * bit[128] chia ra 4 sequence dai 32 bit
 * moi sequence la mot block cua QInt
 */

// so string 10-digits chia 2 (ket qua > 0)
void chia_2_str10(char *num)
{
	char *thuong = (char *)malloc(sizeof(char) * (strlen(num) + 1));

	int i = 0;
	if (num[i] == '-' || num[i] == '+')
		++i;
	int temp = num[i] - '0';
	if (num[i] - '0' < 2) {
		if (num[i + 1] == '\0') {
			strcpy(num, "0");
			free(thuong);
			return;
		}
		temp = temp * 10 + num[i + 1] - '0';
		++i;
	}

	int j = 0;
	for (; num[i] != '\0'; ++i) {
		thuong[j++] = temp / 2 + '0';
		temp = (temp % 2) * 10 + num[i + 1] - '0';
	}
	thuong[j] = '\0';
	strcpy(num, thuong);
	free(thuong);
}

// Doi string 10-digits ra bits[128]
bool *str10_to_bit(char *num)
{
	const int size = 128;
	bool *bits = (bool *)malloc(sizeof(bool) * size);
	if (!la_hop_le_QInt(num, 10)) {
		printf("Input khong hop le\n");
		for (int i = 0; i < size; ++i) {
			bits[i] = 0;
		}
		return bits;
	}

	char *temp_num = (char *)malloc(sizeof(char) * (strlen(num) + 1));
	temp_num[strlen(num)] = '\0';
	strcpy(temp_num, num);
	int i = 0;
	int laSoAm = 0;
	if (temp_num[i] == '-' || temp_num[i] == '+') {
		if (temp_num[i] == '-') {
			laSoAm = 1;
		}
		++i;
	}

	for (i = size - 1; i >= 0; --i) {
		if ((temp_num[strlen(temp_num) - 1] - '0') % 2 == 0) {
			bits[i] = 0;
		} else {
			bits[i] = 1;
		}
		chia_2_str10(temp_num);
	}
	if (laSoAm) {
		doi_dau_bit(bits, size);
	}
	free(temp_num);
	return bits;
}

// chuyen 1 sequence (day bit bu 2) sang so nguyen
int seq_to_int(bool *bits, int from, int to)
{
	int size = to - from + 1;
	if (size != 32) {
		printf("Sequence khong du 32 bit\n");
		return 0;
	}
	bool *temp_bits = (bool *)malloc(sizeof(bool) * size);
	for (int i = 0; i < size; ++i) {
		temp_bits[i] = bits[from + i];
	}
	bool laSoAm = 0;
	if (temp_bits[0] == 1) {
		laSoAm = 1;
		doi_dau_bit(temp_bits, size);
	}
	int result = 0;
	for (int i = 0; i < size; ++i) {
		result = result * 2 + temp_bits[i];
	}
	free(temp_bits);
	return laSoAm == 0 ? result : -result;
}

/* Chuyen QInt -> input
 * cu the, doi tung block cua QInt ve sequence dai 32 bit
 * gop 4 sequence -> bit[128]
 * bit[128] -> input (so dang string)
 */
void int_to_seq(int x, bool *bits, int from, int to)
{
	int size = to - from + 1;
	if (size != 32) {
		printf("Sequence khong du 32 bit\n");
		return;
	}
	for (int i = to; i >= from; --i) {
		bits[i] = x & 1;
		x >>= 1;
	}
}

// A = A + B (10-digits, A va B > 0)
void cong_str10(char *A, char *B)
{
	if (!A || !B || strlen(A) != strlen(B)) {
		printf("A, B khong cung chieu dai\n");
		return;
	}
	int d = 0;
	for (int i = strlen(A) - 1; i >= 0; --i) {
		int sum = A[i] - '0' + B[i] - '0' + d;
		A[i] = sum % 10 + '0';
		d = sum / 10;
	}
}

// num = num * 2 (10-digits, num > 0)
void nhan_2_str10(char *num)
{
	if (!num || strlen(num) < 1) {
		printf("num khong hop le\n");
		return;
	}
	int d = 0;
	for (int i = strlen(num) - 1; i >= 0; --i) {
		int multi = (num[i] - '0') * 2 + d;
		num[i] = multi % 10 + '0';
		d = multi / 10;
	}
}

/* vi du doi 0101(2) ra 10-digits
 * di tu phai qua trai, cho num = 0
 * so 1 dau tien la 2^0 -> cong vao num
 * so 1 cuoi cung la 2^2 -> cong vao num
 */
char *bit_to_str10(bool *bits, int size)
{
	// Kiem tra so am
	bool *temp_bits = (bool *)malloc(sizeof(int) * size);
	for (int i = 0; i < size; ++i) {
		temp_bits[i] = bits[i];
	}
	bool laSoAm = 0;
	if (temp_bits[0] == 1) {
		laSoAm = 1;
		doi_dau_bit(temp_bits, size);
	}

	// 2^10 = 1024 xap xi 10^3, 128 : 10 x 3 = 38.4
	// nen so thap phan khong qua 39 chu so
	// lay 50 cho dep
	const int max_size = 50;

	// xay dung num="00..01"
	// roi nhan 2 dan dan theo temp_bits[]
	char *num = (char *)malloc(sizeof(char) * (max_size + 1));
	num[max_size] = '\0';
	for (int i = 0; i < max_size; ++i) {
		num[i] = '0';
	}

	// temp_bits[] -> num
	for (int i = 0; i < size; ++i) {
		if (temp_bits[size - 1 - i] == 1) {
			// lay 2^i
			char *temp =
			    (char *)malloc(sizeof(char) * (max_size + 1));
			temp[max_size] = '\0';
			for (int j = 0; j < max_size - 1; ++j) {
				temp[j] = '0';
			}
			temp[max_size - 1] = '1';
			for (int j = 0; j < i; ++j) {
				nhan_2_str10(temp);
			}
			cong_str10(num, temp);
			free(temp);
		}
	}

	free(temp_bits);
	int i = 0;
	while (num[i] == '0')
		++i;
	if (i == max_size)
		--i;
	int new_size = size - i + laSoAm;
	char *new_num = (char *)malloc(sizeof(char) * (new_size + 1));
	new_num[new_size] = '\0';
	if (laSoAm)
		new_num[0] = '-';
	strcpy(new_num + laSoAm, num + i);
	free(num);
	return new_num;
}

// Nhap xuat theo YEUCAU
void ScanQInt(QInt &q)
{
	const int max_size = 50;
	char num[max_size + 1];
	printf("Nhap so nguyen lon: ");
	scanf("%s", num);
	bool *bits = str10_to_bit(num);
	q = BinToDec_int(bits);
	free(bits);
}

void PrintQInt(QInt q)
{
	bool *bits = DecToBin_int(q);
	printf("Xuat so nguyen lon: ");
	char *str10 = bit_to_str10(bits, 128);
	printf("%s\n", str10);
	free(bits);
	free(str10);
}

// Chuyen doi theo YEUCAU, mac dinh bits la 128
bool *DecToBin_int(QInt q)
{
	bool *bits = (bool *)malloc(sizeof(bool) * 128);
	for (int i = 0, y = 0; i < 4; ++i, y += 32) {
		int_to_seq(q.block[i], bits, y, y + 31);
	}
	return bits;
}

QInt BinToDec_int(bool *bits)
{
	QInt x;
	for (int i = 0, j = 0; i < 4; ++i, j += 32) {
		x.block[i] = seq_to_int(bits, j, j + 31);
	}
	return x;
}

// Doi ra so nguyen khong dau
int seq_to_uint(bool *bits, int from, int to)
{
	int size = to - from + 1;
	if (size >= 32) {
		return 0;
	}

	int result = 0;
	for (int i = 0; i < size; ++i) {
		result = result * 2 + bits[i + from];
	}
	return result;
}

char *BinToHex_int(bool *bits)
{
	// cu 4 bits la 1 chu so hexa
	// 128 : 4 = 32
	// chuyen doi tung nibble (4-bit)
	const int size = 32;
	char *hexas = (char *)malloc(sizeof(char) * (size + 1));
	hexas[size] = '\0';
	char hexa_digit[] = "0123456789ABCDEF";
	for (int i = 0, j = 0; i < size; ++i, j += 4) {
		int d = seq_to_uint(bits, j, j + 3);
		hexas[i] = hexa_digit[d];
	}
	return hexas;
}

char *DecToHex_int(QInt q)
{
	bool *temp_bits = DecToBin_int(q);
	bool bits[128];
	for (int i = 0; i < 128; ++i) {
		bits[i] = temp_bits[i];
	}
	free(temp_bits);
	return BinToHex_int(bits);
}

/* Xu ly toan tu + - * /
 * tran so cua cong: am + am = duong va duong + duong = am
 * tru la nguoc lai cua cong
 * nhan su dung booth
 */
QInt operator+(QInt a, QInt b)
{
	bool *bits_1 = DecToBin_int(a);
	bool *bits_2 = DecToBin_int(b);
	const int size = 128;
	bool *tong = (bool *)malloc(sizeof(bool) * (size));
	int rememberNumber = 0; // 1 + 1 = 0 remember 1
	for (int i = size - 1; i >= 0; --i) {
		if (rememberNumber == 0) {
			if (bits_1[i] == 1 && bits_2[i] == 1) {
				tong[i] = 0;
				rememberNumber = 1;
			} else {
				if (bits_1[i] == 0 && bits_2[i] == 0)
					tong[i] = 0;
				else
					tong[i] = 1;
			}
		} else {
			if (bits_1[i] == 0 && bits_2[i] == 0) {
				tong[i] = 1;
				rememberNumber = 0;
			} else {
				if (bits_1[i] == 1 && bits_2[i] == 1)
					tong[i] = 1;
				else
					tong[i] = 0;
			}
		}
	}
	QInt q = BinToDec_int(tong);
	free(bits_1);
	free(bits_2);
	free(tong);
	return q;
}

QInt operator-(QInt a, QInt b)
{
	// tru la cong voi so doi
	bool *bits_2 = DecToBin_int(b);
	doi_dau_bit(bits_2, 128);
	QInt new_b = BinToDec_int(bits_2);
	free(bits_2);
	return a + new_b;
}

// nhan bang thuat toan booth
// A  Q  Q0  M
// 0  a  0   .
QInt operator*(QInt a, QInt M)
{
	const int size = 128;
	bool *bits_Q = DecToBin_int(a);
	bool *bits_A = (bool *)malloc(sizeof(bool) * (size));
	for (int i = 0; i < size; ++i) {
		bits_A[i] = 0;
	}
	bool bit_Q0 = 0;

	// 128 step
	for (int i = 0; i < size; ++i) {
		if (bits_Q[size - 1] != bit_Q0) {
			QInt A = BinToDec_int(bits_A);
			if (bits_Q[size - 1] == 1 && bit_Q0 == 0) {
				A = A - M;
			} else {
				A = A + M;
			}
			bool *bits_temp = DecToBin_int(A);
			for (int j = 0; j < size; ++j) {
				bits_A[j] = bits_temp[j];
			}
			free(bits_temp);
		}
		// dich phai bit
		bool last_A = bits_A[size - 1];
		bit_Q0 = bits_Q[size - 1];
		for (int j = size - 1; j > 0; --j) {
			bits_A[j] = bits_A[j - 1];
			bits_Q[j] = bits_Q[j - 1];
		}
		bits_Q[0] = last_A;
	}
	// Ket qua cuoi cung la day bit A va Q ket hop lai
	// nhung bo di A vi overflow
	QInt q = BinToDec_int(bits_Q);
	free(bits_Q);
	free(bits_A);
	return q;
}

// Chia bang thuat toan chia khong dau
// A  Q           M
// 0  so bi chia  so chia
QInt operator/(QInt Q, QInt M)
{
	bool *Q_bits = DecToBin_int(Q);
	bool *M_bits = DecToBin_int(M);
	// Xet dau
	bool Q_sign = 0;
	bool M_sign = 0;
	if (Q.block[0] < 0) {
		Q_sign = 1;
		doi_dau_bit(Q_bits, QInt_Size);
	}
	if (M.block[0] < 0) {
		M_sign = 1;
		doi_dau_bit(M_bits, QInt_Size);
		M = BinToDec_int(M_bits);
	}
	free(M_bits);

	// A la bien tam
	bool *A_bits = (bool *)malloc(sizeof(bool) * QInt_Size);
	QInt A;
	for (int i = 0; i < QInt_Size; ++i) {
		A_bits[i] = 0;
	}
	bool Q_first = 0;

	for (int i = 0; i < QInt_Size; ++i) {
		// dich trai bits
		Q_first = Q_bits[0];
		for (int j = 0; j < QInt_Size - 1; ++j) {
			Q_bits[j] = Q_bits[j + 1];
			A_bits[j] = A_bits[j + 1];
		}
		A_bits[QInt_Size - 1] = Q_first;

		A = BinToDec_int(A_bits);
		A = A - M;

		// A < 0
		if (A.block[0] < 0) {
			Q_bits[QInt_Size - 1] = 0;
			A = A + M;
		} else {
			Q_bits[QInt_Size - 1] = 1;
		}
		free(A_bits);
		A_bits = DecToBin_int(A);
	}

	// Xet dau ket qua
	if ((Q_sign ^ M_sign) == 1) {
		doi_dau_bit(Q_bits, QInt_Size);
	}

	QInt q = BinToDec_int(Q_bits);
	free(Q_bits);
	free(A_bits);
	return q;
}

/* Xu ly toan tu AND (&), OR(|), XOR(^), NOT(~)
 * dich trai(<<) va dich phai(>>)
 */
QInt operator&(QInt a, QInt b)
{
	bool *bits_1 = DecToBin_int(a);
	bool *bits_2 = DecToBin_int(b);
	bool *bits_3 = (bool *)malloc(sizeof(bool) * 128);

	for (int i = 0; i < 128; ++i) {
		if (bits_1[i] == 1 && bits_2[i] == 1)
			bits_3[i] = 1;
		else
			bits_3[i] = 0;
	}

	QInt q = BinToDec_int(bits_3);

	free(bits_1);
	free(bits_2);
	free(bits_3);
	return q;
}

QInt operator|(QInt a, QInt b)
{
	bool *bits_1 = DecToBin_int(a);
	bool *bits_2 = DecToBin_int(b);
	bool *bits_3 = (bool *)malloc(sizeof(bool) * 128);

	for (int i = 0; i < 128; ++i) {
		if (bits_1[i] == 0 && bits_2[i] == 0)
			bits_3[i] = 0;
		else
			bits_3[i] = 1;
	}

	QInt q = BinToDec_int(bits_3);

	free(bits_1);
	free(bits_2);
	free(bits_3);
	return q;
}

QInt operator^(QInt a, QInt b)
{
	bool *bits_1 = DecToBin_int(a);
	bool *bits_2 = DecToBin_int(b);
	bool *bits_3 = (bool *)malloc(sizeof(bool) * 128);

	for (int i = 0; i < 128; ++i) {
		if (bits_1[i] == bits_2[i])
			bits_3[i] = 0;
		else
			bits_3[i] = 1;
	}

	QInt q = BinToDec_int(bits_3);

	free(bits_1);
	free(bits_2);
	free(bits_3);
	return q;
}

QInt operator~(QInt a)
{
	bool *bits_1 = DecToBin_int(a);
	nghich_dao_bit(bits_1, 128);
	QInt q = BinToDec_int(bits_1);
	free(bits_1);
	return q;
}

/* Dich sang trai, phai dung 1 bit
 * Dich trai luon them so 0
 * Dich phai them bit dau tien (0 hoac 1)
 */
void dich_trai_1_bit(bool *bits, int size)
{
	for (int i = 0; i < size - 1; ++i) {
		bits[i] = bits[i + 1];
	}
	bits[size - 1] = 0;
}
void dich_phai_1_bit(bool *bits, int size)
{
	for (int i = size - 1; i > 0; --i) {
		bits[i] = bits[i - 1];
	}
}

QInt operator<<(QInt a, int count)
{
	const int size = 128;
	bool *bits = DecToBin_int(a);
	if (count >= size) {
		for (int i = 0; i < size; ++i) {
			bits[i] = 0;
		}
	} else {
		for (int i = 0; i < count; ++i) {
			dich_trai_1_bit(bits, size);
		}
	}
	QInt q = BinToDec_int(bits);
	free(bits);
	return q;
}

QInt operator>>(QInt a, int count)
{
	const int size = 128;
	bool *bits = DecToBin_int(a);
	if (count >= size) {
		for (int i = 1; i < size; ++i) {
			bits[i] = bits[0];
		}
	} else {
		for (int i = 0; i < count; ++i) {
			dich_phai_1_bit(bits, size);
		}
	}
	QInt q = BinToDec_int(bits);
	free(bits);
	return q;
}

// Cac ham kiem tra
void test_input_convert_int()
{
	printf("Test input output int\n");
	QInt q;
	ScanQInt(q);
	PrintQInt(q);

	printf("DecToBin_int\n");
	bool *bits = DecToBin_int(q);
	in_bit(bits, 128);

	printf("BinToDec_int\n");
	QInt q_2 = BinToDec_int(bits);
	PrintQInt(q_2);

	printf("BinToHex_int\n");
	char *s1 = BinToHex_int(bits);
	printf("%s\n", s1);
	free(s1);

	printf("DecToHex_int\n");
	char *s2 = DecToHex_int(q);
	printf("%s\n", s2);
	free(s2);

	free(bits);
}

void test_cong_tru_int()
{
	printf("Test cong tru int\n");
	QInt q_1, q_2;
	ScanQInt(q_1);
	ScanQInt(q_2);

	printf("Tong\n");
	QInt q_3 = q_1 + q_2;
	PrintQInt(q_3);

	printf("Hieu\n");
	QInt q_4 = q_1 - q_2;
	PrintQInt(q_4);
}

void test_nhan_chia_int()
{
	printf("Test nhan chia int\n");
	QInt q_1, q_2;
	ScanQInt(q_1);
	ScanQInt(q_2);

	printf("Nhan\n");
	QInt q_3 = q_1 * q_2;
	PrintQInt(q_3);

	printf("Chia\n");
	QInt q_4 = q_1 / q_2;
	PrintQInt(q_4);
}

void test_bit_operator_int()
{
	printf("Test bit operator int\n");
	QInt q_1, q_2;
	ScanQInt(q_1);
	ScanQInt(q_2);

	printf("AND\n");
	QInt q_3 = q_1 & q_2;
	PrintQInt(q_3);

	printf("OR\n");
	QInt q_4 = q_1 | q_2;
	PrintQInt(q_4);

	printf("XOR\n");
	QInt q_5 = q_1 ^ q_2;
	PrintQInt(q_5);

	printf("NOT\n");
	QInt q_6 = ~q_1;
	QInt q_7 = ~q_2;
	PrintQInt(q_6);
	PrintQInt(q_7);

	printf("Left Shift\n");
	QInt q_8 = q_1 << 1;
	QInt q_9 = q_2 << 2;
	PrintQInt(q_8);
	PrintQInt(q_9);

	printf("Right Shift\n");
	QInt q_10 = q_1 >> 1;
	QInt q_11 = q_2 >> 2;
	PrintQInt(q_10);
	PrintQInt(q_11);
}
