#ifndef __MY_NAMED_PIPE__H
#define __MY_NAMED_PIPE__H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define PIPE_NAME_C2S "c2s"
#define PIPE_NAME_S2C "s2c"

enum PIPE_MODE
{
	PIPE_READ = 1,
	PIPE_WRITE
};

class NamedPipe
{
public:
	bool Open(const char* path, PIPE_MODE mode)
	{
		int ret = mkfifo(path, 0777);
		if(0 != ret && EEXIST != errno)
		{
			m_errno = errno;
			return false;
		}

		int fd = open(path, mode==PIPE_READ?O_RDONLY:O_WRONLY);
		if(-1 == fd)
		{
			m_errno = errno;
			return false;
		}
		m_fd = fd;
		return true;
	}

	void Close()
	{
		if(-1 != m_fd){
			close(m_fd);
		}
		Reset();
	}

	int Write(const void* buf, size_t count)
	{
		int ret = write(m_fd, buf, count);
		if(ret < 0){
			m_errno = errno;
		}
		return ret;
	}

	int Read(void* buf, size_t count)
	{
		int ret = read(m_fd, buf, count);
		if(ret < 0){
			m_errno = errno;
		}
		return ret;
	}

	int GetLastErr()
	{
		return m_errno;
	}


	NamedPipe()
	{
		Reset();
	}
	~NamedPipe()
	{
		Close();
	}

private:
	void Reset()
	{
		m_fd = -1;
		m_mode = (PIPE_MODE)0;
		m_errno = 0;
	}

	int m_fd;
	PIPE_MODE m_mode;
	int m_errno;
};
#endif
