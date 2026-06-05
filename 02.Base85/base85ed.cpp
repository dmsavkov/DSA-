#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "base85ed.h"

// TODO: remove this
static std::vector<uint8_t> run_command_io(const std::string &command,
        const std::vector<uint8_t> &in)
{
    int inpipe[2];   // parent -> child
    int outpipe[2];  // child -> parent

    if (pipe(inpipe) == -1) throw std::runtime_error(strerror(errno));
    if (pipe(outpipe) == -1)
    {
        close(inpipe[0]);
        close(inpipe[1]);
        throw std::runtime_error(strerror(errno));
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        close(inpipe[0]);
        close(inpipe[1]);
        close(outpipe[0]);
        close(outpipe[1]);
        throw std::runtime_error(strerror(errno));
    }

    if (pid == 0)
    {
        // child
        dup2(inpipe[0], STDIN_FILENO);
        dup2(outpipe[1], STDOUT_FILENO);
        close(inpipe[0]);
        close(inpipe[1]);
        close(outpipe[0]);
        close(outpipe[1]);
        execl("/bin/sh", "sh", "-c", command.c_str(), (char*)nullptr);
        _exit(127);
    }

    // parent
    close(inpipe[0]);
    close(outpipe[1]);

    // write input
    const uint8_t *wp = in.data();
    ssize_t remaining = static_cast<ssize_t>(in.size());
    while (remaining > 0)
    {
        ssize_t n = write(inpipe[1], wp, remaining);
        if (n == -1)
        {
            if (errno == EINTR) continue;
            close(inpipe[1]);
            close(outpipe[0]);
            waitpid(pid, nullptr, 0);
            throw std::runtime_error(strerror(errno));
        }
        remaining -= n;
        wp += n;
    }
    close(inpipe[1]); // signal EOF

    // read all stdout
    std::vector<uint8_t> out;
    uint8_t buf[4096];
    while (true)
    {
        ssize_t n = read(outpipe[0], buf, sizeof(buf));
        if (n > 0) out.insert(out.end(), buf, buf + n);
        else if (n == 0) break;
        else
        {
            if (errno == EINTR) continue;
            close(outpipe[0]);
            waitpid(pid, nullptr, 0);
            throw std::runtime_error(strerror(errno));
        }
    }
    close(outpipe[0]);

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) throw std::runtime_error(strerror(errno));
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        throw std::runtime_error("child exited with non-zero status");

    return out;
}


// TODO: implement this in C++
std::vector<uint8_t> base85::encode(std::vector<uint8_t> const &bytes)
{
    return run_command_io(
               "/usr/bin/env -S python3 -c 'import sys; import base64; sys.stdout.buffer.write(base64.b85encode(sys.stdin.buffer.read()))'",
               bytes
           );
}


// TODO: implement this in C++
std::vector<uint8_t> base85::decode(std::vector<uint8_t> const &b85str)
{
    return run_command_io(
               "/usr/bin/env -S python3 -c 'import sys; import base64; sys.stdout.buffer.write(base64.b85decode(sys.stdin.buffer.read()))'",
               b85str
           );
}
