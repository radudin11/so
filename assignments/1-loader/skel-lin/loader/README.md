# ELF LOADER

This is my solution for the first homework of the Operating System class

## README contents:
1. Changes made to the given skel.
2. Implementation logic
3. Additional code explanation

## Changes made to the given skel
Added libraries:
- fcntl.h -> used for file open
- unistd.h -> used for read and seek
- sys/mman.h -> used for mapping and permissions
- signal.h -> used for signal handling

Added static variables:
- int pageSize -> initialized in so_execute with getPageSize() value
- int fd -> file descriptor; initialized in so_execute. Stores value returned by open()
- struct sigaction default_handler -> used for storing the default comportament of the handler
These variables were made global so that the signatures of the given funtions (in loader.c)
would stay the same.

Added functions:
- readData -> reads the data of a page form the executable

## Implementation logic
For this homework we had to implement a SIGSEGV handler in the segv_handler() function.

There are 3 causes for a segfault:
1. Invalid memory access (and page is mapped)
2. Trying to access an address without permission
3. Page containing memory address is not mapped -> the case we want to resolve

For the first two, the default comportament of the handler is used

To check if the the page is mapped, we parse each segment until we find the one
containing the memory address being accessed. We use an auxiliary struct to store
the pages already mapped. If the page index of the current segment is not found
amongst the mapped indexes, we must map it.

After the page has been mapped we can read it and set its permissions.

If any of the conditions is not met, we call the default handler.

## Additional code explanaition.
```
// if last page, read only the remaining bytes
	if (exec->segments[curr_segment].file_size < (pageIndex + 1) * pageSize)
		read(fd, buffer, exec->segments[curr_segment].file_size % pageSize);
	else
		read(fd, buffer, pageSize);
```
If the file size is not a multiple of pageSize, the last page will have less then
pageSize bytes. For this case we have to read only the remaining unread bytes.

```
if ((long)info->si_addr >= (long)exec->segments[i].vaddr &&
			(long)info->si_addr < (long)exec->segments[i].vaddr + exec->segments[i].mem_size)
```
Translates to:
    If the faulting address is (after) the starting address of the segment i and before the
    ending address.

```
void *page = mmap(pageStartAddress, pageSize, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
```
Map a page that may be read or written (PROT_READ | PROT_WRITE):
    MAP_PRIVATE -> not visible to other processes
    MAP_ANONYMOUS -> not backed by any file and contents will be 0
    MAP_FIXED -> tells the kernel to map at the exact location given. Could work without
                as i doubt the kernel will stray as far off as to start at a diffrent
                page address

```
if (page == MAP_FAILED) {
				// failed to map the page - call default handler
				default_handler.sa_sigaction(signum, info, context);
				return;
			}
```
Not necessary to call default handler. Could just give an error directly.
    



