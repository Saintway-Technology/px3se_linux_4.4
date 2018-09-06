#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/of_gpio.h>


MODULE_LICENSE("GPL");


struct device * pdevice;

int wakein;
int dtr;
int rst;
int disable;

int gprs_init(struct device_node *np)
{
	if(gpio_request(wakein,NULL))
	{
		printk("gpio %d request faild\n",wakein);
		return -1;	
	}	
	if(gpio_direction_output(wakein, 0))
	{
		printk("set %d gpio_direction_output err\n",wakein);
		return -1;	
	}
        gpio_set_value(wakein, 0);	

	if(gpio_request(rst,NULL))
	{
		printk("gpio %d request faild\n",rst);
		return -1;	
	}	
	if(gpio_direction_output(rst, 1))
	{
		printk("set %d gpio_direction_output err\n",rst);
		return -1;	
	}
        gpio_set_value(rst, 1);
	
	if(gpio_request(dtr,NULL))
	{
		printk("gpio %d request faild\n",dtr);
		return -1;	
	}	
	if(gpio_direction_output(dtr, 0))
	{
		printk("set %d gpio_direction_output err\n",dtr);
		return -1;	
	}
        gpio_set_value(dtr, 0);
	
	if(gpio_request(disable,NULL))
	{
		printk("gpio %d request faild\n",disable);
		return -1;	
	}	
	if(gpio_direction_output(disable, 0))
	{
		printk("set %d gpio_direction_output err\n",wakein);
		return -1;	
	}
        gpio_set_value(disable, 0);	

	msleep(200);
	
	if(gpio_direction_output(rst, 0))
	{
		printk("set %d gpio_direction_output err\n",rst);
		return -1;	
	}
        gpio_set_value(rst, 0);	

	return 0;	
}

int get_gprs_gpio(struct device_node *np)
{
	unsigned long gpios;
	
        wakein = of_get_named_gpio_flags(np, "wakein", 0,(enum of_gpio_flags *)&gpios);
	if (!gpio_is_valid(wakein)){
		printk("invalid wakein_gpio: %d\n",wakein);
		return -1;
	}
 
        dtr = of_get_named_gpio_flags(np, "dtr", 0,(enum of_gpio_flags *)&gpios);
	if (!gpio_is_valid(dtr)){
		printk("invalid dtr_gpio: %d\n",dtr);
		return -1;
	}
 
        rst = of_get_named_gpio_flags(np, "rst", 0,(enum of_gpio_flags *)&gpios);
	if (!gpio_is_valid(rst)){
		printk("invalid rst_gpio: %d\n",rst);
		return -1;
	}
 
        disable = of_get_named_gpio_flags(np, "disable", 0,(enum of_gpio_flags *)&gpios);
	if (!gpio_is_valid(disable)){
		printk("invalid disable_gpio: %d\n",disable);
		return -1;
	} 
	
	return 0;	
}

int demo_probe(struct platform_device * pdevice) 
{

	struct device_node *np = pdevice->dev.of_node;

	printk("probe ok!\n");
	get_gprs_gpio(np);
	
	if((gprs_init(np)) == -1)
	{
		gpio_free(wakein);
		gpio_free(dtr);
		gpio_free(rst);
		gpio_free(disable);
	}
	gprs_init(np);

	return 0;
}

int demo_remove(struct platform_device * pdevice)
{
	printk("%s,%d\n", __func__, __LINE__);	

	gpio_free(wakein);
	gpio_free(dtr);
	gpio_free(rst);
	gpio_free(disable);
	return 0;
}


struct of_device_id  idts[] = {
	{.compatible = "stw,gprs-slm730"},
	{/*Nothing  to be done.*/},
};

struct platform_driver pdriver = {
	.probe = demo_probe,
	.remove = demo_remove,
	.driver = {
		.name = "stw_gprs",
		.of_match_table = idts,
		.owner  = THIS_MODULE,
	},
};

module_platform_driver(pdriver);
