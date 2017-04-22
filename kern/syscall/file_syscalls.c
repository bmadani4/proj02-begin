/*
 * File-related system call implementations.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/fcntl.h>
#include <kern/limits.h>
#include <kern/seek.h>
#include <kern/stat.h>
#include <lib.h>
#include <uio.h>
#include <proc.h>
#include <current.h>
#include <synch.h>
#include <copyinout.h>
#include <vfs.h>
#include <vnode.h>
#include <openfile.h>
#include <filetable.h>
#include <syscall.h>

/*
 * open() - get the path with copyinstr, then use openfile_open and
 * filetable_place to do the real work.
 */
int
sys_open(const_userptr_t upath, int flags, mode_t mode, int *retval)
{
	const int allflags = O_ACCMODE | O_CREAT | O_EXCL | O_TRUNC | O_APPEND | O_NOCTTY;

	char kpath[PATH_MAX];
	struct openfile *file;
	struct filetable *ft = curproc->p_filetable;
	int result = 0;
	result = copyinstr(upath, kpath, sizeof(kpath), NULL);
	if(result){
		return result;
	}
	result = openfile_open(kpath, flags, mode, &file);
	if(result){
		return result;
	}
	result = filetable_place(ft,file,retval);
	if(result){
		return result;
	}
	/* 
	 * Your implementation of system call open starts here.  
	 *
	 * Check the design document design/filesyscall.txt for the steps
	 */
	//(void) upath; // suppress compilation warning until code gets written
	//(void) flags; // suppress compilation warning until code gets written
	//(void) mode; // suppress compilation warning until code gets written
	//(void) retval; // suppress compilation warning until code gets written
	(void) allflags; // suppress compilation warning until code gets written
	//(void) kpath; // suppress compilation warning until code gets written
	//(void) file; // suppress compilation warning until code gets written

	return result;
}

/*
 * read() - read data from a file
 */
int
sys_read(int fd, userptr_t buf, size_t size, int *retval)
{
	int result = 0;
	struct openfile *file;
	struct filetable *ft;
	ft = curproc->p_filetable;
	struct iovec iov;
	struct uio u;
	result = filetable_get(ft,fd,&file);
	if(result)
		return result;
	lock_acquire(file->of_offsetlock);
	if(file->of_accmode == O_WRONLY){
		lock_release(file->of_offsetlock);
		return EBADF;

	}
	uio_kinit(&iov, &u, buf, size, file->of_offset, UIO_READ);
	result = VOP_READ(file->of_vnode, &u);
	if(result){
		lock_release(file->of_offsetlock);
		return result;
	}
	file->of_offset = u.uio_offset;
	lock_release(file->of_offsetlock);
	filetable_put(ft,fd,file);
	*retval = size - u.uio_resid;
       /* 
        * Your implementation of system call read starts here.  
        *
        * Check the design document design/filesyscall.txt for the steps
        */
	//(void) fd; // suppress compilation warning until code gets written
	//(void) buf; // suppress compilation warning until code gets written
        //(void) size; // suppress compilation warning until code gets written
        //(void) retval; // suppress compilation warning until code gets written

       return result;
}

/*
 * write() - write data to a file
 */
int
sys_write(int fd, userptr_t buf, size_t size, int *retval)
{
	int result = 0;
	struct openfile *file;
	struct filetable *ft;
	ft = curproc->p_filetable;
	struct iovec iov;
	struct uio u;
	result = filetable_get(ft,fd,&file);
	if(result)
		return result;
	lock_acquire(file->of_offsetlock);
	if(file->of_accmode == O_RDONLY){
		lock_release(file->of_offsetlock);
		return EBADF;

	}
	uio_kinit(&iov, &u, buf, size, file->of_offset, UIO_WRITE);
	result = VOP_WRITE(file->of_vnode, &u);
	if(result){
		lock_release(file->of_offsetlock);
		return result;
	}
	file->of_offset = u.uio_offset;
	lock_release(file->of_offsetlock);
	filetable_put(ft,fd,file);
	*retval = size - u.uio_resid;
       /* 
        * Your implementation of system call read starts here.  
        *
        * Check the design document design/filesyscall.txt for the steps
        */
	//(void) fd; // suppress compilation warning until code gets written
	//(void) buf; // suppress compilation warning until code gets written
        //(void) size; // suppress compilation warning until code gets written
        //(void) retval; // suppress compilation warning until code gets written

       return result;
}
/*
 * close() - remove from the file table.
 */
int
sys_close(int fd){

	struct openfile *file;
	int result = 0;
	struct filetable *ft = curproc->p_filetable;
	result = filetable_get(ft,fd,&file);
	if(result){
		return result;

	}
	if(filetable_okfd(ft,fd)){
		filetable_placeat(ft,NULL,fd,&file);
	}
	
	
	
	openfile_decref(file);
	



	//(void) fd; // suppress compilation warning until code gets written
	return result;

}
/* 
* encrypt() - read and encrypt the data of a file
*/

int
encrypt(const_userptr_t upath,int fileS)
{

	
		
	//const int allflags = O_ACCMODE | O_CREAT | O_EXCL | O_TRUNC | O_APPEND | O_NOCTTY;

	char kpath[PATH_MAX];
	struct openfile *file;
	struct filetable *ft = curproc->p_filetable;
	int result = 0;
	int flags, mode_t mode, int *retval;
	flags = 0;
	mode = 0;
	
	int endFlag = 0;
	int fd = sys_open(upath,O_RDONLY,mode,retval);
	char byteBuffer[4];
	unsigned int mask1 = 0;
	unsigned int mask2 = 0;
	unsigned int retArray[fileS/4];
	int i = 0;
	do{
		mask1 = 0;
		sys_read(fd,byteBuffer,4,retval);
		//Do bitwise here
		if(byteBuffer[0] == EOF)
			break;
		mask2 = mask2 | byteBuffer[0];
		mask2 << 24;
		mask1 = mask1 | mask2;
		mask2 =0;
		if(byteBuffer[1] == EOF)
			break;
		mask2 = mask2 | byteBuffer[1];
		mask2 << 16;
		mask1 = mask1 | mask2;
		mask2 =0;
		if(byteBuffer[2] == EOF)
			break;
		mask2 = mask2 | byteBuffer[2];
		mask2 <<8;
		mask1 = mask1 | mask2;
		mask2 =0;
		if(byteBuffer[3] == EOF)
			break;
		mask2 = mask2 | byteBuffer[3];
		mask1 = mask1 | mask2;
		mask2 =0;
		
		
		mask1 = (mask1 >> 10) | (mask1 << (22))
		retArray[i] = mask1;
		i++;
	}while(endFlag == 0)
	
	sys_close(fd);
	
	fd = sys_open(upath,O_WRONLY|O_CREAT|O_TRUNC,mode,retval);
	
		
	sys_write(fd,retArray,fileS/4,retval);
	
	sys_close(fd);	


	return 1;




}
