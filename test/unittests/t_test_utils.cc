/**
 * This file is part of the CernVM File System.
 */

#include <gtest/gtest.h>
#include <unistd.h>

#include "testutil.h"
#include "util/posix.h"

// Verifies GetParentPid resolves parent PIDs in host and container (PID 1) environments.
TEST(T_TestUtilities, GetParentPid) {
  const pid_t my_pid = getpid();
  const pid_t my_parent_pid = getppid();

  // In container PID namespaces, the init process has my_pid == 1, my_parent_pid == 0.
  ASSERT_GE(my_pid, 1);
  ASSERT_GE(my_parent_pid, 0);

  const pid_t result = GetParentPid(my_pid);
  EXPECT_EQ(my_parent_pid, result);

  // Fork a child (guaranteed parent > 0) and verify GetParentPid via exit code.
  const pid_t child_pid = fork();
  ASSERT_GE(child_pid, 0);
  if (child_pid == 0) {
    _exit(GetParentPid(getpid()) == my_pid ? 0 : 1);
  }
  EXPECT_EQ(0, WaitForChild(child_pid));
}


TEST(T_TestUtilities, GetExecutablePath) {
  EXPECT_NE("", GetExecutablePath("cat"));
  EXPECT_NE("", GetExecutablePath("grep"));
  EXPECT_NE("", GetExecutablePath("ls"));
  EXPECT_EQ("", GetExecutablePath("foobar1234renem"));
  EXPECT_EQ("", GetExecutablePath("test/foo"));
  EXPECT_EQ("", GetExecutablePath("../foo/bar"));
  EXPECT_EQ("", GetExecutablePath(""));
}
