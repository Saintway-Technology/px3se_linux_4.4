#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>

#define GPIO 'A'
#define GPIO_HIGH  _IO('A',1)
#define GPIO_LOW  _IO('A',0)

#define DEVICE_NAME "stw_gpio_control"
int gpio_num;
int set_gpio_value(int value)
{
        int ret;
        if (!gpio_is_valid(gpio_num)) {
                printk("%s: get property: gpio_num = %d. is invalid\n", __func__, gpio_num);
                return -ENODEV;
        }
        ret = gpio_request(gpio_num,NULL);
        if (ret) {
                printk("%s: Failed to request %d\n", __func__, gpio_num);
                gpio_free(gpio_num);
                ret = gpio_request(gpio_num,NULL);
        }

        gpio_direction_output(gpio_num,value);
        gpio_set_value(gpio_num,value);
        printk("%s: GPIO set value = %d\n", __func__, value);
	
	return 0;	
}

int gpio_control_open(struct inode * inode, struct file * file){
        printk("%s gpio open success!!!\n",__func__);
        return 0;
}


static long gpio_control_ioctl(struct file *file, unsigned int gpio, unsigned long arg){

	if(gpio >= 0 && gpio <= 128)
	{
		gpio_num = gpio;
       		printk("%s icotl success! gpio_num = %d\n",__func__,gpio_num);
	}
	else
	{
	switch(gpio){
	case(GPIO_HIGH) : 
		set_gpio_value(1);
		break;
	case(GPIO_LOW) : 
		set_gpio_value(0);
		break;
	default:
		break;
	}

	}
        return 0;
}

int gpio_control_write(struct file *file, const char __user *userbuf, size_t size, loff_t *ppos){
        printk("%s gpio write success!\n",__func__);
        return 0;
}

int gpio_control_read(struct file *file, char __user *buf, size_t size, loff_t *ppos){
        printk("%s gpio read success!\n",__func__);
        return 0;
}

static struct file_operations gpio_control_dev_fops = {
        .owner = THIS_MODULE,
        .open = gpio_control_open,
        .read = gpio_control_read,
        .write = gpio_control_write,
        .unlocked_ioctl = gpio_control_ioctl
};

static struct miscdevice gpio_control_dev = {
        .minor = MISC_DYNAMIC_MINOR,
        .name = DEVICE_NAME,
        .fops = &gpio_control_dev_fops,
};

static int gpio_control_probe(struct platform_device * pdev){
        int ret = 0;
        //struct device_node *node = pdev->dev.of_node;
        ret = misc_register(&gpio_control_dev);
        if (ret < 0){
                printk("%s gpio driver probe failed!\n",__func__);
                return ret;
        }
        printk("%s gpio driver probe success!\n",__func__);
        return 0;
}

static int gpio_control_remove(struct platform_device * pdev){
        misc_deregister(&gpio_control_dev);
        printk("%s gpio driver remove success!\n",__func__);
        return 0;
}

struct of_device_id gpio_control_idts[] = {
        {.compatible = "stw,gpio_control"},
};

static struct platform_driver gpio_control_driver = {
        .probe = gpio_control_probe,
        .remove = gpio_control_remove,
        .driver = {
                .name = DEVICE_NAME,
                .owner = THIS_MODULE,
                .of_match_table = gpio_control_idts,
        },
};

static int __init gpio_control_module_init(void){
        return platform_driver_register(&gpio_control_driver);
}

static void __exit gpio_control_module_exit(void){
        platform_driver_unregister(&gpio_control_driver);
}

module_init(gpio_control_module_init);
module_exit(gpio_control_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lwh@saintway.com");
