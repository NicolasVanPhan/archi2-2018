#include "stdio.h"

#define DEPTH		4
#define NMAX 		50

/******************/
typedef struct lock
{
    unsigned int current;    // current slot index
    unsigned int free;       // next free slot index
} lock_t;

/******************/
typedef struct fifo 
{
    int	    buf[DEPTH];
    int	    ptr;
    int	    ptw;
    int	    sts;
    int     depth;
    lock_t  lock;
} fifo_t;

volatile fifo_t fifo = { {} , 0 , 0 , 0 , DEPTH };

/************************************************/
unsigned int atomic_increment( unsigned int*  ptr,
                               unsigned int   increment )
{
    unsigned int value;

    asm volatile (
        "atomic_start:                    \n"
        "move  $10,     %1                \n"
        "move  $11,     %2                \n"
        "ll    $12,     0($10)            \n"
        "addu  $13,     $11,     $12      \n"
        "sc    $13,     0($10)            \n"
        "beqz  $13,     atomic_start      \n"
        "move  %0,      $12               \n"
        : "=r" (value)
        : "r" (ptr) , "r" (increment)
        : "$10" , "$11" , "$12" , "$13" , "memory" );

    return value; 
}

/*******************************/
void lock_acquire( lock_t* plock )
{
    // get next free slot index
    unsigned int  ticket = atomic_increment ( &plock->free , 1 );

    // get address of the current slot index
    unsigned int pcurrent = (unsigned int)(&plock->current);

    // poll current slot index until current == ticket
    asm volatile (
        "lock_try:                        \n"
        "lw    $10,     0(%0)             \n"
        "move  $11,     %1                \n"
        "bne   $10,     $11,     lock_try \n"
        :
        : "r" (pcurrent) , "r" (ticket )
        : "$10" , "$11" );
}

/*******************************/
void lock_release( lock_t* plock )
{
    asm volatile ( "sync" );

    plock->current = plock->current + 1;
}

/*************************************/
void fifo_write(fifo_t* fifo, int* val)
{
    int done = 0;

    while(done == 0)
    {
        lock_acquire( (lock_t*)(&fifo->lock) );

        if(fifo->sts == fifo->depth) 
        {
            lock_release( (lock_t*)(&fifo->lock) );
        }
        else
        {
			/* Write in FIFO */
			fifo->buf[fifo->ptw] = *val;
			/* Increment FIFO write pointer */
			(fifo->ptw)++;
			/* The FIFO is circular */
			if (fifo->ptw == fifo->depth)
				fifo->ptw = 0;	
			/* Update sts */
			(fifo->sts)++;
			/* Release the lock */
            lock_release( (lock_t*)(&fifo->lock) );
			done = 1;
        }
    }
}
/************************************/
void fifo_read(fifo_t* fifo, int* val)
{
    int done = 0;

    while(done == 0)
    {
        lock_acquire( (lock_t*)(&fifo->lock) );

        if(fifo->sts == 0) 
        {
            lock_release( (lock_t*)(&fifo->lock) );
        }
        else
        {
			/* Read FIFO */
			*val = fifo->buf[fifo->ptr];
			/* Increment FIFO read pointer */
			(fifo->ptr)++;
			/* The FIFO is circular */
			if (fifo->ptr == fifo->depth)
				fifo->ptr = 0;	
			/* Update sts */
			(fifo->sts)--;
			/* Release the lock */
            lock_release( (lock_t*)(&fifo->lock) );
			done = 1;
        }
    }
}

/********************************************/
__attribute__ ((constructor)) void producer()
{
    int n;
    int x;
    int tempo = 0;
    int val;

    tty_printf("*** Starting task producer on processor %d ***\n\n", procid());

    for(n = 0 ; n < NMAX ; n++) 
    { 
        tempo = rand()>>6;
        val = n;
        fifo_write((fifo_t*)&fifo, &val);
        for(x = 0 ; x < tempo ; x++) asm volatile ("");
        tty_printf("transmitted value : %d      temporisation = %d\n", val, tempo);
    }

    tty_printf("\n*** Completing producer at cycle %d ***\n", proctime());
    exit();

} // end producer()

/*******************************************/
__attribute__ ((constructor)) void consumer()
{
    int n;
    int x;
    int tempo = 0;
    int val;

    tty_printf("*** Starting task consumer on processor %d ***\n\n", procid());

    for(n = 0 ; n < NMAX ; n++) 
    { 
        tempo = rand()>>6;
        fifo_read((fifo_t*)&fifo, &val);
        for(x = 0 ; x < tempo ; x++) asm volatile ("");
        tty_printf("received value : %d      temporisation = %d\n", val, tempo);
    }

    tty_printf("\n*** Completing consumer at cycle %d ***\n", proctime());
    exit();

} // end consumer()

