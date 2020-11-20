/************************************************************************
Lab 9 Nios Software

Dong Kai Wang, Fall 2017
Christine Chen, Fall 2013

For use with ECE 385 Experiment 9
University of Illinois ECE Department
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "aes.h"

// Pointer to base address of AES module, make sure it matches Qsys
volatile unsigned int * AES_PTR = (unsigned int *) 0x00000040;

// Execution mode: 0 for testing, 1 for benchmarking
int run_mode = 0;

/** charToHex
 *  Convert a single character to the 4-bit value it represents.
 *  
 *  Input: a character c (e.g. 'A')
 *  Output: converted 4-bit value (e.g. 0xA)
 */
char charToHex(char c)
{
	char hex = c;

	if (hex >= '0' && hex <= '9')
		hex -= '0';
	else if (hex >= 'A' && hex <= 'F')
	{
		hex -= 'A';
		hex += 10;
	}
	else if (hex >= 'a' && hex <= 'f')
	{
		hex -= 'a';
		hex += 10;
	}
	return hex;
}

/** charsToHex
 *  Convert two characters to byte value it represents.
 *  Inputs must be 0-9, A-F, or a-f.
 *  
 *  Input: two characters c1 and c2 (e.g. 'A' and '7')
 *  Output: converted byte value (e.g. 0xA7)
 */
char charsToHex(char c1, char c2)
{
	char hex1 = charToHex(c1);
	char hex2 = charToHex(c2);
	return (hex1 << 4) + hex2;
}

void printMat(unsigned char (*w)[4]){
	int i;
	for(i=0;i<4;i++){
		printf("%2x  %2x %2x %2x\n", w[i][0],w[i][1],w[i][2],w[i][3]);
	}
}

void AddRoundKey(unsigned char(*state)[4], unsigned char(*w)[4])
{
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			state[i][j]=state[i][j]^w[i][j];
		}
	}
}

void SubBytes(unsigned char(*state)[4])
{
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			state[i][j]=aes_sbox[state[i][j]];
		}
	}
}

void ShiftRows(unsigned char(*state)[4])
{
	unsigned char line0[4]={state[0][0],state[0][1],state[0][2],state[0][3]};
	unsigned char line1[4]={state[1][1],state[1][2],state[1][3],state[1][0]};
	unsigned char line2[4]={state[2][2],state[2][3],state[2][0],state[2][1]};
	unsigned char line3[4]={state[3][3],state[3][0],state[3][1],state[3][2]};

	int i;
	for (i=0;i<4;i++)
	{
		state[0][i]=line0[i];
	}

	for (i=0;i<4;i++)
	{
		state[1][i]=line1[i];
	}

	for (i=0;i<4;i++)
	{
		state[2][i]=line2[i];
	}

	for (i=0;i<4;i++)
	{
		state[3][i]=line3[i];
	}
}

void MixColumns(unsigned char(*state)[4])
{
	unsigned char Col[4];
	unsigned char newCol[4]; //result
	int i,j;
	for (j=0;j<4;j++){
		for(i=0;i<4;i++){
			Col[i]=state[i][j];
		}


		//use gf_mul according to the instruction
		newCol[0]=gf_mul[Col[0]][0]^gf_mul[Col[1]][1]^Col[2]^Col[3];
		newCol[1]=Col[0]^gf_mul[Col[1]][0]^gf_mul[Col[2]][1]^Col[3];
		newCol[2]=Col[0]^Col[1]^gf_mul[Col[2]][0]^gf_mul[Col[3]][1];
		newCol[3]=gf_mul[Col[0]][1]^Col[1]^Col[2]^gf_mul[Col[3]][0];

		for(i=0;i<4;i++)
		{
			state[i][j]=newCol[i];
		}
	}
}

void KeyExpansion(unsigned char(*w)[4][4])
{
	int key;
	int word;
	int byte;
	unsigned char temp[4];
	unsigned char tempbyte;
	unsigned char tempvalue[11][4][4];
	int Nr=11;
	for (key=1;key<Nr;key++)
	{
		for(word=0;word<4;word++){
			//rotword
			if(word==0){
				for(byte=0;byte<4;byte++){
					temp[byte]=w[key-1][3][byte];
				}

				tempbyte=temp[0];

				for(byte=0;byte<3;byte++){
					temp[byte]=temp[byte+1];
				}

				temp[3]=tempbyte;
				//subword
				for(byte=0;byte<4;byte++){
					temp[byte]=aes_sbox[temp[byte]];
				}
				
				//XOR step
				temp[0]=temp[0]^(Rcon[key]>>24);
				temp[1]=temp[1]^((Rcon[key]<<8)>>24);
				temp[2]=temp[2]^((Rcon[key]<<16)>>24);
				temp[3]=temp[3]^((Rcon[key]<<24)>>24);
			}else
			{
				for(byte=0;byte<4;byte++)
				{
					temp[byte]=w[key][word-1][byte];
				}
			}
			for(byte=0;byte<4;byte++)
			{
				w[key][word][byte]=temp[byte]^w[key-1][word][byte];
			}
		}
	}
	for(key=0;key<Nr;key++){
		for(word=0;word<4;word++){
			for(byte=0;byte<4;byte++){
				tempvalue[key][byte][word] = w[key][word][byte];
			}
		}
	}
	for(key=0;key<Nr;key++){
		for(word=0;word<4;word++){
			for(byte=0;byte<4;byte++){
				w[key][word][byte] = tempvalue[key][word][byte];
			}
		}
	}
}
/** encrypt
 *  Perform AES encryption in software.
 *
 *  Input: msg_ascii - Pointer to 32x 8-bit char array that contains the input message in ASCII format
 *         key_ascii - Pointer to 32x 8-bit char array that contains the input key in ASCII format
 *  Output:  msg_enc - Pointer to 4x 32-bit int array that contains the encrypted message
 *               key - Pointer to 4x 32-bit int array that contains the input key
 */
void encrypt(unsigned char * msg_ascii, unsigned char * key_ascii, unsigned int * msg_enc, unsigned int * key)
{
	// Implement this function

	int i,j,k;
	unsigned char w[11][4][4];
	unsigned char state[4][4];
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			k=i*2+j*8;
			w[0][j][i]=charsToHex(key_ascii[k],key_ascii[k+1]);
			state[i][j]=charsToHex(msg_ascii[k],msg_ascii[k+1]);
		}
	}

	KeyExpansion(w);
	AddRoundKey(state,w[0]);
	for(i=0;i<9;i++)
	{
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(state,w[i+1]);

	}
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state, w[10]);

	for(i=0;i<4;i++){
		msg_enc[i] = (state[0][i]<<24) ^ (state[1][i]<<16) ^ (state[2][i]<<8) ^ (state[3][i]);
		key[i] = (w[0][0][i]<<24) ^ (w[0][1][i]<<16) ^ (w[0][2][i]<<8) ^ (w[0][3][i]);
	}
}

/** decrypt
 *  Perform AES decryption in hardware.
 *
 *  Input:  msg_enc - Pointer to 4x 32-bit int array that contains the encrypted message
 *              key - Pointer to 4x 32-bit int array that contains the input key
 *  Output: msg_dec - Pointer to 4x 32-bit int array that contains the decrypted message
 */
void decrypt(unsigned int * msg_enc, unsigned int * msg_dec, unsigned int * key)
{
	// Implement this function
	AES_PTR[0] = key[0];
	AES_PTR[1] = key[1];
	AES_PTR[2] = key[2];
	AES_PTR[3] = key[3];
	AES_PTR[4] = msg_enc[0];
	AES_PTR[5] = msg_enc[1];
	AES_PTR[6] = msg_enc[2];
	AES_PTR[7] = msg_enc[3];

	AES_PTR[14] = 1;
	while(AES_PTR[15] == 0){}
	msg_dec[0] = AES_PTR[8];
	msg_dec[1] = AES_PTR[9];
	msg_dec[2] = AES_PTR[10];
	msg_dec[3] = AES_PTR[11];
	AES_PTR[14] = 0;

}

/** main
 *  Allows the user to enter the message, key, and select execution mode
 *
 */
int main()
{
	// Input Message and Key as 32x 8-bit ASCII Characters ([33] is for NULL terminator)
	unsigned char msg_ascii[33];
	unsigned char key_ascii[33];
	// Key, Encrypted Message, and Decrypted Message in 4x 32-bit Format to facilitate Read/Write to Hardware
	unsigned int key[4];
	unsigned int msg_enc[4];
	unsigned int msg_dec[4];

	printf("Select execution mode: 0 for testing, 1 for benchmarking: ");
	scanf("%d", &run_mode);

	if (run_mode == 0) {
		// Continuously Perform Encryption and Decryption
		while (1) {
			int i = 0;
			printf("\nEnter Message:\n");
			scanf("%s", msg_ascii);
			printf("\n");
			printf("\nEnter Key:\n");
			scanf("%s", key_ascii);
			printf("\n");
			encrypt(msg_ascii, key_ascii, msg_enc, key);
			printf("\nEncrpted message is: \n");
			for(i = 0; i < 4; i++){
				printf("%08x", msg_enc[i]);
			}
			printf("\n");
			decrypt(msg_enc, msg_dec, key);
			printf("\nDecrypted message is: \n");
			for(i = 0; i < 4; i++){
				printf("%08x", msg_dec[i]);
			}
			printf("\n");
		}
	}
	else {
		// Run the Benchmark
		int i = 0;
		int size_KB = 2;
		// Choose a random Plaintext and Key
		for (i = 0; i < 32; i++) {
			msg_ascii[i] = 'a';
			key_ascii[i] = 'b';
		}
		// Run Encryption
		clock_t begin = clock();
		for (i = 0; i < size_KB * 64; i++)
			encrypt(msg_ascii, key_ascii, msg_enc, key);
		clock_t end = clock();
		double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		double speed = size_KB / time_spent;
		printf("Software Encryption Speed: %f KB/s \n", speed);
		// Run Decryption
		begin = clock();
		for (i = 0; i < size_KB * 64; i++)
			decrypt(msg_enc, msg_dec, key);
		end = clock();
		time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
		speed = size_KB / time_spent;
		printf("Hardware Encryption Speed: %f KB/s \n", speed);
	}
	return 0;
}
