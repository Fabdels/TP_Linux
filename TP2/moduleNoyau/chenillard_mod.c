#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>

#include <linux/miscdevice.h>
#include <linux/uaccess.h>

// #include <stdint.h>
// #include <sys/mman.h>
// #include <fcntl.h>

#define DRIVER_AUTHOR "Christophe Barès (non c'est moi)"
#define DRIVER_DESC "Module for my chenillard"
#define DRIVER_LICENSE "GPL"
#define TIMER_INTERVALLE 1000

//static uint32_t * LEDPointer;
//static int fd;

// ca c'est la vitesse
static int vitesse = 1;
module_param(vitesse, int, 0);
MODULE_PARM_DESC(vitesse, "Ceci est la vitesse");



// ca c'est pour le pattern
char chenillePatern = 1;
struct uneStruc {
    struct miscdevice miscdev;
    //void __iomem *regs;
    //u8 leds_value;
};
static ssize_t my_read_function(struct file *file, 
								char __user *buf, 
								size_t count, 
								loff_t *ppos) {
	printk(KERN_DEBUG "pattern: '%02x'\n", chenillePatern);
	return 0;
}
static ssize_t my_write_function(	struct file *file, 
									const char __user *buf,
									size_t count,
									loff_t *ppos) {
	//chenillePatern = buf[0];
	//struct uneStruc *dev = container_of(file->private_data, struct uneStruc, miscdev);
    int success = copy_from_user(&chenillePatern, buf, 1);

	return count;
}
static int my_open_function(struct inode *inode, struct file *file) {
//printk(KERN_DEBUG "open()\n");
return 0;
}
static int my_release_function(struct inode *inode, struct file *file) {
//printk(KERN_DEBUG "close()\n");
return 0;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_read_function,
	.write = my_write_function,
	.open = my_open_function,
	.release = my_release_function // correspond a close
};




// — timer pour la mise-à-jour du chenillard
#define NB_LED 10
#define CHAR_BIT_SIZE 8
int pos = -CHAR_BIT_SIZE, sensChenille = 1;

void chenillard(int* chenillePos, int* sensChenille){
	if (*chenillePos <= -CHAR_BIT_SIZE){
		*sensChenille = 1;
	}
	else if (*chenillePos >= NB_LED)
	{
		*sensChenille = -1;
	}
	*chenillePos += *sensChenille;
}
void affiChenille(int LedGrid){
	char affiNum[NB_LED+1];
	affiNum[NB_LED] = '\0';
	int i;
	for(i=0;i<NB_LED;i++){
		affiNum[NB_LED -1 -i] = '0' + (LedGrid >> i & 1) ;
	}
	printk(KERN_ALERT "Chenille (%s)\n", affiNum);
}

static struct timer_list mon_timer_chenille;

static void montimer_handle(struct timer_list *t) {
	chenillard(&pos, &sensChenille);
	//*LEDPointer = chenillePatern << pos;
	if(pos<0)
		affiChenille(chenillePatern >> (-pos));
	else
		affiChenille(chenillePatern << pos);

	//Il faut réarmer le timer si l'on veut un appel périodique
	mod_timer(&mon_timer_chenille,jiffies 
		+ msecs_to_jiffies((int)(TIMER_INTERVALLE / vitesse)));
}


struct proc_dir_entry* myEntry;
struct proc_dir_entry* myDir;

int hello_init(void){
	printk(KERN_INFO "la chenille demarre! -> launching with speed = %d\n", vitesse);
	myDir = proc_mkdir("ensea", NULL);
	myEntry = proc_create("chenille", 0666, myDir, &fops);
	//fd = open("/dev/mem", O_RDWR);
	//LEDPointer = (uint32_t*) mmap(
	//	NULL, 4, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0xFF203000);

	// — utilisation d’un timer
	setup_timer(&mon_timer_chenille, montimer_handle, 0); // timer_setup / setup_timer
	mod_timer(&mon_timer_chenille, jiffies + msecs_to_jiffies(TIMER_INTERVALLE));

	return 0;
}

void hello_exit(void)
{
	del_timer(&mon_timer_chenille);
	//close(fd);
	remove_proc_entry("chenille",myDir);
	remove_proc_entry("ensea",NULL);
	printk(KERN_ALERT "la chenille s'arrete\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

