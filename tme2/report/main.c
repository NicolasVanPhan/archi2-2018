#include "stdio.h"

__attribute__((constructor)) void main()
{
	char		c;
	char		s[] = "\n Hello World! \n";

	while(1) 
	{
		tty_puts(s);	
		tty_getc(&c);
	}

} // end main
