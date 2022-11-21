/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#include "exec_parser.h"

static so_exec_t *exec;

static int pageSize;
static struct sigaction default_handler;
static int fd;

typedef struct customData {
	// array containing the indexes of the pages already mapped
	unsigned int *mappedPages;
	// number of pages already mapped
	unsigned int mappedPagesCount;
} customData;

void readData(int pageIndex, void *buffer, int curr_segment)
{
	// read page data from the file
	// place the file pointer at the beginning of the page
	lseek(fd, exec->segments[curr_segment].offset + pageIndex * pageSize, SEEK_SET);

	// if last page, read only the remaining bytes
	if (exec->segments[curr_segment].file_size < (pageIndex + 1) * pageSize)
		read(fd, buffer, exec->segments[curr_segment].file_size % pageSize);
	else
		read(fd, buffer, pageSize);
}

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	/* TODO - actual loader implementation */
	if (signum != SIGSEGV) {
		// not a segfault - call default handler
		default_handler.sa_sigaction(SIGSEGV, info, context);
		return;
	}

	for (int i = 0; i < exec->segments_no; i++) {
		// check if the faulting address is in the segment
		if ((long)info->si_addr >= (long)exec->segments[i].vaddr &&
			(long)info->si_addr < (long)exec->segments[i].vaddr + exec->segments[i].mem_size) {
			// get the custom data
			struct customData *data = (struct customData *)exec->segments[i].data;

			if (data == NULL) {
				// first time we map this segment
				data = (struct customData*)malloc(sizeof(struct customData));
				data->mappedPagesCount = 0;
				data->mappedPages = (unsigned int *)malloc(sizeof(unsigned int) * exec->segments[i].mem_size / pageSize);
				exec->segments[i].data = data;
			}

			// get the page index
			unsigned int pageIndex = ((long)info->si_addr - (long)exec->segments[i].vaddr) / pageSize;

			// check if the page is already mapped
			for (int j = 0; j < data->mappedPagesCount; j++) {
				if (data->mappedPages[j] == pageIndex) {
					// page already mapped - call default handler
					default_handler.sa_sigaction(signum, info, context);
					return;
				}
			}

			// map the page
			void *pageStartAddress = (void *)(exec->segments[i].vaddr + pageIndex * pageSize);

			void *page = mmap(pageStartAddress, pageSize, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

			if (page == MAP_FAILED) {
				// failed to map the page - call default handler
				default_handler.sa_sigaction(signum, info, context);
				return;
			}

			// read the page from the file
			readData(pageIndex, page, i);

			// set the page permissions
			mprotect(page, pageSize, exec->segments[i].perm);

			// add the page to the list of mapped pages
			data->mappedPages[data->mappedPagesCount] = pageIndex;
			data->mappedPagesCount++;
			return;
		}
	}
	// call default handler
	default_handler.sa_sigaction(signum, info, context);
}

int so_init_loader(void)
{
	int rc;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &sa, NULL);
	if (rc < 0) {
		perror("sigaction");
		return -1;
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		perror("open");
		return -1;
	}
	pageSize = getpagesize();

	exec = so_parse_exec(path);
	if (!exec)
		return -1;

	so_start_exec(exec, argv);

	return -1;
}
