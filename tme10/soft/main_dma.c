
#include "stdio.h"

#define NPIXEL 256
#define NLINE  256

///////////////////////////////////////////////////////////////////////////////
//	main function
///////////////////////////////////////////////////////////////////////////////
__attribute__ ((constructor)) void main() 
{
    unsigned char 	BUF1[NPIXEL*NLINE];
    unsigned char 	BUF2[NPIXEL*NLINE];
    unsigned char	*BUF;
    unsigned int 	line;
    unsigned int 	pixel;
    unsigned int 	step;

    /* --------------------------- PROLOGUE --------------------------------- */
    step = 1;
    BUF = BUF1;

    for(pixel = 0 ; pixel < NPIXEL ; pixel++)
    { 
        for(line = 0 ; line < NLINE ; line++) 
        {
            if( ( (pixel>>step & 0x1) && !(line>>step & 0x1)) || 
                (!(pixel>>step & 0x1) &&  (line>>step & 0x1)) )  	BUF[NPIXEL*line + pixel] = 0xFF;
            else 							BUF[NPIXEL*line + pixel] = 0x0;
        }
    }
    tty_printf(" - build   %d OK at cycle %d\n", step, proctime());

    // Pipeline synchronization 

    /* --------------------------- LOOP ------------------------------------- */
    for(step = 2 ; step < 6 ; step++)
    {
	/* ---- DISPLAY PICTURE (STEP - 1) ---- */
	BUF = ((step & 1) == 0) ? BUF1 : BUF2;
        if(fb_write(0, BUF, NLINE*NPIXEL) != 0)
	{
            tty_printf("\n!!! error in fb_syn_write syscall !!!\n"); 
	    exit();
	}

	/* ---- BUILD PICTURE (STEP) ---- */
	BUF = (BUF == BUF1) ? BUF2 : BUF1;
        for(pixel = 0 ; pixel < NPIXEL ; pixel++)
        { 
            for(line = 0 ; line < NLINE ; line++) 
            {
                if( ( (pixel>>step & 0x1) && !(line>>step & 0x1)) || 
                    (!(pixel>>step & 0x1) &&  (line>>step & 0x1)) )  	BUF[NPIXEL*line + pixel] = 0xFF;
                else 							BUF[NPIXEL*line + pixel] = 0x0;
            }
        }
        tty_printf(" - build   %d OK at cycle %d\n", step, proctime());

	/* ---- Pipeline synchronisation ---- */
	fb_completed();
        tty_printf(" - display %d OK at cycle %d\n", step, proctime());

    }
    /* --------------------------- EPILOGUE --------------------------------- */
    BUF = ((step & 1) == 0) ? BUF1 : BUF2;
    if(fb_write(0, BUF, NLINE*NPIXEL) != 0)
    {
	tty_printf("\n!!! error in fb_syn_write syscall !!!\n"); 
	exit();
    }
    fb_completed();
    tty_printf(" - display %d OK at cycle %d\n", step, proctime());

    tty_printf("\nFin du programme au cycle = %d\n\n", proctime());
    exit(); 
} // end main

