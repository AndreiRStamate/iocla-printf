#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>

static int write_stdout(const char *token, int length)
{
	int rc;
	int bytes_written = 0;

	do {
		rc = write(1, token + bytes_written, length - bytes_written);
		if (rc < 0)
			return rc;

		bytes_written += rc;
	} while (bytes_written < length);

	return bytes_written;
}

int iocla_printf(const char *format, ...)
{
	va_list args;
	size_t i, k = 0;
	va_start(args, format);
	
	// variabila in care se construieste sirul de afisat
	char* result = (char*)malloc(30000*sizeof(char)); 

	// se parcurge tot formatul si se verifica aparitia '%'
	for (i = 0; i < strlen(format); i++)
	{
		if (format[i] == '%')
		{
			if (format[i+1] == '%')									// cazul %% 
			{														// se insereaza un '%' in rezultat
				result[k++] = '%';
				i++;
			}
			else if (format[i+1] == 'c')							// cazul %c
			{
				result[k++] = (char)va_arg(args, int);				// se insereaza caracterul dat
				i++;												// ca parametru in functie
			}
			else if (format[i+1] == 's')							// cazul %s
			{

				char* stringArg = (char*)malloc(30000*sizeof(char));
				strcpy(stringArg, va_arg(args, char*));				
				int j;												// se salveaza intr-o variabila
				for (j = 0; j < strlen(stringArg); j++)				// auxiliara textul dat ca 
				{													// parametru si se "construieste"
					result[k++] = stringArg[j];						// sirul rezultat caracter cu caracter
				}
				i++;
			}
			else if (format[i+1] == 'd')							// cazul %d
			{														// se construieste un sir auxiliar
				int auxInt = va_arg(args, int);						// in care se introduc cifrele
				char auxString[24];									// numarului pe rand
																	
				int negative = 0;
				if (auxInt < 0)
				{
					negative = 1;
				}

				int j = 0;
				while (auxInt)
				{
					if (negative)									// daca numarul de baza este negativ
					{												// operatia "% 10" returneaza tot un
						auxString[j++] = (auxInt % 10 * -1) + '0'; 	// negativ si inmultim cu "-1"
					}									
					else
					{
						auxString[j++] = (auxInt % 10) + '0';
					}
					auxInt /= 10;
				}
				auxString[j] = '\0';

				if (negative)										
				{
					result[k++] = '-';
				}
				for (j = strlen(auxString)-1; j >= 0; j--)			// la final sirul auxiliar se
				{													// parcurge de la cap la coada
					result[k++] = auxString[j];						// si se introduc "cifrele" in 
				}													// sirul final "result"
				i++;
			}
			else if (format[i+1] == 'u')							// cazul %u
			{
				unsigned auxUnsignedInt = va_arg(args, unsigned);	// similar cu %d, nu se mai tine
				char auxString[24];									// cont de semnul numarului dat
																	// ca parametru
				int j = 0;
				while (auxUnsignedInt)
				{
					auxString[j++] = (auxUnsignedInt % 10) + '0';
					auxUnsignedInt /= 10;
				}
				auxString[j] = '\0';
				for (j = strlen(auxString)-1; j >= 0; j--)
				{
					result[k++] = auxString[j];
				}
				i++;
			}
			else if (format[i+1] == 'x')							// cazul %x
			{														
				char hexa[100];
				int j = 0;
				unsigned auxHex = va_arg(args, unsigned);			// numarul dat ca parametru
				while (auxHex)										// se imparte la 16 si se salveaza
				{													// resturile intr-un sir de caractere
					unsigned copy = auxHex;
					unsigned remainder = copy % 16;					// rest intre 0 si 15 inclusiv

					if (remainder >= 10)
					{
						remainder += 87;							// deoarece baza 16 are drept "valori"
					}												// si caracterele 'a-f', se aduna constanta
					else											// 87 (10 + 87 = 97 = caracter 'a')
					{
						remainder += '0';							// se converteste cifra in caracter
					}
					hexa[j++] = remainder;
					auxHex /= 16;
				}
				hexa[j] = '\0';
				int jj;
				for (jj = j-1; jj >= 0; jj--)						// similar cazurilor %d si %u,
				{													// sirul auxiliar se parcurge de la
					result[k++] = hexa[jj];							// cap la coada si se construieste
				}													// "result", caracter cu caracter
				i++;
			}
		}
		else														// niciun caz special
		{															// se adauga urmatorul caracter din
			result[k++] = format[i];								// format in sirul final
		}
	}
	result[k] = '\0';												// se adauga terminatorul de sir si
	return write_stdout(result, strlen(result));					// se afiseaza tot sirul
}