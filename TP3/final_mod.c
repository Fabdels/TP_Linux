#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>

// Prototypes
static int leds_probe(struct platform_device *pdev);
static int leds_remove(struct platform_device *pdev);

// An instance of this structure will be created for every ensea_led IP in the system
struct ensea_leds_dev {
    struct miscdevice miscdev;
    void __iomem *regs;
    u8 leds_value;
};

// Specify which device tree devices this driver supports
static struct of_device_id ensea_leds_dt_ids[] = {
    {
        .compatible = "dev,ensea"
    },
    { /* end of table */ }
};

// Inform the kernel about the devices this driver supports
MODULE_DEVICE_TABLE(of, ensea_leds_dt_ids);

struct platform_device * pdev2; // ajoutXXX
int tockenTrick = 0;
#define DEFAULT_PATTERN 0x7
#define TIMER_INTERVALLE 5000
#define NB_LED 10
#define PATTERN_BIT_SIZE 8


int chenillePatern = DEFAULT_PATTERN;
static int vitesse = 1;
module_param(vitesse, int, 0);
MODULE_PARM_DESC(vitesse, "Ceci est la vitesse");

// Data structure that links the probe and remove functions with our driver
static struct platform_driver leds_platform = {
    .probe = leds_probe,
    .remove = leds_remove,
    .driver = {
        .name = "Ensea LEDs Driver",
        .owner = THIS_MODULE,
        .of_match_table = ensea_leds_dt_ids
    }
};

// — timer pour la mise-à-jour du chenillard
signed char pos = -PATTERN_BIT_SIZE;
char sensChenille = 1;
int fileID;

void chenillard(signed char* chenillePos, char *sensChenille){
    if (*chenillePos <= -PATTERN_BIT_SIZE){
        //*sensChenille = 1;
        *chenillePos = NB_LED;
    }
    else if (*chenillePos > NB_LED)
    {
        //*sensChenille = -1;
        *chenillePos = -PATTERN_BIT_SIZE;
    }
    *chenillePos += *sensChenille;
}
void affiChenille(int LedGrid){
    int i;
    char affiNum[NB_LED+1];
    affiNum[NB_LED] = '\0';
    for(i=0;i<NB_LED;i++){
        affiNum[NB_LED -1 -i] = '0' + (LedGrid >> i & 1) ;
    }
    printk(KERN_ALERT "Chenille (%s)\n", affiNum);
}

static struct timer_list mon_timer_chenille;

static void montimer_handle(struct timer_list *t) {
    int notbuf;
    chenillard(&pos, &sensChenille);
    if(pos<0){
        //affiChenille(chenillePatern >> (-pos));
        notbuf = chenillePatern >> (-pos); // 1/5
    }
    else{
        //affiChenille(chenillePatern << pos);
        notbuf = chenillePatern << pos; // 2/5
    }
    if(tockenTrick){
        struct ensea_leds_dev *dev = (struct ensea_leds_dev*) platform_get_drvdata(pdev2); // ajoutXXX
        iowrite32(notbuf, dev->regs); // ajoutXXX
    }

    //Il faut réarmer le timer si l'on veut un appel périodique
    mod_timer(&mon_timer_chenille,jiffies 
        + msecs_to_jiffies((int)(TIMER_INTERVALLE / vitesse)));
}



static ssize_t read_function_speed(struct file *file, char __user *buf, 
                                size_t count, loff_t *ppos) {
    //printk(KERN_DEBUG "vitesse: '%d'\n", vitesse);
    if (count>3){
        //buf[0] = (char) vitesse;
        //buf[1] = (char) vitesse>>8;
        //buf[2] = (char) vitesse>>16;
        //buf[3] = (char) vitesse>>24;
        int success = copy_to_user(buf, &vitesse, sizeof(vitesse));
        if(success != 0) printk(KERN_INFO "Pb: (%d)\n", success);
        return 4;
    }
    else if(count>0){
        //buf[0] = (char) vitesse;
        int success = copy_to_user(buf, &vitesse, 1);
        if(success != 0) printk(KERN_INFO "Pb: (%d)\n", success);
        return 1;
    }
    else
        return 0;
}
struct file_operations fopsSpeed = {
    .owner = THIS_MODULE,
    .read = read_function_speed, //no write
};


static ssize_t read_function_dir(struct file *file, char __user *buf, 
                                size_t count, loff_t *ppos) {
    //printk(KERN_DEBUG "sensChenille: '%d'\n", sensChenille);
    // if(count>0){
    //     //buf[0] = sensChenille;
    //     int success = copy_to_user(buf, &sensChenille, 1);
    //     if(success != 0) printk(KERN_INFO "Pb: (%d)\n", success);
    //     return 1;
    // }
    // else
    //     return 0;

    int errno=0;
    int copy;
    if (count > 1) count=1;
    if ((copy=copy_to_user(buf, &sensChenille, 2)))
        errno = -EFAULT;
    printk(KERN_INFO "message read, %d, %p\n", copy, buf);
    return count-copy;
}
static ssize_t write_function_dir(  struct file *file, const char __user *buf,
                                    size_t count, loff_t *ppos) {
    if(count>0){
        //sensChenille = buf[0];
        int success = copy_from_user(&sensChenille, buf, 1);
        if(success != 0) printk(KERN_INFO "Pb: (%d)\n", success);
        return 1;}
    else
        return 0;
}
struct file_operations fopsDir = {
    .owner = THIS_MODULE,
    .read = read_function_dir,
    .write = write_function_dir,
};

//  3/5
static ssize_t read_function_pattern(struct file *file, char *buf,
                                     size_t count, loff_t *ppos) {
    printk(KERN_ALERT "chenillePatern: '%x'\n", chenillePatern); //KERN_DEBUG
    return 0;
    if(1 || count>0){
        //buf[0] = chenillePatern;
        int success = copy_to_user(buf, &chenillePatern, 1);
        if(success != 0) printk(KERN_INFO "Pb: (%d)\n", success);
        return 1;
    }
    else
        return 0;
}
static ssize_t write_function_pattern(struct file *file, const char *buffer,
                                        size_t len, loff_t *offset){
    int success = 0;
    //struct ensea_leds_dev *dev = container_of(file->private_data, struct ensea_leds_dev, miscdev);
    success = copy_from_user(&chenillePatern, buffer, sizeof(chenillePatern));

    if(success != 0) {
        pr_info("Failed to read led value from userspace\n");
        return -EFAULT;
    } else {
        pr_info("c'est cool\n");
    }

    return len;
}
// (struct file *file, const char *buf, size_t count, loff_t *ppos) {
//     printk(KERN_ALERT "chenillePatern writing: '%x'\n", *buf); //KERN_DEBUG
//     // if(count>0){
//     //     //chenillePatern = buf[0];
//     //     int success = copy_from_user(&chenillePatern, buf, 1);
//     //     if(success != 0) printk(KERN_INFO "Pb: (%d)\n", success);
//     //     return 1;
//     // }
//     // else
//         return 0;
// }
static int my_open_function(struct inode *inode, struct file *file) {
//printk(KERN_DEBUG "open()\n");
return 0;
}
static int my_release_function(struct inode *inode, struct file *file) {
//printk(KERN_DEBUG "close()\n");
return 0;
}
struct file_operations fopsPattern = {
    .owner = THIS_MODULE,
    .read = read_function_pattern,
    .write = write_function_pattern,
    .open = my_open_function,
    .release = my_release_function // correspond a close
};

// Called whenever the kernel finds a new device that our driver can handle
// (In our case, this should only get called for the one instantiation of the Ensea LEDs module)
static int leds_probe(struct platform_device *pdev)
{
    int ret_val = -EBUSY;
    struct ensea_leds_dev *dev;
    struct resource *r = 0;

    pr_info("leds_probe enter\n");

    // Get the memory resources for this LED device
    r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if(r == NULL) {
        pr_err("IORESOURCE_MEM (register space) does not exist\n");
        goto bad_exit_return;
    }

    // Create structure to hold device-specific information (like the registers)
    dev = devm_kzalloc(&pdev->dev, sizeof(struct ensea_leds_dev), GFP_KERNEL);
    pdev2 = pdev; // carjackingXXX
    tockenTrick = 1;

    // Both request and ioremap a memory region
    // This makes sure nobody else can grab this memory region
    // as well as moving it into our address space so we can actually use it
    dev->regs = devm_ioremap_resource(&pdev->dev, r);
    if(IS_ERR(dev->regs))
        goto bad_ioremap;

    // Turn the LEDs on (access the 0th register in the ensea LEDs module)
    dev->leds_value = 0xFF;
    iowrite32(dev->leds_value, dev->regs);

    // Initialize the misc device (this is used to create a character file in userspace)
    dev->miscdev.minor = MISC_DYNAMIC_MINOR;    // Dynamically choose a minor number
    dev->miscdev.name = "ensea_leds";
    //dev->miscdev.fops = &ensea_leds_fops;

    ret_val = misc_register(&dev->miscdev);
    if(ret_val != 0) {
        pr_info("Couldn't register misc device :(");
        goto bad_exit_return;
    }

    // Give a pointer to the instance-specific data to the generic platform_device structure
    // so we can access this data later on (for instance, in the read and write functions)
    platform_set_drvdata(pdev, (void*)dev);

    pr_info("leds_probe exit\n");

    return 0;

bad_ioremap:
   ret_val = PTR_ERR(dev->regs);
bad_exit_return:
    pr_info("leds_probe bad exit :(\n");
    return ret_val;
}

// Gets called whenever a device this driver handles is removed.
// This will also get called for each device being handled when
// our driver gets removed from the system (using the rmmod command).
static int leds_remove(struct platform_device *pdev)
{
    // Grab the instance-specific information out of the platform device
    struct ensea_leds_dev *dev = (struct ensea_leds_dev*)platform_get_drvdata(pdev);

    pr_info("leds_remove enter\n");

    // Turn the LEDs off
    iowrite32(0x00, dev->regs);

    // Unregister the character file (remove it from /dev)
    misc_deregister(&dev->miscdev);

    pr_info("leds_remove exit\n");

    return 0;
}

struct proc_dir_entry* myProcDir;
struct proc_dir_entry* dirEntry;
struct proc_dir_entry* speedEntry;
// Called when the driver is installed
static int final_mod_init(void)
{
    int ret_val = 0;
    printk(KERN_INFO "la chenille demarre! -> launching with speed = %d\n", vitesse);

    // Register our driver with the "Platform Driver" bus
    ret_val = platform_driver_register(&leds_platform);
    if(ret_val != 0) {
        pr_err("platform_driver_register returned %d\n", ret_val);
        return ret_val;
    }

    myProcDir = proc_mkdir("ensea", NULL);
    speedEntry = proc_create("speed", 0666, myProcDir, &fopsSpeed);
    dirEntry = proc_create("dir", 0666, myProcDir, &fopsDir);
    // mknod /dev/ensea-led c 100 0
    register_chrdev(100, "ensea-led", &fopsPattern); // 4/5
    pr_info("Ensea LEDs module successfully initialized!\n");

    // — utilisation du timer
    setup_timer(&mon_timer_chenille, montimer_handle, 0);  // timer_setup / setup_timer
    mod_timer(&mon_timer_chenille, jiffies + msecs_to_jiffies(TIMER_INTERVALLE));

    return 0;
}

// Called when the driver is removed
static void final_mod_exit(void)
{
    printk(KERN_INFO "Debug:\nsens: %d\npattern: %x\npos: %d\n",
        sensChenille, chenillePatern, pos);
    pr_info("Ensea 'final_mod' module exit\n");

    del_timer(&mon_timer_chenille);
    //close(fileID);
    unregister_chrdev(100, "ensea-led"); // 5/5
    remove_proc_entry("speed",myProcDir);
    remove_proc_entry("dir",myProcDir);
    remove_proc_entry("ensea",NULL);
    // Unregister our driver from the "Platform Driver" bus
    // This will cause "leds_remove" to be called for each connected device
    platform_driver_unregister(&leds_platform);

    pr_info("Ensea LEDs module successfully unregistered, la chenille s'arrete\n");

}

// Tell the kernel which functions are the initialization and exit functions
module_init(final_mod_init);
module_exit(final_mod_exit);

// Define information about this kernel module
MODULE_LICENSE("GPL");
MODULE_AUTHOR("un binome random");
MODULE_DESCRIPTION("déplace un chenillard (dans 1 sens)");
MODULE_VERSION("1.0");
