 //
//  mygpio.c
//  Created by Prashant & Kevin
//

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>			// lots of stuff
#include <linux/types.h>		// size_t...
#include <linux/timer.h>		// Ktimers
#include <linux/jiffies.h>		// Jiffies
#include <linux/errno.h>				// returning proper errors

#include <asm/gpio.h>				// interface for the GPIO registers, functions and macros
#include <asm/hardware.h>
#include <asm/arch/pxa-regs.h>


MODULE_LICENSE("Dual BSD/GPL");

// module specific defines

/*************************		Declaration of mygpio.c functions		*************************/
static int mygpio_init(void);
static void mygpio_exit(void);
module_exit(mygpio_exit);
module_init(mygpio_init);


/*********** Ktimer functions *********/
static void mygpio_ktimer_callback(unsigned long data);			// callback function for the ktimer
//static void convert_to_binary(int num);							
// helper function to set the LEDS


/*************************		Global variables section				*************************/
//static int mygpio_major = 61;			// probably don't need a major number since we arent' registering

static const int B=2;
static const int F=1;
static const int R=3;
static const int L=0;

//typedef (GPLR(Sensor[B]) & GPIO_bit(Sensor[B])) BackS;
//typedef (GPLR(Sensor[F]) & GPIO_bit(Sensor[F])) ForwardS;
//typedef (GPLR(Sensor[R]) & GPIO_bit(Sensor[R])) RightS;
//typedef (GPLR(Sensor[L]) & GPIO_bit(Sensor[L])) LeftS;



static struct timer_list *mytimer = NULL;		// timer to generate the interrupts and count
//static int counter = 15;						// counting varible for the LEDs

//static const int BUTTON0 = 17;					// GPIO number for button 0 input
//static const int BUTTON1 = 101;
static const int Sensor[4] = {113,9,29,31};	// GPIO numbers for Sensor inputs 
static const int forward = 28;
static const int left = 17;
static const int right = 101;
static const int back = 2;

static int initial_counter;
/*************************		Implementation of mygpio.c functions		*********************/

static int mygpio_init(void) {
	int result;

	mytimer = (struct timer_list *)kmalloc(sizeof(struct timer_list), GFP_KERNEL);// allocate space for timer
	if (mytimer == NULL)
		return -ENOMEM;

	printk("Waddup\n");
    	
	pxa_gpio_mode(Sensor[0]);
	pxa_gpio_mode(Sensor[1]);
 	pxa_gpio_mode(Sensor[2]);
 	pxa_gpio_mode(Sensor[3]);
 	
	initial_counter = 0;

	//pxa_gpio_mode(Sensor[4]);  // Do nothing for sensor 4
 	//pxa_gpio_mode(Sensor[5]); // Do nothing for sensor 5
	
	
	GPCR(forward) = GPIO_bit(forward);
	GPCR(left) = GPIO_bit(left);
	GPCR(right) = GPIO_bit(right);
	GPCR(back) = GPIO_bit(back);
	
	setup_timer( mytimer, mygpio_ktimer_callback, 0);		// initialize our timer
	result = mod_timer( mytimer, jiffies + HZ);			// set the timer for 1 second

	if (result != 0) {
		printk(KERN_ALERT"Error in mod_timer");
		return 1;
	}
	return 0;
}


static void mygpio_ktimer_callback(unsigned long data) {

	int result;
	
	if(initial_counter <5)
	{
		initial_counter ++;
			GPSR(forward) = GPIO_bit(forward);
		printk("Module Initializing... \n");
		if(initial_counter ==5)
			printk("Sensors now active. \n");
	}
		
	else
	{
	
	if (((GPLR(Sensor[B]) & GPIO_bit(Sensor[B])) == 0)  && ((GPLR(Sensor[F]) & GPIO_bit(Sensor[F])) == 0))  
	{
		printk("Both Back and Forward are on. RC Car will not move \n"); 
		        GPCR(forward) = GPIO_bit(forward);
       			GPCR(left) = GPIO_bit(left);
        		GPCR(right) = GPIO_bit(right);
        		GPCR(back) = GPIO_bit(back);

	}
	else if (((GPLR(Sensor[R]) & GPIO_bit(Sensor[R])) ==0) && ((GPLR(Sensor[L]) & GPIO_bit(Sensor[L])) ==0))
	{
		printk("Both Left and Right are on. RC Car will not move \n");
                        GPCR(forward) = GPIO_bit(forward);
                        GPCR(left) = GPIO_bit(left);
                        GPCR(right) = GPIO_bit(right);
                        GPCR(back) = GPIO_bit(back);		
	}
	else
	{
		if((GPLR(Sensor[L]) & GPIO_bit(Sensor[L])) ==0)
		{
			if((GPLR(Sensor[B]) & GPIO_bit(Sensor[B])) ==0 )
			{
			printk("Both Left & Back are on. Move Forward and Left \n");
			}
			else if((GPLR(Sensor[F]) & GPIO_bit(Sensor[F])) == 0 )
			{
			printk("Both Forward and Left are on. Move Forward and Left \n");
			}
			else 
			{
			printk("Only Left is on. Move Forward and Left\n");
			}
			GPSR(forward) = GPIO_bit(forward);
                        GPSR(left) = GPIO_bit(left);
                        GPCR(right) = GPIO_bit(right);
                        GPCR(back) = GPIO_bit(back);
		}
		else if((GPLR(Sensor[R]) & GPIO_bit(Sensor[R])) ==0)
		{
                        if((GPLR(Sensor[B]) & GPIO_bit(Sensor[B])) == 0 )
                        {
                        printk("Both Right & Back are on. Move Forward and Right \n");
                        }
                        else if((GPLR(Sensor[F]) & GPIO_bit(Sensor[F])) == 0 )
                        {
                        printk("Both Forward and Right are on. Move Forward and Right\n");
                        }
                        else
                        {
                        printk("Only Right is on. Move Forward and Right\n");
                        }
                        GPSR(forward) = GPIO_bit(forward);
                        GPCR(left) = GPIO_bit(left);
                        GPSR(right) = GPIO_bit(right);
                        GPCR(back) = GPIO_bit(back);
			
		}
		else
		{
			if((GPLR(Sensor[B]) & GPIO_bit(Sensor[B])) == 0)
			{
			printk("Only Back is on. Move Forward and Right\n");

                        GPSR(forward) = GPIO_bit(forward);
                        GPCR(left) = GPIO_bit(left);
                        GPSR(right) = GPIO_bit(right);
                        GPCR(back) = GPIO_bit(back);

			}
			else
			{
				if((GPLR(Sensor[F]) & GPIO_bit(Sensor[F])) == 0)
				{
				printk("Only Forward is on. Move forward.\n");
                        	GPSR(forward) = GPIO_bit(forward);
                        	GPCR(left) = GPIO_bit(left);
                        	GPCR(right) = GPIO_bit(right);
                        	GPCR(back) = GPIO_bit(back);
				}
				else 
				{
				printk("All Sensors Off. Do Nothing\n");
                                GPCR(forward) = GPIO_bit(forward);
                                GPCR(left) = GPIO_bit(left);
                                GPCR(right) = GPIO_bit(right);
                                GPCR(back) = GPIO_bit(back);
				
				}
			}
		}
	}
	}
	
	result = mod_timer( mytimer, jiffies + HZ);			// set the timer for 1 second

	if (result != 0) {
		printk(KERN_ALERT"Error in mod_timer");
		return;
	}

}
static void mygpio_exit(void) {

	del_timer(mytimer);

	kfree(mytimer);		
        GPCR(forward) = GPIO_bit(forward); 
        GPCR(back) = GPIO_bit(back); 
        GPCR(left) = GPIO_bit(left); 
        GPCR(right) = GPIO_bit(right); 

}
