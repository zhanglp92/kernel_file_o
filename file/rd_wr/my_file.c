/*************************************************************************
	> File Name: file.c
	> Author: zhanglp
	> Mail: zhanglp92@gmail.com 
	> Blog: http://blog.csdn.net/zhanglpql 
	> Github: http://github.com/zhanglp92 
	> Created Time: 2014年05月23日 星期五 18时08分56秒
 ************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>

static char buf[] = "hello !";
static char buf1[10];

int __init file_init (void) 
{
    struct file     *fp;
    mm_segment_t    fs;
    loff_t          pos;

    printk ("file enter ! \n");
    fp = filp_open ("/home/zhanglp/zlp/module/file/test", O_RDWR | O_CREAT, 0644);
    if (IS_ERR (fp)) {
        
        printk ("create file error ! \n");
        return -1;
    }

    fs = get_fs (); /* 备份 */
    set_fs (KERNEL_DS); /* 告诉系统忽略对 user 的校验 */
    pos = 0;
    vfs_write (fp, buf, sizeof (buf), &pos);
    pos = 0;
    vfs_read (fp, buf1, sizeof (buf1), &pos);
    printk ("read : %s \n", buf1);
    filp_close (fp, NULL);
    set_fs (fs); /* 恢复环境 */

    return 0;
}

void __exit file_exit (void) 
{
    printk ("file exit !\n");
}

module_init (file_init);
module_exit (file_exit);
MODULE_LICENSE ("GPL");
