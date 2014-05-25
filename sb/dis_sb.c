/*************************************************************************
	> File Name: dis_sb.c
	> Author: zhanglp
	> Mail: zhanglp92@gmail.com 
	> Blog: http://blog.csdn.net/zhanglpql 
	> Github: http://github.com/zhanglp92 
	> Created Time: 2014年05月21日 星期三 08时42分08秒
 ************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/kdev_t.h>

/* 系统中已安装的文件系统的超级块链表头指针 */
#define SUPER_BLOCKS_ADDRESS    0xffffffff81c672e0
#define SB_LOCK_ADDRESS         0xffffffff81fac320

static int __init my_init (void)
{
    struct super_block  *sb;
    struct list_head    *pos, *pos1;
    struct list_head    *linode;
    struct inode        *pinode;
    long long   count = 0;
    struct dentry       *dentry;

    printk ("\n Print some fields of super_blocks : \n");
    spin_lock ((spinlock_t*)SB_LOCK_ADDRESS);   /* 加锁 */

    list_for_each (pos, (struct list_head*)SUPER_BLOCKS_ADDRESS) {
        
        sb = list_entry (pos, struct super_block, s_list); 
        /* 打印文件系统所在设备的主设备号和次设备号 */
        printk ("dev_t : %d:%d, ", MAJOR (sb->s_dev), MINOR (sb->s_dev));
        /* 打印文件系统名 */
        printk ("file_type name : %s \n", sb->s_type->name); 

        list_for_each (linode, &sb->s_inodes) {
            
            pinode = list_entry (linode, struct inode, i_sb_list);
            count++;
            /* 打印索引节点号 */
            printk ("inode = %lu , mode = %x \n", pinode->i_ino, pinode->i_mode); 
        }
    }
    
    spin_unlock ((spinlock_t*)SB_LOCK_ADDRESS);
    printk ("The number of inodes : %llu \n",  count);

    return 0;
}

static void __exit my_exit (void)
{
    printk ("unloading... \n");
}


module_init (my_init);
module_exit (my_exit);
MODULE_LICENSE ("GPL");
