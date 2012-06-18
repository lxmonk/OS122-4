struct file {
                         //A&T added FD_SYMLNK
  enum { FD_NONE, FD_PIPE, FD_INODE ,FD_SYMLNK} type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  uint off;
};


// in-memory copy of an inode
struct inode {
  uint dev;           // Device number
  uint inum;          // Inode number
  int ref;            // Reference count
  int flags;          // I_BUSY, I_VALID

  short type;         // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT+1+1];      /* A&T +1 for Double indirect */
};
#define I_BUSY 0x1
#define I_VALID 0x2
#define I_SYMLNK 0x4

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode*, char*, int);
  int (*write)(struct inode*, char*, int);
};

extern struct devsw devsw[];

#define CONSOLE 1
