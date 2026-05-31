import paramiko


COMMANDS = [
    (
        "tree",
        "find ~/MicroRos -maxdepth 4 -type f | "
        "sed 's#^/home/yahboom/MicroRos/##' | sort | head -250",
    ),
    (
        "topics",
        "grep -RInE 'cmd_vel|odom_raw|imu_raw|scan|LaserScan|base_footprint|base_link|map|odom|robot_state_publisher|static_transform|servo|pan|tilt|camera|image_raw|depth|tf' "
        "~/MicroRos/src ~/MicroRos/*.sh 2>/dev/null | head -300",
    ),
    (
        "launch_files",
        "find ~/MicroRos/src -maxdepth 6 -path '*launch*' -type f -print",
    ),
]


def main():
    client = paramiko.SSHClient()
    client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    client.connect(
        "192.168.1.105",
        username="yahboom",
        password="yahboom",
        timeout=8,
        banner_timeout=8,
        auth_timeout=8,
    )

    try:
        for name, command in COMMANDS:
            stdin, stdout, stderr = client.exec_command(command)
            print(f"### {name}")
            print(stdout.read().decode("utf-8", "replace"))
            err = stderr.read().decode("utf-8", "replace")
            if err:
                print(err)
    finally:
        client.close()


if __name__ == "__main__":
    main()
