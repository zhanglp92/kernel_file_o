/*************************************************************************
	> File Name: my_stat.c
	> Author: zhanglp
	> Mail: zhanglp92@gmail.com 
	> Blog: http://blog.csdn.net/zhanglpql 
	> Github: http://github.com/zhanglp92 
	> Created Time: 2014年05月23日 星期五 22时41分13秒
 ************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/syscalls.h>
#include <linux/stat.h>
#include <asm/unistd.h>

static char *filename;
module_param (filename, charp, 0644);

void print_inode_info (struct inode ind, char *path)
{
    /* 在 linux/stat.h 中都能找到相应的宏 */
    printk ("%s mode = %x \n", path, ind.i_mode);

    /* 打印文件类型 */
    #define DIS_TYPE(type, c) \
        (S_IS ##type (ind.i_mode) ? printk (c) : 0)) ;

    if (DIS_TYPE (LNK, "l")
    else if (DIS_TYPE (REG, "-")
    else if (DIS_TYPE (DIR, "d")
    else if (DIS_TYPE (CHR, "c")
    else if (DIS_TYPE (BLK, "b")
    else if (DIS_TYPE (FIFO, "f")
    else if (DIS_TYPE (SOCK, "s")
    #undef  DIS_TYPE


    #define DIS_CHM(chm, c) \
        (ind.i_mode & S_I ##chm ? printk (c) : printk ("-"))

    DIS_CHM (RUSR, "r");
    DIS_CHM (WUSR, "w");
    DIS_CHM (XUSR, "x");

    DIS_CHM (RGRP, "r");
    DIS_CHM (WGRP, "w");
    DIS_CHM (XGRP, "x");

    DIS_CHM (ROTH, "r");
    DIS_CHM (WOTH, "w");

    /* 粘滞位 */
    S_ISVTX & ind.i_mode ? printk ("t") : 
    DIS_CHM (XOTH, "x");
    #undef  DIS_CHM 

    printk ("  ");

#if 0
    /* 打印文件所有者的用户名和组名 */
//    psd = getpwuid (buf.st_uid);
//    grp = getgrgid (buf.st_gid);
    printk ("%4ld ", ind.i_nlink);

    printk ("%-8s", getpwuid (ind.i_uid)->pw_name);
//    printk ("%-8o", ind.i_gid);
#endif
    /* 打印文件的大小 */
    printk ("%8ld", (long)ind.i_size);

#if 0
    char    buf_time[32] = {0}, *tmp;
    if (GET_U_STAT ())  /* 有参数 u 时表示最后一次被访问的时间 */
        tmp = ctime (&buf.st_atime);
    else                /* 没有参数 u 时表示最后u一次修改的时间 */
        tmp = ctime (&buf.st_mtime);
        
    strncpy (buf_time, tmp, strlen (tmp)-1);
    printf ("  %-20s ", buf_time);
#endif
    printk (" %s\n", path);
}

void print_dentry_info (struct dentry dt, char *path) 
{
    struct inode    *inode = dt.d_inode;
    
    print_inode_info (*inode, path);
    printk ("print directory dentry info ! \n");
}

void display_dir (char *filename)
{
    struct file     *fp;
    struct dentry   *dt;

    /* 打开目录，获取dentry */
    if (IS_ERR ((fp = filp_open (filename, O_RDONLY, 0644)))) {
        
        printk ("%s open fail ! \n", filename);
        return ;
    }
    if (IS_ERR (dt = fp->f_dentry)) {
        
        printk ("get %s'dentry fail! \n", filename);
        return ;
    }

    /* 打印目录的 dentry 信息 */
    print_dentry_info (*dt, filename);

    /* 关闭文件 */
    filp_close (fp, NULL);
}

void display_file (char *filename)
{
    struct file     *fp;
    struct inode    *ind;

    /* 打开文件，获取inode */
    if (IS_ERR ((fp = filp_open (filename, O_RDONLY, 0644)))) {

        printk ("%s open fail ! \n", filename);
        return ;
    }
    ind = fp->f_inode;

    /* 打印文件的inode 信息 */
    print_inode_info (*ind, filename);

    /* 关闭文件 */
    filp_close (fp, NULL);
}

static int __init stat_init (void)
{
    mm_segment_t    fs;
    struct kstat    buf;

    /* 备份环境 */
    fs = get_fs (); 
    /* 告诉系统忽略对 user 的校验 */ 
    set_fs (KERNEL_DS); 

    /* 取得路径的属性 */
    if (0 > vfs_stat (filename, &buf)) { 

        /* 路径错误，或系统获取文件属性失败提示 */
        printk ("stat %s fail !\n", filename); 
        return -1;
    }

    /* 如果是目录 */
    if (S_ISDIR (buf.mode)) {
        
        printk ("%s is directory !\n", filename);
        display_dir (filename); 

    }else { /* 非目录文件 */

        printk ("%s is noknow !\n", filename);
        display_file (filename); 
    }

    /* 恢复环境 */
    set_fs (fs); 

    return 0;
}

static void __exit stat_exit (void) 
{
    printk ("stat exit... \n");
}

module_init (stat_init);
module_exit (stat_exit);
MODULE_LICENSE ("GPL");
