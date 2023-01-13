#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>

#define DRIVER_AUTHOR "Christophe Barès"
#define DRIVER_DESC "Hello world Module"
#define DRIVER_LICENSE "GPL"
#define TIMER_INTERVALLE 2000

// — utilisation de paramètres au chargement du module
static int param1 = 3;
module_param(param1, int, 0);
MODULE_PARM_DESC(param1, "Ceci est mon paramètre");

// — création d’un entrée dans /proc
static ssize_t my_read_function(struct file *file, 
								char __user *buf, 
								size_t count, 
								loff_t *ppos) {
	printk(KERN_DEBUG "calling read from hello\n");
	return 0;
}
static ssize_t my_write_function(	struct file *file, 
									const char __user *buf,
									size_t count,
									loff_t *ppos) {
	return printk(KERN_DEBUG "write(%s)\n", buf);
	return 0;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_read_function,
	.write = my_write_function,
	//.open = my_open_function,
	//.release = my_release_function // correspond a close 
};

// — utilisation d’un timer
int timerCounter = 0;
static struct timer_list mon_timer;

static void montimer_handle(struct timer_list *t) {
	printk(KERN_INFO "Tuez moi (%d)\n", timerCounter);
	timerCounter++;
	//Il faut réarmer le timer si l'on veut un appel périodique
	mod_timer(&mon_timer,jiffies + msecs_to_jiffies(TIMER_INTERVALLE));
}


int hello_init(void)
{
	printk(KERN_INFO "Hello world! potatoes -> %d\n", param1);
	
	// — création d’un entrée dans /proc
	proc_create("mon_entree_proc", 0666, NULL, &fops);

	// — utilisation d’un timer
	timer_setup(&mon_timer, montimer_handle, 0);
	/*init_timer(&mon_timer);
	mon_timer.function = montimer_handle;
	mon_timer.data = 0;*/
	mod_timer(&mon_timer, jiffies + msecs_to_jiffies(TIMER_INTERVALLE));

	return 0;
}

void hello_exit(void)
{
	//printk(KERN_ALERT "I HAD %d POTATOES\n", param1);
	printk(KERN_ALERT "Bye bye...\n");
	del_timer(&mon_timer);
	remove_proc_entry("mon_entree_proc",NULL);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
