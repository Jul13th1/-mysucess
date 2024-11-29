#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main() 
{
    const char *filename = "example.txt";  // 要映射的文件
    int fd = open(filename, O_RDWR);        // 以读写模式打开文件
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // 获取文件大小
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }
    size_t filesize = st.st_size;

    // 使用 mmap 映射文件
    char *mapped_memory = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_memory == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 关闭文件描述符，映射后的内存已经不再需要
    close(fd);

    // 修改映射区域内容，写入一个新的字符（例如，将第一个字符设置为 'X'）
    mapped_memory[0] = 'X';

    // 在内存中修改的数据会自动同步到文件
    // 如果想强制同步修改到文件，可以使用 msync()
    if (msync(mapped_memory, filesize, MS_SYNC) == -1) {
        perror("msync");
        munmap(mapped_memory, filesize);
        exit(EXIT_FAILURE);
    }

    // 解除映射
    if (munmap(mapped_memory, filesize) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    printf("File mapped and modified successfully.\n");

    return 0;
}