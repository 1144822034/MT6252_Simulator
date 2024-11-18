#include "socket.c"

socketHandle *create_socket(char handle)
{
    socketHandle **current = &socketHandleHead;
    socketHandle *prev = 0;
    while (1)
    {
        if (*current == 0)
        {
            *current = malloc(sizeof(socketHandle));
            (*current)->handle = handle;
            (*current)->prev = prev;
            if (prev != 0)
                prev->next = *current;
            break;
        }
        else
            current = &((*current)->next);
        prev = *current;
    }
}

socketHandle *get_socket(char handle)
{
    socketHandle *current = socketHandleHead;
    socketHandle *prev = 0;
    while (1)
    {
        if (current != 0)
        {
            if (current->handle == handle)
            {
                return current;
            }
            else
                current = current->next;
            prev = current;
        }
        else
            break;
    }
    return 0;
}

void socket_send(char handle, char *buff, int len)
{
    socketHandle *tmp = get_socket(handle);
}

void delete_socket(socketHandle *item)
{
    socketHandle *current = socketHandleHead;
    socketHandle *prev = 0;
    while (1)
    {
        if (current != 0)
        {
            if (current == item)
            {
                current->prev->next = current->next;
                current->next->prev = prev;
                free(current);
                break;
            }
            else
                current = current->next;
            prev = current;
        }
        else
            break;
    }
}