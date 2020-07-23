
def on_cmd(cmd):
    cmd_arr = cmd.split(" ")
    if len(cmd_arr) == 0:
        return
    cmd = cmd_arr[0]
    if cmd == "hotfix":
        import hotfix.hotfix
        hotfix.hotfix.start_hotfix()
    else:
        print("unknown cmd:", cmd)
