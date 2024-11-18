

#include <windows.h>
#include <winhttp.h>
#include <stdbool.h>
#include <stdio.h>

void Http_Get(char *url)
{
}

bool canLockFile(HANDLE file)
{
    OVERLAPPED ov = {0};
    if (LockFileEx(file, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &ov))
    {
        UnlockFileEx(file, 0, MAXDWORD, MAXDWORD, &ov);
        return true;
    }
    return false;
}
/**
 * 获取运行的exe所在目录
 */
void GetProgramDirectory(char *directory, size_t size)
{
    char path[MAX_PATH];

    // 获取程序的完整路径
    GetModuleFileNameA(NULL, path, MAX_PATH);

    // 查找最后一个反斜杠，找到目录部分
    char *lastSlash = strrchr(path, '\\');
    if (lastSlash != NULL)
    {
        // 计算目录长度并复制到 directory
        size_t dirLength = lastSlash - path + 1;
        if (dirLength < size)
        {
            strncpy(directory, path, dirLength);
            directory[dirLength] = '\0'; // 添加字符串终止符
        }
    }
}
// IDYES=6
int confirm(const char *title, const char *message)
{
    return MessageBox(
        NULL,                      // 父窗口句柄（NULL 表示没有父窗口）
        message,                   // 消息内容
        title,                     // 消息标题
        MB_YESNO | MB_ICONQUESTION // 窗口类型：带有 Yes 和 No 按钮，带问号图标
    );
}

void system_exit()
{
    exit(0);
}