#include "linux/usb.h"
#include "linux/module.h"
#include "linux/kernel.h"

#ifndef IDVENDOR
#define IDVENDOR 0x0080
#endif
#ifndef IDPRODUCT
#define IDPRODUCT 0x7801
#endif
void log(const char* text){
  printk(KERN_ALERT "usb_driver: %s.\n");
}
int ret=-1;

struct usb_device_id flash_table[]= {
  /* data */
  {USB_DEVICE(IDVENDOR,IDPRODUCT)},
  {}
};


MODULE_DEVICE_TABLE(usb,flash_table);

int flash_probe(struct usb_interface* interface,const usb_device_id* id) {
  /* code */
  printk(KERN_INFO "usb_driver: flash(%d:%d) is plugged",id->idVendor,id->idProduct);
  if (id->idVendor==IDVENDOR&&id->idProduct==IDPRODUCT) {
    /* code */
    usb_fill_control_urb(urb,USB_DEVICE(id->idVendor,id->idProduct),interface->);
    ret=usb_submit_urb(urb,GFP_KERNEL);
    switch (ret) {
      case 0:log("submit urb success.");
      case -ENOMEM:printk(KERN_ALERT "usb_driver: out of memory.\n");break;
      case -ENODEV:log("no device was set.");
      case -EPIPE:log("stalled endpoint.");
      case -EAGAIN:log("too many queued ISO transfers.");
      case -EFBIG:log("too many requested ISO frames.");
      case -EINVAL:log("invalid INT interval. More than one packet for INT");
      default:log("submit failed with unknown reason.")

    }
  }

  return 0;
}


struct usb_driver flash_driver{
  /* data */
  .name="flash driver",
  .id_table=flash_table,
  .probe=flash_probe,
  .disconnect=flash_disconnect
};

int __init driver_enter(void) {
  /* code */
  /*
  *create urb=usb request block
  **/
  struct urb *urb=usb_alloc_urb(0,GFP_KERNEL);

  if (urb==NULL) {
    /* code */
    printk(KERN_ALERT "usb_driver: allocate urb failed.\n");
    return -1;
  }

  ret=usb_register(flash_driver);
  if(ret){
    log("register usb driver failed");
  }

  return ret;
}


void __exit driver_exit(void) {
  /* code */
  if (urb!=NULL) {
    /* code */
    usb_kill_urb(urb);
    usb_free_urb(urb);
  }
  usb_deregister(flash_driver);

  return;
}

module_init(driver_enter);
module_exit(driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SU XING FACTORY");
MODULE_DESCRIPTION("Example for usb driver");
