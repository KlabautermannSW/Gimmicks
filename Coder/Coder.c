#include <stdio.h>
#include <string.h>


/*	function:	main
	-----------------------
	in: 		int argc					number of command line parameters
				char *argv[]				command line parameter list
	out:		int							error code
	-----------------------
	comment:
*/
int main( int argc, char *argv[] )
	{
	char * mailto = "mailto:";
	char buffer[1000];
	int i;
	int length;
	
	if( argc > 1 )
		{
		length = strlen(argv[1]);
		
		memcpy(buffer, argv[1], length);
		buffer[length] = 0;

		printf("%s\n", buffer);
		
		printf("<a href=\"");
		for( i = 0; i < strlen(mailto); ++i )
			printf("&#%d;", mailto[i]);
		for(i = 0; i < strlen(buffer); ++i )
			printf("&#%d;", buffer[i]);
		printf("\">");
		for(i = 0; i < strlen(buffer); ++i )
			printf("&#%d;", buffer[i]);
		printf("</a>");
		printf("\n");
		}
	else
		printf("Bitte Text zum Codieren eingeben.\n");

	return 0;
	}
