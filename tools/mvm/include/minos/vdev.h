#ifndef __MVM_MVM_DEVICE_H__
#define __MVM_MVM_DEVICE_H__

#include <minos/mvm.h>
#include <pthread.h>

#define PDEV_NAME_SIZE		(31)

struct vdev;

struct vdev_ops {
	char *name;
	int (*init)(struct vdev *vdev, char *args);
	void (*deinit)(struct vdev *vdev);
	int (*reset)(struct vdev *vdev);
	int (*setup)(struct vdev *vdev, void *data, int os);
	int (*event)(struct vdev *vdev, int type,
			uint64_t addr, uint64_t *value);
};

#define VDEV_TYPE_PLATFORM	(0x0)
#define VDEV_TYPE_VIRTIO	(0x1)

struct vdev {
	int id;
	struct vm *vm;
	int gvm_irq;
	uint64_t guest_iomem;
	size_t iomem_size;
	void *iomem;
	struct vdev_ops *ops;
	void *pdata;
	int dev_type;
	unsigned long flags;
	char name[PDEV_NAME_SIZE + 1];
	struct list_head list;
	pthread_mutex_t lock;
};

#define DEFINE_VDEV_TYPE(ops)	\
	static void *mvdev_ops_##ops __used __section("vdev_ops") = &ops

extern void * __start_vdev_ops;
extern void *__stop_vdev_ops;

int create_vdev(struct vm *vm, char *class, char *args);
void *vdev_map_iomem(unsigned long iomem, size_t size);
void vdev_unmap_iomem(void *iomem, size_t size);
void vdev_setup_env(struct vm *vm, void *data, int os_type);
void vdev_send_irq(struct vdev *vdev);
void release_vdevs(struct vm *vm);
int vdev_alloc_irq(struct vm *vm, int nr);
int vdev_alloc_and_request_irq(struct vm *vm, int nr);

static void inline vdev_set_pdata(struct vdev *vdev, void *data)
{
	if (vdev)
		vdev->pdata = data;
}

static inline void *vdev_get_pdata(struct vdev *vdev)
{
	return (vdev ? vdev->pdata : NULL);
}

#endif
