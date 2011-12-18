/*
 * CROSSMETA Windows porting changes.
 * Copyright (c) 2001-2011 Supramani Sammandam.  suprasam _at_ crossmeta.org
 */
#include <ntifs.h>
#include <sys/param.h>
#include <sys/kern_svcs.h>
#include <sys/uio.h>
#include <sys/cred.h>
#include <sys/vfs.h>
#include <sys/vnode.h>
#include <sys/namei.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <linux/fs.h>
#include <linux/ext2_fs.h>
#include <sys/errno.h>

/*
 * Windows NT file system driver interface routines
 */

#define	XFS_DEVICE_NAME		L"\\Device\\ext2fs"
#define	XFS_DOSDEV_NAME		L"\\DosDevices\\ext2fs"
#define	XFS_DOSDEV		"\\\\.\\ext2fs"

NTSTATUS 	DriverEntry(PDRIVER_OBJECT, PUNICODE_STRING);
NTSTATUS 	ext2fs_mod_iodispatch(PDEVICE_OBJECT devp, PIRP);
void		ext2fs_mod_unload(PDRIVER_OBJECT);
NTSTATUS 	ext2fs_mod_ioctl(PDEVICE_OBJECT, PIRP, PIO_STACK_LOCATION);

extern int  	ext2fs_init(struct vfssw *, int fstype);
extern void 	ext2fs_unload(void);
STATIC int ext2fs_mount(vfs_t *, vnode_t *, struct mount_args *, cred_t *);

PDEVICE_OBJECT	ext2fs_fsdevptr;	/* our device object pointer */

struct vfsops ext2fs_vfsops = {
	ext2fs_mount,
	generic_fs_unmount,
	generic_fs_root,
	generic_fs_statfs,
	generic_fs_sync,
	generic_fs_vget,
	generic_fs_mountroot,
};

vnodeops_t ext2fs_vnodeops = {
	generic_vop_open,
	generic_vop_close,
	generic_vop_read,
	generic_vop_write,
	generic_vop_ioctl,
	generic_vop_getattr,
	generic_vop_setattr,
	generic_vop_access,
	generic_vop_lookup,
	generic_vop_create,
	generic_vop_remove,
	generic_vop_link,
	generic_vop_rename,
	generic_vop_mkdir,
	generic_vop_rmdir,
	generic_vop_readdir,
	generic_vop_symlink,
	generic_vop_readlink,
	generic_vop_fid,
	generic_vop_fsync,
	generic_vop_rwlock,
	generic_vop_rwunlock,
	generic_vop_bmap,
	generic_vop_strategy,
	generic_vop_inactive,
	generic_vop_reclaim,
	generic_vop_seek,
	generic_vop_prewrite,
	generic_vop_commit_write,
};

static struct file_system_type ext2fs_type = {
	"ext2fs",
	ext2_read_super,
	ext2fs_mod_unload,
	VT_EXT2FS,
	&ext2fs_vnodeops,
};

int
ext2fs_init(struct vfssw *vswp, int fstype)
{
	ext2fs_type.fs_type = fstype;
	vswp->vsw_vfsops = &ext2fs_vfsops;
	vswp->vsw_unload = ext2fs_unload;
	register_filesystem(&ext2fs_type);
	return (0);
}

void
ext2fs_unload(void)
{

	unregister_filesystem(&ext2fs_type);
}

STATIC int
ext2fs_mount(vfs_t *vfsp, vnode_t *mvp, struct mount_args *uap, cred_t *cr)
{
	int error;
	struct vfsmount *mp;
	struct super_block *s;
	struct ext2_super_block *es;

	error = generic_fs_mount(vfsp, mvp, uap, cr);
	if (error)
		return (error);

	s = vfsp->vfs_data;
	es = EXT2_SB(s)->s_es;
	mp = VFSTOMP(vfsp);
	strncpy(es->s_last_mounted, mp->mnt_stat.f_mntonname,
		 sizeof (es->s_last_mounted) - 1);
	return (0);
}

NTSTATUS
DriverEntry(PDRIVER_OBJECT drvobp, PUNICODE_STRING regpath)
{
	UNICODE_STRING devname, dosdev;
	NTSTATUS stat;
	int i;
	

#if 0
	/* Create a device object for this driver */

	RtlInitUnicodeString(&devname, XFS_DEVICE_NAME);
	stat = IoCreateDevice(drvobp, 0, &devname, FILE_DEVICE_FILE_SYSTEM,
				0, FALSE, &xfs_fsdevptr);
	if (!NT_SUCCESS(stat)) {
		printf("XFS: unable to create device object: %x\n", stat);
		return (stat);
	}

	xfs_fsdevptr->Flags |= DO_DIRECT_IO;

	RtlInitUnicodeString(&dosdev, XFS_DOSDEV_NAME);
	stat = IoCreateSymbolicLink(&dosdev, &devname);
	if (!NT_SUCCESS(stat)) {
		printf("nfsrv: Unable to create dosdevice : %x\n", stat);
		IoDeleteDevice(xfs_fsdevptr);
		return (stat);
	}
#endif

	ext2fs_type.owner = drvobp;
	/* Setup the driver entrypoints */
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		if (i == IRP_MJ_FLUSH_BUFFERS)
			continue;
		drvobp->MajorFunction[i] = ext2fs_mod_iodispatch;
	}

	/*
	 * Register ext2fs file system with VFS layer
	 */
	vfs_registerfs("ext2fs", ext2fs_init, &ext2fs_vfsops);


#if 0
	nfssysctl_ctxp = sysctl_register_external_set(nfssysctl_set,
							nfs_numsysctl);
#endif

	drvobp->DriverUnload = ext2fs_mod_unload;
	return (STATUS_SUCCESS);
}


/*
 * Driver dispatch routine for any I/O requests
 */
NTSTATUS
ext2fs_mod_iodispatch(PDEVICE_OBJECT devp, PIRP irp)
{
	PIO_STACK_LOCATION irpsp;
	NTSTATUS stat;

	PAGED_CODE();

	irpsp = IoGetCurrentIrpStackLocation(irp);

#if 0
	printf("nfsdrvio: MajorFunction code = 0x%x\n", irpsp->MajorFunction);
#endif

	switch (irpsp->MajorFunction) {

	case IRP_MJ_CREATE:
		stat = STATUS_SUCCESS;
		break;

	case IRP_MJ_CLEANUP:
		stat = STATUS_SUCCESS;
		break;

	case IRP_MJ_CLOSE:
		stat = STATUS_SUCCESS;
		break;

	case IRP_MJ_DEVICE_CONTROL:
		stat = ext2fs_mod_ioctl(devp, irp, irpsp);
		break;
	default:
		stat = STATUS_NOT_IMPLEMENTED;
		break;
	}

	irp->IoStatus.Status = stat;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return (stat);
}

NTSTATUS
ext2fs_mod_ioctl(PDEVICE_OBJECT devp, PIRP irp, PIO_STACK_LOCATION irpsp)
{
	int error;
	void *sysarg;
	uint_t inlen;
	ULONG code;
	NTSTATUS stat;
	PIRP toplevel;

	code = irpsp->Parameters.DeviceIoControl.IoControlCode;
	inlen = irpsp->Parameters.DeviceIoControl.InputBufferLength;
	if ((code & METHOD_NEITHER) == METHOD_NEITHER)
		sysarg = irpsp->Parameters.DeviceIoControl.Type3InputBuffer;
	else
		sysarg = irp->AssociatedIrp.SystemBuffer;

	toplevel = filesys_enter();

	switch (code) {


	default:
		printf("ext2fsmod: invalid ioctl code = %lx\n", code);
		stat = STATUS_INVALID_PARAMETER;
		break;
	}

	filesys_leave(toplevel);

	irp->IoStatus.Status = stat;
	return (stat);
}

void
ext2fs_mod_unload(PDRIVER_OBJECT drvobp)
{
	UNICODE_STRING dosdev;
	extern int xfs_fstype;

	if (ext2fs_type.fs_type != 0) {
		ext2fs_unload();
	}

	bdflush_exit();
        vfs_unregisterfs("ext2fs");

#if 0
	sysctl_ctx_free(nfssysctl_ctxp);
	kmem_free(nfssysctl_ctxp);

	/* remove symbolic-link to our device */
	RtlInitUnicodeString(&dosdev, XFS_DOSDEV_NAME);
	(void)IoDeleteSymbolicLink(&dosdev);

	/* delete our device itself */
	IoDeleteDevice(xfs_fsdevptr);
#endif
}

