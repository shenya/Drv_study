#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Guangyao Shen");
MODULE_DESCRIPTION("driver study");

#define MODULE_NAME "Hello"
static struct proc_dir_entry *proc_dir = NULL;
static struct proc_dir_entry *proc_file_bis;

static int BIHMODE = 1;


static int proc_read_bis(char *page, char **start, off_t off, int *eof, void *data)
{
	int len;

	if(BIHMODE == 1)
	{
		len = sprintf(page, "ENABLE\n");
	}
	else
	{
		len = sprintf(page, "DISABLE\n");
	}

	return len;
}

static int proc_write_mode(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int len;

	char buff[32];

	if(count > sizeof(buff))
	{
		len = sizeof(buff) -1;
	}
	else
	{
		len = count;
	}
	if(copy_from_user(buff, buffer, len))
	{
		printk("fail to copy_from_user");
		return -1;
	}
	buff[len] = 0;

	if(strncmp(buff, "BIS", 3) == 0)
	{
		BIHMODE = 0;
		printk("BIHMODE is %d\n", BIHMODE);
	}

	return len;
}
static int proc_init_bis(char *name, mode_t mode)
{
	if(proc_dir == NULL)
	{
		printk("proc_dir not exist");
		
		return -1;
	}

	proc_file_bis = create_proc_entry(name, mode, proc_dir);
	if(proc_file_bis == NULL)
	{
		printk("create proc_file_bis fail");
		return -1;
	}
	proc_file_bis->data = NULL;
	proc_file_bis->read_proc = (read_proc_t *)proc_read_bis;
	proc_file_bis->write_proc = proc_write_mode;

	return 0;
}

static int __init hello_init(void)
{
	int ret;

	proc_dir = proc_mkdir(MODULE_NAME, NULL);
	if(proc_dir == NULL)
	{
		printk("fail to proc_mkdir");
		return -1;
	}
	ret = proc_init_bis("bis", 06444);
	if(ret != 0)
	{
		remove_proc_entry(MODULE_NAME, NULL);
		return -1;
	}
	printk(KERN_ALERT "module init\n");

	return 0;
}



static void __exit hello_exit(void)
{
	remove_proc_entry(MODULE_NAME, NULL);
	remove_proc_entry("bis", proc_dir);
	printk(KERN_ALERT "module exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
